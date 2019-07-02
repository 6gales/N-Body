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

	const Vector3D& operator+=(const Vector3D &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	const Vector3D& operator-=(const Vector3D &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	double module(const Vector3D &v)
	{
		return x * x + y * y + z * z;
	}

};


Vector3D operator*(const Vector3D &v, double mul)
{
	return Vector3D(v.x * mul, v.y * mul, v.z * mul);
}

Vector3D operator*(double mul, const Vector3D &v)
{
	return v * mul;
}
