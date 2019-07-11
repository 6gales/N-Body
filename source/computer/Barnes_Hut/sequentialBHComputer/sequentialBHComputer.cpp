#include "sequentialBHComputer.h"

std::vector<Particle> SequentialBHComputer::iterate(int key)
{

	containersm.lock();

	t = &tasks.find(key)->second;

	ull N = t->N;

	Acces = new Vector3D[N];

	OctTree tree(t->particleVectors[t->previous], Domain3D(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0));
	for(int i = 0; i < N; ++i)
	{
		Vector3D force = tree.computeForce(&(t->particleVectors[t->previous][i]), theta);
		Acces[i] = tree.computeForce(&(t->particleVectors[t->previous][i]), theta) * (1.0 / t->particleVectors[t->previous][i].mass);
	}


	for(int i = 0; i < N; ++i)
		t->particleVectors[t->current][i].vel = t->particleVectors[t->previous][i].vel + Acces[i] * dt;


	for(int i = 0; i < N; ++i)
		t->particleVectors[t->current][i].coords =  t->particleVectors[t->previous][i].coords + t->particleVectors[t->current][i].vel * dt;

	t->next();

	delete[] Acces;

	std::vector<Particle> result = t->particleVectors[t->previous];
	containersm.unlock();

	return result;
}
