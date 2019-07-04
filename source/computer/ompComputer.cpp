#include "ompComputer.h"

void ompComputer::fillForces()
{
	#pragma omp parallel for schedule(dynamic)
	for(size_t i = 0; i < N; ++i)
	{
		for(size_t j = i; j < N; ++j)
		{
			if(i == j)
			{
				forces[i][j] = 0.0;
				continue;
			}

			Vector3D delta = (parts.get()[i].coords - parts.get()[j].coords);

			double mod = delta.module();
			if (mod != 0.0)
			{
				forces[i][j] = (gravity * parts.get()[i].mass * parts.get()[j].mass / mod / mod / mod) * delta;
				forces[j][i] = (-1) * forces[i][j];
			}
			else
			{
				forces[i][j] = 0;
				forces[j][i] = 0;
			}
		}
	}
}

void ompComputer::init(std::shared_ptr <Particle> _parts, size_t _N)
{
	parts = _parts;
	N = _N;
	forces = new Vector3D*[N];

	#pragma omp parallel for
	for(size_t i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}
}



Particle* ompComputer::iterate()
{
	fillForces();

	#pragma omp parallel for
	for(size_t i = 0; i < N; ++i)
	{
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F += forces[j][i];
		}
		Vector3D acc = F * (1.0 / parts.get()[i].mass);
		parts.get()[i].vel = parts.get()[i].vel + acc * dt;
		parts.get()[i].coords = parts.get()[i].coords + parts.get()[i].vel * dt;
	}

	return parts.get();
}


ompComputer::~ompComputer()
{
	#pragma omp parallel for
	for(size_t i = 0; i < N; ++i)
	{
		delete[] forces[i];
	}
	delete[] forces;
}