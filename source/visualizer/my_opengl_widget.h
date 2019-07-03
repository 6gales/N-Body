#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QTimer>
#include <memory>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QEvent>
#include <iostream>
#include <fstream>
#include <ctime>

class QOpenGLShaderProgram;

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=nullptr);
    ~MyOpenGLWidget() override;

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;
    virtual void mouseMoveEvent(QMouseEvent *me) override;
    virtual void mousePressEvent(QMouseEvent *me) override;
    virtual void wheelEvent(QWheelEvent *qe) override;

private:
    void initProgram();
    void initView();
    void onTimer();

private:
    std::shared_ptr<QOpenGLShaderProgram> program;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertex_buffer, color_buffer, index_buffer;
    QVector3D eye=QVector3D(4.0f,4.0f,4.0f);
    QVector3D * vertices,*speed;
    QMatrix4x4 model_matrix, view_matrix, projection_matrix;
    bool pressedButton =false;
    int scoord[2],lcoord[2];
    int large;
    float xrotation_angle {0.0f};
    float yrotation_angle {0.0f};
    QTimer timer;
    int num_of_indices {0};
    unsigned int numOfVectors;
};

