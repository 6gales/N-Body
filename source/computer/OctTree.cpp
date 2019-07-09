#include "OctTree.h"

void OctTree::populate(std::vector<Particle>& parts, const Domain3D &domain)
{
	const Domain3D expandedDomain = getExpandedDomain(parts, domain);
	std::cout << "after expanded Domain" << std::endl;
	root.reset(new OctTreeNode(expandedDomain));
	std::cout << "after reset" << std::endl;
	for(int i = 0; i < parts.size(); ++i)
	{
		std::cout << "part[" << i << "] insertion " << parts[i].coords << " " << parts[i].mass << std::endl;
		root->insertPart(&parts[i]);
	}
	std::cout << "after insertion" << std::endl;
	root->computeMassCenter();
}

Vector3D OctTree::computeForce(Particle* _part, const double &theta) const
{
	std::cout << "compute Force in Tree" << std::endl;
	return root->computeForce(_part, theta);
}

Domain3D OctTree::getExpandedDomain(const std::vector<Particle> & parts, const Domain3D &domain) const
{
	Domain3D expandedDomain = domain;
	for(int i = 0; i < parts.size(); ++i)
	{
		expandedDomain.expand(parts[i].coords);
	}
	return expandedDomain;
}