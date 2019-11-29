#define _USE_MATH_DEFINES
#define TIME_PERIOD (1000 * 1000)
#define MINSCALE .05
#include "GLSLWidget.h"
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
	m_scale.setToIdentity();
	m_scale.scale(1.f, 1.f, 1.f);
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
	m_proj.setToIdentity();
	m_proj.perspective(45.0f, GLfloat(width) / height, 0.01f, 100.0f);
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
	_lastMousePos = e->pos();
}
void GLSLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - _lastMousePos.x();
	int dy = event->y() - _lastMousePos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(_xRot + 8 * dy);
		setYRotation(_yRot - 8 * dx);
	}
	else if (event->buttons() & Qt::RightButton) {
		setXRotation(_xRot + 8 * dy);
		setZRotation(_zRot + 8 * dx);
	}
	_lastMousePos = event->pos();
	_xRot = qNormalizeAngle(_xRot);
	_yRot = qNormalizeAngle(_yRot);
	_zRot = qNormalizeAngle(_zRot);
	m_world.rotate(_xRot / 16.0f, 1, 0, 0);
	m_world.rotate(-_yRot / 16.0f, 0, 1, 0);//track ball to get rotate axis and angle
	m_world.rotate(_zRot / 16.0f, 0, 0, 1);//float _r = 1.0;
	_xRot = 0;
	_yRot = 0;
	_zRot = 0;
	update();
}

void GLSLWidget::wheelEvent(QWheelEvent *e) {

	QPoint numDegrees = e->angleDelta() / 8;
	float degree = numDegrees.y() * WHEEL_ROTATED_PARAMETER;
	////qDebug() << degree;
	QVector3D _cur_cam_pos = _m_camera_position;
	QVector3D _new_cam_pos = _m_camera_position + (_m_camera_position - QVector3D(0, 0, 0)) * degree;
	_d_cam_pos = _new_cam_pos - _cur_cam_pos;
	m_camera.translate(_d_cam_pos);
	update();
}

void GLSLWidget::keyPressEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_Up) {
		m_world.translate(0, .1, 0);
	}
	if (e->key() == Qt::Key_Down) {
		m_world.translate(0, -.1, 0);

	}
	if (e->key() == Qt::Key_Left) {
		m_world.translate(-.1, 0, 0);
	}
	if (e->key() == Qt::Key_Right) {
		m_world.translate(.1, 0, 0);
	}
	update();
}
