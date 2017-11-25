#ifndef MapRenderer_H
#define MapRenderer_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QVector3D>

class ObjLoader;
class QOpenGLWidget;
class QOpenGLShaderProgram;

class MapRenderer : protected QOpenGLFunctions
{
public:
	MapRenderer(const ObjLoader& pc_objLoader);
	virtual ~MapRenderer();

	void init(QOpenGLShaderProgram* p_shaderProgram);
	void render(QOpenGLShaderProgram* p_shaderProgram);

private:
	struct VertexData
	{
		QVector3D position[4];
		QVector3D color;
	};

	const ObjLoader& mc_objLoader;
   	QOpenGLVertexArrayObject m_VertexArrayObject;
	QOpenGLBuffer m_MapPositionArrayBuffer;
	QOpenGLBuffer m_MapColorArrayBuffer;
	QOpenGLBuffer m_BackgroundPositionArrayBuffer;
	QOpenGLBuffer m_WavefrontModelsArrayBuffer;

	void initMap(QOpenGLShaderProgram* p_shaderProgram);
	void initWavefrontModels();

	void renderMap(QOpenGLShaderProgram* p_shaderProgram);
	void renderWavefrontModels();
};

#endif // MapRenderer_H