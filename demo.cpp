/*
 * Simple glut demo that can be used as a template for
 * other projects by Garrett Aldrich
 */


#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//other includes
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <list>
#include <cstddef>
#define GLUT_KEY_TAB 9

using namespace std;

typedef struct {
    float x, y;
    int ID; //Curve Points belong to
}vpt;

//translated point pointer
static vpt *t;

//original point for deletion
vpt *t_original;

list<vpt> ctrPts;
vector< list<vpt> > Curves;

//AddMode
bool AddMode = false;
bool ModifyMode = false;
bool InsertMode = false;

//Working Curve
int CurrentC;



/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;

/*Window information*/
int win_height;
int win_width;


void init();
void idle();
void display();
void draw_pix(int x, int y);
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void SpecialInput(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();

int main(int argc, char **argv)
{
    //initialize Working Curve
    CurrentC = 0;

    //the number of pixels in the grid
    grid_width = 800;
    grid_height = 600;

    //the size of pixels sets the inital window height and width
    //don't make the pixels too large or the screen size will be larger than
    //your display size
    pixel_size = 1;

    /*Window information*/
    win_height = grid_height*pixel_size;
    win_width = grid_width*pixel_size;

    /*Set up glut functions*/
    /** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(win_width, win_height);
    //windown title is "glut demo"
    glutCreateWindow("glut demo");



    /*defined glut callback functions*/
    glutDisplayFunc(display); //rendering calls here
    glutReshapeFunc(reshape); //update GL on window size change
    glutMouseFunc(mouse); //mouse button events
    glutMotionFunc(motion); //mouse movement events
    glutKeyboardFunc(key); //Keyboard events
    glutSpecialFunc(SpecialInput);
    glutIdleFunc(idle); //Function called while program is sitting "idle"

    //initialize opengl variables
    init();
    //start glut event loop
    glutMainLoop();
    return 0;
}

/*initialize gl stufff*/
void init()
{
    Curves.resize(1);
    //set clear color (Default background to white)
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //checks for OpenGL errors
    check();
}

//called repeatedly when glut isn't doing anything else

void idle()
{
    //redraw the scene over and over again
    glutPostRedisplay();
}

//this is where we render the screen

void display()
{
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();




    //Display FrameBuffer

    //   cout << "Size ==> " << Curves.size() << "\n";
    //   cout << "Curve ==> " << CurrentC << "\n";
    //    

    //Display Convex HULL
    glPointSize(1);
    glLineWidth(1);
    for (auto c : Curves) {
        int size = 0;
        for (auto itr = c.begin(); itr != c.end(); itr++) {
            size++;
            if (size < c.size()) {
                glBegin(GL_LINES);
                if (CurrentC == itr->ID)
                    glColor3f(0.0, 1.0, 0.0);
                else
                    glColor3f(0.0, 0.0, 1.0);
                glVertex2f(itr->x, itr->y);
                glVertex2f(next(itr)->x, next(itr)->y);
                glEnd();
            }
        }//SingleCurve
    }//displayAllCurves


    //Display Control Points
    glPointSize(10);
    for (auto c : Curves) {
        for (auto itr = c.begin(); itr != c.end(); itr++) {

            glBegin(GL_POINTS);
            glColor3f(1.0, 0.0, 0.0);
            glVertex2f(itr->x, itr->y);
            glEnd();
        }//Single Curve
    }//ForAllCurves


    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
    check();
}


//Draws a single "pixel" given the current grid size
//don't change anything in this for project 1

void draw_pix(int x, int y)
{
    glBegin(GL_POINTS);
    glColor3f(.2, .2, 1.0);
    glVertex3f(x + .5, y + .5, 0);
    glEnd();
}

/*Gets called when display size changes, including initial craetion of the display*/
void reshape(int width, int height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    win_width = width;
    win_height = height;

    //creates a rendering area across the window
    glViewport(0, 0, width, height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, grid_width, 0, grid_height, -10, 10);

    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    pixel_size = width / (float) grid_width;

    //set pixel size relative to the grid cell size
    glPointSize(pixel_size);
    //check for opengl errors
    check();
}

void SpecialInput(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP:
        printf("Up Arrow \n");

        break;
    case GLUT_KEY_DOWN:
        printf("Down Arrow \n");

        break;
    case GLUT_KEY_LEFT:
        printf("Left Arrow\n");

        break;
    case GLUT_KEY_RIGHT:
        printf("Right Arrow\n");

        break;
    }

    glutPostRedisplay();
}//Arrow-Keys

//gets called when a key is pressed on the keyboard

void key(unsigned char ch, int x, int y)
{

    switch ((int) ch) {
    case GLUT_KEY_TAB:
        if (CurrentC == Curves.size() - 1) {
            CurrentC = 0; //Cycle Back
        } else {
            CurrentC += 1; //Next Curve
        }
        printf("User hit the 'TAB' key\n");
        break;
    default:
        //prints out which key the user hit
        printf("User hit the \"%c\" key\n", ch);
        break;
    }

    if (ch == 'a') {

        ModifyMode = InsertMode = false;

        if (AddMode == true && ctrPts.size() > 1) {
            Curves.push_back(ctrPts);
            ctrPts.clear();
            CurrentC = Curves.size() - 1; //Next Curve
        }//Add new Curve

        AddMode = true;
    }//AddMode

    if (ch == 'm') {
        //Modify Working Curve Points
        InsertMode = AddMode = false;

        ModifyMode = true;

    }//Modify Points

    if (ch == 'i') {
        //Insert Points 


    }


    //redraw the scene after keyboard input
    glutPostRedisplay();
}


//gets called when a mouse button is pressed

void mouse(int button, int state, int x, int y)
{
    bool Add = false;
    bool Delete = false;
    vpt pt;
    pt.x = (int) (x / pixel_size);
    pt.y = (int) ((win_height - y) / pixel_size);
    pt.ID = CurrentC;

    //print the pixel location, and the grid location
    printf("MOUSE AT PIXEL: %d %d, GRID: %d %d\n", x, y, (int) (x / pixel_size), (int) ((win_height - y) / pixel_size));

    switch (button) {
    case GLUT_LEFT_BUTTON: //left button
        printf("LEFT ");
        //Add Point
        if (AddMode)
            Add = true;
        break;
    case GLUT_RIGHT_BUTTON: //right button   
        Delete = true;
        printf("RIGHT ");
        break;
    default:
        printf("UNKNOWN "); //any other mouse button
        break;
    }
    if (state != GLUT_DOWN) {
        printf("BUTTON UP\n");

        if (ModifyMode) {
            Curves.at(CurrentC) = ctrPts;    
        }//Remove Point after Modification
 
    }//button released 

    else {
        printf("BUTTON DOWN\n");
        if (Add) {
            ctrPts.push_back(pt);
            Curves.at(CurrentC) = ctrPts;
        } else if (Delete) {
            for (auto itr = Curves.at(CurrentC).begin(); itr != Curves.at(CurrentC).end(); itr++) {
                if (abs(itr->x - pt.x) < 5 && abs(itr->y - pt.y) < 5) {

                    Curves.at(CurrentC).erase(itr++);
                    ctrPts = Curves.at(CurrentC);
                }
            }//Remove Point 
        }//DeletePoint
        else if (ModifyMode) {
            for (auto itr = Curves.at(CurrentC).begin(); itr != Curves.at(CurrentC).end(); itr++) {
                if (abs(itr->x - pt.x) < 5 && abs(itr->y - pt.y) < 5) {
                    t = &(*itr);
                    break;
                }
                else{
                    t = nullptr;
                }
            }//Remove Point
        }
    }//button clicked

    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene

void motion(int x, int y)
{
    if (ModifyMode & t != nullptr) {
        t->x = (int) (x / pixel_size);
        t->y = (int) ((win_height - y) / pixel_size);       
    }

    ctrPts = Curves.at(CurrentC);
    //redraw the scene after mouse movement
    glutPostRedisplay();
}

//checks for any opengl errors in the previous calls and
//outputs if they are present

void check()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("GLERROR: There was an error %s\n", gluErrorString(err));
        exit(1);
    }
}
