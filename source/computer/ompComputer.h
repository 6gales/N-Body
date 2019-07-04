#pragma once

#include "abstractComputer.h"
#include "omp.h"

class ompComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();

public:
	void init(std::shared_ptr<Particle>, size_t);
	Particle* iterate();
	~ompComputer();
};