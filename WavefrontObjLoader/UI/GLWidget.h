#ifndef GLWidget_H
#define GLWidget_H

#include "../ObjectLoader/ObjLoader.h"
#include "../Rendering/MapRenderer.h"
#include <QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/qmatrix4x4.h>

class QOpenGLShaderProgram;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	GLWidget(ObjLoader& p_objLoader, QWidget* p_parent = NULL);
	virtual ~GLWidget();
protected:
	void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);
private:
	struct Hit 
	{
		int id_count, zmin, zmax, id;
	};
	QOpenGLShaderProgram* m_shaderProgram{ nullptr };
	QMatrix4x4 m_projectionMatrix;
	ObjLoader& m_objLoader;
	MapRenderer* m_mapRenderer{ nullptr };
	double m_width;
	double m_height;
	double m_currentZoom;
	double m_translateX;
	double m_translateY;
	double m_cameraAngleX;
	double m_cameraAngleY;
	double m_cameraAngleZ;
	double m_lastXPos;
	double m_lastYPos;
	bool m_mousePressed;
	bool m_moveMap;

	void initShaders();
	void processMovingMap(double p_xPosDelta, double p_yPosDelta);
};

#endif // GLWidget_H