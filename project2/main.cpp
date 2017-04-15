// Name: Kevin Hsieh
// Quarter, Year: Fall 2014
// Assignment 2
//
// main.cpp
////////////////////////////////////////////////////////////
#include "triangle.h"
#include <vector>
#include <GL/glut.h>
#include <fstream>
#include <iostream>
#include "color.h" //provided for Lab 8


using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float VIEW_LEFT = 0.0;
const float VIEW_RIGHT = WINDOW_WIDTH;
const float VIEW_BOTTOM = 0.0;
const float VIEW_TOP = WINDOW_HEIGHT;
const float VIEW_NEAR = -400;
const float VIEW_FAR = 400;



void renderPixel(int x, int y, Color3d& color, float sz = 1.0)
{
	glPointSize(sz);
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

//paul's renderline function
void renderLine(Vector3 p0, Vector3 p1, Color3d& color)
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
        renderPixel((int)(x + 0.5), (int)(y + 0.5), color);
        x += stepx;
        y += stepy;
    }
}

/*START HERE*/

//my global vars

bool xPress = true;
vector <Vector3> points;
vector <Tri> triangles;
size_t point_size = 0;
size_t tri_num = 0;
string filename = "";

Vector3 originOfLightRay(0,0,0);
Vector3 directionOfLightRay(1,1,1);
const Ray light(originOfLightRay, directionOfLightRay);

Vector3 originOfViewerRay(WINDOW_WIDTH/2, WINDOW_HEIGHT/2,0);
Vector3 directionOfViewerRay(1,1,1);
const Ray viewer(originOfViewerRay,directionOfViewerRay);

//Assignment 2

void drawFrame()
{
    Color3d frameColor(1,0,0);
    for(int i = 0; i < tri_num; i++)
    {
        Vector3 point1 = triangles[i].p1;
        Vector3 point2 = triangles[i].p2;
        Vector3 point3 = triangles[i].p3;

        renderLine(point1,point2,frameColor);
        renderLine(point2,point3,frameColor);
        renderLine(point3,point1,frameColor);
    }
}


void pixelOn(int x, int y)
{
    Ray currentLight(Vector3(x,y,0.0), Vector3(0.0,0.0,1.0));
    for(int t = 0; t < tri_num; t++)
    {
        float t;
        if(intersects(currentLight,triangles[t],t))
        {
            Vector3 N = triangles[t].findNormal();
            Color3d C(1,0,0);
            renderPixel(x,y,C);
        }
    }
}

void Flat(const Ray& light)
{
    cerr << "calling FLAT!" << endl;
    for (int i = 0; i < WINDOW_WIDTH; i++)
    {
        for(int j = 0; j < WINDOW_HEIGHT; j++)
        {
            pixelOn(i,WINDOW_HEIGHT - j);
        }
    }
    cerr << "FLAT endinig..." << endl;
}

void Phong() //pass in obj, need all triangle points and vertieces
{
    cerr << "calling PHONG!" << endl;
    // define I, illumination

    //ambient + diffusion + specular constants...
    float ks = 10;
    float kd = 20;
    float ka = 100;
    float alph = 75;

    Vector3 Ld(1,1,1);
    Vector3 La(1,1,1);
    Vector3 Ls(1,1,1);


    //Ip = ka*ia + kd(Lm dot N)im,d + ks(Rm dot V)^alph im,s
    Vector3 L;      //light;
    Vector3 N;      //normal;
    Vector3 H;      //reflect;
    Vector3 V;      //viewer;

    for(int i = 0; i < WINDOW_WIDTH; ++i)
    {
        for(int j = 0; j < WINDOW_HEIGHT; ++j)
        {
            for(int k = 0; k < tri_num; ++k)
            {
                Tri curTri = triangles[k];
                Ray currentRay(Vector3(i,j,0.0),Vector3(0.0,0.0,1.0));
                float t = -1.0;
                if(intersects(currentRay,curTri,t))
                {
                    Vector3 currentPoint(i,j,0.0);
                    N = curTri.findNormal();
                    L = originOfLightRay - currentPoint;
                    V = originOfViewerRay - currentPoint;
                    H = (L + V)/((L+V).magnitude());
                    Vector3 tmp = (Ld*kd*(L.dot(N)))+ (Ls*ks* pow((H.dot(N)),alph)) + La*ka;
                    Color3d I(tmp.x,tmp.y, tmp.z);
                    renderPixel(i,j,I);
                }
            }
        }
    }

    cerr << "PHONG ending..." << endl;
}

void GL_KeyPress(unsigned char key, int x, int y);
void GLrender();
void getFileInput(string filename);
void loadFloor();

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Project 2 - Kevin Hsieh");
	glutDisplayFunc(GLrender);
	// glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, VIEW_NEAR, VIEW_FAR);
    glutKeyboardFunc(GL_KeyPress);
}

int main(int argc, char** argv)
{
	GLInit(&argc, argv);
    if(argc > 1)
    {
        filename = argv[1];
        getFileInput(filename);
    }
    else{
        cerr << "\nERROR\n>Usage: ./rayTracer.out filename.txt\n" << endl;
        exit(1);
    }

	glutMainLoop();
	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT);
    drawFrame();
	glFlush();	
	glutSwapBuffers();
}

void getFileInput(string filename)
{
    ifstream input;
    input.open(filename.c_str());
    float x=0, y=0, z=0;

    int t1, t2, t3;

    input >> point_size >> tri_num;

    //get all coordinate points.
    for(size_t i = 0; i < point_size; i++)
    {
        input >> x >> y >> z;
        points.push_back(Vector3(x,y,z));
    }

    //sort though each vertices index to have triangle vertices right next to each other.
    for(size_t i = 0; i < tri_num; i++)
    {
        input >> t1 >> t2 >> t3;
        //sort through each point by highest y val
        if(points[t1].y < points[t2].y)
            swap(t1,t2);
        if(points[t2].y < points[t3].y)
            swap(t2,t3);


        Vector3 p1 = points[t1];
        Vector3 p2 = points[t2];
        Vector3 p3 = points[t3];

        Tri newTri(p1,p2,p3);
        triangles.push_back(newTri);
    }
    //close file stream
    input.close();
}

void GL_KeyPress(unsigned char key, int x, int y)
{
    if(key =='0') // 0 to clear screen
    {
        points.clear();
        triangles.clear();
        glutPostRedisplay();
        cerr << "Cleaering screen..." << endl;
    }
    else if(key == 'r') // r to reLoad file with the same input file
    {
        //clearscreen first
        points.clear();
        triangles.clear();
        //reload the file and redisplay
        getFileInput(filename);
        glutPostRedisplay();
        cerr << "Reloading file..." << endl;
    }
    else if(key == 'x' || key == 'X')//cycle though render modes.
    {
        cerr << "X is pressed! " << endl;
        xPress = !xPress;
        if(xPress)
            Flat(light);
        else
            Phong();

        glutPostRedisplay();
    }
    else if(key == 27) // 'esc' to close program
    {
        cerr << "\nClosing program...\n" << endl;
        exit(0);
    }
}

void loadFloor()
{
    cerr << "Loading Floor..." << endl;
    Color3d floorColor(1,0,0);
    Vector3 f1(WINDOW_WIDTH,0,WINDOW_WIDTH);
    Vector3 f2(0,0,WINDOW_WIDTH);
    Vector3 f3(WINDOW_WIDTH,0,0);
    Vector3 f4(0,0,0);

    renderLine(f1,f2,floorColor);
    renderLine(f2,f3,floorColor);
    renderLine(f3,f1,floorColor);
    renderLine(f1,f4,floorColor);
    renderLine(f3,f4,floorColor);

}
