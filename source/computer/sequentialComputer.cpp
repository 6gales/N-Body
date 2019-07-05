#include "sequentialComputer.h"


void SequentialComputer::fillForces()
{
	for(size_t i = 0; i < N; ++i)
	{
		for (size_t j = i; j < N; ++j)
		{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
			if(i == j)
=======
			if (i == j)
>>>>>>> server
=======
			if (i == j)
>>>>>>> computer
			{
				forces[i][j] = 0.0;
				continue;
			}
<<<<<<< HEAD
<<<<<<< HEAD

=======
			if(i == j) forces[i][j] = 0.0;
>>>>>>> server
=======
>>>>>>> server
=======
>>>>>>> computer

			Vector3D delta = (particleVectors[previous][i].coords - particleVectors[previous][j].coords);
			double mod = delta.module();

			forces[i][j] = gravity * particleVectors[previous][i].mass * particleVectors[previous][j].mass
					/ (mod * mod * mod + 1.0e-8) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}


<<<<<<< HEAD
void SequentialComputer::init(std::vector<Particle> &particles, ull _N)
=======
void SequentialComputer::init(std::vector<Particle> &particles, size_t _N)
>>>>>>> computer
{
	particleVectors[previous] = particles;
	particleVectors[current] = particles;
	N = _N;
	forces = new Vector3D*[N];
	for(size_t i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}
}



const std::vector<Particle> &SequentialComputer::iterate()
{
	fillForces();

	std::cout << std::endl;

<<<<<<< HEAD
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

=======
>>>>>>> computer
	for(size_t i = 0; i < N; ++i)
	{
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F += forces[j][i];
<<<<<<< HEAD
=======
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
<<<<<<< HEAD
			F += forces[i][j];
>>>>>>> server
=======
			F -= forces[i][j];
>>>>>>> server
=======
>>>>>>> computer
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


SequentialComputer::~SequentialComputer()
{
	for(size_t i = 0; i < N; ++i)
	{
		delete[] forces[i];
	}
	delete[] forces;
}