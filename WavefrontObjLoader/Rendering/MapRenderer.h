#ifndef MapRenderer_H
#define MapRenderer_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QVector3D>

class ObjLoader;
class QOpenGLWidget;
class QOpenGLShaderProgram;
class QOpenGLBuffer;

class MapRenderer : protected QOpenGLFunctions
{
public:
	MapRenderer(const ObjLoader& pc_objLoader);
	virtual ~MapRenderer();

	void init();
	void render(QOpenGLShaderProgram* p_shaderProgram);

private:
	struct VertexData
	{
		QVector3D position[4];
		QVector3D color;
	};

	const ObjLoader& mc_objLoader;
	QOpenGLBuffer m_MapPositionArrayBuffer;
	QOpenGLBuffer m_MapColorArrayBuffer;
	QOpenGLBuffer m_BackgroundPositionArrayBuffer;
	QOpenGLBuffer m_WavefrontModelsArrayBuffer;

	void initMap();
	void initWavefrontModels();

	void renderMap(QOpenGLShaderProgram* p_shaderProgram);
	void renderWavefrontModels();
};

#endif // MapRenderer_H