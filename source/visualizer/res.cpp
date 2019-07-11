#include "res.h"
std::vector<QVector3D> makeStarPalette() {
    const std::vector<QVector3D> star = {
        QVector3D(1.0f, 1.0f, 1.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 0.5f, 0.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        QVector3D(0.0f, 0.5f, 1.0f),
        QVector3D(0.0f, 0.0f, 1.0f)
    };
    return star;
}
std::vector<QVector3D> makePlanetPalette() {
    const std::vector<QVector3D> planet = {
        QVector3D(0.6f, 0.4f, 0.2f),
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.7f, 0.5f, 0.4f),
    };
    return planet;
}
std::vector<QVector3D> q(){
    const std::vector<QVector3D> q = {
        QVector3D(-1.0f,1.0f,0.0f),
        QVector3D(1.0f,1.0f,0.0f),
        QVector3D(-1.0f,-1.0f,0.0f),
        QVector3D(1.0f,-1.0f,0.0f)
    };
    return q;
}
std::vector<QVector2D> texCoord(){
    const std::vector<QVector2D> texcoord = {
        QVector2D(0.0f,1.0f),
        QVector2D(1.0f,1.0f),
        QVector2D(0.0f,0.0f),
        QVector2D(1.0f,0.0f)
    };
    return texcoord;
}

std::vector<unsigned int> quadIndices() {
    return {0, 2, 3,
            0, 3, 1};
}
