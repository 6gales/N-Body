#pragma once

#include "mpi.h"
#include "abstractComputer.h"

class mpiComputer : public Computer
{
private:
	Vector3D** forces;

	void fillForces();
	int rank, size;

public:
	mpiComputer(int argc, char **argv);
	void init(std::vector<Particle> &, size_t);
	const std::vector<Particle> &iterate();
	~mpiComputer();
};