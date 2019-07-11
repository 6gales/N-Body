#pragma once

#include "../abstractComputer.h"
#include "omp.h"

class OMPRKComputer : public Computer
{
private:
	Vector3D** forces;
	Task *t;
	void fillForces();
	void fillForces(std::vector<Vector3D> coords);
	Vector3D getAcc(ull i);

public:
	OMPRKComputer(size_t numThreads)
	{
		omp_set_num_threads(numThreads);
		forces = nullptr;
	}
	std::vector<Particle> iterate(int key);

};