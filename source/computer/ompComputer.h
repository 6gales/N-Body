#pragma once

#include "abstractComputer.h"
#include "omp.h"

class ompComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();

public:
	ompComputer(size_t numThreads)
	{
		omp_set_num_threads(numThreads);
		forces = nullptr;
	}
	void init(std::shared_ptr<Particle>, size_t);
	Particle* iterate();
	~ompComputer();
};