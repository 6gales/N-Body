#include "mpiComputer.h"

mpiComputer::mpiComputer(ComputerType type, size_t threads) : Computer(type)
{
	std::string command = "launch.sh";
	command.append(" " + std::to_string(threads));// + " ")add ip addr and port
	system(command.c_str());
}

const std::vector<Particle>& mpiComputer::iterate(int key)
{

}

int mpiComputer::SocketReader::parseInt(std::string buff)
{
	int count =     (((int) buff.at(0) << 24) & 0xFF000000)
					| (((int) buff.at(1) << 16) & 0x00FF0000)
					| (((int) buff.at(2) << 8)  & 0x0000FF00)
					| (((int) buff.at(3))       & 0x000000FF);
	return count;
}

void mpiComputer::SocketReader::initRead()
{
	read_msg = new char[4];
	asio::read(sock, asio::buffer(read_msg, 4),
			   bind(&SocketReader::handleMessageSize, this,
					asio::placeholders::error));
}

void mpiComputer::SocketReader::handleMessageSize(const boost::system::error_code &er)
{
	if (!er)
	{
		std::string str_msg{read_msg, 5};

		size = parseInt(str_msg);

		if (size == 5) //case iterate
		{
			boost::asio::read(sock, boost::asio::buffer(read_msg, 5),
							  bind(&SocketReader::handleIterateKey, this,
								   asio::placeholders::error));
		}
		else if (size > 5) //case add new task
		{
			delete[] read_msg;
			read_msg = new char[size];
			asio::read(sock, boost::asio::buffer(read_msg, 4),
					   bind(&SocketReader::handleNewTask, this,
							asio::placeholders::error));
		}
	} else {
//			server.remove_connection(shared_from_this());
	}
}

void mpiComputer::SocketReader::handleIterateKey(const boost::system::error_code &er)
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

void mpiComputer::SocketReader::handleNewTask(const boost::system::error_code &er)
{
	if (!er)
	{
		std::string str_msg{read_msg, 4};
		int count = parseInt(str_msg);
		delete[] read_msg;
		size = count;
		read_msg = new char[sizeof(int) + sizeof(float) * 7 * count];
		asio::read(sock, asio::buffer(read_msg, sizeof(int) + sizeof(float) * 7 * count),
				   bind(&SocketReader::handleParticles, this,
						asio::placeholders::error));
	}
}

void mpiComputer::SocketReader::handleParticles(const boost::system::error_code &er)
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

Particle *mpiComputer::SocketReader::parseData(const char* rawData)
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

std::string mpiComputer::SocketReader::writeMessage(Particle *particles, int N)
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

void mpiComputer::SocketReader::sendParticles(int key)
{
	Task &t = tasks[key];
	std::string send_msg = writeMessage(t.particles, t.N);
	asio::write(sock, asio::buffer(send_msg.c_str(), send_msg.length()));
}