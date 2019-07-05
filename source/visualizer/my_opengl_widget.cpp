#include "../client/client.hpp"
#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer),
    linev_buffer(QOpenGLBuffer::VertexBuffer),
    linec_buffer(QOpenGLBuffer::VertexBuffer),
    linex_buffer(QOpenGLBuffer::IndexBuffer)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    connect(&timer, &QTimer::timeout, this, &MyOpenGLWidget::onTimer);
    timer.start(60);
}

void MyOpenGLWidget::set_client(std::shared_ptr<Client> cl, std::ifstream &file) {
    client = cl;
    client->start(file);
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);

    initView();
    initProgram();
}
QVector3D colorFromMass(double mass){ //prototype realization
    if(mass<1e+29)
        return QVector3D(1.0f,1.0f,1.0f);
    if(mass<1e+32)
        return QVector3D(1.0f,0.5f,0.0f);
    if(mass<4e+40)
        return QVector3D(1.0f,0.0f,0.0f);
    if(mass<1e+70)
        return QVector3D(0.5f,0.0f,0.5f);
    if(mass<1e+90)
        return QVector3D(0.0f,0.0f,1.0f);
    return QVector3D(1.0f,1.0f,1.0f);
}
void MyOpenGLWidget::initProgram() {
//    std::ifstream in("C://ssd.sccc//N-Body//source//visualizer//in.txt");
//    in>>numOfVectors;
//    computer = getInstanceOf();

    vertices = new QVector3D[numOfVectors] () ;
    QVector3D *colors = new QVector3D[numOfVectors] () ;
    unsigned int *points = new unsigned int[numOfVectors] ();
    std::vector<Particle> parts{numOfVectors};

//    for(unsigned int i = 0; i <numOfVectors; ++i){
//            double mass, x, y, z, vx, vy, vz;
//            in >> mass >> x >> y >> z >> vx >> vy >> vz;
//            std::cout << mass<< std::endl;
//            parts[i] = Particle(mass, x, y, z, vx, vy, vz);
//            colors[i]=colorFromMass(mass);
//            points[i]=i;
////            std::cout << parts[i] << std::endl;
//    }
//    in.close();
//    computer->init(parts, numOfVectors);
    program = std::make_shared<QOpenGLShaderProgram>();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/basic.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/basic.frag");
    program->link();
    program->bind();

    vao.create();
    vao.bind();

    // Load vertex coordinates.
    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(vertices, static_cast<int>((numOfVectors*sizeof(QVector3D))));

    const auto v_loc = program->attributeLocation("vertex");
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);

    // Load vertex colors.
    color_buffer.create();
    color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    color_buffer.bind();
    color_buffer.allocate(colors, static_cast<int>(numOfVectors*sizeof(QVector3D)));

    const auto c_loc = program->attributeLocation("color");
    program->enableAttributeArray(c_loc);
    program->setAttributeBuffer(c_loc, GL_FLOAT, 0, 3);

    // Load points.
    index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(points, static_cast<int>(numOfVectors*sizeof(points)));

    vao.release();

    lao.create();
    lao.bind();
    const std::vector<QVector3D> xyz = {QVector3D(0.0f,0.0f,0.0f),QVector3D(1.0f,0.0f,0.0f),QVector3D(0.0f,1.0f,0.0f),QVector3D(0.0f,0.0f,1.0f) };

    linev_buffer.create();
    linev_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    linev_buffer.bind();
    linev_buffer.allocate(xyz.data(), static_cast<int>((4*sizeof(QVector3D))));
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);

    linec_buffer.create();
    linec_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    linec_buffer.bind();
    linec_buffer.allocate(xyz.data(), static_cast<int>((4*sizeof(QVector3D))));
    program->enableAttributeArray(c_loc);
    program->setAttributeBuffer(c_loc, GL_FLOAT, 0, 3);

    std::vector<int> lin = {0,1,0,2,0,3};

    linex_buffer.create();
    linex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    linex_buffer.bind();
    linex_buffer.allocate(lin.data(), static_cast<int>((3*sizeof(QVector3D))));

    lao.release();
    program->release();
    delete [] points;
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    auto *gl = context()->functions();

    gl->glViewport(0, 0, width, height);

    initView();
}

void MyOpenGLWidget::initView() {
    model_matrix.setToIdentity();
    projection_matrix.setToIdentity();
    const auto angle = 45.0f;
    const auto aspect = float(width())/height();
    const auto near_plane = 0.01f;
    const auto far_plane = 10000.0f;
    projection_matrix.perspective(angle, aspect, near_plane, far_plane);
}

void MyOpenGLWidget::paintGL() {
    auto *gl = context()->functions();

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program) {
        return;
    }

    program->bind();

    view_matrix.setToIdentity();
    const auto center = QVector3D(0.0f, 0.0f, 0.0f);
    const auto up = QVector3D(0.0f, 0.0f, 1.0f);
    view_matrix.lookAt(eye, center, up);

    QMatrix4x4 rotate;
    rotate.rotate(xrotation_angle, QVector3D(1.0f, 0.0f, 0.0f));
    rotate.rotate(yrotation_angle, QVector3D(0.0f, 1.0f, 0.0f));
    rotate.rotate(zrotation_angle, QVector3D(0.0f, 0.0f, 1.0f));

    const auto mvp_loc = program->uniformLocation("MVP");
    program->setUniformValue(mvp_loc, projection_matrix*view_matrix*rotate*model_matrix);
    const auto eye_loc = program->uniformLocation("eye");
    program->setUniformValue(eye_loc, eye);

    lao.bind();
    gl->glEnable(GL_LINE_SMOOTH);
    gl->glLineWidth(3);
    gl->glDrawElements(GL_LINES, static_cast<int>(6), GL_UNSIGNED_INT, nullptr);
    lao.release();

    vao.bind();
    gl->glEnable(GL_PROGRAM_POINT_SIZE);
    gl->glDrawElements(GL_POINTS, static_cast<int>(numOfVectors), GL_UNSIGNED_INT, nullptr);
    vao.release();

    program->release();
}
void MyOpenGLWidget::mousePressEvent(QMouseEvent *me){
    if(me->button()==Qt::LeftButton){
        pressedButton=true;
        scoord[0]=me->x();
        scoord[1]=me->y();
    }
}
void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *me){
    pressedButton=false;
}
void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *me){
    if(pressedButton){
        lcoord[0]=me->x();
        lcoord[1]=me->y();

        xrotation_angle += 720.0f*float(lcoord[0]-scoord[0])/size().width();
        yrotation_angle += 720.0f*float(lcoord[1]-scoord[1])/size().height();

        scoord[0]=me->x();
        scoord[1]=me->y();

        timer.start();
        update();
    }
}
void MyOpenGLWidget::wheelEvent(QWheelEvent *qe){
    if(qe->angleDelta().ry()>0)
        eye+=QVector3D(20.0f,0.0f,0.0f);
    else
        eye-=QVector3D(20.0f,0.0f,0.0f);
    update();

}
void MyOpenGLWidget::keyPressEvent(QKeyEvent *ke){
    if(ke->key()==Qt::Key_1){
        xrotation_angle = 0.0f;
        yrotation_angle = 90.0f;
        zrotation_angle = 0.0f;
        update();
    }
    if(ke->key()==Qt::Key_2){
        xrotation_angle = 0.0f;
        yrotation_angle = 0.0f;
        zrotation_angle = -90.0f;
        update();
    }
    if(ke->key()==Qt::Key_3){
        xrotation_angle = 0.0f;
        yrotation_angle = 0.0f;
        zrotation_angle = 0.0f;
        update();
    }
}
void MyOpenGLWidget::onTimer() {
    auto particles = client->get_front();
    client->pop_front();
    if (particles.empty()) return;
    fromParticleM(particles);
    vao.bind();
    vertex_buffer.bind();
    vertex_buffer.write(NULL,vertices, static_cast<int>(numOfVectors*sizeof(QVector3D)));
    vao.release();
    update();
}
MyOpenGLWidget::~MyOpenGLWidget(){
    delete[] vertices;
}
QVector3D MyOpenGLWidget::fromParticle(const Particle &part){
    return QVector3D(part.getX()/1.0e+9,part.getY()/1.0e+9,part.getZ()/1.0e+9);
}
void MyOpenGLWidget::fromParticleM(std::vector<Particle> &parts){
    for(unsigned int i = 0; i<numOfVectors; i++)
        vertices[i]=fromParticle(parts[i]);

}
