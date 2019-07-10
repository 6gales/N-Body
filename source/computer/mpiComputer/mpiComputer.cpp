//#include "mpiComputer.h"
//
//mpiComputer::mpiComputer(int argc, char **argv)
//{
//	MPI_Init(&argc, &argv);
//	MPI_Comm_rank(MPI_COMM_WORLD, &localRank);
//	MPI_Comm_size(MPI_COMM_WORLD, &localSize);
//
//}
//
//void mpiComputer::fillForces()
//{
//	for(size_t i = 0; i < N; ++i)
//	{
//		for(size_t j = i; j < N; ++j)
//		{
//			if(i == j)
//			{
//				forces[i][j] = 0.0;
//				continue;
//			}
//
//			Vector3D delta = (parts.get()[i].coords - parts.get()[j].coords);
//
//			forces[i][j] = (gravity * parts.get()[i].mass * parts.get()[j].mass / delta.module()) * delta;
//			forces[j][i] = (-1) * forces[i][j];
//		}
//	}
//}//0: 0,1 0,2 0,3 0,4
// //   1,0 2,0 3,0 4,0
//
//
//void mpiComputer::init(std::vector<Particle> &_parts, size_t _N)
//{
//	parts = _parts;
//	N = _N;
//	forces = new Vector3D*[N];
//
//	for(size_t i = 0; i < N; ++i)
//	{
//		forces[i] = new Vector3D[N];
//	}
//}
//
//
//
//Particle* mpiComputer::iterate()
//{
//	fillForces();
//
//	for(size_t i = 0; i < N; ++i)
//	{
//		Vector3D F;
//		for(size_t j = 0; j < N; ++j)
//		{
//			F += forces[j][i];
//		}
//		Vector3D acc = F * (1.0 / parts.get()[i].mass);
//		parts.get()[i].vel = parts.get()[i].vel + acc * dt;
//		parts.get()[i].coords = parts.get()[i].coords + parts.get()[i].vel * dt;
//	}
//
//	return parts.get();
//}
//
//
//mpiComputer::~mpiComputer()
//{
//	for(size_t i = 0; i < N; ++i)
//	{
//		delete[] forces[i];
//	}
//	delete[] forces;
//}