#pragma once

#include "../abstractComputer.h"
#include "omp.h"

class ompComputer : public Computer
{
private:
	Vector3D** forces;
	Task* t;
	void fillForces();

public:
	ompComputer(size_t numThreads)
	{
		omp_set_num_threads(numThreads);
		forces = nullptr;
	}
	std::vector<Particle> iterate(int key);
};