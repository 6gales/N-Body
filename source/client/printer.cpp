

#include <iostream>
#include <vector>
#include "../computer/abstractComputer.h"

void printParticles(const std::vector<Particle> &particles) {
    for (const Particle &p : particles) {
        std::cerr << "mass = " << p.getMass() << ", x = " << p.getX() << ", y = " << p.getY() << ", z = " << p.getZ() << std::endl;
    }
}