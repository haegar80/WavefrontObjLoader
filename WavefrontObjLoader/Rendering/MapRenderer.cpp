#include "MapRenderer.h"
#include "../ObjectLoader/ObjLoader.h"
#include <sstream>
#include <QOpenGLWidget>
#include <QtGui/QOpenGLShaderProgram>

MapRenderer::MapRenderer(const ObjLoader& pc_objLoader) :
	mc_objLoader(pc_objLoader)
{
	initializeOpenGLFunctions();

	m_MapPositionArrayBuffer.create();
	m_MapColorArrayBuffer.create();
	m_BackgroundPositionArrayBuffer.create();
	m_WavefrontModelsArrayBuffer.create();
}

MapRenderer::~MapRenderer()
{
	m_MapPositionArrayBuffer.destroy();
	m_BackgroundPositionArrayBuffer.destroy();
	m_WavefrontModelsArrayBuffer.destroy();
}


void MapRenderer::init()
{
	initMap();
	initWavefrontModels();
}

void MapRenderer::render(QOpenGLShaderProgram* p_shaderProgram)
{
	renderWavefrontModels();
	renderMap(p_shaderProgram);
}

void MapRenderer::initMap()
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

	QVector3D defaultColor(0.0f, 0.2f/255.0f, 0.0f);

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

	m_MapPositionArrayBuffer.bind();
	m_MapPositionArrayBuffer.allocate(vertexPositions, 20 * sizeof(QVector3D));

	m_MapColorArrayBuffer.bind();
	m_MapColorArrayBuffer.allocate(vertexColors, 5 * sizeof(QVector3D));
}

void MapRenderer::initWavefrontModels()
{
}

void MapRenderer::renderMap(QOpenGLShaderProgram* p_shaderProgram)
{
	constexpr int PositionOffset = 0;
	constexpr int ColorOffset = sizeof(QVector3D);
	constexpr int PositionSize = 3;

	m_MapPositionArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(0);
	p_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);

	m_MapColorArrayBuffer.bind();
	p_shaderProgram->enableAttributeArray(1);
	p_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3);

	glDrawArrays(GL_POLYGON, 0, 20);
}

void MapRenderer::renderWavefrontModels()
{
	/**
	std::vector<ObjVertexCoords> vertices = mc_objLoader.GetVertices();
	glBegin(GL_POLYGON);
	for(ulong i = 0; i < vertices.size(); i++) {
		glVertex3f(vertices.at(i).X, vertices.at(i).Y, vertices.at(i).Z);
	}
	glEnd();
	*/
}