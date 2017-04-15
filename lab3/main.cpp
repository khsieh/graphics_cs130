// Name: Kevin Hsieh
// Quarter, Year: Fall 2014
// Lab: 3
//
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include "point2d.h"

using namespace std;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

//points of all mouse click
vector < Point2D > points;
vector<float> colors;

float randRGB(){return rand()% 256;}

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

void swap_Point2D(vector<Point2D> & points, size_t i,size_t j)
{
    Point2D tmp = points[i];
    points[i] = points[j];
    points[j] = tmp;
}

//bubble sort ftw!?
void sort_Points(vector<Point2D> & points)
{
    for(size_t i = 0; i < points.size(); i+=3)
    {
        for(size_t k = i; k < i+2;k++)
        {
            for(size_t j = i; j < i+2; j++)
            {
                if(points[j].y > points[j+1].y)
                    swap_Point2D(points,j, j+1);
            }
        }

        Point2D min = points[i];
        Point2D mid = points[i+1];
        Point2D max = points[i+2];

        if(mid.x > max.x && mid.y == max.y)
            swap(mid,max);
        if(min.x > mid.x && min.y == min.y)
            swap(min,mid);

        // cout << "Line " << i << endl;
        // cout << "Min y: " << min.y << " x: " << min.x <<endl;
        // cout << "Mid y: " << mid.y << " x: " << mid.x <<endl;
        // cout << "Max y: " << max.y << " x: " << max.x <<endl;
        // cout << "===================================="<< endl;

    }
}

void fillTopTriangle(Point2D min, Point2D mid, Point2D max)
{
     //calc for slopes
    float m1 = (max.y - min.y)/(max.x-min.x); //min to mid
    float m2 = (max.y - mid.y)/(max.x-mid.x); //min to max
    
    for(float yi = min.y; yi <= max.y; yi++)
    {
        float xLeft  = min.x + ((yi-min.y)/m1);
        float xRight = mid.x + ((yi-mid.y)/m2);
        for(float xi= xLeft; xi <= xRight; xi++)
        {
            renderPixel(xi,yi); //add color
        }
    }
}

void fillDownTriangle(Point2D min, Point2D mid, Point2D max)
{
     //calc for slopes
    float m1 = (mid.y - min.y)/(mid.x-min.x); //min to mid
    float m2 = (max.y - min.y)/(max.x-min.x); //min to max
    
    for(float yi = min.y; yi <= max.y; yi++)
    {
        float xLeft  = min.x + ((yi-min.y)/m1);
        float xRight = min.x + ((yi-min.y)/m2);
        for(float xi= xLeft; xi <= xRight; xi++)
        {
            renderPixel(xi,yi); //add color
        }
    }
}

Point2D findMidPt(Point2D min, Point2D mid, Point2D max)
{
    Point2D midPt (0.0,mid.y);

    float m = (max.y - min.y)/(max.x-min.x);
    float b = min.y - (min.x*m);

    midPt.x = (mid.y - b)/m;
    return midPt;
}

void fillWholeTriangle(Point2D min, Point2D mid, Point2D max)
{
    Point2D midPt = findMidPt(min,mid,max);
    if(midPt.x == min.x || midPt.x == max.x){
        if(min.x == midPt.x)
            fillTopTriangle(min,mid,max);
        else if(midPt.x == max.x)
            fillTopTriangle(min,mid,max);
    }
    else if(midPt.x > mid.x)
    {
        fillTopTriangle(mid,midPt,max);
        fillDownTriangle(min,mid,midPt);
    }
    else if(midPt.x < mid.x)
    {
        fillTopTriangle(midPt,mid,max);
        fillDownTriangle(min, midPt, mid);
    }
}

void renderAllTriangle()
{
    if(points.size() >= 3 && points.size() % 3 ==0)
    {
        float newRGB = randRGB();
        colors.push_back(newRGB);
        
        newRGB = randRGB();
        colors.push_back(newRGB);

        newRGB = randRGB();
        colors.push_back(newRGB);

        vector<Point2D>:: iterator iter = points.begin();
        sort_Points(points);
        unsigned i = 0;
        for(;iter != points.end(); iter+=3)
        {
            //draw wireframe
            Point2D midPoint(0.0,0.0);
            Point2D min = *iter;
            Point2D mid = *(iter+1);
            Point2D max = *(iter+2);
            
            glColor3ub(colors[i],colors[i+1],colors[i+2]);
            renderLine(min,max);
            renderLine(min,mid);
            renderLine(mid,max);

            glColor3ub(colors[i],colors[i+1],colors[i+2]);
            fillWholeTriangle(min,mid,max);

            i+=3;
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

void GL_KeyPress(unsigned char key, int x, int y) //zero out screen
{
    if(key =='0')
    {
        points.resize(0);
        glutPostRedisplay();
    }
}

//Output function to OpenGL Buffer
void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(2.5);
    glColor3f(1.0,0.0,0.0);
    for(vector<Point2D>::iterator i = points.begin(); i != points.end(); ++i)
        renderPixel(i->x, i->y);
    glPointSize(1.0);
    renderAllTriangle();
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
    glutMouseFunc(GL_MousePress);
    glutKeyboardFunc(GL_KeyPress);
    glutDisplayFunc(GL_render);

}

int main(int argc, char** argv)
{   
    GLInit(&argc, argv);
    glutMainLoop();

    return 0;
}
