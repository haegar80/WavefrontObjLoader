#include "GLWidget.h"
#include <ctime>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMouseEvent>

GLWidget::GLWidget(ObjLoader& p_objLoader, QWidget* p_parent) : QOpenGLWidget(p_parent),
m_objLoader(p_objLoader)
{
    setMouseTracking(true);
    setAutoFillBackground(false);
}

GLWidget::~GLWidget(void)
{
    makeCurrent();

    delete m_shaderProgram;
    delete m_mapRenderer;

    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

    initShaders();

    m_mapRenderer = new WavefrontRenderer(m_objLoader, width(), height());
    m_mapRenderer->init(m_shaderProgram);
}

void GLWidget::resizeGL(int w, int h)
{
    m_width = static_cast<double>(w);
    m_height = static_cast<double>(h);

    // Viewport
    glViewport(0, 0, w, h);

    // Projektionsmatrix
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(30.0f, static_cast<float>(m_width / m_height), 1.0f, 10000.0f);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Translate object coordinates to model coordinates
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_translateX, m_translateY, m_currentZoom);

    // Viewmatrix
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(0.0f, 0.0f, 0.0f);
    viewMatrix.rotate(m_cameraAngleY, 0.0f, 1.0f, 0.0f);
    viewMatrix.rotate(m_cameraAngleX, 1.0f, 0.0f, 0.0f);
    viewMatrix.rotate(m_cameraAngleZ, 0.0f, 0.0f, 1.0f);

    QMatrix4x4 modelViewMatrix = modelMatrix * viewMatrix;
    m_shaderProgram->setUniformValue("modelview_matrix", modelViewMatrix);
    m_shaderProgram->setUniformValue("projection_matrix", m_projectionMatrix);

    m_mapRenderer->render(m_shaderProgram);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;
    if (Qt::RightButton == event->buttons())
    {
        m_moveMap = true;
    }
    else if ((Qt::LeftButton | Qt::RightButton) == event->buttons())
    {
        m_turnMap = true;
        m_moveMap = true;
    }

    m_lastXPos = event->x();
    m_lastYPos = event->y();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveMap = false;
    m_turnMap = false;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int currentXPos = event->x();
    int currentYPos = event->y();
    if (!m_mousePressed)
    {
        return;
    }

    if (m_turnMap || m_moveMap)
    {
        double deltaX = currentXPos - m_lastXPos;
        double deltaY = currentYPos - m_lastYPos;

        if (m_turnMap)
        {
            processTurningMap(deltaX, deltaY);
        }
        if (m_moveMap)
        {
            processMovingMap(deltaX, deltaY);
        }
    }

    m_lastXPos = currentXPos;
    m_lastYPos = currentYPos;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        m_currentZoom += 200;
    }
    else {
        m_currentZoom -= 200;
    }
    update();
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}

void GLWidget::initShaders()
{
    m_shaderProgram = new QOpenGLShaderProgram{};

    if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/new/prefix1/Resources/vshader.glsl"))
    {
        close();
    }

    if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/new/prefix1/Resources/fshader.glsl"))
    {
        close();
    }

    if (!m_shaderProgram->link())
    {
        close();
    }

    if (!m_shaderProgram->bind())
    {
        close();
    }

    m_shaderProgram->setUniformValue("ambient_map", 0);
    m_shaderProgram->setUniformValue("diffuse_map", 1);
    m_shaderProgram->setUniformValue("specular_map", 2);
}

void GLWidget::processMovingMap(double p_xPosDelta, double p_yPosDelta)
{
    m_translateX += p_xPosDelta;
    m_translateY -= p_yPosDelta;

    update();
}

void GLWidget::processTurningMap(double p_xPosDelta, double p_yPosDelta)
{
    m_cameraAngleX += p_yPosDelta / 8.0;
    m_cameraAngleY += p_xPosDelta / 8.0;

    if (m_cameraAngleX > 360.0)
    {
        m_cameraAngleX -= 360.0;
    }
    else if (m_cameraAngleX < 0.0)
    {
        m_cameraAngleX += 360.0;
    }

    if (m_cameraAngleY > 360.0)
    {
        m_cameraAngleY -= 360.0;
    }
    else if (m_cameraAngleY < 0.0)
    {
        m_cameraAngleY += 360.0;
    }

    update();
}