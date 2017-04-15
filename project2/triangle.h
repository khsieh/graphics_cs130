// CS130 Fall 2013: Computer Graphics
// triangle.h
//
// You may modify this file and are encouraged to do so
/////////////////////////////////////////

#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <cmath>

// A simple wrapper for store 3D vectors
struct Vector3
{
	float x;
	float y;
	float z;
    
	Vector3() : x(0.0), y(0.0), z(0.0)
	{}
    
	Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
	{}
    
	Vector3(const Vector3 & v)
    : x(v.x), y(v.y), z(v.z)
	{}
    
	Vector3 operator+(const Vector3 & rhs) const
	{ return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator-(const Vector3 & rhs) const
	{ return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	
    Vector3 operator-(const float & rhs) const //my own stuff
	{ return Vector3(x - rhs, y - rhs, z - rhs); }
    
	Vector3 operator*(float rhs) const
	{ return Vector3(x * rhs, y * rhs, z * rhs); }
	Vector3 operator/(float rhs) const
	{ return Vector3(x / rhs, y / rhs, z / rhs); }
	Vector3 operator+=(const Vector3 & rhs)
	{ x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3 operator-=(const Vector3 & rhs)
	{ x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	Vector3 operator*=(float rhs)
	{ x *= rhs; y *= rhs; z *= rhs; return *this; }
	Vector3 operator/=(float rhs)
	{ x /= rhs; y /= rhs; z /= rhs; return *this; }
    
	float magnitude() const
	{ return sqrt(x * x + y * y + z * z); }
	void normalize()
	{ *this /= magnitude(); }
	Vector3 normalized() const
	{ return *this / magnitude(); }
	float dot(const Vector3 & rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	Vector3 cross(const Vector3 & rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y,
                       z * rhs.x - x * rhs.z,
                       x * rhs.y - y * rhs.x);
	}
};

struct Ray
{
    Vector3 origin;
    Vector3 direction;
    Ray() {Vector3 d(0.0, 0.0, 1.0);  direction = d;}
    Ray(const Vector3& o, const Vector3& dir)
    {
        origin = o;
        Vector3 d(0.0, 0.0, 1.0);
        float mag = dir.magnitude();
        if (mag > 0.0) {d = dir;}
        direction = d;
    }
};

struct Tri
{
	Vector3 p1;
	Vector3 p2;
	Vector3 p3;

	Tri() : p1(Vector3()), p2(Vector3()), p3(Vector3())
	{}
	Tri(const Vector3 & a, const Vector3 & b, const Vector3 & c)
	: p1(a), p2(b), p3(c)
	{}

	Vector3 findNormal()
	{
		Vector3 Normal;
		Vector3 d1 = p2 - p1;
        Vector3 d2 = p3 - p2;
        Normal = d1.cross(d2);
        Normal.normalized();
        return Normal;
	}

};


bool intersects(const Ray& ray, Tri& curT, float& t)
{
	// 	if(t_small != NULL) {*t_small = -1.0}
	// 	if(t_large != NULL) {*t_large = -1.0}
	Vector3 d1 = curT.p2 - curT.p1;
	Vector3 d2 = curT.p3 - curT.p1;
	Vector3 dir = ray.direction;
	Vector3 h = dir.cross(d2);
	float a = d1.dot(h);

	if(a > -0.00001 && a < 0.00001 )
		return false;
	float f = 1/a;
	Vector3 s = ray.origin - curT.p1;
	float u = f * s.dot(h);

	if( u < 0.0 || u > 1.0)
		return false;

	Vector3 q = s.cross(d1);
	float v = f * dir.dot(q);

	if(v < 0.0 || u + v > 1.0)
		return false;

	//pass all three cases: it does intersect
	//calculate t to see where it intersects with tri
	float tmp_t = f * d2.dot(q);
	if(tmp_t > 0.00001)
	{
		t = tmp_t;
		return true;
	}

	return false;
}

#endif
