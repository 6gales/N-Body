#pragma once
#include <memory>
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


class Computer
{
protected:
	double gravity = 6.67408e-11;
	double dt = 0.001;
	std::shared_ptr<Particle> parts;
	size_t N;

public:
	virtual void init(std::shared_ptr<Particle>, size_t) = 0;
	virtual Particle* iterate() = 0;
	virtual ~Computer() {};
	void setGravity(double _gravity){ gravity = _gravity; }
	size_t getSize() const { return N; }
};