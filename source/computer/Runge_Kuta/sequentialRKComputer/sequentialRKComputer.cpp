#include "sequentialRKComputer.h"

void SequentialRKComputer::fillForces()
{
	for(ull i = 0; i < t->N; ++i)
	{
		for (ull j = i; j < t->N; ++j)
		{
			if (i == j)
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

Vector3D SequentialRKComputer::getAcc(ull i)
{
	Vector3D F;
	for(ull j = 0; j < t->N; ++j)
	{
		F -= forces[i][j];
	}
	Vector3D acc = F * (1.0 / t->particleVectors[t->previous][i].mass);
}


void SequentialRKComputer::fillForces(std::vector<Vector3D> coords)
{
	for(ull i = 0; i < t->N; ++i)
	{
		for (ull j = i; j < t->N; ++j)
		{
			if (i == j)
			{
				forces[i][j] = 0.0;
				continue;
			}

			Vector3D delta = (coords[i] - coords[j]);
			double mod = delta.module();

			forces[i][j] = gravity * t->particleVectors[t->previous][i].mass * t->particleVectors[t->previous][j].mass
						   / (mod * mod * mod + 1.0e-8) * delta;
			forces[j][i] = (-1) * forces[i][j];
		}
	}
}

std::vector<Particle> SequentialRKComputer::iterate(int key)
{
	containersm.lock();

	t = &tasks.find(key)->second;

	ull N = t->N;


	forces = new Vector3D*[N];
	for(ull i = 0; i < N; ++i)
	{
		forces[i] = new Vector3D[N];
	}

	std::vector<Vector3D> xBuffer(N);
	std::vector<std::vector<Vector3D>> accCoef(4, std::vector<Vector3D>(N));
	std::vector<std::vector<Vector3D>> velCoef(3, std::vector<Vector3D>(N));

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
			accCoef[k][i] = getAcc(i);
			if(k != 3)
			{
				xBuffer[i] = t->particleVectors[t->previous][i].coords + t->particleVectors[t->previous][i].vel * (dt / 2.0)
							 + accCoef[k][i] * (dt * dt / 2.0 / 2.0 / 2.0);
			}
			if(k != 0)
			{
				velCoef[k - 1][i] = t->particleVectors[t->previous][i].coords + (dt / 2) * accCoef[k - 1][i];
			}
		}
	}

	for(ull i = 0; i < N; ++i)
	{
		Vector3D newVel = t->particleVectors[t->previous][i].vel
						+ (dt / 6.0)* (accCoef[0][i] + 2.0 * accCoef[1][i] + 2.0 * accCoef[2][i] + accCoef[3][i]);
		t->particleVectors[t->current][i].vel = newVel;
		t->particleVectors[t->current][i].coords = t->particleVectors[t->previous][i].coords + (dt / 6) *
				(t->particleVectors[t->previous][i].vel + 2.0 * velCoef[0][i] + 2.0 * velCoef[1][i] + velCoef[2][i]);
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