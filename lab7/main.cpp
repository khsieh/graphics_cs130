// Name: Kevin Hsieh
// Quarter, Year: Fall 2014
// Lab: 7
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <cmath>
#include <vector>
#include <GL/glut.h>
#include <iostream>

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float VIEW_LEFT = 0.0;
const float VIEW_RIGHT = WINDOW_WIDTH;
const float VIEW_BOTTOM = 0.0;
const float VIEW_TOP = WINDOW_HEIGHT;
const float VIEW_NEAR = -400;
const float VIEW_FAR = 400;

// Retrieve random float
float randFloat()
{
	return rand() / static_cast<float>(RAND_MAX);
}
float randFloat(float min, float max)
{
	return randFloat() * (max - min) + min;
}

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

// A simple wrapper to store colors
struct Color3d
{
	float r;
	float g;
	float b;
	float a;
    
	Color3d()
    : r(0.0), g(0.0), b(0.0), a(1.0)
	{}
	Color3d(float r, float g, float b, float a = 1.0)
    : r(r), g(g), b(b), a(a)
	{}
};

void renderPixel(int x, int y, Color3d& color, float sz = 1.0)
{
	glPointSize(sz);
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


struct Sphere
{
    Vector3 center;
    float radius;
    Color3d color;
    Sphere() : radius(1.0) {}
    Sphere(const Vector3& cent, const float& R, const Color3d& col)
    {
        if (R > 0.0) {radius = R;} else {radius = 1.0;}
        center = cent;  color = col;
    }
};

vector<Sphere> spheres;

void loadSpheres()
{
    /* TODO
     Add some spheres to the "spheres" vector.*/
    // //first sphere
    // Vector3 P1(randFloat(0,800),randFloat(0,800),randFloat(0,800));
    // Color3d C1(randFloat(0,1),randFloat(0,1),randFloat(0,1));
    // Sphere S1(P1,randFloat(1,5),C1);
    // spheres.push_back(S1);
    // //second sphere
    // Vector3 P2(randFloat(0,800),randFloat(0,800),randFloat(0,800));
    // Color3d C2(randFloat(0,1),randFloat(0,1),randFloat(0,1));
    // Sphere S2(P2,randFloat(1,5),C2);
    // spheres.push_back(S2);
    // //third sphere
    // Vector3 P3(randFloat(0,800),randFloat(0,800),randFloat(0,800));
    // Color3d C3(randFloat(0,1),randFloat(0,1),randFloat(0,1));
    // Sphere S3(P3,randFloat(1,5),C3);
    // spheres.push_back(S3);

    // Vector3 P1(200,200,300);
    // Color3d C1(0.2,0.5,0.5);
    // Sphere S1(P1,50,C1);
    // spheres.push_back(S1);
    Vector3 center(400,400,300);
    Color3d col(1,1,0);
    spheres.push_back(Sphere(center,100,col));
    spheres.push_back(Sphere(Vector3(400,300,400),90,Color3d(0,0,1)));
    spheres.push_back(Sphere(Vector3(400,500,500),110,Color3d(1,0,0)));

}

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

bool rayIntersectsSphere(const Ray& ray, const Sphere& sphere, float * t_small, float * t_large)
{
    //Ray can be written origin + direction * t where t >= 0
    //Sphere can be written as (x - center.x)^2 + (y - center.y)^2 + (z - center.z)^2 = radius^2
    //Need to determine where these equations coincide.  If
    
    if (t_small != NULL) {*t_small = -1.0;} //These default values mean these values are invalid.
    if (t_large != NULL) {*t_large = -1.0;}
    
    /*
     TODO
     The ray is written as origin + direction * t where t >= 0.
     Write a function to solve for t_small <= t_large.  These are the different values of t
     for which the ray intersects the sphere.  These t-values are solutions to a quadratic equation
     which will have 0, 1, or 2 real solutions.  If there are 0 solutions, return false.
     If neither t_small nor t_large are >= 0, return false.  Otherwise, return true.
     */
    
    // if((t_small != NULL && *t_small < 0) || ( t_large != NULL && *t_large < 0)) 
    //     return false;
    //calc t values...
    Vector3 v = ray.direction;
    float a = (v.magnitude()) * (v.magnitude());

    Vector3 temp = ray.origin - sphere.center;
    Vector3 tempV = v*2;
    float b = tempV.dot(temp);

    float c = (temp.magnitude()*temp.magnitude()) - (sphere.radius*sphere.radius);
    
    float temp2 = ((b*b) - (4*a*c));
    // cout << "====\na: " << a << endl;
    // cout << "b: " << b << endl;
    // cout << "c: " << c << endl;

    if(temp2 < 0) return false;
    if(a != 0)
    {
        *t_small = (-b - sqrt(temp2) / (2*a));
        *t_large = (-b + sqrt(temp2) / (2*a));
    }
    else
    {
        *t_small = 0;
        *t_large = 0;
    }
    
    if(*t_small <= *t_large)
    {
        if(*t_large >= 0) return true;
    } 
    return false;
    
}

//Returns -1 if no sphere should appear in pixel, otherwise returns index of sphere.
int pixelOn(float pixel_x, float pixel_y)
{
    Ray current(Vector3(pixel_x, pixel_y, 0.0), Vector3(0.0, 0.0, 1.0));
    int num_spheres = (int) spheres.size();
    float min = -1.0;
    int min_i = -1;
    float calculatedT = 0.0;
    for (int i = 0; i < num_spheres; i++)
    {
        /*
         TODO
         Fill this in.
         The loop should make min_i the index of the first sphere intersected by current ray.
         If none exists, min_i will stay = -1.  Use the float min to keep track of smallest
         nonnegative t-value.
         */
         float * t_min = new float;
         float * t_max = new float;
        if(rayIntersectsSphere(current,spheres[i],t_min,t_max))
        {
            if( (t_min && t_max) && (*t_min < min || min_i == -1))
            {
                min = *t_min;
                min_i = i;
            }
        }
        
        // if((calculatedT < min) || min == -1)//if Ray intersects smallest sphere...
        // {
        //     min = calculatedT;
        //     min_i = i;
        // }
        delete t_min;
        delete t_max;
    }
    return min_i;
}

void GLrender();

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Lab 7 - Kevin Hsieh");
	glutDisplayFunc(GLrender);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, VIEW_NEAR, VIEW_FAR);
}

int main(int argc, char** argv)
{
	GLInit(&argc, argv);
    loadSpheres();
	glutMainLoop();
	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < WINDOW_WIDTH; i++)
    {
        for (int j = 0; j < WINDOW_HEIGHT; j++)
        {
            int which_sphere = pixelOn(i, 800 - j);
            if (which_sphere >= 0)
            {
                renderPixel(i, 800 - j, spheres[which_sphere].color);
            }
        }
    }
    
	glFlush();	
	glutSwapBuffers();
}

