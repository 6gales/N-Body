#include <vector>
#include <cmath>
#include "Particle.h"
#include "Vector3D.h"
#include "mpi.h"

using ull = unsigned long long;

#define index2D(i, j) (i * N + j)

int rank, size;

MPI_Datatype MPI_VECTOR3D,
		MPI_PARTICLE;

void createMPIVector()
{
	const int nitems = 3;
	int blocklengths[] = { 1, 1, 1 };
	MPI_Datatype types[] = { MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE };
	MPI_Aint offsets[3];

	offsets[0] = offsetof(Vector3D, x);
	offsets[1] = offsetof(Vector3D, y);
	offsets[2] = offsetof(Vector3D, z);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_VECTOR3D);
	MPI_Type_commit(&MPI_VECTOR3D);
}

void createMPIParticle()
{
	const int nitems = 3;
	int blocklengths[] = { 1, 1, 1 };
	MPI_Datatype types[] = { MPI_DOUBLE, MPI_VECTOR3D, MPI_VECTOR3D };
	MPI_Aint offsets[3];

	offsets[0] = offsetof(Particle, mass);
	offsets[1] = offsetof(Particle, coords);
	offsets[2] = offsetof(Particle, vel);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_PARTICLE);
	MPI_Type_commit(&MPI_PARTICLE);
}

void divideParticles(int *counts, int *shifts, ull N)
{
	shifts[size] = N;
	int n1 = std::ceil(N / sqrt(size));

}

char communicate()
{

}


Vector3D *fillForces(Particle *particles, ull N)
{
	Vector3D *forces = new Vector3D[N];
	return forces;
}

void iterate(int *counts, int *shifts, Particle *particles, ull N, double dt)
{
	Vector3D *forces = fillForces(particles, N);

	for (int r = 0; r <= rank; r++) //each process sends and receive data
	{
		if (rank == r) //sender
		{
			for (int recv = r + 1; recv < size; recv++) //send to each process
			{
				for (int i = 0; i < counts[r]; i++) //send each line of matrix
				{
					MPI_Send(forces + shifts[recv] + i * N, counts[rank], MPI_VECTOR3D, recv,
							1010, MPI_COMM_WORLD);
				}
			}
		}
		else
		{
			for (int i = 0; i < counts[r]; i++) //receive each line of matrix
			{
				MPI_Recv(forces + shifts[rank] + i * N, counts[rank], MPI_VECTOR3D, r,
						1010, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


			}
		}
	}

	Particle *dividedParts = new Particle[counts[rank]];

	for(ull i = shifts[rank]; i < shifts[rank + 1]; ++i)
	{
		Vector3D F;
		for(ull j = 0; j < N; ++j)
		{
			F += forces[index2D(i, j)];
		}
		Vector3D acc = F * (1.0 / particles[i].mass);
		particles[i].vel = particles[i].vel + acc * dt;
		particles[i].coords = particles[i].coords + particles[i].vel * dt;
	}

	MPI_Gatherv(particles, counts[rank], MPI_PARTICLE, dividedParts, counts, shifts, MPI_PARTICLE, 0, MPI_COMM_WORLD);

//	return particles;
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	createMPIVector();
	createMPIParticle();

	ull N = 1;
	Particle *particles;

	//recv initial data


	int *counts = new int[size * 2 + 1],
		*shifts = counts + size;

	divideParticles(counts, shifts, N);


	char notFinished = 1;

	while (notFinished)
	{
		iterate();

		if (rank == 0)
			notFinished = communicate();

		MPI_Bcast(&notFinished, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	}

	delete[] counts;

	MPI_Finalize();
	return 0;
}