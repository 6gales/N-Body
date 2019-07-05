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

			Vector3D delta = (particleVectors[previous][i].coords - particleVectors[previous][j].coords);
			double mod = delta.module();

			forces[i][j] = gravity * particleVectors[previous][i].mass * particleVectors[previous][j].mass
						   / (mod * mod * mod + 1.0e-8) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}

void ompComputer::init(std::vector<Particle> &particles, ull _N)
{
	particleVectors[previous] = particles;
	particleVectors[current] = particles;
	N = _N;
	forces = new Vector3D*[N];

	#pragma omp parallel for
	for(size_t i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}
}



const std::vector<Particle> &ompComputer::iterate()
{
	fillForces();

	#pragma omp parallel for
	for(size_t i = 0; i < N; ++i)
	{
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F -= forces[i][j];
		}
		Vector3D acc = F * (1.0 / particleVectors[previous][i].mass);
		particleVectors[current][i].vel = particleVectors[previous][i].vel + acc * dt;
		particleVectors[current][i].coords = particleVectors[previous][i].coords
											 + particleVectors[previous][i].vel * dt;
	}

	previous ^= 1;
	current ^= 1;

	return particleVectors[previous];
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