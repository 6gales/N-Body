#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer)
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

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);

    initView();
    initProgram();
}
QVector3D colorFromMass(unsigned int mass){ //prototype realization
    if(mass<1500)
        return QVector3D(1.0f,1.0f,1.0f);
    if(mass<3000)
        return QVector3D(1.0f,0.5f,0.0f);
    if(mass<4500)
        return QVector3D(1.0f,0.0f,0.0f);
    if(mass<6000)
        return QVector3D(0.5f,0.0f,0.5f);
    if(mass<8500)
        return QVector3D(0.0f,0.0f,1.0f);
    return QVector3D(1.0f,1.0f,1.0f);
}
void MyOpenGLWidget::initProgram() {
    float a[3];
    numOfVectors=120000;
    srand(time(0));

    vertices = new QVector3D[numOfVectors] () ;
    speed = new QVector3D[numOfVectors] () ;

    QVector3D *colors = new QVector3D[numOfVectors] () ;
    unsigned int *mass = new unsigned int[numOfVectors] ();
    unsigned int *points = new unsigned int[numOfVectors] ();

    for(unsigned int i =0; i<numOfVectors; i++){ //some data creating
        for(int j =0; j<3; j++){
            a[j]=(float)(rand()/(RAND_MAX/1000.0)-500.0);
        }
        vertices[i]=QVector3D(a[0],a[1],a[2]);
        speed[i]=QVector3D(a[0]/50,a[1]/50,a[2]/50);
        mass[i]=(float)(rand()/(RAND_MAX/10000));

        colors[i]=colorFromMass(mass[i]);
        points[i]=i;
    }

    num_of_indices = static_cast<int>(numOfVectors);
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
    program->release();
    delete [] mass;
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

    // Apply rotation.
    view_matrix.setToIdentity();
    const auto center = QVector3D(0.0f, 0.0f, 0.0f);
    const auto up = QVector3D(0.0f, 1.0f, 0.0f);
    view_matrix.lookAt(eye, center, up);

    QMatrix4x4 rotate;

    rotate.rotate(xrotation_angle, QVector3D(1.0f, 0.0f, 0.0f));
    rotate.rotate(yrotation_angle, QVector3D(0.0f, 1.0f, 0.0f));
    const auto mvp_loc = program->uniformLocation("MVP");
    program->setUniformValue(mvp_loc, projection_matrix*view_matrix*rotate*model_matrix);
    const auto eye_loc = program->uniformLocation("eye");
    program->setUniformValue(eye_loc, eye);

    // Draw the cube with triangles.
    vao.bind();
    gl->glEnable(GL_PROGRAM_POINT_SIZE);
    gl->glDrawElements(GL_POINTS, num_of_indices, GL_UNSIGNED_INT, nullptr);
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
        eye+=QVector3D(5.0f,5.0f,5.0f);
    else
        eye-=QVector3D(5.0f,5.0f,5.0f);
    update();

}

void MyOpenGLWidget::onTimer() {
    for(unsigned int i =0; i<numOfVectors; i++){
        vertices[i]+=speed[i];
    }
    vao.bind();
    vertex_buffer.bind();
    vertex_buffer.write(NULL,vertices, static_cast<int>(numOfVectors*sizeof(QVector3D)));
    vao.release();
    update();
}
MyOpenGLWidget::~MyOpenGLWidget(){
    delete[] vertices;
    delete[] speed;
}
