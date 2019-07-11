#include "sequentialComputer.h"


void SequentialComputer::fillForces()
{
	for(size_t i = 0; i < N; ++i)
	{
		for (size_t j = i; j < N; ++j)
		{
			if (i == j)
			{
				forces[i][j] = 0.0;
				continue;
			}
			if(i == j) forces[i][j] = 0.0;

			Vector3D delta = (particleVectors[previous][i].coords - particleVectors[previous][j].coords);
			double mod = delta.module();

			Vector3D delta = (parts.get()[i].coords - parts.get()[j].coords);
			
            double mod = delta.module()+1e-8;
            forces[i][j] = (gravity * parts.get()[i].mass * parts.get()[j].mass / mod / mod / mod) * delta;
            forces[j][i] = (-1) * forces[i][j];
		}
	}
}

void SequentialComputer::init(std::vector<Particle> &particles, ull _N)
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
//    for(size_t i = 0; i < N; ++i)
//    {
//        for(size_t j = 0; j < N; ++j)
//        {
//            std::cout << forces[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }

//    std::cout << std::endl;

	for(size_t i = 0; i < N; ++i)
	{
		Vector3D F;
		for(size_t j = 0; j < N; ++j)
		{
			F -= forces[i][j];

		}
		Vector3D acc = F * (1.0 / parts.get()[i].mass);
		parts.get()[i].vel = parts.get()[i].vel + acc * dt;
        parts.get()[i].coords = parts.get()[i].coords + parts.get()[i].vel * dt;
//        std::cout << parts.get()[i].vel << " " << parts.get()[i].coords << std::endl;
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
