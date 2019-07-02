#pragma once

struct Particle
{
	double mass = 0.0;
	Vector3D coords;

	Particle(double _mass, double _x, double _y, double _z)
			: mass(_mass), coords(_x, _y, _z) {}
	Particle(){}
};

struct WideParticle : public Particle
{
	Vector3D vel;

	WideParticle() : Particle() {}
	WideParticle(double _mass, double _x, double _y, double _z,
				 double _v_x, double _v_y, double _v_z)
			: Particle(_mass, _x, _y, _z), vel(v_x, v_y, v_z) {}
};

class Computer
{
public:
	virtual void init() = 0;
	virtual Particle *iterate() = 0;
};