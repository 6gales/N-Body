#pragma once

#include "../client/client.hpp"
#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QTimer>
#include <memory>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QEvent>
#include <iostream>
#include <fstream>
#include <ctime>
#include <atomic>
#include "res.h"
#include "Particle.h"

class QOpenGLShaderProgram;

class MapOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MapOpenGLWidget(QWidget *parent=nullptr);
    void set_client(Client *client);
    void resize_dist_max(bool isStellar);
    ~MapOpenGLWidget() override { timer.stop(); }

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;

private:
    void initProgram();
    void onTimer();
    void setPalette(const std::vector<QVector4D> &palette);
    void fromParticleM(std::vector<Particle> &part);

    QVector4D fromParticle(const Particle &part);
    QVector4D gridCoord(QVector4D coord);
    std::vector<QVector4D> createGrid(QVector4D *vertices,unsigned long long numOfVectors);

private:
    std::shared_ptr<QOpenGLShaderProgram> program;

    Client *client;

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertex_buffer, texcoord_buffer, index_buffer;
    QOpenGLTexture color_texture {QOpenGLTexture::Target2D};
    QVector3D eye=QVector3D(1.0f,0.0f,0.0f);
    std::vector<QVector4D> vertices;
    QMatrix4x4 model_matrix, view_matrix, projection_matrix;
    QTimer timer;

    std::atomic<float> dist_max{1e+26};
    int num_of_indices {0};
    unsigned int shift = 0;
    unsigned long long numOfVectors;
    double massmax=0.0;
};

