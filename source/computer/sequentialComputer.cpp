#include "sequentialComputer.h"


void SequentialComputer::fillForces()
{
	for(size_t i = 0; i < N; ++i)
	{
		for(size_t j = i; j < N; ++j)
		{
			if(i == j) forces[i][j] = 0.0;

			Vector3D delta = (parts.get()[i].getCoords - parts.get()[j].getCoords);

			forces[i][j] = (gravity * parts.get()[i].mass * parts.get()[j].mass / delta.module()) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}


void SequentialComputer::init(std::shared_ptr <Particle> _parts, size_t _N)
{
	parts = _parts;
	N = _N;
	forces = new (Vector3D*)[N];
	for(size_t i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}
}



Particle* SequentialComputer::iterate()
{
	fillForces();
	for(size_t i = 0; i < N; ++i)
	{
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F += forces[i][j];
		}
		Vector3D acc = F * (1.0 / parts.get()[i].getMass());
		parts.get()[i].vel = parts.get()[i].vel + acc * dt;
		parts.get()[i].coords = parts.get()[i].coords + parts.get()[i].vel * dt;
	}

}


SequentialComputer::~Computer()
{
	for(size_t i = 0; i < N; ++i)
	{
		delete[] forces[i];
	}
	delete[] forces;
}