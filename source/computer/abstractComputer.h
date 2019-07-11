#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <map>
#include <deque>
#include "Particle/Particle.h"
#include "ComputerType.h"

using ull = unsigned long long;

struct Task
{
	const ull N;
	std::vector<Particle> particleVectors[2];
	char previous = 1,
			current = 0;

	Task(ull _N, const std::vector<Particle> &parts) : N{_N}
	{
		particleVectors[0] = parts;
		particleVectors[1] = parts;
	}

	Task(const Task &t) : N(t.N)
	{
		particleVectors[0] = t.particleVectors[0];
		particleVectors[0] = t.particleVectors[0];
		previous = t.previous;
		current = t.current;
	}

	void next()
	{
		previous ^= 1;
		current ^= 1;
	}

	const std::vector<Particle> &getUpdated() { return particleVectors[previous]; }
};

class Computer
{
	const ComputerType type;

protected:
	double gravity = 6.67408e-11;
	double dt = 0.001;

	std::mutex containersm;
	std::map<int, Task> tasks;

	ull weight = 0;

public:
	Computer(ComputerType _type) : type{ _type }
	{}

	void add(int key, std::vector<Particle> &particles, ull N)
	{
		containersm.lock();
		tasks.emplace(key, N, particles);
		containersm.unlock();
	}

	virtual std::vector <Particle> iterate(int) = 0;

	void remove(int key)
	{
		getAndRemove(key);
	}

	std::pair<int, Task> getAndRemove(int key)
	{
		containersm.lock();
		auto rem = tasks.find(key);
		if (rem == tasks.end())
		{
			containersm.unlock();
			return std::make_pair(key, Task{0, std::vector<Particle>{}});
		}
		weight -= rem->second.N;

		std::pair<int, Task> res = *rem;

		tasks.erase(rem);
		containersm.unlock();

		return res;
	}

	ull getWeight() { return weight; }

	bool haveTask(int key)
	{
		containersm.lock();
		bool pres = tasks.count(key);
		containersm.unlock();
		return pres;
	}

	virtual ~Computer() {};
	void setGravity(double _gravity){ gravity = _gravity; }
};