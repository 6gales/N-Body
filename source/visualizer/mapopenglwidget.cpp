#define DIST_MAX 1e+26
#include "mapopenglwidget.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MapOpenGLWidget::MapOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    texcoord_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    connect(&timer, &QTimer::timeout, this, &MapOpenGLWidget::onTimer);
    timer.start(1);
}

void MapOpenGLWidget::set_client(Client *client) {
    this->client = client;
    client->setIsMap(true);
}

void MapOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);

    initProgram();
}
void MapOpenGLWidget::initProgram() {
    numOfVectors = client->get_count();

    vertices.resize(numOfVectors);

    std::vector<float> particles_mass = client->get_particles_mass();
    std::vector<Particle> parts = client->get_first_particles();

    for(unsigned int i = 0; i <numOfVectors; ++i) {
//            double mass, x, y, z, vx, vy, vz;
//            in >> mass >> x >> y >> z >> vx >> vy >> vz;
//            parts.get()[i] = Particle(mass, x, y, z, vx, vy, vz);
            vertices[i] = QVector4D(static_cast<float>(parts[i].getX()),static_cast<float>(parts[i].getY()),static_cast<float>(parts[i].getZ()),static_cast<float>(parts[i].getMass()));
            massmax+=particles_mass[i];
    }

    const std::vector<unsigned int> indices = quadIndices();

    program = std::make_shared<QOpenGLShaderProgram>();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/map.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/map.frag");
    program->link();
    program->bind();

    vao.create();
    vao.bind();

    const auto vert_coords = q();
    const auto tex_coords = texCoord();

    // Load vertex coordinates.
    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(vert_coords.data(), static_cast<int>((vert_coords.size()*sizeof(decltype (vert_coords)::value_type))));

    const auto v_loc = program->attributeLocation("vertex");
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);

    texcoord_buffer.create();
    texcoord_buffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    texcoord_buffer.bind();
    texcoord_buffer.allocate(tex_coords.data(), static_cast<int>((tex_coords.size()*sizeof(decltype (tex_coords)::value_type))));

    const auto t_loc = program->attributeLocation("colorD");
    program->enableAttributeArray(t_loc);
    program->setAttributeBuffer(t_loc, GL_FLOAT, 0, 2);

    index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(indices.data(), static_cast<int>(indices.size()*sizeof(unsigned int)));
    setPalette(createGrid(vertices.data(), numOfVectors));

    num_of_indices = indices.size();

    vao.release();
    program->release();
}
void MapOpenGLWidget::setPalette(const std::vector<QVector4D> &palette){
    color_texture.destroy();
    color_texture.setSize(100,100);
    color_texture.setMinMagFilters(QOpenGLTexture::Nearest,  QOpenGLTexture::Nearest);
    color_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    color_texture.setFormat(QOpenGLTexture::RGB8_UNorm);
    color_texture.allocateStorage();
    color_texture.setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, palette.data());
}

void MapOpenGLWidget::resizeGL(int width, int height) {
    auto *gl = context()->functions();

    gl->glViewport(0, 0, width, height);

    initProgram();
}
QVector4D MapOpenGLWidget::gridCoord(QVector4D coord){
    return QVector4D((coord.x()/DIST_MAX)*50+50,(coord.y()/DIST_MAX)*50+50,0,coord.w());
}
std::vector<QVector4D> MapOpenGLWidget::createGrid(QVector4D *vertices,unsigned long long numOfVectors){
    std::vector<QVector4D> palette(10000);
    std::vector<double> mass(100*100, 0.0);
    QVector4D vector;
    for(size_t i =0; i < numOfVectors;i++){
        vector=gridCoord(vertices[i]);
        auto x = static_cast<int>(vector.x());
        auto y = static_cast<int>(vector.y());
        if(x<100&&y<100)
            mass[x*100 + y] += static_cast<double>(vector.w());
    }
    for(size_t i = 0; i<100;i++){
        for(size_t j = 0; j<100; j++){
            float mass_c=static_cast<float>((mass[i*100 + j])/massmax);
            palette[i*100+j]=QVector4D(mass_c*100,mass_c*10,mass_c,1.0f);
        }
    }
    return palette;
}

void MapOpenGLWidget::paintGL() {
    auto *gl = context()->functions();

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program) {
        return;
    }

    program->bind();



    // Draw texture.
    vao.bind();
    const int pal_loc = program->uniformLocation("text");
    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(pal_loc, 0);
    color_texture.bind();
    gl->glEnable(GL_PROGRAM_POINT_SIZE);
    gl->glDrawElements(GL_TRIANGLES, num_of_indices, GL_UNSIGNED_INT, nullptr);
    color_texture.release();
    vao.release();

    program->release();
}

void MapOpenGLWidget::onTimer() {
    if (!client)  return;
    auto particles = client->get_front_map();
    client->pop_front_map();
    if (particles.empty()) {
//        update();
        return;
    }
    fromParticleM(particles);
    if (shift == numOfVectors) {
        setPalette(createGrid(vertices.data(), numOfVectors));
        shift = 0;
        update();
    }
}


QVector4D MapOpenGLWidget::fromParticle(const Particle &part){
    return QVector4D(part.getX(),part.getY(),part.getZ(), part.getMass());
}

void MapOpenGLWidget::fromParticleM(std::vector<Particle> &part){
    for(unsigned int i = 0; i<part.size(); i++)
        vertices[i+shift]=fromParticle(part[i]);
    shift += part.size();
}
