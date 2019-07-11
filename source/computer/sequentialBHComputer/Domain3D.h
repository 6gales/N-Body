#pragma once

#include "../Vector3D/Vector3D.h"

enum Direction
{
	NORTH_WEST_UP = 0,
	NORTH_EAST_UP = 1,
	SOUTH_WEST_UP = 2,
	SOUTH_EAST_UP = 3,
	NORTH_WEST_DOWN = 4,
	NORTH_EAST_DOWN = 5,
	SOUTH_WEST_DOWN = 6,
	SOUTH_EAST_DOWN = 7
};


class Domain3D
{
public:
	double left,
			right,
			bottom,
			top,
			up,
			down;


	Domain3D(){}
	Domain3D(const double &l, const double &r,
			 const double &b, const double &t,
			 const double &d, const double &u) :
			 left(l), right(r), bottom(b), top(t), down(d), up(u) {}
	Domain3D(const Vector3D &first, const Vector3D &second)
	{
		if(first.x > second.x)
		{
			left = second.x;
			right = first.x;
		}
		else
		{
			right = second.x;
			left = first.x;
		}

		if(first.y > second.y)
		{
			bottom = second.y;
			top = first.y;
		}
		else
		{
			top = second.y;
			bottom = first.y;
		}

		if(first.z > second.z)
		{
			down = second.z;
			up = first.z;
		}
		else
		{
			up = second.z;
			down = first.z;
		}

	}
	~Domain3D(){}

	double width() const
	{
		return right - left;
	}

	double height() const
	{
		return top - bottom;
	}

	double depth() const
	{
		return up - down;
	}

	Direction detectOctant(const Vector3D &point) const;
	Domain3D getOctant(Direction &d) const;

	void expand(const Vector3D &point);
};
