#pragma once
#include <memory>

using ull = unsigned long long;

struct Task
{
	double mass = 0.0;
	Vector3D coords;
	Vector3D vel;

	Particle(){}
	Particle(double _mass, double _x, double _y, double _z,
				 double _v_x, double _v_y, double _v_z)
			: mass(_mass), coords(_x, _y, _z), vel(v_x, v_y, v_z) {}

	float getMass() const { return mass;	}
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
	double gravity = 9.81;
	double dt = 0.001;
	std::shared_ptr<Particle> parts;
	size_t N;

public:
	virtual void init(std::shared_ptr<Particle>, size_t) = 0;
	virtual Particle* iterate() = 0;
	virtual ~Computer() = 0;
	void setGravity(double _gravity){ gravity = _gravity; }
	size_t getSize() const { return N; }
};