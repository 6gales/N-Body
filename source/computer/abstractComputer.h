#pragma once
<<<<<<< HEAD
#include <memory>
<<<<<<< HEAD
#include <iostream>
#include "Particle.h"
=======
#include "Vector3D.h"

struct Particle
{
	double mass = 0.0;
	Vector3D coords;
	Vector3D vel;

	Particle(){}
	Particle(double _mass, double x, double y, double z,
				 double v_x, double v_y, double v_z)
			: mass(_mass), coords(x, y, z), vel(v_x, v_y, v_z) {}

	float getMass()    const { return mass;	}
	float getX() 	const { return coords.x;}
	float getY()	const { return coords.y;}
	float getZ()	const { return coords.z;}
	float getVelX()	const { return vel.x;	}
	float getVelY()	const { return vel.y;	}
	float getVelZ()	const { return vel.z;	}
};

>>>>>>> server
=======
#include <iostream>
#include <vector>
#include "Particle.h"

using ull = unsigned long long;
>>>>>>> server

class Computer
{
protected:
	double gravity = 6.67408e-11;
	double dt = 0.001;
	ull N;
	std::vector<Particle> particleVectors[2];
	char previous = 1,
		current = 0;

public:
	virtual void init(std::vector<Particle> &, ull) = 0;
	virtual const std::vector <Particle>& iterate() = 0;
	virtual ~Computer() {};
	void setGravity(double _gravity){ gravity = _gravity; }
	ull getSize() const { return N; }
};