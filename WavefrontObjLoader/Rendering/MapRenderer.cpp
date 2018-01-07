#include "MapRenderer.h"
#include <QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>

MapRenderer::MapRenderer(const ObjLoader& pc_objLoader, int p_width, int p_height) :
	mc_objLoader(pc_objLoader),
	m_wavefrontModelIndexArrayBuffer(QOpenGLBuffer::IndexBuffer),
	m_width(p_width),
	m_height(p_height)
{
	initializeOpenGLFunctions();

	m_vertexArrayObjectWavefrontModel.create();
	m_wavefrontModelVertexBuffer.create();
	m_wavefrontModelNormalBuffer.create();
	m_wavefrontModelIndexArrayBuffer.create();
}

MapRenderer::~MapRenderer()
{
	m_vertexArrayObjectWavefrontModel.destroy();
	m_wavefrontModelVertexBuffer.destroy();
	m_wavefrontModelNormalBuffer.destroy();
	m_wavefrontModelIndexArrayBuffer.destroy();
}

void MapRenderer::init(QOpenGLShaderProgram* p_shaderProgram)
{
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vertexArrayObjectWavefrontModel);
	initWavefrontModels(p_shaderProgram);
}

void MapRenderer::render(QOpenGLShaderProgram* p_shaderProgram)
{
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vertexArrayObjectWavefrontModel);
	renderWavefrontModels(p_shaderProgram);
}

void MapRenderer::initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
	std::vector<ObjVertexCoords> vertices = getScaledVerticesFromWavefrontModel();
	std::vector<ObjVertexCoords> normals = getNormalsFromWaveFrontModel();

	m_wavefrontModelVertexBuffer.bind();
	m_wavefrontModelVertexBuffer.allocate(vertices.data(), vertices.size() * sizeof(ObjVertexCoords));

	m_wavefrontModelNormalBuffer.bind();
	m_wavefrontModelNormalBuffer.allocate(normals.data(), normals.size() * sizeof(ObjVertexCoords));

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
}

void MapRenderer::renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
	p_shaderProgram->setUniformValue("light.Position", QVector3D(0.0f, 0.0f, -1.0f));
	p_shaderProgram->setUniformValue("light.AmbientColor", QVector3D(0.0f, 0.3f, 0.0f));
	p_shaderProgram->setUniformValue("light.DiffuseColor", QVector3D(0.0f, 0.3f, 0.0f));
	p_shaderProgram->setUniformValue("light.SpecularColor", QVector3D(0.6f, 0.6f, 0.6f));
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
		
		glDrawElements(GL_TRIANGLES, submeshData.second, GL_UNSIGNED_SHORT, (GLvoid*) (indexOffset * sizeof(unsigned short)));
		indexOffset += submeshData.second;
	}
}

std::vector<ObjVertexCoords> MapRenderer::getScaledVerticesFromWavefrontModel()
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

std::vector<ObjVertexCoords> MapRenderer::getNormalsFromWaveFrontModel()
{
	// Todo: Only one mesh supported yet
	return mc_objLoader.GetMeshes().back()->GetNormals();
}