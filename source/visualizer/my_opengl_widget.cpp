#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),

    vertex_buffer(QOpenGLBuffer::VertexBuffer),
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

void MyOpenGLWidget::set_client(Client *client) {
    this->client = client;
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);


    initView();
    initProgram();
}
void MyOpenGLWidget::initProgram() {
    numOfVectors = client->get_count();

    vertices.resize(numOfVectors);
    std::vector<float> masses(numOfVectors);
    std::vector<unsigned int> points(numOfVectors);

    std::vector<float> particles_mass = client->get_particles_mass();

    for(unsigned int i = 0; i <numOfVectors; ++i){
            masses[i]=static_cast<float>(particles_mass[i]);
            points[i]=i;
    }

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
    vertex_buffer.allocate(vertices.data(), static_cast<int>((numOfVectors*sizeof(QVector3D))));

    const auto v_loc = star->attributeLocation("vertex");
    star->enableAttributeArray(v_loc);
    star->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);
    const auto vp_loc = planet->attributeLocation("vertex");
    planet->enableAttributeArray(vp_loc);
    planet->setAttributeBuffer(vp_loc, GL_FLOAT, 0, 3);

    mass_buffer.create();
    mass_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    mass_buffer.bind();
    mass_buffer.allocate(masses.data(), static_cast<int>(numOfVectors*sizeof(float)));


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
    index_buffer.allocate(points.data(), static_cast<int>(numOfVectors*sizeof(points)));

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

    std::vector<int> lin = {0,1,0,2,0,3};
    linex_buffer.create();
    linex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    linex_buffer.bind();
    linex_buffer.allocate(lin.data(), static_cast<int>((3*sizeof(QVector3D))));

    lao.release();
    axisP->release();
    setShaderProgram(1);
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
    const auto far_plane = 1e20f;
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
        scale+=1;
    else
        scale-=1;
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
    if (!client) return;
    auto particles = client->get_front_dyn();
    client->pop_front_dyn();
    if (particles.empty()) {
//        update();
        return;
    }
    fromParticleM(particles);
    vao.bind();
    vertex_buffer.bind();
    vertex_buffer.write(NULL,vertices.data(), static_cast<int>(numOfVectors*sizeof(QVector3D)));
    vao.release();
    if (numOfVectors == sh) sh = 0;
    update();
}
QVector3D MyOpenGLWidget::fromParticle(const Particle &part){
    return QVector3D(part.getX()/pow(10,scale),part.getY()/pow(10,scale),part.getZ()/pow(10,scale));
}
void MyOpenGLWidget::fromParticleM(std::vector<Particle> &part){
    for(unsigned int i = 0; i<part.size(); i++)
        vertices[i+sh]=fromParticle(part[i]);
    sh += part.size();
}
