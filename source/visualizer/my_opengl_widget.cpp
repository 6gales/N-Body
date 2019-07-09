#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),

    vertex_buffer(QOpenGLBuffer::VertexBuffer),

//    color_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer),
    mass_buffer(QOpenGLBuffer::VertexBuffer),
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
    timer.start(1);
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);


    initView();
    initProgram();
}
//QVector3D colorFromMass(double mass){ //prototype realization
//    if(mass<1e+30)
//        return QVector3D(1.0f,1.0f,1.0f);
//    if(mass<1e+35)
//        return QVector3D(1.0f,0.5f,0.0f);
//    return QVector3D(1.0f,1.0f,1.0f);
//}
void MyOpenGLWidget::initProgram() {
    std::ifstream in("C://ssd.sccc//N-Body//source//visualizer//in.txt");
    in>>numOfVectors;
    computer = getInstanceOf();

    vertices = new QVector3D[numOfVectors] () ;
//    QVector3D *colors = new QVector3D[numOfVectors] () ;
    float *masses = new float[numOfVectors] () ;
    unsigned int *points = new unsigned int[numOfVectors] ();
    std::shared_ptr<Particle> parts(new Particle[numOfVectors]);

    for(unsigned int i = 0; i <numOfVectors; ++i){
            double mass, x, y, z, vx, vy, vz;
            in >> mass >> x >> y >> z >> vx >> vy >> vz;
//            std::cout << mass<< std::endl;
            parts.get()[i] = Particle(mass, x, y, z, vx, vy, vz);
            masses[i]=static_cast<float>(mass);
//            colors[i]=colorFromMass(mass);
            points[i]=i;
//            std::cout << parts.get()[i] << std::endl;
    }
    in.close();
    computer->init(parts, numOfVectors);

    initShaderProgram(&planet, "shaders/planet.frag", "shaders/planet.vert");
    initShaderProgram(&star, "shaders/basic.frag", "shaders/basic.vert");

    star->bind();
    planet->bind();

    vao.create();
    vao.bind();

    // Load vertex coordinates.
    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(vertices, static_cast<int>((numOfVectors*sizeof(QVector3D))));

    const auto v_loc = star->attributeLocation("vertex");
    star->enableAttributeArray(v_loc);
    star->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);
    const auto vp_loc = planet->attributeLocation("vertex");
    planet->enableAttributeArray(vp_loc);
    planet->setAttributeBuffer(vp_loc, GL_FLOAT, 0, 3);

    mass_buffer.create();
    mass_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    mass_buffer.bind();
    mass_buffer.allocate(masses, static_cast<int>(numOfVectors*sizeof(float)));


    const auto m_loc = star->attributeLocation("mass");
    star->enableAttributeArray(m_loc);
    star->setAttributeBuffer(m_loc, GL_FLOAT, 0,1);
    const auto mp_loc = planet->attributeLocation("mass");
    planet->enableAttributeArray(mp_loc);
    planet->setAttributeBuffer(mp_loc, GL_FLOAT, 0,1);
    // Load points.
    index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(points, static_cast<int>(numOfVectors*sizeof(points)));

    setPalette(makeStarPalette());

    vao.release();
    star->release();

    initShaderProgram(&axisP, "shaders/axis.frag", "shaders/axis.vert");

    axisP->bind();

    lao.create();
    lao.bind();
    const std::vector<QVector3D> xyz = {QVector3D(0.0f,0.0f,0.0f),QVector3D(1.0f,0.0f,0.0f),QVector3D(0.0f,1.0f,0.0f),QVector3D(0.0f,0.0f,1.0f) };

    linev_buffer.create();
    linev_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    linev_buffer.bind();
    linev_buffer.allocate(xyz.data(), static_cast<int>((4*sizeof(QVector3D))));

    const auto va_loc = axisP->attributeLocation("vertex");
    axisP->enableAttributeArray(va_loc);
    axisP->setAttributeBuffer(va_loc, GL_FLOAT, 0, 3);

//    const auto va1_loc = axisP->attributeLocation("vertex1");
//    axisP->enableAttributeArray(va1_loc);
//    axisP->setAttributeBuffer(va1_loc, GL_FLOAT, 0, 3);


    std::vector<int> lin = {0,1,0,2,0,3};
    linex_buffer.create();
    linex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    linex_buffer.bind();
    linex_buffer.allocate(lin.data(), static_cast<int>((3*sizeof(QVector3D))));

    lao.release();
    axisP->release();
    setShaderProgram(1);
    delete [] points;

//    delete [] colors;
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    auto *gl = context()->functions();

    gl->glViewport(0, 0, width, height);

    initView();
}
void MyOpenGLWidget::setPalette(std::vector<QVector3D> palette){
    color_texture.destroy();
    color_texture.setSize(static_cast<int>(palette.size()));
    color_texture.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    color_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    color_texture.setFormat(QOpenGLTexture::RGB8_UNorm);
    color_texture.allocateStorage();
    color_texture.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, palette.data());
}
void MyOpenGLWidget::initShaderProgram(std::shared_ptr<QOpenGLShaderProgram> *shpr, const QString &frag, const QString &vert){
    (*shpr) = std::make_shared<QOpenGLShaderProgram>();
    (*shpr)->addShaderFromSourceFile(QOpenGLShader::Fragment, frag);
    (*shpr)->addShaderFromSourceFile(QOpenGLShader::Vertex, vert);
    (*shpr)->link();
}
void MyOpenGLWidget::setShaderProgram(bool palette){
    if(palette){
        program=star;
    }
    else if(!palette){
        program=planet;
    }
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

    if (!program||!axisP) {
        return;
    }


    axisP->bind();

    view_matrix.setToIdentity();
    const auto center = QVector3D(0.0f, 0.0f, 0.0f);
    const auto up = QVector3D(0.0f, 0.0f, 1.0f);
    view_matrix.lookAt(eye, center, up);

    QMatrix4x4 rotate;
    rotate.rotate(xrotation_angle, QVector3D(1.0f, 0.0f, 0.0f));
    rotate.rotate(yrotation_angle, QVector3D(0.0f, 1.0f, 0.0f));
    rotate.rotate(zrotation_angle, QVector3D(0.0f, 0.0f, 1.0f));

    const auto mvp_loc = axisP->uniformLocation("MVP");
//    program->setUniformValue(mvp_loc, projection_matrix*view_matrix*rotate*model_matrix);
    axisP->setUniformValue(mvp_loc, projection_matrix*view_matrix*rotate*model_matrix);


    lao.bind();
    gl->glEnable(GL_LINE_SMOOTH);
    gl->glLineWidth(3);
    gl->glDrawElements(GL_LINES, static_cast<int>(6), GL_UNSIGNED_INT, nullptr);
    lao.release();

    axisP->release();

    program->bind();
    const auto mvpp_loc = program->uniformLocation("MVP");
    program->setUniformValue(mvpp_loc, projection_matrix*view_matrix*rotate*model_matrix);
    vao.bind();
    const int pal_loc = program->uniformLocation("colorD");
    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(pal_loc, 0);
    color_texture.bind();
    gl->glEnable(GL_PROGRAM_POINT_SIZE);
    gl->glDrawElements(GL_POINTS, static_cast<int>(numOfVectors), GL_UNSIGNED_INT, nullptr);
    color_texture.release();
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
    fromParticleM(computer->iterate());
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
void MyOpenGLWidget::fromParticleM(const Particle *part){
    for(unsigned int i = 0; i<numOfVectors; i++)
        vertices[i]=fromParticle(part[i]);

}
