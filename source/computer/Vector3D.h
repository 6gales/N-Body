#pragma once

class Vector3D
{
public:
	double  x,
		y,
		z;

	Vector3D() : x(0.0), y(0.0), z(0.0);
	Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z){}
	~Vector3D() {}

	void set(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	const Vector3D& operator=(double val)
	{
		x = val;
		y = val;
		z = val;
		return *this;
	}

	const Vector3D& operator=(const Vector3D &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	void operator+=(const Vector3D &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void operator-=(const Vector3D &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

};
