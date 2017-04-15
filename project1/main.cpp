// Name: Kevin Hsieh
// Quarter, Year: Fall 2014
// Project 1
//
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "point3d.h"
#include "index.h"
#include <cstdlib>

using namespace std;

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 800;

size_t coordinate_size = 0, triangleNum = 0; //for inputfile
const size_t ZBUFFMAX = 1000;
bool wireframe = false; //wireframe bool to toggle
string filename = "";

const float PI = 3.14159265;

Point3D Origin (0.0,0.0,0.0);
Point3D Center (0.0,0.0,0.0);

//points of all mouse click
vector < Point3D > points;
//points of all triangles to render
vector < Point3D > triangles;
//all indices of where the triangles are in points
vector < Index > indices;
//all colors in RGB
vector<int> colors;
//2D vector to store zBuffer
vector< vector<int> > zBuffer (ZBUFFMAX,vector<int> (ZBUFFMAX));

//initialize all to be inf
void setZBuffer()
{
    for(size_t i = 0; i < ZBUFFMAX; i++)
        for(size_t j = 0; j < ZBUFFMAX; j++)
            zBuffer.at(i).at(j) = INFINITY;
}

int randRGB(){
    int val = rand()% 256;
    return val+50;
}

//Converts the provided point, p, from screen coordinates to OpenGL coordinate system
Point3D GLscreenToWindowCoordinates(const Point3D & p)
{
    //Obtain conversion values
    GLdouble model[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    Point3D converted;
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
void renderLine(Point3D p0, Point3D p1)
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

void swap_Point3D(vector<Point3D> & triangles, size_t i,size_t j)
{
    Point3D tmp = triangles[i];
    triangles[i] = triangles[j];
    triangles[j] = tmp;
}

//bubble sort ftw!?
void sort_Points(vector<Point3D> & triangles)
{
    for(size_t i = 0; i < triangles.size(); i+=3)
    {
        for(size_t k = i; k < i+2;k++)
        {
            for(size_t j = i; j < i+2; j++)
            {
                if(points[j].y > triangles[j+1].y)
                    swap_Point3D(triangles,j, j+1);
            }
        }
    }
}

Point3D xProduct(const Point3D & v1, const Point3D & v2)//checked!
{
    Point3D result(0,0,0);
    result.x = ((v1.y*v2.z) - (v1.z*v2.y));
    result.y = ((v1.z*v2.x) - (v1.x*v2.z));
    result.z = ((v1.x*v2.y) - (v1.y*v2.x));
    return result;
}

float findZval(Point3D min, Point3D mid, Point3D max, float curX, float curY) //checked
{
    Point3D v1 = mid - min;
    Point3D v2 = max - min;

    Point3D XP = xProduct(v1,v2);

    float a = XP.x;
    float b = XP.y;
    float c = XP.z;

    if(XP.z == 0 ) return 0; 
    else
    {
        //plan equation magic...
        float d = -((a*min.x) + (b*min.y) + (c*min.z));
        return ((-a*curX - b*curY - d)/c) ; 
    }
}

void fillBottomHalfTri(Point3D bottom,Point3D mid, Point3D top, double m1, double m2)
{
    float xLeft = bottom.x - (m1*bottom.y);
    float xRight = bottom.x - (m2*bottom.y);


    if(xRight < xLeft) 
    {
        float tmp = xLeft;
        xLeft = xRight;
        xRight = tmp;
    }

    for(unsigned y = bottom.y; y <= top.y; y++)
    {
        float xL = (y*m1) + xLeft;
        float xR = (y*m2) + xRight;
        
        // if(xL > xR){
        //     float tmp = xL;
        //     xL = xR;
        //     xR = tmp;
        // }
        for(unsigned x=xL; x <= xR; x++)
        {
            // if(m1 == 0) x = xL;
            if(x <= WINDOW_WIDTH)
            {
                float z = findZval(bottom,mid,top,x,y);
                if(z < zBuffer[x][y]){
                    //Z buffer shit here!?
                    renderPixel(x,y);
                    zBuffer[x][y] = z;
                }           
            }
        }
    }

}

void fillTopHalfTri(Point3D bottom,Point3D mid, Point3D top, double m1, double m2)
{
    float xLeft = top.x - (top.y*m1);
    float xRight = top.x - (top.y*m2);

    if(xRight < xLeft) 
    {
        float tmp = xLeft;
        xLeft = xRight;
        xRight = tmp;
    }

    for(unsigned y = top.y; y >= bottom.y; y--)
    {
        float xL = (y*m1) + xLeft;
        float xR = (y*m2) + xRight;
       
        // if(xL > xR)
        // {
        //     float tmp = xL;
        //     xL = xR;
        //     xR = tmp;
        // }        
         // swap(xL,xR);
        for(unsigned x=xL; x <= xR; ++x)
        {
            // if(m1 == 0) x = xL;
            if(x <= WINDOW_WIDTH)
            {
                float z = findZval(bottom,mid,top,x,y);
                if(z < zBuffer[x][y]){
                    // Z buffer shit here!?
                    renderPixel(x,y);
                    zBuffer[x][y] = z;
                }
            }
        }
    }
}

void fillWholeTriangle(Point3D min, Point3D mid, Point3D max)
{
    setZBuffer();
    //calculate for slopes
    double m1 = 0.0;
    double m2 = 0.0;
    //fillbottom half 
    if( (max.y - min.y) != 0)
        m1 = (max.x - min.x) / (max.y - min.y);
    if( (mid.y - min.y) != 0)
        m2 = (mid.x - min.x) / (mid.y - min.y);
    // if(m1 != 0 && m2 != 0)
    fillBottomHalfTri(min,max,mid,m1,m2);
    //fill top half
    m1 = 0.0;
    m2 = 0.0;
    if( (mid.y - max.y) != 0)
        m1 = (mid.x - max.x) / (mid.y - max.y);
    if( (min.x - max.x) != 0)
        m2 = (min.x - max.x) / (min.y - max.y);
    // if(m1 != 0 && m2 !=0)
    fillTopHalfTri(mid,min,max,m1,m2);

}

void renderAllTriangle()
{    

    sort_Points(triangles); // FIX SORT!!!
    unsigned i =0;
    for(size_t k = 0; k < triangles.size(); k+=3)
    {
        //draw Point3D
        Point3D min = triangles[k];
        Point3D mid = triangles[k+1];
        Point3D max = triangles[k+2];

        //pushback new color
        colors.push_back(randRGB());
        colors.push_back(randRGB());
        colors.push_back(randRGB());
        
        glColor3ub(colors[i],colors[i+1],colors[i+2]);
        renderLine(min,max);
        renderLine(min,mid);
        renderLine(mid,max);

        if(!wireframe)
            fillWholeTriangle(min,mid,max);
        i+=3; //increment i for colorz~
    }
}

void findCenter()
{
    Center.x = 0.0;
    Center.y = 0.0;
    Center.z = 0.0;

    for(size_t i = 0; i < coordinate_size; ++i)
    {
        Center.x += points[i].x;
        Center.y += points[i].y;
        Center.z += points[i].z;
    }

    Center.x /= coordinate_size;
    Center.y /= coordinate_size;
    Center.z /= coordinate_size;

    // cerr << "Center of Obj: (" << Center.x  << ", "
}

//rotation
void rotateObj()
{
    setZBuffer();
    double x;
    double z;
    int theta = 10; // rotate by 12deg
    for(size_t i = 0; i < triangles.size(); i+=3)
    {
        Point3D min = triangles[i];
        Point3D mid = triangles[i+1];
        Point3D max = triangles[i+2];
        //coposite transform...
        //translate to origin
        min -= Center;
        mid -= Center;
        max -= Center;

        // cout << "Min: " << min.x << ", " << min.y << ")" << endl
        //      << "Mid: " << mid.x << ", " << mid.y << ")" << endl
        //      << "Max: " << max.x << ", " << max.y << ")" << endl;

        //Rotate
        float q = theta*PI/180;

        x = (min.z*sin(q)) + (min.x*cos(q));
        z = (min.z*cos(q)) - (min.x*sin(q));
        min.x = x;
        min.z = z;

        x = (mid.z*sin(q)) + (mid.x*cos(q));
        z = (mid.z*cos(q)) - (mid.x*sin(q));
        mid.x = x;
        mid.z = z;

        x = (max.z*sin(q)) + (max.x*cos(q));
        z = (max.z*cos(q)) - (max.x*sin(q));
        max.x = x;
        max.z = z;

        //translate back to og position
        min += Center;
        mid += Center;
        max += Center;

        triangles[i] = min;
        triangles[i+2] = mid;
        triangles[i+3] = max;
    }
}   


void getFileInput(string filename)
{

    ifstream input;
    input.open(filename.c_str());
    // input.open("samplecube.txt");
    double x, y, z;
    int t1, t2, t3; 

    input >> coordinate_size >> triangleNum;
    for(size_t i = 0; i < coordinate_size; i++)
    {
        input >> x >> y >> z;
        points.push_back(Point3D(x,y,z));
    }

    //get indecies and push into Triangles vector
    for(size_t i = 0; i < triangleNum; i++)
    {
        input >> t1 >> t2 >> t3;
        triangles.push_back(points[t1]);
        triangles.push_back(points[t2]);
        triangles.push_back(points[t3]);
    }
    input.close();
    // fileLoad = true;
}

//rotate 3d model
void GL_MousePress(int button, int state, int x, int y)
{
    // if(state == GLUT_DOWN) //&& button == GLUT_LEFT_BUTTON)
    // {
    //     rotateObj();
    //     glutPostRedisplay();
    // }
}

void GL_KeyPress(unsigned char key, int x, int y)
{
    if(key =='0') // 0 to clear screen
    {
        points.clear();
        triangles.clear();
        indices.clear();
        colors.clear();
        glutPostRedisplay();
        cout << "Cleaering screen..." << endl;
    }
    if(key == 'w') // w to toggle wireframe
    {
        wireframe = !wireframe;
        glutPostRedisplay();
        cout <<"Wireframe Toggle: " << wireframe << endl; 
    }
    if(key == 'r') // r to reLoad file
    {
        //clearscreen first
        points.clear();
        triangles.clear();
        indices.clear();
        colors.clear();
        //reload the file and redisplay
        getFileInput(filename);
        glutPostRedisplay();
        cout << "Reloading file..." << endl;
    }
    if(key == 'a') // a to rotate 
    {
        rotateObj();
        glutPostRedisplay();
        cout << "Rotating Left." << endl;
    }
    if(key == 27) // 'esc' to close program
    {
        cout << "\nClosing program...\n" << endl;
        exit(0);
    }
}

//Output function to OpenGL Buffer
void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    setZBuffer();

    glPointSize(3.0);
    glColor3f(1.0,0.0,0.0);
    // for(vector<Point3D>::iterator i = points.begin(); i != points.end(); ++i)
    //     renderPixel(i->x, i->y);
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

void checkVector()
{
    for(unsigned i = 0; i < triangles.size(); ++i)
    {    
        cout << "====triangles["<<i<<"]====" <<endl;
        cout << "(" << triangles[i].x << ", " << triangles[i].y << ", " <<triangles[i].z << ")" << endl;
    }
}

int main(int argc, char** argv)
{   
    GLInit(&argc, argv);
    //get file input
    if(argc > 1)
    {
        filename = argv[1];
        getFileInput(filename);
        // checkVector();
    }
    else{
        cerr << "\nERROR: Usage: ./proj1.out FileNameToLoad.txt\n" << endl;
        exit(1);
    }

    findCenter();
    glutMainLoop();

    return 0;
}
