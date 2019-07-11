#include "OctTreeNode.h"
#include <cmath>
#include <algorithm>
#include <iostream>

void OctTreeNode::insertPart(Particle *_part)
{
	if(isEmpty)
	{
		isEmpty = false;
		part = _part;
	}
	else
	{
		if(part)
		{
			Particle* old = part;
			part = nullptr;
			getChild(old->coords)->insertPart(old);
			getChild(_part->coords)->insertPart(_part);
		}
		else
		{
			getChild(_part->coords)->insertPart(_part);
		}
	}
}

bool OctTreeNode::computeMassCenter()
{
	if(isEmpty)
		return false;
	if(part)
	{
		massCenter.mass = part->mass;
		massCenter.coords = part->coords;
	}
	else
	{
		massCenter.mass = 0.0;
		massCenter.coords = 0.0;
		for(int i = 0; i < NUM_OF_CHILDREN; ++i)
		{
			if(children[i].get() && children[i]->computeMassCenter())
			{
				const Particle &mc = children[i]->getMassCenter();
				massCenter.mass = mc.mass;
				massCenter.coords += mc.coords * mc.mass;
			}
		}
		massCenter.coords = massCenter.coords * (1.0 / massCenter.mass);
	}
	return true;
}

OctTreeNode* OctTreeNode::getChild(Direction d)
{
	if(!children[d].get())
	{
		children[d].reset(new OctTreeNode(domain.getOctant(d)));
	}
	return children[d].get();
}

OctTreeNode* OctTreeNode::getChild(const Vector3D &coord)
{
	Direction oct = domain.detectOctant(coord);
	return getChild(oct);
}

int OctTreeNode::getHeight() const
{
	int height[NUM_OF_CHILDREN];
	for(int i = 0; i < NUM_OF_CHILDREN; ++i)
	{
		height[i] = (children[i].get()) ? 1 + children[i]->getHeight() : 1;
	}
	return *std::max_element(height, height + NUM_OF_CHILDREN);
}

int OctTreeNode::getSize() const
{
	int size = 1;
	for(int i = 0; i < NUM_OF_CHILDREN; ++i)
	{
		if(children[i].get())
			size += children[i]->getSize();
	}
	return size;
}

Vector3D OctTreeNode::computeForce(Particle* part1, const Particle* part2) const
{
	Vector3D delta = part2->coords - part1->coords;
	double module  = delta.module();
	return (gravity * part1->mass * part2->mass / (module * module * module  + 1.0e-8) ) * delta;
}


Vector3D OctTreeNode::computeForce(Particle* target, const double &theta) const
{
	if(isEmpty || (part == target))
	{
		return Vector3D(0.0, 0.0, 0.0);
	}
	if(part)
	{
		Vector3D result = computeForce(target, part);
		return result;
	}
	else
	{
		Vector3D delta = target->coords - massCenter.coords;
		const double dist = delta.module() + 1.0e-8;
		if(std::max({domain.height(), domain.width(), domain.depth()}) / dist < theta)
		{
			Vector3D result = computeForce(target, &massCenter);
			return result;
		}
		else
		{
			Vector3D force(0.0, 0.0, 0.0);
			for(int i = 0; i < NUM_OF_CHILDREN; ++i)
			{
				if (children[i].get())
				{
					force += children[i]->computeForce(target, theta);
				}
			}
			return force;
		}
	}
}