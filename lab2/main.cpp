// Name: Kevin Hsieh
// Quarter, Year: Fall 2014
// Lab: 2
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include "point2d.h"

using namespace std;

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

//points of all mouse click
vector < Point2D > points;


//test points for cubic bezier
Point2D p1(100,100);
Point2D p2(175,400);
Point2D p3(225,275);
Point2D p4(250,200);
//cubic bezier vector
vector < Point2D > cubicBezier(21);


//Converts the provided point, p, from screen coordinates to OpenGL coordinate system
Point2D GLscreenToWindowCoordinates(const Point2D & p)
{
 //Obtain conversion values
 GLdouble model[16];
 glGetDoublev(GL_MODELVIEW_MATRIX, model);
 GLdouble projection[16];
 glGetDoublev(GL_PROJECTION_MATRIX, projection);
 GLint viewport[4];
 glGetIntegerv(GL_VIEWPORT,viewport);
    
 Point2D converted;
 GLdouble temp;
 //viewport[3] - p.y because of the flip on the y axis
 gluUnProject(p.x, viewport[3] - p.y, 0, model, projection, viewport,
                 &converted.x, &converted.y, &temp);
 return converted;
}

void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

//Paul's renderLine function with a little modification...
// void renderLine(int x0, int y0, int x1, int y1)
void renderLine(Point2D p0, Point2D p1)
{
    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    
    int steps;
    if (abs((float)dx) > abs((float)dy))
        steps = abs((float)dx);
    else
        steps = abs((float)dy);

    float stepx = static_cast<float>(dx) / steps;
    float stepy = static_cast<float>(dy) / steps;

    float x = p0.x;
    float y = p0.y;
    for (int i = 0; i <= steps; ++i) {
        renderPixel((int)(x + 0.5), (int)(y + 0.5));
        x += stepx;
        y += stepy;
    }
}

//math helper functions
float factorial(float n){
    if(n <= 1)
        return 1;
    float f = 1;
    for(double i = 1; i <= n; i++)
        f *= i;
    return f;
}

float choose(float a, float b){
    return factorial(a) / (factorial(b) *factorial(a - b));
}

Point2D calcBezierPt(double t)
{
    Point2D pt(0,0);
    size_t sz = points.size();
    for(size_t i = 0; i < sz; i++)
    {
        double a = choose((float) (sz-1), (float) i);
        double b = pow(t, (double)i);
        double c = pow((1-t), (double)(sz-1-i) );
        pt.x = pt.x + a * b * c * points[i].x;
        pt.y = pt.y + a * b * c * points[i].y;
    }
    return pt;
}

void bezierCurve()
{
    size_t sz = points.size();

    glColor3f(0.2, 0.5, 0.5);
    if(sz == 2)
        renderLine(points[0], points[1]);

    if(sz >= 3){
        double delT = 0.005;
        Point2D pInit = points[0];
        for(double t = 0; t <= 1; t+=delT)
        {
            Point2D ptToDraw = calcBezierPt(t);
            renderLine(pInit,ptToDraw);
            pInit = ptToDraw;
        }
    }
    
}

void bezierCubic()
{
    for(unsigned i = 0; i <= 20; ++i)
    {
        double t = (double)i / 20;

        double a = pow((1-t),3);
        double b = 3 * t * pow((1-t), 2);
        double c = 3.0 * pow(t,2) * (1-t);
        double d = pow(t,3);

        double x = a*p1.x + b*p2.x + c*p3.x + d*p4.x;
        double y = a*p1.y + b*p2.y + c*p3.y + d*p4.y;
        glPointSize(2.0);
        glColor3f(0.5, 0.0, 0.5);
        renderPixel(x,y);
        Point2D newPt(x,y);
        cubicBezier[i] = newPt;
    }

    if(cubicBezier.size() > 2)
    {
        for(vector<Point2D>::iterator i = cubicBezier.begin(); i != cubicBezier.end();i++)
        {
            vector<Point2D>::iterator j = i + 1;
            if(j != cubicBezier.end())
                renderLine(*i,*j);
        }
    }
}

//collect points from mouse press into points vector
void GL_MousePress(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        points.push_back(GLscreenToWindowCoordinates( Point2D(x,y) ) );
        glutPostRedisplay();
    }
}

//Output function to OpenGL Buffer
void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    // glPointSize(3.5);
    // glColor3f(1.0, 0.0, 0.0);
    // renderPixel(p1.x,p1.y);
    // renderPixel(p2.x,p2.y);
    // renderPixel(p3.x,p3.y);
    // renderPixel(p4.x,p4.y);
    
    glPointSize(2.5);
    glColor3f(1.0, 0.0, 0.0);
    for(vector<Point2D>::iterator i = points.begin(); i != points.end(); i++)
    {
        renderPixel(i->x, i->y);
    }

    glPointSize(1.0);
    bezierCurve(); 
    // bezierCubic();

    glutSwapBuffers();
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    glutCreateWindow("CS 130 - <Kevin Hsieh>");
    
    // The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
    // For the purposes of this lab, this is set to the number of pixels
    // in each dimension.
    glMatrixMode(GL_PROJECTION_MATRIX);
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glutDisplayFunc(GL_render);
    glutMouseFunc(GL_MousePress);
}

int main(int argc, char** argv)
{   
    GLInit(&argc, argv);
    glutMainLoop();

    return 0;
}
