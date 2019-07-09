#include "OctTreeNode.h"
#include <cmath>
#include <algorithm>
#include <iostream>

void OctTreeNode::insertPart(Particle *_part)
{
	if(isEmpty)
	{
		std::cout << "insert false "<< std::endl;
		std::cout << _part->coords << std::endl;
		isEmpty = false;
		part = _part;
	}
	else
	{
		if(part)
		{
			std::cout << "Part exist " << _part->coords << std::endl;
			Particle* old = part;
			part = nullptr;
			getChild(old->coords)->insertPart(old);
			getChild(_part->coords)->insertPart(_part);
		}
		else
		{
			std::cout << "Part doesn't exist" << _part->coords << std::endl;
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
	std::cout << part1->coords << " " << part2->coords << " " << part1->mass << " " << part2->mass << std::endl;
	Vector3D delta = part2->coords - part1->coords;
	double module  = delta.module();
	return (6.67*1.0e-11 * part1->mass * part2->mass / module / module / module) * delta;
}


Vector3D OctTreeNode::computeForce(Particle* target, const double &theta) const
{
	std::cout << "compute Force in Node" << std::endl;
	if(isEmpty || (part == target))
	{
		std::cout << "in empty return " << Vector3D(0.0, 0.0, 0.0) << std::endl;
		return Vector3D(0.0, 0.0, 0.0);
	}
	if(part)
	{
		std::cout << "part has 1 part" << std::endl;
		Vector3D result = computeForce(target, part);
		std::cout << "Force between " << part->coords << " " << target->coords << " " << result << std::endl;
		return result;
	}
	else
	{
		std::cout << "more than 1 part in domain" << std::endl;
		Vector3D delta = target->coords - massCenter.coords;
		const double dist = delta.module() + 1.0e-8;
		if(std::max({domain.height(), domain.width(), domain.depth()}) / dist < theta)
		{
			std::cout << "condition is ok" << std::endl;
			Vector3D result = computeForce(target, &massCenter);
			std::cout << "force between object and massCenter" << target->coords << " " << massCenter.coords << " " <<  result << std::endl;
			return result;
		}
		else
		{
			std::cout << "condition is not ok" << std::endl;
			Vector3D force(0.0, 0.0, 0.0);
			for(int i = 0; i < NUM_OF_CHILDREN; ++i)
			{
				if(children[i].get())
				{
					force += children[i]->computeForce(target, theta);
				}
			}
			std::cout << "return summ of forces of subtrees" << target->coords << " " << force << std::endl;
			return force;
		}
	}
}