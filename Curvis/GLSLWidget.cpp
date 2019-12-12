#define _USE_MATH_DEFINES
#define TIME_PERIOD (1000 * 1000)
#define MINSCALE .05
#include "GLSLWidget.h"
#include <glm/gtx/quaternion.hpp>
#include <qapplication.h>
using glm::vec3;
using glm::vec4;
using glm::mat4;

const double PI = float(M_PI);

double g_per_time_radio;
//sensitivity of mouse
#define MOUSE_ROTATED_PARAMETER 0.05
#define WHEEL_ROTATED_PARAMETER 0.0025
static float qNormalizeAngle(float angle)
{
	float new_angle = angle;
	while (new_angle < -360)
		new_angle += 360;
	while (new_angle > 360)
		new_angle -= 360;
	return new_angle;
}

void GLSLWidget::setXRotation(float angle)
{
	if (angle != _xRot) {
		_xRot = angle;
		//emit xRotationChanged(angle);
		update();
	}
}

void GLSLWidget::setYRotation(float angle)
{
	if (angle != _yRot) {
		_yRot = angle;
		//emit yRotationChanged(angle);
		update();
	}
}

void GLSLWidget::setZRotation(float angle)
{
	if (angle != _zRot) {
		_zRot = angle;
		//emit zRotationChanged(angle);
		update();
	}
}
void GLSLWidget::uniformChange() {
	parentWidget()->update();
	update();


}
GLSLWidget::~GLSLWidget()
{
	this->cleanup();
}
QSize GLSLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLSLWidget::sizeHint() const
{
	return QSize(400, 400);
}

GLSLWidget::GLSLWidget(QWidget *parent, Qt::WindowFlags flags) : QOpenGLWidget(parent),
_xRot(0),
_yRot(0),
_zRot(0),
_isTransparent(false) {
	m_scale = glm::mat4(1.0f);
	m_scale = glm::scale(glm::vec3(1.f,1.f,1.f))*m_scale;
	_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
	if (_isTransparent) {
		QSurfaceFormat fmt = format();
		fmt.setAlphaBufferSize(8);
		setFormat(fmt);
	}
}

void GLSLWidget::initializeGL()
{
	OnInit();
}

void GLSLWidget::paintGL()
{
	OnUpdate();
}

void GLSLWidget::resizeGL(int width, int height)
{
	if (0 == height) {
		height = 1;
	}
	m_proj = glm::mat4(1.0f);
	m_proj = glm::perspective(45.0f, (float)width / (float)height, 0.01f, 100.0f);
}

void GLSLWidget::timerEvent(QTimerEvent * timer)
{
	//caculate the periodic time radio
	uint ms = QDateTime::currentMSecsSinceEpoch() - QDateTime(QDate::currentDate()).toMSecsSinceEpoch();	// milliseconds
	ms %= TIME_PERIOD;
	g_per_time_radio = ms * 1.0 / TIME_PERIOD;  // [ 0., 1. )
	update();
}

// interaction
void GLSLWidget::mousePressEvent(QMouseEvent *e)
{
	_lastMousePos = glm::vec3(e->pos().x(), e->pos().y(), 0.0f);
}
void GLSLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == Qt::LeftButton) {
		int dx = -event->x() + _lastMousePos.x;
		int dy = -event->y() + _lastMousePos.y;
		Quaternion Q1(cos((float)dx*.5f*.005f), sin((float)dx*.5f*.005f)*vec3(0.0f, 1.0f, 0.0f));
		Q1.normalize();
		Quaternion Q2(cos((float)dy*.5f*.005f), sin((float)dy*.5f*.005f)*vec3(1.0f, 0.0f, 0.0f));
		Q2.normalize();
		Quaternion Q = Q2 * Q1;
		m_world = Q.matrix() *m_world;
		
	}
	else if (event->buttons() == Qt::RightButton && !(QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))) {
		int dx = event->x() - _lastMousePos.x;
		int dy = -event->y() + _lastMousePos.y;
		m_camera = glm::translate(vec3(.005f*(float)dx, .005f*(float)dy, 0.0f))*m_camera;
	}
	if (event->buttons() == Qt::RightButton && QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier)) {
		int dy = -event->y() + _lastMousePos.y;
		m_camera = glm::translate(vec3(0.0f, 0.0f, .005f*(float)dy))*m_camera;
	}
	_lastMousePos = glm::vec3(event->pos().x(), event->pos().y(), 0.0f);
	update();
}

void GLSLWidget::wheelEvent(QWheelEvent *e) {

	//QPoint numDegrees = e->angleDelta() / 8;
	//float degree = numDegrees.y() * WHEEL_ROTATED_PARAMETER;
	//////qDebug() << degree;
	//QVector3D _cur_cam_pos = _m_camera_position;
	//QVector3D _new_cam_pos = _m_camera_position + (_m_camera_position - QVector3D(0, 0, 0)) * degree;
	//_d_cam_pos = _new_cam_pos - _cur_cam_pos;
	//m_camera.translate(_d_cam_pos);
	update();
}

void GLSLWidget::keyPressEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_Up) {
		//m_world.translate(0, .1, 0);
		m_world = glm::translate(vec3(0.f, 0.1f, 0.0f))* m_world;
	}
	if (e->key() == Qt::Key_Down) {
		m_world = glm::translate(vec3(0.f, -0.1f, 0.0f)) * m_world;

	}
	if (e->key() == Qt::Key_Left) {
		m_world = glm::translate(vec3(-0.1f, 0.0f, 0.0f)) * m_world;
	}
	if (e->key() == Qt::Key_Right) {
		m_world = glm::translate(vec3(0.1f, 0.0f, 0.0f)) * m_world;
	}
	update();
}
