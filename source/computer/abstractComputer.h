#pragma once

struct Particle
{
	double mass = 0.0,
			x = 0.0,
			y = 0.0,
			z = 0.0;

	Particle(double _mass, double _x, double _y, double _z)
			: mass(_mass), x(_x), y(_y), z(_z) {}
	Particle(){}
};

struct WideParticle : public Particle
{
	double v_x = 0.0,
			v_y = 0.0,
			v_z = 0.0;

	WideParticle() : Particle() {}
	WideParticle(double _mass, double _x, double _y, double _z,
				 double _v_x, double _v_y, double _v_z)
			: Particle(_mass, _x, _y, _z), v_x(_v_x), v_y(_v_y), v_z(_v_z) {}
};

class Computer
{
public:
	virtual void init() = 0;
	virtual Particle *iterate() = 0;
};