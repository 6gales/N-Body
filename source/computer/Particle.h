#pragma once
#include "Vector3D.h"
#include <ostream>


struct Particle
{
	double mass = 0.0;
	Vector3D coords;
	Vector3D vel;

	Particle(){}
	Particle(double _mass, double x, double y, double z,
			 double v_x, double v_y, double v_z)
			: mass(_mass), coords(x, y, z), vel(v_x, v_y, v_z) {}

	float getMass() const { return mass;	}
	float getX() 	const { return coords.x;}
	float getY()	const { return coords.y;}
	float getZ()	const { return coords.z;}
	float getVelX()	const { return vel.x;	}
	float getVelY()	const { return vel.y;	}
	float getVelZ()	const { return vel.z;	}
};

std::ostream& operator<<(std::ostream &, const Particle&);