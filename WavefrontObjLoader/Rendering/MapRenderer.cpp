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

	m_vertexArrayObject.create();
	m_vertexArrayObjectWavefrontModel.create();
	m_mapPositionArrayBuffer.create();
	m_wavefrontModelArrayBuffer.create();
	m_wavefrontModelIndexArrayBuffer.create();
}

MapRenderer::~MapRenderer()
{
	m_vertexArrayObject.destroy();
	m_vertexArrayObjectWavefrontModel.destroy();
	m_mapPositionArrayBuffer.destroy();
	m_wavefrontModelArrayBuffer.destroy();
	m_wavefrontModelIndexArrayBuffer.destroy();
}


void MapRenderer::init(QOpenGLShaderProgram* p_shaderProgram)
{
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vertexArrayObject);
	initMap(p_shaderProgram);
	QOpenGLVertexArrayObject::Binder vaoBinderWavefrontModel(&m_vertexArrayObjectWavefrontModel);
	initWavefrontModels(p_shaderProgram);
}

void MapRenderer::render(QOpenGLShaderProgram* p_shaderProgram)
{
	QOpenGLVertexArrayObject::Binder vaoBinder(&m_vertexArrayObject);
	renderMap(p_shaderProgram);
	QOpenGLVertexArrayObject::Binder vaoBinderWavefrontModel(&m_vertexArrayObjectWavefrontModel);
	renderWavefrontModels(p_shaderProgram);
}

void MapRenderer::initMap(QOpenGLShaderProgram* p_shaderProgram)
{
	float size = static_cast<float>(m_height);

	// front
	QVector3D frontPositions[4] = {
		QVector3D(size, 0.0f, -10.0f),
		QVector3D(size, size, -10.0f),
		QVector3D(0.0f, size, -10.0f),
		QVector3D(0.0f, 0.0f, -10.0f)
	};

	// top
	QVector3D topPositions[4] = {
		QVector3D(size, size, -10.0f),
		QVector3D(size, size, -100.0f),
		QVector3D(0.0f, size, -100.0f),
		QVector3D(0.0f, size, -10.0f)
	};

	// bottom
	QVector3D bottomPositions[4] = {
		QVector3D(0.0f, 0.0f, -10.0f),
		QVector3D(0.0f, 0.0f, -100.0f),
		QVector3D(size, 0.0f, -100.0f),
		QVector3D(size, 0.0f, -10.0f)
	};

	// right
	QVector3D rightPositions[4] = {
		QVector3D(size, 0.0f, -10.0f),
		QVector3D(size, 0.0f, -100.0f),
		QVector3D(size, size, -100.0f),
		QVector3D(size, size, -10.0f)
	};

	// left
	QVector3D leftPositions[4] = {
		QVector3D(0.0f, size, -10.0f),
		QVector3D(0.0f, size, -100.0f),
		QVector3D(0.0f, 0.0f, -100.0f),
		QVector3D(0.0f, 0.0f, -10.0f)
	};

	QVector3D vertexPositions[5][4] = {
		{ frontPositions[0], frontPositions[1], frontPositions[2], frontPositions[3] },
		{ topPositions[0], topPositions[1], topPositions[2], topPositions[3] },
		{ bottomPositions[0], bottomPositions[1], bottomPositions[2], bottomPositions[3] },
		{ rightPositions[0], rightPositions[1], rightPositions[2], rightPositions[3] },
		{ leftPositions[0], leftPositions[1], leftPositions[2], leftPositions[3] }
	};

	m_mapPositionArrayBuffer.bind();
	m_mapPositionArrayBuffer.allocate(vertexPositions, 20 * sizeof(QVector3D));

	m_mapPositionArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(0);
	p_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);
}

void MapRenderer::initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
	std::vector<ObjVertexCoords> vertices = getScaledVerticesFromWavefrontModel();

	m_wavefrontModelArrayBuffer.bind();
	m_wavefrontModelArrayBuffer.allocate(vertices.data(), vertices.size() * sizeof(ObjVertexCoords));

	std::vector<Mesh*> meshes = mc_objLoader.GetMeshes();
	std::vector<unsigned short> vertexIndices;

	for each(Mesh* mesh in meshes)
	{
		for each(SubMesh* submesh in mesh->GetSubmeshes())
		{
			for each(ObjFace face in submesh->GetFaces())
			{
				for each(ObjFaceIndices index in face.Indices)
				{
					vertexIndices.push_back(index.VertexIndex);
				}
			}
		}
	}
	
	m_wavefrontModelIndexCount = vertexIndices.size();
	
	m_wavefrontModelIndexArrayBuffer.bind();
	m_wavefrontModelIndexArrayBuffer.allocate(vertexIndices.data(), m_wavefrontModelIndexCount * sizeof(unsigned short));

	m_wavefrontModelArrayBuffer.bind();
	m_wavefrontModelIndexArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(0);
	p_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);
}

void MapRenderer::renderMap(QOpenGLShaderProgram* p_shaderProgram)
{
	QVector3D frontColor(0.0f, 0.3f, 0.0f);
	p_shaderProgram->setUniformValue("color", frontColor);

	glDrawArrays(GL_POLYGON, 0, 20);
}

void MapRenderer::renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
	QVector3D defaultColor(0.0f, 0.0f, 0.4f);
	p_shaderProgram->setUniformValue("color", defaultColor);

	glDrawElements(GL_TRIANGLES, m_wavefrontModelIndexCount, GL_UNSIGNED_SHORT, 0);
}

std::vector<ObjVertexCoords> MapRenderer::getScaledVerticesFromWavefrontModel()
{
	std::vector<ObjVertexCoords> scaledVertices;
	for each(Mesh* mesh in mc_objLoader.GetMeshes()) {
		std::vector<ObjVertexCoords> vertices = mesh->GetVertices();

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

	}
	return scaledVertices;
}