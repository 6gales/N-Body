#pragma once
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QVector2D>

#include <vector>
#include <string>
#include <functional>

std::vector<QVector3D> makeStarPalette();
std::vector<QVector3D> makePlanetPalette();
std::vector<QVector3D> grid();
std::vector<QVector3D> q();
std::vector<QVector2D> texCoord();
std::vector<unsigned int> quadIndices();
