#include "GLWidget.h"
#include <ctime>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMouseEvent>

GLWidget::GLWidget(ObjLoader& p_objLoader, QWidget* p_parent) : QOpenGLWidget(p_parent),
    m_objLoader(p_objLoader),
	m_width(0.0),
	m_height(0.0),
	m_currentZoom(-2100.0),
	m_translateX(-500.0),
	m_translateY(-300.0),
	m_cameraAngleX(-30.0),
	m_cameraAngleY(0.0),
	m_cameraAngleZ(0.0),
	m_lastXPos(0.0),
	m_lastYPos(0.0),
	m_mousePressed(false),
	m_moveMap(false)
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

	m_mapRenderer = new MapRenderer(m_objLoader, width(), height());
	m_mapRenderer->init(m_shaderProgram);

	// Default values
	m_shaderProgram->setUniformValue("material.Ka", QVector3D(1.0f, 1.0f, 1.0f));
	m_shaderProgram->setUniformValue("material.Kd", QVector3D(1.0f, 1.0f, 1.0f));
	m_shaderProgram->setUniformValue("material.Ks", QVector3D(1.0f, 1.0f, 1.0f));
	m_shaderProgram->setUniformValue("material.Shininess", 50.f);
	m_shaderProgram->setUniformValue("light.Position", QVector3D(0.0f, 0.0f, 1.0f));
	m_shaderProgram->setUniformValue("light.AmbientColor", QVector3D(0.15f, 0.15f, 0.15f));
	m_shaderProgram->setUniformValue("light.DiffuseColor", QVector3D(1.0f, 1.0f, 1.0f));
	m_shaderProgram->setUniformValue("light.SpecularColor", QVector3D(1.0f, 1.0f, 1.0f));
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
	viewMatrix.rotate(m_cameraAngleY, 0.0, 1.0, 0.0);
	viewMatrix.rotate(m_cameraAngleX, 1.0, 0.0, 0.0);
	viewMatrix.rotate(m_cameraAngleZ, 0.0, 0.0, 1.0);

	QMatrix4x4 modelViewMatrix = modelMatrix * viewMatrix;
	m_shaderProgram->setUniformValue("modelview_matrix", modelViewMatrix);
	m_shaderProgram->setUniformValue("projection_matrix", m_projectionMatrix);

	m_mapRenderer->render(m_shaderProgram);
}

void GLWidget::mousePressEvent(QMouseEvent *event) 
{
	m_mousePressed = true;
	if(Qt::RightButton == event->buttons()) {
		m_moveMap = true;
	}
	//processFindingObject(event->x(), m_height - event->y());
	m_lastXPos = event->x();
	m_lastYPos = event->y();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) 
{
	m_mousePressed = false;
	m_moveMap = false;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) 
{
	int currentXPos = event->x();
	int currentYPos = event->y();
	if(!m_mousePressed) {
		return;
	}
	if(Qt::RightButton == event->buttons()) {
		double deltaX = currentXPos - m_lastXPos;
		double deltaY = currentYPos - m_lastYPos;
	
		if (m_moveMap) {
			processMovingMap(deltaX, deltaY);
		}
	}
    m_lastXPos = currentXPos;
	m_lastYPos = currentYPos;
}

void GLWidget::wheelEvent(QWheelEvent *event) 
{
	if(event->delta() > 0) {
		m_currentZoom -= 200;
	}
	else {
		m_currentZoom += 200;
	}
	update();
}

void GLWidget::keyPressEvent(QKeyEvent* event) 
{
    switch(event->key()) {
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

	m_shaderProgram->bindAttributeLocation("in_Position", 0);

	if (!m_shaderProgram->link())
	{
		close();
	}

	if (!m_shaderProgram->bind())
	{
		close();
	}
}

void GLWidget::processMovingMap(double p_xPosDelta, double p_yPosDelta)
{
	m_translateX += p_xPosDelta;
	m_translateY -= p_yPosDelta;

	update();
}