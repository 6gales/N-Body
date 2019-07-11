#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QTimer>
#include <QImage>
#include <memory>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QEvent>
#include <iostream>
#include <fstream>
#include <ctime>
#include "computer.h"
#include "res.h"

class QOpenGLShaderProgram;

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=nullptr);
    void setPalette(std::vector<QVector3D> palette);
    void setShaderProgram(bool palette);

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;
    virtual void mouseMoveEvent(QMouseEvent *me) override;
    virtual void mousePressEvent(QMouseEvent *me) override;
    virtual void mouseReleaseEvent(QMouseEvent *me) override;
    virtual void wheelEvent(QWheelEvent *qe) override;
    virtual void keyPressEvent(QKeyEvent *ke) override;
signals:
    void initialized();
private:
    void initProgram();
    void initView();
    QVector3D fromParticle(const Particle &part);
    void fromParticleM(const Particle *part);
    void onTimer();
    void initShaderProgram(std::shared_ptr<QOpenGLShaderProgram> *shpr, const QString &frag, const QString &vert);
private:
    std::shared_ptr<QOpenGLShaderProgram> star, planet, axisP, program;

    QOpenGLVertexArrayObject vao, lao;
    QOpenGLBuffer vertex_buffer, /*color_buffer,*/ index_buffer,mass_buffer,linev_buffer, linec_buffer, linex_buffer;
    QVector3D eye=QVector3D(1.0f,0.0f,0.0f);
    std::vector<QVector3D>  vertices;
    QMatrix4x4 model_matrix, view_matrix, projection_matrix;
    QOpenGLTexture color_texture {QOpenGLTexture::Target1D};
    Computer* computer;

    bool pressedButton =false;

    int scoord[2],lcoord[2];
    int large;
    int scale=10;

    float xrotation_angle {0.0f};
    float yrotation_angle {0.0f};
    float zrotation_angle {0.0f};
    QTimer timer;
    unsigned int numOfVectors;
};

