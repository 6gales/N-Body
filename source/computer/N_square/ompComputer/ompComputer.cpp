#include "ompComputer.h"

void OMPComputer::fillForces()
{
	#pragma omp parallel for schedule(dynamic)
	for(ull i = 0; i < t->N; ++i)
	{
		for(ull j = i; j < t->N; ++j)
		{
			if(i == j)
			{
				forces[i][j] = 0.0;
				continue;
			}

			Vector3D delta = (t->particleVectors[t->previous][i].coords - t->particleVectors[t->previous][j].coords);
			double mod = delta.module();

			forces[i][j] = gravity * t->particleVectors[t->previous][i].mass * t->particleVectors[t->previous][j].mass
						   / (mod * mod * mod + 1.0e-8) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}

std::vector<Particle> OMPComputer::iterate(int key)
{

	//как оно будет работать с очередью?
	containersm.lock();

	t = &tasks.find(key)->second;

	ull N = t->N;

	forces = new Vector3D*[N];
	for(ull i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}

	fillForces();

	#pragma omp parallel for
	for(ull i = 0; i < t->N; ++i)
	{
		Vector3D F;
		for(ull j = 0; j < t->N; ++j)
		{
			F -= forces[i][j];
		}
		Vector3D acc = F * (1.0 / t->particleVectors[t->previous][i].mass);
		t->particleVectors[t->current][i].vel = t->particleVectors[t->previous][i].vel + acc * dt;
		t->particleVectors[t->current][i].coords = t->particleVectors[t->previous][i].coords
											 + t->particleVectors[t->previous][i].vel * dt;
	}

	t->next();

	for(ull i = 0; i < N; ++i)
	{
		delete[] forces[i];
	}
	delete[] forces;

	std::vector<Particle> result = t->particleVectors[t->previous];
	containersm.unlock();

	return result;
}
