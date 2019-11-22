#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include <string>
#include <tuple>  
#include <memory>
#include <array>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <map>

#include <OpenGLFiles\glm-0.9.9-a2\glm\mat4x4.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\glm.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\gtc\matrix_transform.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\gtc\type_ptr.hpp>
#include <OpenGLFiles\glm-0.9.9-a2\glm\gtx\rotate_vector.hpp>

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

/*
* =====================================================================================
*
*       Filename:
*
*    Description:	Moved from JT's QtOGL solution.
*
*        Version:	1.0
*        Created:	12/17/2018
*       Compiler:	VS2017
*			Note:	This uses the QOpenGLWidget class not QGLWidget (which is no longer supported)
*
*         Author:	JT, Josiah B.
*
* =====================================================================================
*/

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
using std::vector;
using std::tuple;
using std::string;
class GLSLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	GLSLWidget(QWidget *parent, Qt::WindowFlags flags = 0);
	~GLSLWidget();
	bool isTransparent() { return _isTransparent; }
	void setTransparent(bool t) { _isTransparent = t; }

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;
	inline float FrameRatio() {

		return this->width() / this->height();
	}
public slots:
	void setXRotation(float angle);
	void setYRotation(float angle);
	void setZRotation(float angle);
	void uniformChange();
	virtual void cleanup() {};
signals:
	//void xRotationChanged(int angle);
	//void yRotationChanged(int angle);
	//void zRotationChanged(int angle);
protected:
	virtual void OnUpdate() {}
	virtual void OnInit() {}
	virtual void setupVertexAttribs() {}
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;
	void timerEvent(QTimerEvent * timer);
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent * e);
	void keyPressEvent(QKeyEvent *e) override;

	QOpenGLShaderProgram*		_program;
	bool _core;
	int  _xRot;
	int  _yRot;
	int  _zRot;
	QPoint	_lastMousePos;
	int _projMatrixLoc;
	int _mvMatrixLoc;
	int _normalMatrixLoc;
	int _lightPosLoc;
	QVector3D _m_camera_position;
	QVector3D _m_camera_up;
	QVector3D _d_cam_pos;
	QMatrix4x4 m_proj;
	QMatrix4x4 m_camera;
	QMatrix4x4 m_world;
	bool _isTransparent;

};
