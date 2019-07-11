#include <map>
#include <deque>
#include <cmath>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <utility>
#include "mpi.h"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "../../Particle/Particle.h"
#include "../../Vector3D/Vector3D.h"

using namespace boost::asio::ip;
using namespace boost;

#define index2D(i, j) (i * N + j)

int localRank, localSize;

std::condition_variable cond;
std::mutex mutex;


void divideParticles(int *counts, int *shifts, int N)
{
	int n = N;
	shifts[0] = 0;
//	shifts[localSize] = n;

	if (localSize >= n - 1)
	{
		for (int i = 1; i < localSize; i++)
		{
			shifts[i] = i;
			counts[i - 1] = shifts[i] - shifts[i - 1];
		}
		counts[localSize - 1] = n/*shifts[localSize]*/ - shifts[localSize - 1];

		for (int i = 0; i < localSize; i++)
		{
			std::cout << "#" << localRank << " count N" << i << ": " << counts[i] << std::endl;
		}
		for (int i = 0; i < localSize/* + 1*/; i++)
		{
			std::cout << "#" << localRank << " shift N" << i << ": " << shifts[i] << std::endl;
		}
		return;
	}


	int n1 = std::ceil(N / sqrt(localSize));
	std::cout << "n1: " << n1 << std::endl;
	shifts[localSize - 1] = n - n1;

	for (int r = 2; r < localSize; r++)
	{
		shifts[localSize - r] = n - static_cast<int>(std::ceil(sqrt(r) * n1));
	}

	for (int r = 0; r < localSize - 1/*0*/; r++)
	{
		counts[r] = shifts[r + 1] - shifts[r];
	}
	counts[localSize - 1] = n - shifts[localSize - 1];


	///////////
	for (int i = 0; i < localSize; i++)
	{
		std::cout << "#" << localRank << " count N" << i << ": " << counts[i] << std::endl;
	}
	for (int i = 0; i < localSize/* + 1*/; i++)
	{
		std::cout << "#" << localRank << " shift N" << i << ": " << shifts[i] << std::endl;
	}
}

struct Task {
	const int N;
	Particle *particles;
	int *counts,
		*shifts;

	Task(int _N, Particle *ptr) : N{ _N }, particles{ ptr }
	{
		counts = new int[localSize];
		shifts = new int[localSize];
		divideParticles(counts, shifts, N);
	}

	~Task()
	{
		delete[] particles;
		delete[] counts;
		delete[] shifts;
	}
};

class SocketReader
{
	volatile bool notFinished = true;
	char *read_msg;
	tcp::socket &sock;
	std::deque <int> &orders;
	std::map <int, Task> &tasks;
	int size;
public:
	SocketReader(tcp::socket &_sock, std::deque <int> &_orders, std::map <int, Task> &_tasks)
			: sock{_sock}, orders{_orders}, tasks{_tasks}
	{}

	int parseInt(std::string buff)
	{
		int count =     (((int) buff.at(0) << 24) & 0xFF000000)
						| (((int) buff.at(1) << 16) & 0x00FF0000)
						| (((int) buff.at(2) << 8)  & 0x0000FF00)
						| (((int) buff.at(3))       & 0x000000FF);
		return count;
	}

	bool isNotFinished()
	{
		return notFinished;
	}

	void finish()
	{
		notFinished = false;
	}

	void initRead()
	{
		read_msg = new char[4];
		asio::async_read(sock, asio::buffer(read_msg, 4),
				   bind(&SocketReader::handleMessageSize, this,
						asio::placeholders::error));
	}

	void handleMessageSize(const boost::system::error_code &er)
	{
		if (!er)
		{
			std::string str_msg{read_msg, 5};

			size = parseInt(str_msg);

			if (size == 5) //case iterate
			{
				boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
								  bind(&SocketReader::handleIterateKey, this,
									   asio::placeholders::error));
			}
			else if (size > 5) //case add new task
			{
				delete[] read_msg;
				read_msg = new char[size];
				asio::async_read(sock, boost::asio::buffer(read_msg, 4),
						   bind(&SocketReader::handleNewTask, this,
								asio::placeholders::error));
			}
		} else {
//			server.remove_connection(shared_from_this());
		}
	}

	void handleIterateKey(const boost::system::error_code &er)
	{
		if (!er)
		{
			char command = read_msg[0];
			std::string str_msg{read_msg + 1, 4};
			int key = parseInt(str_msg);

			mutex.lock();

			switch (command)
			{
				case 'A':
					orders.push_back(key);
					cond.notify_one();
					break;

				case 'R':
					tasks.erase(key);
					break;

				case 'D':
					orders.clear();
					tasks.clear();
					notFinished = false;
			}

			mutex.unlock();
			delete[] read_msg;
		}
	}

	void handleNewTask(const boost::system::error_code &er)
	{
		if (!er)
		{
			std::string str_msg{read_msg, 4};
			int count = parseInt(str_msg);
			delete[] read_msg;
			size = count;
			read_msg = new char[sizeof(int) + sizeof(float) * 7 * count];
			asio::async_read(sock, asio::buffer(read_msg, sizeof(int) + sizeof(float) * 7 * count),
					   bind(&SocketReader::handleParticles, this,
							asio::placeholders::error));
		}
	}

	void handleParticles(const boost::system::error_code &er)
	{
		if (!er)
		{
			int key = parseInt(read_msg);
			Particle *particles = parseData(read_msg + sizeof(int));

			mutex.lock();
			tasks.emplace(key, particles);
			mutex.unlock();

			delete[] read_msg;
		}
	}

	Particle *parseData(const char* rawData)
	{
		auto res = new Particle[size];
		std::string data{read_msg, sizeof(float)*7*size};

		for (int i = 0; i < size; ++i)
		{
			float mass, x, y, z, v_x, v_y, v_z;
			memcpy(&mass, data.c_str() +  i*7      * 4, sizeof(float));
			memcpy(&x,    data.c_str() + (i*7 + 1) * 4, sizeof(float));
			memcpy(&y,    data.c_str() + (i*7 + 2) * 4, sizeof(float));
			memcpy(&z,    data.c_str() + (i*7 + 3) * 4, sizeof(float));
			memcpy(&v_x,  data.c_str() + (i*7 + 4) * 4, sizeof(float));
			memcpy(&v_y,  data.c_str() + (i*7 + 5) * 4, sizeof(float));
			memcpy(&v_z,  data.c_str() + (i*7 + 6) * 4, sizeof(float));
			res[i] = {mass, x, y, z, v_x, v_y, v_z};
		}
		return res;
	}

	std::string writeMessage(Particle *particles, int N)
	{
		std::string send_msg{};
		send_msg.append(std::to_string(N));
//    TODO if need sending count, delete comments
//    char counts[8];
//    for (size_t j = 0; j < 8; ++j) {
//        counts[j] = (char)(count >> ((7-j)*8));
//    }
//    send_msg.append(counts, 8);
		for (int i = 0; i < N; i++)
		{
			float mass = particles[i].getMass();
			float x = particles[i].getX();
			float y = particles[i].getY();
			float z = particles[i].getZ();
			float vx = particles[i].getVelX();
			float vy = particles[i].getVelY();
			float vz = particles[i].getVelZ();
			send_msg.append(reinterpret_cast<const char*>(&(mass)), 4);
			send_msg.append(reinterpret_cast<const char*>(&(x)), 4);
			send_msg.append(reinterpret_cast<const char*>(&(y)), 4);
			send_msg.append(reinterpret_cast<const char*>(&(z)), 4);
			send_msg.append(reinterpret_cast<const char*>(&(vx)), 4);
			send_msg.append(reinterpret_cast<const char*>(&(vy)), 4);
			send_msg.append(reinterpret_cast<const char*>(&(vz)), 4);
		}

		return send_msg;
	}

	void sendParticles(int key)
	{
		Task &t = tasks[key];
		std::string send_msg = writeMessage(t.particles, t.N);
		asio::write(sock, asio::buffer(send_msg.c_str(), send_msg.length()));
	}


};

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

int border(const int *shifts, int N)
{
	if (localRank == localSize - 1)
		return N;

	return shifts[localRank + 1];
}

Vector3D *fillForces(const int *shifts, Particle *particles, int N)
{
	auto forces = new Vector3D[N];

	for(int i = shifts[localRank]; i < border(shifts, N)/*shifts[localRank + 1]*/; ++i)
	{
		for (int j = i; j < N; ++j)
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

Particle *iterate(const int *counts, const int *shifts, Particle *particles, int N)
{
	Vector3D *forces = fillForces(shifts, particles, N);

	std::cout << "forced" << std::endl;

	for (int r = 0; r <= localRank; r++) //each process sends and receive data
	{
		if (localRank == r) //sender
		{
			for (int recv = r + 1; recv < localSize; recv++) //send to each process
			{
				for (int i = 0; i < counts[r]; i++) //send each line of matrix
				{
					MPI_Send(forces + shifts[recv] + i * N, counts[localRank], MPI_VECTOR3D, recv,
							 1010, MPI_COMM_WORLD);
				}
			}
		} else
		{
			for (int i = 0; i < counts[r]; i++) //receive each line of matrix
			{
				MPI_Recv(forces + shifts[localRank] + i * N, counts[localRank], MPI_VECTOR3D, r,
						 1010, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				for (int j = shifts[localRank]; j < border(shifts, N)/*shifts[localRank + 1]*/; j++)
				{
					forces[index2D(j, i)] = forces[index2D(i, j)];
				}
			}
		}
	}

	auto dividedParts = new Particle[counts[localRank]];

	std::cout << "neeew\n";

	for (int i = shifts[localRank]; i < border(shifts, N)/*shifts[localRank + 1]*/; ++i)
	{
		Vector3D F;
		for (int j = 0; j < N; ++j)
		{
			F -= forces[index2D(i, j)];
		}
		Vector3D acc = F * (1.0 / particles[i].mass);
		dividedParts[i - shifts[localRank]].mass = particles[i].mass;
		dividedParts[i - shifts[localRank]].vel = particles[i].vel + acc * dt;
		dividedParts[i - shifts[localRank]].coords = particles[i].coords + particles[i].vel * dt;
	}

	MPI_Allgatherv(dividedParts, counts[localRank], MPI_PARTICLE, particles, counts, shifts, MPI_PARTICLE,
				   MPI_COMM_WORLD);

	delete[] dividedParts;
	delete[] forces;

	return particles;
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &localRank);
	MPI_Comm_size(MPI_COMM_WORLD, &localSize);

	std::cout << localRank << std::endl;

	createMPIVector();
	createMPIParticle();

	std::map <int, Task> tasks;
	std::deque <int> orders;

	std::cout << localRank << std::endl;

	asio::io_service *ioService = nullptr;
	tcp::socket *sock = nullptr;

	if (localRank == 0)
	{
		const auto ip_address = (argc > 1 ? std::string{argv[1]} : std::string{"127.0.0.1"});
		const unsigned short port = (argc > 2 ? static_cast<unsigned short>(std::stoi(argv[2])) : 8000);

		ioService = new asio::io_service;
		tcp::endpoint ep{ address::from_string(ip_address), port };
		tcp::acceptor acceptor{*ioService, ep};
		sock = new tcp::socket(*ioService);
		acceptor.accept(*sock);

	}
	MPI_Barrier(MPI_COMM_WORLD);

	std::thread thread;
	SocketReader *sr = nullptr;
	if (!localRank)
	{
		sr = new SocketReader{*sock, orders, tasks};
		auto func = [&sr]() mutable
		{
			while (sr->isNotFinished())
			{
				sr->initRead();
			}
		};

		thread = std::thread(func);
	}

//	int N;
//	std::ifstream in;
////	std::ofstream out;
//
//	if (localRank == 0)
//	{
//		in.open("in.txt");
////		out.open("out.txt");
//
//		in >> N;
//		std::cout << N;
//	}
//
//	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//	auto particles = new Particle[N];
//
//
//	if (localRank == 0)
//	{
//		for (int i = 0; i < N; ++i)
//		{
//			double mass, x, y, z, vx, vy, vz;
//			in >> mass >> x >> y >> z >> vx >> vy >> vz;
//			particles[i] = Particle(mass, x, y, z, vx, vy, vz);
//			std::cout << particles[i] << std::endl;
//		}
//		in.close();
//	}
//	//recv initial data
//
//
//	int *counts = new int[localSize],
//		*shifts = new int[localSize];// + 1];
//
//	std::cout << "fok\n";
//	divideParticles(counts, shifts, N);
//	std::cout << "fok\n";
//
//	if (localSize > 1)
//		MPI_Bcast(particles, N, MPI_PARTICLE, 0, MPI_COMM_WORLD);
//
//	std::cout << localRank << "brah\n";

	char notFinished = 1;

	while (notFinished)
	{
		if (!localRank)
		{
			std::unique_lock<std::mutex> lk(mutex);
//			mutex.lock();
			while (orders.size() == 0)
			{
				cond.wait(lk);
			}
			int key = orders.front();
			orders.pop_front();



//			mutex.unlock();
		}
		//iterate(counts, shifts, particles, N);


		MPI_Bcast(&notFinished, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
	}


	delete sock;
	delete ioService;

	std::cout << localRank << " freed mem\n";

	MPI_Finalize();
//	std::cout << localRank << "bb\n";
	return 0;
}