#include "Particle.h"


std::ostream& operator<<(std::ostream &os, const Particle &p)
{
	os << p.mass << " " << p.getX() << " " << p.getY() << " " << p.getZ()
	   << " " << p.getVelX() << " " << p.getVelY() << " " << p.getVelZ();
	return os;
}