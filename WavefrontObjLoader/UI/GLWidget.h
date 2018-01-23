#ifndef GLWidget_H
#define GLWidget_H

#include "../ObjectLoader/ObjLoader.h"
#include "../Rendering/WavefrontRenderer.h"
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
    WavefrontRenderer* m_mapRenderer{ nullptr };
    double m_width{ 0.0 };
    double m_height{ 0.0 };
    double m_currentZoom{ -4000.0 };
    double m_translateX{ -500.0 };
    double m_translateY{ -300.0 };
    double m_cameraAngleX{ 30.0 };
    double m_cameraAngleY{ 0.0 };
    double m_cameraAngleZ{ 0.0 };
    double m_lastXPos{ 0.0 };
    double m_lastYPos{ 0.0 };
    bool m_mousePressed{ false };
    bool m_moveMap{ false };
    bool m_turnMap{ false };

    void initShaders();
    void processMovingMap(double p_xPosDelta, double p_yPosDelta);
    void processTurningMap(double p_xPosDelta, double p_yPosDelta);
};

#endif // GLWidget_H