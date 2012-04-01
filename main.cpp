#include <stdlib.h>
#include <glut.h>
#include "Simulator.h"
#include "Timing.h"
#include "windows.h"
#include <iostream>
#include <ctime>
using namespace std;

static Simulator *gSim = NULL;

// Display globals
int gMainHandle;
int mx = 0;
int my = 0;
time_t goalPauseTime;

// Camera globals
NxVec3 gCameraPos(0,5,-18);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 0.12;


// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
bool bPause = false;

void ProcessKeys()
{
	// Process keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }
		
		switch (i)
		{
			//case 'w':{ gCameraPos += gCameraForward*gCameraSpeed; break; }
			//case 's':{ gCameraPos -= gCameraForward*gCameraSpeed; break; }

			// Camera controls -- Constrain viewing area
			// vertical range: between 4.0 and 10.0
			// horizontal range: between -2.5 and 15.0
			case 'a':
				{
					if (gCameraPos[0] <= -2.5)
						gCameraPos -= gCameraRight*gCameraSpeed;
					break; 
				}
			case 'd':
				{
					if (gCameraPos[0] >= -16.0)
						gCameraPos += gCameraRight*gCameraSpeed;
					break; 
				}
			case 's':
				{
					if (gCameraPos[1] >= 4.0)
						gCameraPos -= NxVec3(0,1,0)*gCameraSpeed;
					break; 
				}
			case 'w':
				{ 
					if (gCameraPos[1] <= 10.0)
						gCameraPos += NxVec3(0,1,0)*gCameraSpeed;
					break; 
				}
			case 'z':
				{
					if (gCameraPos[1] >= 4.0)
						gCameraPos -= NxVec3(0,1,0)*gCameraSpeed;
					break; 
				}
			case 'q':
				{ 
					if (gCameraPos[1] <= 10.0)
						gCameraPos += NxVec3(0,1,0)*gCameraSpeed;
					break; 
				}
			case 'n':
				{ 
					gSim->ResetScene();
					gSim->CreateScene();
				}
		}
	}
}

void SetupCamera()
{
	// Setup camera
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x,gCameraPos.y,gCameraPos.z,gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 0.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderCallback()
{
	if (gSim->goal)
	{
		bPause = true;
	}
	else
	{
		bPause = false;
	}

    if (gSim && !bPause)
	{
        gSim->RunPhysics();
	}
	
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ProcessKeys();
	SetupCamera();
    if (gSim){
		gSim->RenderScene();
		gSim->ProcessKeys(gKeys);
	}
		
    glutSwapBuffers();
}

void ReshapeCallback(int width, int height)
{
    glViewport(0, 0, width, height);
}

void IdleCallback()
{
    glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;
	
	switch (key)
	{
		case 27 : { exit(0); break; }
		case 'p': { bPause = !bPause; getElapsedTime(); break; }
		default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;
}

void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
    my = y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !bPause) 
	{
		gSim->launch(mx,my,false);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && !bPause) 
	{
		gSim->launch(mx,my,true);
	}
}

void MotionCallback(int x, int y)
{
    int dx = mx - x;
    int dy = my - y;
    
	//DEBUG -- Remove in final version
    gCameraForward.normalize();
    gCameraRight.cross(gCameraForward,NxVec3(0,1,0));
	
    NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0,1,0));
    qx.rotate(gCameraForward);
    NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
    qy.rotate(gCameraForward);
	
    mx = x;
    my = y;
}

void ExitCallback()
{
	gSim->ReleaseNx();
}

void InitGlut(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(1280, 800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("Simulation");
	glutSetWindow(gMainHandle);
    glutDisplayFunc(RenderCallback);
    glutReshapeFunc(ReshapeCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutKeyboardUpFunc(KeyboardUpCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	
    // Setup default render states
	glClearColor(0.52f, 0.60f, 0.71f, 1.0f);  
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
	
    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.2f, 0.2f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 0.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}
 
int main(int argc, char** argv)
{
    InitGlut(argc, argv);
	
	gSim = new Simulator();
	// Initialize physics scene and start the application main loop if scene was created
	if (gSim->InitNx()){
		gSim->CreateScene();
		atexit(ExitCallback);
		glutMainLoop();
	}
	delete gSim;
	return 0;
}
