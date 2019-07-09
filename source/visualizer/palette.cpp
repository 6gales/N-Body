#include "palette.h"
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

