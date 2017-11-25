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
   	QOpenGLVertexArrayObject m_vertexArrayObject;
	QOpenGLVertexArrayObject m_vertexArrayObjectWavefrontModel;
	QOpenGLBuffer m_mapPositionArrayBuffer;
	QOpenGLBuffer m_mapColorArrayBuffer;
	QOpenGLBuffer m_wavefrontModelArrayBuffer;
	QOpenGLBuffer m_wavefrontModelIndexArrayBuffer;
	QOpenGLBuffer m_wavefrontModelColorArrayBuffer;
	int m_wavefrontModelIndexCount{ 0 };

	void initMap(QOpenGLShaderProgram* p_shaderProgram);
	void initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram);

	void renderMap(QOpenGLShaderProgram* p_shaderProgram);
	void renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram);
};

#endif // MapRenderer_H