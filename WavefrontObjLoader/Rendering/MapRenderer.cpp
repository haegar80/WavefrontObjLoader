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
	m_mapColorArrayBuffer.create();
	m_wavefrontModelArrayBuffer.create();
	m_wavefrontModelIndexArrayBuffer.create();
	m_wavefrontModelColorArrayBuffer.create();
}

MapRenderer::~MapRenderer()
{
	m_vertexArrayObject.destroy();
	m_vertexArrayObjectWavefrontModel.destroy();
	m_mapPositionArrayBuffer.destroy();
	m_mapColorArrayBuffer.destroy();
	m_wavefrontModelArrayBuffer.destroy();
	m_wavefrontModelIndexArrayBuffer.destroy();
	m_wavefrontModelColorArrayBuffer.destroy();
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

	QVector3D frontColor(0.0f, 0.45f, 0.0f);

	// front
	QVector3D frontPositions[4] = {
		QVector3D(size, 0.0f, -10.0f),
		QVector3D(size, size, -10.0f),
		QVector3D(0.0f, size, -10.0f),
		QVector3D(0.0f, 0.0f, -10.0f)
	};

	QVector3D defaultColor(0.0f, 0.2f, 0.0f);

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

	QVector3D vertexColors[] = {
		frontColor, defaultColor, defaultColor, defaultColor, defaultColor
	};

	m_mapPositionArrayBuffer.bind();
	m_mapPositionArrayBuffer.allocate(vertexPositions, 20 * sizeof(QVector3D));

	m_mapColorArrayBuffer.bind();
	m_mapColorArrayBuffer.allocate(vertexColors, 5 * sizeof(QVector3D));

	m_mapPositionArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(0);
	p_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);

	m_mapColorArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(1);
	p_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3);
}

void MapRenderer::initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
	std::vector<ObjFace> faces = mc_objLoader.GetFaces();
	std::vector<ObjVertexCoords> vertices = getScaledVerticesFromWavefrontModel();

	m_wavefrontModelArrayBuffer.bind();
	m_wavefrontModelArrayBuffer.allocate(vertices.data(), vertices.size() * sizeof(ObjVertexCoords));

	std::vector<unsigned short> indices;

	for each (ObjFace face in faces)
	{
		for each(ObjFaceIndices index in face.Indices)
		{
			indices.push_back(index.VertexIndex);
		}
	}
	m_wavefrontModelIndexCount = indices.size();
	
	m_wavefrontModelIndexArrayBuffer.bind();
	m_wavefrontModelIndexArrayBuffer.allocate(indices.data(), m_wavefrontModelIndexCount * sizeof(unsigned short));

	QVector3D wavefrontModelColors(0.5f, 0.2f, 0.1f);
	m_wavefrontModelColorArrayBuffer.bind();
	m_wavefrontModelColorArrayBuffer.allocate(&wavefrontModelColors, sizeof(QVector3D));

	m_wavefrontModelArrayBuffer.bind();
	m_wavefrontModelIndexArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(0);
	p_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);

	m_wavefrontModelColorArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(1);
	p_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3);
}

void MapRenderer::renderMap(QOpenGLShaderProgram* p_shaderProgram)
{
	glDrawArrays(GL_POLYGON, 0, 20);
}

void MapRenderer::renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram)
{
	glDrawElements(GL_TRIANGLES, m_wavefrontModelIndexCount, GL_UNSIGNED_SHORT, 0);
}

std::vector<ObjVertexCoords> MapRenderer::getScaledVerticesFromWavefrontModel()
{
	std::vector<ObjVertexCoords> vertices = mc_objLoader.GetVertices();

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

	std::vector<ObjVertexCoords> scaledVertices;

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