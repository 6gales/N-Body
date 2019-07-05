#pragma once

#include "abstractComputer.h"
#include "omp.h"

class ompRKComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();

public:
	ompRKComputer(size_t numThreads)
	{
		omp_set_num_threads(numThreads);
		forces = nullptr;
	}
	void init(std::vector<Particle> &, ull);
	const std::vector<Particle> &iterate();
	~ompRKComputer();

	Vector3D getAcc(ull i);

	void fillForces(std::vector<Vector3D> coords);
};