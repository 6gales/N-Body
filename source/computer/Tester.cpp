#include "computer.h"
#include <fstream>
#include <memory>
#include <iostream>


int main()
{
	std::ifstream in("in.txt");
	std::ofstream out("out.txt");

<<<<<<< HEAD
	Computer* computer = getInstanceOf();
	int N;
	in >> N;

	std::shared_ptr<Particle> parts(new Particle[N]);
=======
	Computer* computer = new SequentialComputer();//new ompComputer(4);
	int N;
	in >> N;

	std::vector<Particle> parts(N);
>>>>>>> computer


	for(int i = 0; i < N; ++i)
	{
		double mass, x, y, z, vx, vy, vz;
		in >> mass >> x >> y >> z >> vx >> vy >> vz;
<<<<<<< HEAD
		parts.get()[i] = Particle(mass, x, y, z, vx, vy, vz);
		std::cout << parts.get()[i] << std::endl;
=======
		parts[i] = Particle(mass, x, y, z, vx, vy, vz);
		std::cout << parts[i] << std::endl;
>>>>>>> computer
	}
	in.close();

	computer->init(parts, N);

	for(int iter = 0; iter < 1000; ++iter)
	{
		out << "====================ITERATION "<< iter <<"====================="<< std::endl;
<<<<<<< HEAD
		Particle* buff = computer->iterate();
=======
		auto buff = computer->iterate();
>>>>>>> computer
		for(int i = 0; i < N; ++i)
		{
			//std::cout << buff[i] << std::endl;
			out << buff[i] << std::endl;
		}
		out << "====================ITERATION "<< iter <<"====================="<< std::endl << std::endl;
	}

	out.close();
}