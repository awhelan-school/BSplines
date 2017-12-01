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

#include "FileIO.h"
#include "Bezier.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <list>
#include <cstddef>
#define GLUT_KEY_TAB 9

using namespace std;

typedef struct{
    float x,y;
    float max, min;
    
}vk;
//translated point pointer
static vpt *t;
vk *knot_ptr;
float click;
float Gratio;

//original point for deletion
vpt *t_original;

list<vpt> ctrPts;
vector< list<vpt> > Curves;
vector< vector<float> > Knots;
vector< list<vk> > VK; // virtual Knots

//AddMode
bool AddMode = false;
bool ModifyMode = false;
bool InsertMode = false;
bool KnotMode = false;

bool CHull = true;
//Working Curve
int CurrentC;

//deBoor Order K (default = 3)
int order = 3;

//Type of Curve to Use (BSpline=0 or Bezier=1)
int CType;



/****set in main()****/
//the number of pixels in the grid
float grid_width;
float grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;

float sx, sy;

/*Window information*/
float win_height;
float win_width;


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
    FileIO::read(Curves);

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
    //Initialize Curve knots
    if (Curves.size()) {
        float i = 0;
        int index = 0;
        Knots.resize(Curves.size());
        //Calculate Knots

        for (auto c : Curves) {

            for (i = 0; i < order + c.size(); i++) {
                Knots.at(index).push_back(i);
            }
            index++;
        }
    }

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
    VK.resize(Curves.size());
    
    int CID = 0;
    int index = 0;
    //clears the screen
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
    glLoadIdentity();




    //Display FrameBuffer
    for (auto c : Curves) {

        if (CType == 0)
            Bezier::bezier(c, c.size(), 1000);
        else
            Bezier::spline(order, Knots.at(index), c, c.size(), 1000);

        index++;
    }//displayAllCurves



    //Display Convex HULL
    glPointSize(1);
    glLineWidth(1);
    if (CHull)
        for (auto c : Curves) {
            int size = 0;

            for (auto itr = c.begin(); itr != c.end(); itr++) {
                size++;

                if (size < c.size()) {
                    glBegin(GL_LINES);
                    if (CurrentC == CID)
                        glColor3f(0.0, 1.0, 0.0);
                    else
                        glColor3f(0.0, 0.0, 1.0);
                    glVertex2f(itr->x, itr->y);
                    glVertex2f(next(itr)->x, next(itr)->y);
                    glEnd();
                }
            }//SingleCurve
            CID++;
        }//displayAllCurves

    //Display knotBar
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0);

    //Menu Separator
    glVertex2f(0, 30);
    glVertex2f(grid_width, 30);

    float center = grid_width / 2;

    //Right
    glVertex2f(center, 15);
    glVertex2f(center + Knots.at(CurrentC).size()*25, 15);

    //Left
    glVertex2f(center - Knots.at(CurrentC).size()*30, 15);
    glVertex2f(center, 15);
    glEnd();

    int pt = 0;
    vk location;
    
    
    
    for (auto k : Knots.at(CurrentC)) {
        
        
        location.x = center - Knots.at(CurrentC).size()*25 + Knots.at(CurrentC).at(pt)*50;
        location.y = 15;
        location.max = location.x + 50;
        location.min = location.x - 50;
        VK.at(CurrentC).push_back(location);
        pt++;
    }
    
    //Draw Vitrual Knots
    for(auto itr = VK.at(CurrentC).begin(); itr != VK.at(CurrentC).end(); itr++){
       
        glPointSize(7);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(itr->x, 15);
        glEnd();
         
    }


    //Display Control Points
    glPointSize(10);
    if (CHull)
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
    //update the new window width and height
    win_width = width;
    win_height = height;

    //creates a rendering area across the window
    glViewport(0, 0, win_width, win_height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, win_width, 0, win_height, -10, 10);

    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    pixel_size = (float) width / grid_width;


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
    cout << "Curve ==> " << CurrentC << "\n";
    cout << "size ==>" << Curves.at(CurrentC).size() << "\n";
    cout << "*** Knots ***\n";

    if (ch == 'p') {
        for (auto i = 0; i < Knots.at(CurrentC).size(); i++) {
            cout << "  " << Knots.at(CurrentC)[i] << "  ";
        }

        AddMode = ModifyMode = InsertMode = false;
        KnotMode = true;
    }



    switch ((int) ch) {
    case GLUT_KEY_TAB:
        KnotMode = AddMode = ModifyMode = InsertMode = false;
        if (CurrentC == Curves.size() - 1) {
            CurrentC = 0; //Cycle Back
        } else {
            CurrentC += 1; //Next Curve
        }
        ctrPts = Curves.at(CurrentC);
        printf("User hit the 'TAB' key\n");
        break;
    default:
        //prints out which key the user hit
        printf("User hit the \"%c\" key\n", ch);
        break;
    }

    if (ch == 'a') {

        KnotMode = ModifyMode = InsertMode = false;

        AddMode = true;
        if (Curves.at(CurrentC).size() == 0) {
            ctrPts.clear(); // Clear Current
        }//Add_New
        else {
            ctrPts = Curves.at(CurrentC);
        }//Add_Existing

    }//AddMode

    if (ch == 'm') {
        //Modify Working Curve Points
        KnotMode = InsertMode = AddMode = false;

        ModifyMode = true;

    }//Modify Points

    if (ch == 'i') {
        //Insert Points 
        KnotMode = AddMode = ModifyMode = false;
        InsertMode = true;
    }

    if (ch == 'c') {

        if (ctrPts.size() > 1) {
            Curves.at(CurrentC) = ctrPts;
            CurrentC = Curves.size(); //New Curve @ Back
            Curves.resize(Curves.size() + 1);
            Knots.resize(Curves.size() + 1);
            ctrPts.clear(); // Clear Current
            Curves.at(CurrentC) = ctrPts;
        }
        KnotMode = AddMode = ModifyMode = InsertMode = false;
    }//Create_Curve_Push

    if (ch == 'h') {
        CHull = CHull ? false : true;
    }

    if (ch == 'k') {
        cout << "\nPlease Enter the new deBoor Order value: ";
        cin >> order;
    }

    if (ch == 'b') {

        CType = CType ? 0 : 1;
        if (CType == 0) {
            cout << "\nEvaluating using BSpline Curve\n";
        } else {
            cout << "\nEvaluating using Bezier Curve\n";
        }
    }


    //redraw the scene after keyboard input
    glutPostRedisplay();
}


//gets called when a mouse button is pressed

void mouse(int button, int state, int x, int y)
{
    bool Add = false;
    bool Delete = false;
    bool Insert = false;
    
    vpt pt;
    pt.x = (int) (x / pixel_size);
    pt.y = (int) ((win_height - y) / pixel_size);

    pt.ID = CurrentC;

    if (pt.y < 30 && KnotMode == false)
        return;

    //print the pixel location, and the grid location
    printf("MOUSE AT PIXEL: %d %d, GRID: %d %d\n", x, y, (int) (x / pixel_size), (int) ((win_height - y) / pixel_size));

    switch (button) {
    case GLUT_LEFT_BUTTON: //left button
        //Add Point
        if (AddMode)
            Add = true;
        else if (InsertMode)
            Insert = true;
        break;
    case GLUT_RIGHT_BUTTON: //right button   
        Delete = true;
        break;
    default:
        break;
    }
    if (state != GLUT_DOWN) {
        printf("BUTTON UP\n");

        if (ModifyMode) {
            Curves.at(CurrentC) = ctrPts;
        }//Remove Point after Modification

        if(KnotMode){
            Knots.at(CurrentC).at(click) += Gratio;
            VK.at(CurrentC).clear();
        }
    }//button released 


    else {
        printf("BUTTON DOWN\n");
        if (Add) {
            ctrPts.push_back(pt);
            Curves.at(CurrentC) = ctrPts;
        }//Add At End 
        else if (Insert) {
            for (auto itr = Curves.at(CurrentC).begin(); itr != Curves.at(CurrentC).end(); itr++) {
                if ((itr->x < pt.x && pt.x <= next(itr)->x) && (itr->y < pt.y && pt.y <= next(itr)->y)) {


                    Curves.at(CurrentC).insert(++itr, pt);
                    ctrPts = Curves.at(CurrentC);
                    break;
                } else if ((itr->x < pt.x && pt.x <= next(itr)->x) && (itr->y > pt.y && pt.y > next(itr)->y)) {

                    Curves.at(CurrentC).insert(++itr, pt);
                    ctrPts = Curves.at(CurrentC);
                    break;
                } else if ((itr->x > pt.x && pt.x > next(itr)->x) && (itr->y > pt.y && pt.y >= next(itr)->y)) {

                    Curves.at(CurrentC).insert(++itr, pt);
                    ctrPts = Curves.at(CurrentC);
                    break;
                } else if ((itr->x > pt.x && pt.x > next(itr)->x) && (itr->y < pt.y && pt.y <= next(itr)->y)) {

                    Curves.at(CurrentC).insert(++itr, pt);
                    ctrPts = Curves.at(CurrentC);
                    break;
                } else if (abs(itr->x - pt.x) < 3 && abs(next(itr)->x - pt.x) < 3) {

                    Curves.at(CurrentC).insert(++itr, pt);
                    ctrPts = Curves.at(CurrentC);
                    break;
                } else if (abs(itr->y - pt.y) < 3 && abs(next(itr)->y - pt.y) < 3) {

                    Curves.at(CurrentC).insert(++itr, pt);
                    ctrPts = Curves.at(CurrentC);
                    break;
                }
            }//Insert Point 

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
                } else {
                    t = nullptr;
                }
            }//Find Closet Point
        }//Modify Point
    }//button clicked


    if (AddMode || ModifyMode || Delete || InsertMode) {

        Knots.resize(Curves.size());
        Knots.at(CurrentC).clear();
        for (float i = 0; i < order + Curves.at(CurrentC).size(); i++) {
            Knots.at(CurrentC).push_back(i);
        }

        VK.at(CurrentC).clear();
    }
    else if(KnotMode){
        int index = 0;
        for(auto itr = VK.at(CurrentC).begin(); itr != VK.at(CurrentC).end(); itr++){
            
            if(abs(itr->x - pt.x) < 5 && abs(itr->y - pt.y) < 5){
                
                knot_ptr = &(*itr);
                click = index;
                
                break;
                
            }
            else{
                t = nullptr;
            }   
            index++;
        }  
    
    
    }



    FileIO::write(Curves);
    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene

void motion(int x, int y)
{
    int xpt = (int) (x / pixel_size);
    
    if (ModifyMode & t != nullptr) {
        t->x = (int) (x / pixel_size);
        t->y = (int) ((win_height - y) / pixel_size);
    }
    if(KnotMode && knot_ptr != nullptr){
        
        if(knot_ptr->max > xpt && xpt > knot_ptr->min){
            
            knot_ptr->x = (int) (x / pixel_size);
            
            if(xpt < (knot_ptr->max + knot_ptr->min)/2 ){
                
                float mid = (knot_ptr->max + knot_ptr->min)/2;
                float ratio = (xpt - knot_ptr->min) / 50;
                
                Gratio = (float)(-1 *(1-ratio));
                
            }//left interval
            else{
                float mid = (knot_ptr->max + knot_ptr->min)/2;
                float ratio = (knot_ptr->max - xpt) / 50;
                
                Gratio = (float)(1 *(1-ratio));   
            }//right interval
        }  
        else 
            return;
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
