#pragma once

#include <cstdlib>
#include "../abstractComputer.h"
#include "boost/bind.hpp"
#include "boost/asio.hpp"

using namespace boost::asio::ip;
using namespace boost;

class mpiComputer : public Computer
{
	class SocketReader
	{
		char *read_msg;
		tcp::socket &sock;
		std::deque <int> &orders;
		std::map <int, Task> &tasks;
		int size;
	public:
		SocketReader(tcp::socket &_sock, std::deque <int> &_orders, std::map <int, Task> &_tasks)
				: sock{_sock}, orders{_orders}, tasks{_tasks}
		{}

		int parseInt(std::string buff);

		void initRead();

		void handleMessageSize(const boost::system::error_code &er);

		void handleIterateKey(const boost::system::error_code &er);

		void handleNewTask(const boost::system::error_code &er);

		void handleParticles(const boost::system::error_code &er);

		Particle *parseData(const char* rawData);

		void sendParticles(int key);
	};

public:
	mpiComputer(ComputerType type, size_t threads);
	const std::vector<Particle> &iterate(int key);
};