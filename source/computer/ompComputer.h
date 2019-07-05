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
<<<<<<< HEAD
	void init(std::vector<Particle> &, ull);
=======
	void init(std::vector<Particle> &, size_t);
>>>>>>> computer
	const std::vector<Particle> &iterate();
	~ompComputer();
};