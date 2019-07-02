#pragma once
#include <iostream>
#include <vector>
#include <mutex>
#include <map>
#include <deque>
#include "Particle/Particle.h"

using ull = unsigned long long;

struct Task
{
	double mass = 0.0,
			x = 0.0,
			y = 0.0,
			z = 0.0;

	Particle(double _mass, double _x, double _y, double _z)
			: mass(_mass), x(_x), y(_y), z(_z) {}
	Particle(){}
};

struct WideParticle : public Particle
{
	double v_x = 0.0,
			v_y = 0.0,
			v_z = 0.0;

	WideParticle() : Particle() {}
	WideParticle(double _mass, double _x, double _y, double _z,
				 double _v_x, double _v_y, double _v_z)
			: Particle(_mass, _x, _y, _z), v_x(_v_x), v_y(_v_y), v_z(_v_z) {}
};

class Computer
{
protected:
	double gravity = 6.67408e-11;
	double dt = 0.001;

	std::mutex containersm;
	std::map<int, Task> tasks;
	std::deque<int> orders;

	ull weight = 0;

public:
	void add(int key, std::vector<Particle> &particles, ull N)
	{
		containersm.lock();
		tasks.emplace(key, N, particles);
		containersm.unlock();
	}

	virtual const std::vector <Particle>& iterate(int) = 0;

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
		rem->second.mutex.lock();
		weight -= rem->second.N;

		std::pair<int, Task> res = *rem;

		rem->second.mutex.unlock();
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