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

	for each(std::pair<SubMesh*, QOpenGLTexture*> texture in m_subMeshAmbientTextures)
	{
		delete texture.second;
	}
	for each(std::pair<SubMesh*, QOpenGLTexture*> texture in m_subMeshDiffuseTextures)
	{
		delete texture.second;
	}
	for each(std::pair<SubMesh*, QOpenGLTexture*> texture in m_subMeshSpecularTextures)
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
	std::vector<ObjTextureCoords> textureCoords = getTextureCoordsFromWaveFrontModel();

	m_wavefrontModelVertexBuffer.bind();
	m_wavefrontModelVertexBuffer.allocate(vertices.data(), vertices.size() * sizeof(ObjVertexCoords));

	m_wavefrontModelNormalBuffer.bind();
	m_wavefrontModelNormalBuffer.allocate(normals.data(), normals.size() * sizeof(ObjVertexCoords));

	m_wavefrontModelTextureBuffer.bind();
	m_wavefrontModelTextureBuffer.allocate(textureCoords.data(), textureCoords.size() * sizeof(ObjTextureCoords));

	std::vector<Mesh*> meshes = mc_objLoader.GetMeshes();
	std::vector<unsigned short> vertexIndices;

	m_subMeshIndicesCount.clear();
	for each(Mesh* mesh in meshes)
	{
		for each(SubMesh* submesh in mesh->GetSubmeshes())
		{
			std::vector<unsigned short> indices;
			int numberOfIndicesPerSubmesh = 0;
			for each(ObjFace face in submesh->GetFaces())
			{
				for each(ObjFaceIndices index in face.Indices)
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
	m_wavefrontModelIndexArrayBuffer.allocate(vertexIndices.data(), vertexIndices.size() * sizeof(unsigned short));

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
	p_shaderProgram->setUniformValue("light.Position", QVector3D(0.5f, 0.4f, 1.0f));
	p_shaderProgram->setUniformValue("light.AmbientColor", QVector3D(0.0f, 0.3f, 0.0f));
	p_shaderProgram->setUniformValue("light.DiffuseColor", QVector3D(0.9f, 0.9f, 0.9f));
	p_shaderProgram->setUniformValue("light.SpecularColor", QVector3D(1.0f, 1.0f, 1.0f));
	p_shaderProgram->setUniformValue("material.Shininess", 50.f);
	int indexOffset = 0;

	for each(std::pair<SubMesh*, int> submeshData in m_subMeshIndicesCount)
	{
		MaterialRGBValue rgbValue = submeshData.first->GetMaterial()->getAmbientColor();
		p_shaderProgram->setUniformValue("material.Ka", QVector3D(rgbValue.R, rgbValue.G, rgbValue.B));
		rgbValue = submeshData.first->GetMaterial()->getDiffuseColor();
		p_shaderProgram->setUniformValue("material.Kd", QVector3D(rgbValue.R, rgbValue.G, rgbValue.B));
		rgbValue = submeshData.first->GetMaterial()->getSpecularColor();
		p_shaderProgram->setUniformValue("material.Ks", QVector3D(rgbValue.R, rgbValue.G, rgbValue.B));

		m_subMeshAmbientTextures.at(submeshData.first)->bind(0);
		m_subMeshDiffuseTextures.at(submeshData.first)->bind(1);
		m_subMeshSpecularTextures.at(submeshData.first)->bind(2);

		glDrawElements(GL_TRIANGLES, submeshData.second, GL_UNSIGNED_SHORT, (GLvoid*) (indexOffset * sizeof(unsigned short)));
		indexOffset += submeshData.second;
	}
}

std::vector<ObjVertexCoords> WavefrontRenderer::getScaledVerticesFromWavefrontModel()
{
	std::vector<ObjVertexCoords> scaledVertices;
	// Todo: Only one mesh supported yet
	std::vector<ObjVertexCoords> vertices = mc_objLoader.GetMeshes().back()->GetVertices();

	float minX = 0.0f;
	float minY = 0.0f;
	float maxX = 0.0f;
	float maxY = 0.0f;
	for each(ObjVertexCoords vertex in vertices)
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
	float moveToCenterX = minX * scaleFactorX;
	float moveToCenterY = minY * scaleFactorY;

	for each(ObjVertexCoords vertex in vertices)
	{
		ObjVertexCoords scaledVertex;
		scaledVertex.X = vertex.X * scaleFactorX - moveToCenterX;
		scaledVertex.Y = vertex.Y * scaleFactorY - moveToCenterY;
		scaledVertex.Z = vertex.Z;
		scaledVertices.push_back(scaledVertex);
	}

	return scaledVertices;
}

std::vector<ObjVertexCoords> WavefrontRenderer::getNormalsFromWaveFrontModel()
{
	// Todo: Only one mesh supported yet
	return mc_objLoader.GetMeshes().back()->GetNormals();
}

std::vector<ObjTextureCoords> WavefrontRenderer::getTextureCoordsFromWaveFrontModel()
{
	// Todo: Only one mesh supported yet
	return mc_objLoader.GetMeshes().back()->GetTextures();
}

void WavefrontRenderer::prepareTextures(SubMesh* p_submesh)
{
	QImage transparentImage(1, 1, QImage::Format::Format_ARGB32_Premultiplied);
	transparentImage.fill(Qt::transparent);

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