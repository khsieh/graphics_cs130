// Name: Kevin Hsieh
// Quarter, Year: Fall 2014
// Lab: 1
//
// This file is to be modified by the student.
// main.cpp
////////////////////////////////////////////////////////////
#include <GL/glut.h>
#include <cmath>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Renders a quad at cell (x, y) with dimensions CELL_LENGTH
void renderPixel(int x, int y, float r = 1.0, float g = 1.0, float b = 1.0)
{
	glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void drawLine(float xi, float yi, float xf, float yf)
{
    
    float m = (yf - yi) / (xf - xi);
    if(m > 1){       
        float i = xi;
        float j = yi; 
        while(j <= yf){
            renderPixel(round(i), j);
            j = j+1;
            i = i + (1/m);
        }
    }
    else{
        float i = round(xi);
        float j = yi + m*(i-xi);
        while(i <= xf){
            renderPixel(i, round(j));
            i++;
            j+=m;
        }
    }
}

void drawCircle (float r, float c_x, float c_y){
    float i = r;
    float j = 0;
    float q = 1 - i;
    
    while( i >= j ){
        renderPixel(c_x + i, c_y + j);
        renderPixel(c_x + j, c_y + i);
        renderPixel(c_x - i, c_y + j);
        renderPixel(c_x - j, c_y + i);
        renderPixel(c_x - i, c_y - j);
        renderPixel(c_x - j, c_y - i);
        renderPixel(c_x + i, c_y - j);
        renderPixel(c_x + j, c_y - i);
        j++;
        if(q < 0)
            q += 2*j+1;
        else{
            i--;
            q += 2*(j-i+1);
        }
    }
    
}


//Output function to OpenGL Buffer
void GL_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    renderPixel(200, 100); // Part 1
    renderPixel(200, 101); // Part 1
    renderPixel(200, 99); // Part 1
    renderPixel(201, 100); // Part 1
    renderPixel(199, 100); // Part 1
    
    drawLine(0, 0, 800, 800); // part 2
    drawLine(0, 400, 800, 400); 
    drawLine(0, 800, 800, 0);
    drawLine(400, 0, 400, 800);
    
    drawCircle(250, 400, 400); // part 3
    
    glutSwapBuffers();
}

//Initializes OpenGL attributes
void GLInit(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
	glutCreateWindow("CS 130 - <Hello World>");
    
	// The default view coordinates is (-1.0, -1.0) bottom left & (1.0, 1.0) top right.
	// For the purposes of this lab, this is set to the number of pixels
	// in each dimension.
	glMatrixMode(GL_PROJECTION_MATRIX);
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glutDisplayFunc(GL_render);
}

int main(int argc, char** argv)
{	
	GLInit(&argc, argv);
	glutMainLoop();

	return 0;
}


//https://www.opengl.org/sdk/docs/

