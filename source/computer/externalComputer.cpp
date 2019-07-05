#include <vector>
#include <cmath>
#include <fstream>
#include "Particle.h"
#include "Vector3D.h"
#include "mpi.h"

using ull = unsigned long long;

#define index2D(i, j) (i * N + j)

int rank, size;

MPI_Datatype MPI_VECTOR3D,
		MPI_PARTICLE;

double gravity = 6.67408e-11,
	dt = 0.001;

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
	int n = N;
	int n1 = std::ceil(N / sqrt(size));
	std::cout << "n1: " << n1 << std::endl;
	shifts[0] = 0;
	shifts[size - 1] = n - n1;
	shifts[size] = n;

	for (int r = 2; r < size; r++)
	{
		shifts[size - r] = n - static_cast<int>(std::ceil(sqrt(r) * n1));
	}

	for (int r = 0; r < size; r++)
	{
		counts[r] = shifts[r + 1] - shifts[r];
	}


	///////////
	for (int i = 0; i < size; i++)
	{
		std::cout << "#" << rank << " count N" << i << ": " << counts[i] << std::endl;
	}
	for (int i = 0; i < size + 1; i++)
	{
		std::cout << "#" << rank << " shift N" << i << ": " << shifts[i] << std::endl;
	}
}

Vector3D *fillForces(const int *shifts, Particle *particles, ull N)
{
	auto forces = new Vector3D[N];

	for(ull i = shifts[rank]; i < shifts[rank + 1]; ++i)
	{
		for (ull j = i; j < N; ++j)
		{
			if (i == j)
			{
				forces[index2D(i, j)] = 0.0;
				continue;
			}

			Vector3D delta = (particles[i].coords - particles[j].coords);
			double mod = delta.module();

			forces[index2D(i, j)] = gravity * particles[i].mass
					* particles[j].mass
					/ (mod * mod * mod + 1.0e-8) * delta;
			forces[index2D(j, i)] = (-1) * forces[index2D(i, j)];
		}
	}

	return forces;
}

Particle *iterate(const int *counts, const int *shifts, Particle *particles, ull N)
{
	Vector3D *forces = fillForces(shifts, particles, N);

	std::cout << "forced" << std::endl;

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

				for (int j = shifts[rank]; j < shifts[rank + 1]; j++)
				{
					forces[index2D(j, i)] = forces[index2D(i, j)];
				}
			}
		}
	}

	auto dividedParts = new Particle[counts[rank]];

	std::cout << "neeew\n";

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

	MPI_Allgatherv(particles, counts[rank], MPI_PARTICLE, dividedParts, counts, shifts, MPI_PARTICLE, MPI_COMM_WORLD);

	delete[] dividedParts;
	delete[] forces;

	return particles;
}

char communicate()
{
	return 0;
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	std::cout << rank << std::endl;

	createMPIVector();
	createMPIParticle();

	std::cout << rank << std::endl;

	int N;
	std::ifstream in;
	std::ofstream out;

	if (rank == 0)
	{
		in.open("in.txt");
		out.open("out.txt");

		in >> N;
		std::cout << N;
	}

	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

	auto particles = new Particle[N];


	if (rank == 0)
	{
		for (int i = 0; i < N; ++i)
		{
			double mass, x, y, z, vx, vy, vz;
			in >> mass >> x >> y >> z >> vx >> vy >> vz;
			particles[i] = Particle(mass, x, y, z, vx, vy, vz);
			std::cout << particles[i] << std::endl;
		}
		in.close();
	}
	//recv initial data


	int *counts = new int[size * 2 + 1],
		*shifts = counts + size;

	std::cout << "fok\n";
	divideParticles(counts, shifts, N);
	std::cout << "fok\n";

	MPI_Bcast(particles, N, MPI_PARTICLE, 0, MPI_COMM_WORLD);
	std::cout << rank << "brah\n";
//	char notFinished = 1;
//
//	while (notFinished)
//	{
//		iterate(counts, shifts, particles, N);
//
//		if (rank == 0)
//			notFinished = communicate();
//
//		MPI_Bcast(&notFinished, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
//	}

	for(int iter = 0; iter < 1000; ++iter)
	{
		auto buff = iterate(counts, shifts, particles, N);

		if (rank == 0)
		{
			out << "====================ITERATION " << iter << "=====================" << std::endl;

			for (int i = 0; i < N; ++i)
			{
				//std::cout << buff[i] << std::endl;
				out << buff[i] << std::endl;
			}
			out << "====================ITERATION " << iter << "=====================" << std::endl << std::endl;
		}
	}

	if (!rank)
		out.close();

	delete[] particles;
	delete[] counts;

	MPI_Finalize();
	return 0;
}