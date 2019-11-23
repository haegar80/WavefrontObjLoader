#include "WavefrontRenderer.h"
#include <QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QImage>

WavefrontRenderer::WavefrontRenderer(const ObjLoader& pc_objLoader, int p_width, int p_height) :
    mc_objLoader(pc_objLoader),
    m_wavefrontModelIndexArrayBuffer(QOpenGLBuffer::IndexBuffer),
    m_width(p_width),
    m_height(p_height)
{
    initializeOpenGLFunctions();

    m_vertexArrayObjectWavefrontModel.create();
    m_wavefrontModelVertexBuffer.create();
    m_wavefrontModelNormalBuffer.create();
    m_wavefrontModelTextureBuffer.create();
    m_wavefrontModelIndexArrayBuffer.create();
}

WavefrontRenderer::~WavefrontRenderer()
{
    m_vertexArrayObjectWavefrontModel.destroy();
    m_wavefrontModelVertexBuffer.destroy();
    m_wavefrontModelNormalBuffer.destroy();
    m_wavefrontModelTextureBuffer.destroy();
    m_wavefrontModelIndexArrayBuffer.destroy();

    for (std::pair<SubMesh*, QOpenGLTexture*> texture : m_subMeshAmbientTextures)
    {
        delete texture.second;
    }
    for (std::pair<SubMesh*, QOpenGLTexture*> texture : m_subMeshDiffuseTextures)
    {
        delete texture.second;
    }
    for (std::pair<SubMesh*, QOpenGLTexture*> texture : m_subMeshSpecularTextures)
    {
        delete texture.second;
    }
}

void WavefrontRenderer::init(QOpenGLShaderProgram* p_shaderProgram)
{
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vertexArrayObjectWavefrontModel);
    initWavefrontModels(p_shaderProgram);
}

void WavefrontRenderer::render(QOpenGLShaderProgram* p_shaderProgram)
{
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vertexArrayObjectWavefrontModel);
    renderWavefrontModels(p_shaderProgram);
}

void WavefrontRenderer::initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
    std::vector<ObjVertexCoords> vertices = getScaledVerticesFromWavefrontModel();
    std::vector<ObjVertexCoords> normals = getNormalsFromWaveFrontModel();
    std::vector<ObjTextureCoords> textureCoords = getScaledTextureCoordsFromWaveFrontModel();

    m_wavefrontModelVertexBuffer.bind();
    m_wavefrontModelVertexBuffer.allocate(vertices.data(), static_cast<int>(vertices.size()) * sizeof(ObjVertexCoords));

    m_wavefrontModelNormalBuffer.bind();
    m_wavefrontModelNormalBuffer.allocate(normals.data(), static_cast<int>(normals.size()) * sizeof(ObjVertexCoords));

    m_wavefrontModelTextureBuffer.bind();
    m_wavefrontModelTextureBuffer.allocate(textureCoords.data(), static_cast<int>(textureCoords.size()) * sizeof(ObjTextureCoords));

    std::vector<Mesh*> meshes = mc_objLoader.GetMeshes();
    std::vector<unsigned short> vertexIndices;

    m_subMeshIndicesCount.clear();

    for (Mesh* mesh : meshes)
    {
        for (SubMesh* submesh : mesh->GetSubmeshes())
        {
            std::vector<unsigned short> indices;
            int numberOfIndicesPerSubmesh = 0;
            for (ObjFace face : submesh->GetFaces())
            {
                for (ObjFaceIndices index : face.Indices)
                {
                    numberOfIndicesPerSubmesh++;
                    vertexIndices.push_back(index.VertexIndex);
                }
            }
            m_subMeshIndicesCount.insert(std::make_pair(submesh, numberOfIndicesPerSubmesh));

            prepareTextures(submesh);
        }
    }
    m_wavefrontModelIndexArrayBuffer.bind();
    m_wavefrontModelIndexArrayBuffer.allocate(vertexIndices.data(), static_cast<int>(vertexIndices.size()) * sizeof(unsigned short));

    m_wavefrontModelVertexBuffer.bind();
    int vertexLocation = p_shaderProgram->attributeLocation("in_Position");
    p_shaderProgram->enableAttributeArray(vertexLocation);
    p_shaderProgram->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3);

    m_wavefrontModelNormalBuffer.bind();
    int normalLocation = p_shaderProgram->attributeLocation("in_Normal");
    p_shaderProgram->enableAttributeArray(normalLocation);
    p_shaderProgram->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3);

    m_wavefrontModelTextureBuffer.bind();
    int textureCoordLocation = p_shaderProgram->attributeLocation("in_Texture");
    p_shaderProgram->enableAttributeArray(textureCoordLocation);
    p_shaderProgram->setAttributeBuffer(textureCoordLocation, GL_FLOAT, 0, 2);
}

void WavefrontRenderer::renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
    p_shaderProgram->setUniformValue("light.Position", QVector3D(0.5f, 0.4f, -1.0f));
    p_shaderProgram->setUniformValue("light.AmbientColor", QVector3D(0.0f, 0.3f, 0.0f));
    p_shaderProgram->setUniformValue("light.DiffuseColor", QVector3D(0.9f, 0.9f, 0.9f));
    p_shaderProgram->setUniformValue("light.SpecularColor", QVector3D(0.8f, 0.8f, 0.8f));
    p_shaderProgram->setUniformValue("material.SpecularExponent", 50.f);
    int indexOffset = 0;

    for (std::pair<SubMesh*, int> submeshData : m_subMeshIndicesCount)
    {
        MaterialRGBValue rgbValue = submeshData.first->GetMaterial()->getAmbientColor();
        p_shaderProgram->setUniformValue("material.Ka", QVector3D(rgbValue.R, rgbValue.G, rgbValue.B));
        rgbValue = submeshData.first->GetMaterial()->getDiffuseColor();
        p_shaderProgram->setUniformValue("material.Kd", QVector3D(rgbValue.R, rgbValue.G, rgbValue.B));
        rgbValue = submeshData.first->GetMaterial()->getSpecularColor();
        p_shaderProgram->setUniformValue("material.Ks", QVector3D(rgbValue.R, rgbValue.G, rgbValue.B));
        float specularExponent = submeshData.first->GetMaterial()->getSpecularExponent();
        p_shaderProgram->setUniformValue("material.SpecularExponent", specularExponent);
        m_subMeshAmbientTextures.at(submeshData.first)->bind(0);
        m_subMeshDiffuseTextures.at(submeshData.first)->bind(1);
        m_subMeshSpecularTextures.at(submeshData.first)->bind(2);

        glDrawElements(GL_TRIANGLES, submeshData.second, GL_UNSIGNED_SHORT, (GLvoid*)(indexOffset * sizeof(unsigned short)));
        indexOffset += submeshData.second;
    }
}

std::vector<ObjVertexCoords> WavefrontRenderer::getScaledVerticesFromWavefrontModel()
{
    std::vector<ObjVertexCoords> scaledVertices;
    // Todo: Only one mesh supported yet
    std::vector<ObjVertexCoords> vertices;
    int sizeMaxVertices = 0;
    for (Mesh* mesh : mc_objLoader.GetMeshes())
    {
        int numberOfVertices = static_cast<int>(mesh->GetVertices().size());
        if (numberOfVertices >= sizeMaxVertices)
        {
            sizeMaxVertices = numberOfVertices;
            vertices = mesh->GetVertices();
        }
    }

    float minX = 0.0f;
    float minY = 0.0f;
    float maxX = 0.0f;
    float maxY = 0.0f;
    for (ObjVertexCoords vertex : vertices)
    {
        if (vertex.X > maxX)
        {
            maxX = vertex.X;
        }
        else if (vertex.X < minX)
        {
            minX = vertex.X;
        }
        if (vertex.Y > maxY)
        {
            maxY = vertex.Y;
        }
        else if (vertex.Y < minY)
        {
            minY = vertex.Y;
        }
    }

    float differenceX = maxX - minX;
    float differenceY = maxY - minY;

    float scaleFactorX = m_width / differenceX;
    float scaleFactorY = m_height / differenceY;
    if (scaleFactorX < scaleFactorY)
    {
        m_scaleFactor = scaleFactorX;
    }
    else {
        m_scaleFactor = scaleFactorY;
    }
    m_moveToCenterX = minX * m_scaleFactor;
    m_moveToCenterY = minY * m_scaleFactor;

    for (ObjVertexCoords vertex : vertices)
    {
        ObjVertexCoords scaledVertex;
        scaledVertex.X = vertex.X * m_scaleFactor - m_moveToCenterX;
        scaledVertex.Y = vertex.Y * m_scaleFactor - m_moveToCenterY;
        scaledVertex.Z = vertex.Z * m_scaleFactor;
        scaledVertices.push_back(scaledVertex);
    }

    return scaledVertices;
}

std::vector<ObjVertexCoords> WavefrontRenderer::getNormalsFromWaveFrontModel()
{
    // Todo: Only one mesh supported yet
    return mc_objLoader.GetMeshes().back()->GetNormals();
}

std::vector<ObjTextureCoords> WavefrontRenderer::getScaledTextureCoordsFromWaveFrontModel()
{
    std::vector<ObjTextureCoords> scaledTextures;
    // Todo: Only one mesh supported yet
    std::vector<ObjTextureCoords> textures = mc_objLoader.GetMeshes().back()->GetTextures();

    for (ObjTextureCoords texture : textures)
    {
        ObjTextureCoords scaledTexture;
        scaledTexture.U = texture.U * m_scaleFactor - m_moveToCenterX;
        scaledTexture.V = texture.V * m_scaleFactor - m_moveToCenterY;
        scaledTextures.push_back(scaledTexture);
    }

    return scaledTextures;
}

void WavefrontRenderer::prepareTextures(SubMesh* p_submesh)
{
    QImage transparentImage(1, 1, QImage::Format::Format_ARGB32_Premultiplied);
    transparentImage.fill(QColor(255, 255, 255, 255));

    QString ambientTexturePath(p_submesh->GetMaterial()->getAmbientTexturePath().c_str());
    QOpenGLTexture* ambientTexture = nullptr;
    if (ambientTexturePath.isEmpty())
    {
        ambientTexture = new QOpenGLTexture(std::move(transparentImage));
    }
    else
    {
        ambientTexture = new QOpenGLTexture(QImage(ambientTexturePath).mirrored());
    }
    m_subMeshAmbientTextures.insert(std::make_pair(p_submesh, ambientTexture));

    QString diffuseTexturePath(p_submesh->GetMaterial()->getDiffuseTexturePath().c_str());
    QOpenGLTexture* diffuseTexture = nullptr;
    if (diffuseTexturePath.isEmpty())
    {
        diffuseTexture = new QOpenGLTexture(std::move(transparentImage));
    }
    else
    {
        diffuseTexture = new QOpenGLTexture(QImage(diffuseTexturePath).mirrored());
    }
    m_subMeshDiffuseTextures.insert(std::make_pair(p_submesh, diffuseTexture));

    QString specularTexturePath(p_submesh->GetMaterial()->getSpecularTexturePath().c_str());
    QOpenGLTexture* specularTexture = nullptr;
    if (specularTexturePath.isEmpty())
    {
        specularTexture = new QOpenGLTexture(std::move(transparentImage));
    }
    else
    {
        specularTexture = new QOpenGLTexture(QImage(specularTexturePath).mirrored());
    }
    m_subMeshSpecularTextures.insert(std::make_pair(p_submesh, specularTexture));
}