#include "MapRenderer.h"
#include "../ObjectLoader/ObjLoader.h"
#include <QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>

MapRenderer::MapRenderer(const ObjLoader& pc_objLoader) :
	mc_objLoader(pc_objLoader),
	m_wavefrontModelIndexArrayBuffer(QOpenGLBuffer::IndexBuffer)
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
	float size = static_cast<float>(1000);

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
	std::vector<ObjVertexCoords> vertices = mc_objLoader.GetVertices();

	m_wavefrontModelArrayBuffer.bind();
	m_wavefrontModelArrayBuffer.allocate(vertices.data(), vertices.size() * sizeof(ObjVertexCoords));

	std::vector<unsigned short> indices;

	for each (ObjFace face in faces)
	{
		for each(unsigned short index in face.Vertices)
		{
			indices.push_back(index);
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
	glDrawElements(GL_LINE_STRIP, m_wavefrontModelIndexCount, GL_UNSIGNED_SHORT, 0);
}