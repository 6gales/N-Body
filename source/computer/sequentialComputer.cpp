#include "sequentialComputer.h"


void SequentialComputer::fillForces()
{
	for(size_t i = 0; i < N; ++i)
	{
		for(size_t j = i; j < N; ++j)
		{
<<<<<<< HEAD
			if(i == j)
			{
				forces[i][j] = 0.0;
				continue;
			}

=======
			if(i == j) forces[i][j] = 0.0;
>>>>>>> server

			Vector3D delta = (parts.get()[i].coords - parts.get()[j].coords);

			forces[i][j] = (gravity * parts.get()[i].mass * parts.get()[j].mass / delta.module()) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}


void SequentialComputer::init(std::shared_ptr <Particle> _parts, size_t _N)
{
	parts = _parts;
	N = _N;
	forces = new Vector3D*[N];
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
<<<<<<< HEAD
		for(size_t j = 0; j < N; ++j)
		{
			std::cout << forces[i][j] << " ";
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;

	for(size_t i = 0; i < N; ++i)
	{
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F += forces[j][i];
=======
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F += forces[i][j];
>>>>>>> server
		}
		Vector3D acc = F * (1.0 / parts.get()[i].mass);
		parts.get()[i].vel = parts.get()[i].vel + acc * dt;
		parts.get()[i].coords = parts.get()[i].coords + parts.get()[i].vel * dt;
	}
	return parts.get();
}


SequentialComputer::~SequentialComputer()
{
	for(size_t i = 0; i < N; ++i)
	{
		delete[] forces[i];
	}
	delete[] forces;
}