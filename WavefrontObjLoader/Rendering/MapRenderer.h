#ifndef MapRenderer_H
#define MapRenderer_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QVector3D>
#include "../ObjectLoader/ObjLoader.h"

class ObjLoader;
class QOpenGLWidget;
class QOpenGLShaderProgram;

class MapRenderer : protected QOpenGLFunctions
{
public:
	MapRenderer(const ObjLoader& pc_objLoader, int p_width, int p_height);
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
	QOpenGLBuffer m_wavefrontModelVertexBuffer;
	QOpenGLBuffer m_wavefrontModelNormalBuffer;
	QOpenGLBuffer m_wavefrontModelIndexArrayBuffer;
	int m_width{ 0 };
	int m_height{ 0 };
	std::map<SubMesh*, std::vector<unsigned short>> m_subMeshIndices;

	void initMap(QOpenGLShaderProgram* p_shaderProgram);
	void initWavefrontModels(QOpenGLShaderProgram* p_shaderProgram);

	void renderMap(QOpenGLShaderProgram* p_shaderProgram);
	void renderWavefrontModels(QOpenGLShaderProgram* p_shaderProgram);

	std::vector<ObjVertexCoords> getScaledVerticesFromWavefrontModel();
	std::vector<ObjVertexCoords> getNormals();
};

#endif // MapRenderer_H