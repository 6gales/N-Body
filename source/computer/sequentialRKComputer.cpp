#include "sequentialRKComputer.h"

void SequentialRKComputer::fillForces()
{
	for(ull i = 0; i < N; ++i)
	{
		for (ull j = i; j < N; ++j)
		{
			if (i == j)
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

Vector3D SequentialRKComputer::getAcc(ull i)
{
	Vector3D F;
	for(ull j = 0; j < N; ++j)
	{
		F -= forces[i][j];
	}
	Vector3D acc = F * (1.0 / particleVectors[previous][i].mass);
}


void SequentialRKComputer::fillForces(std::vector<Vector3D> coords)
{
	for(ull i = 0; i < N; ++i)
	{
		for (ull j = i; j < N; ++j)
		{
			if (i == j)
			{
				forces[i][j] = 0.0;
				continue;
			}

			Vector3D delta = (coords[i] - coords[j]);
			double mod = delta.module();

			forces[i][j] = gravity * particleVectors[previous][i].mass * particleVectors[previous][j].mass
						   / (mod * mod * mod + 1.0e-8) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}


void SequentialRKComputer::init(std::vector<Particle> &particles, ull _N)
{
	particleVectors[previous] = particles;
	particleVectors[current] = particles;
	N = _N;
	forces = new Vector3D*[N];
	for(ull i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}
}

const std::vector<Particle> &SequentialRKComputer::iterate()
{
	//fillForces();
	std::vector<Vector3D> xBuffer(N);
	std::vector<std::vector<Vector3D>> coef(4, std::vector<Vector3D>(N));

	for(size_t k = 0; k < 4; ++k)
	{
		if(k == 0)
		{
			fillForces();
		}
		else
		{
			fillForces(xBuffer);
		}

		for(ull i = 0; i < N; ++i)
		{
			coef[k][i] = getAcc(i);
			if(k != 3)
			{
				xBuffer[i] = particleVectors[previous][i].coords + particleVectors[previous][i].vel * (dt / 2.0)
							 + coef[k][i] * (dt * dt / 2.0 / 2.0 / 2.0);
			}
		}
	}

	for(ull i = 0; i < N; ++i)
	{
		Vector3D newVel = particleVectors[previous][i].vel
						+ (dt / 6.0)* (coef[0][i] + 2.0 * coef[1][i] + 2.0 * coef[2][i] + coef[3][i]);
		particleVectors[current][i].vel = newVel;
		particleVectors[current][i].coords = particleVectors[previous][i].coords + (dt) * newVel;
	}

	previous ^= 1;
	current ^= 1;

	return particleVectors[previous];
}


SequentialRKComputer::~SequentialRKComputer()
{
	for(ull i = 0; i < N; ++i)
	{
		delete[] forces[i];
	}
	delete[] forces;
}