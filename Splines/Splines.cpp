/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing This Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <glut.h>			// Header File For The Glaux Library
#include <vector>
#include <math.h>

#include "vector3.h"
#include "NeHeGL.h"
#include "ArcBall.h"												// NEW: ArcBall Header

#pragma comment( lib, "opengl32.lib" )								// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )									// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )									// Search For GLaux.lib While Linking
#pragma comment( lib, "glut32.lib" )								// Search For GLu32.lib While Linking

GL_Window*	g_window;
Keys*		g_keys;

///funct protos
double catmullrom(double t, double p0,double p1,double p2,double p3);
double bezier(double t, double p0,double p1,double p2,double p3);
double (*splineFunction)(double t, double p0,double p1,double p2,double p3) = &catmullrom;

using namespace std;


const float PI2 = 2.0*3.1415926535f;								// PI Squared

Matrix4fT   Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,				// NEW: Final Transform
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT   LastRot     = {  1.0f,  0.0f,  0.0f,					// NEW: Last Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

Matrix3fT   ThisRot     = {  1.0f,  0.0f,  0.0f,					// NEW: This Rotation
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

ArcBallT    ArcBall(640.0f, 480.0f);				                // NEW: ArcBall Instance
Point2fT    MousePt;												// NEW: Current Mouse Point
bool        isClicked  = false;										// NEW: Clicking The Mouse?
bool        isRClicked = false;										// NEW: Clicking The Right Mouse Button?
bool        isDragging = false;					                    // NEW: Dragging The Mouse?
float		rotY		= 0.0f;


std::vector<vector3> pointList;
std::vector<vector3> lineList;
vector3 trans(0,0,-10),minPt,maxPt;

double bezier(double t, double p0,double p1,double p2,double p3){
	double t2 = t*t;
	double t3 = t2 * t;
	return (0.16667 *(	t3	*	(-p0 +  3 * p1 + -3 * p2 + p3) +	\
						t2	*	(3 * p0 + -6 * p1 + 3 * p2) +		\
						t	*	(-3*p0 + 3*p2) +					\
						1	*	(p0 + 4*p1 + p2)));
}
double catmullrom(double t, double p0,double p1,double p2,double p3){
	double t2 = t*t;
	double t3 = t2 * t;
	return (0.5 *(    	(2 * p1) + (-p0 + p2) * t +(2*p0 - 5*p1 + 4*p2 - p3) * t2 +(-p0 + 3*p1- 3*p2 + p3) * t3));
}
void FillPointVector(){
	//Spiral
	int ht;
	pointList.push_back(vector3(100,0,100));
	for(ht = 0; ht < 200; ht+=30){
		pointList.push_back(vector3(100,ht,100));
		pointList.push_back(vector3(200,ht+10,0));
		pointList.push_back(vector3(100,ht+20,-100));
		pointList.push_back(vector3(0,ht+30,0));
	}
	pointList.push_back(vector3(0,ht,0));
	return;
	///Weird shape
	pointList.push_back(vector3(-500,700,10));
	pointList.push_back(vector3(-500,700,10));
	pointList.push_back(vector3(-300,12,10));
	pointList.push_back(vector3(-300,12,10));
	pointList.push_back(vector3(-100,12,10));
	pointList.push_back(vector3(-50,45,0));
	pointList.push_back(vector3(0,0,20));
	pointList.push_back(vector3(50,0,15));
	pointList.push_back(vector3(200,350,30));
	pointList.push_back(vector3(350,0,20));
	pointList.push_back(vector3(356,35,60));
	pointList.push_back(vector3(400,67,15));
	pointList.push_back(vector3(424,122,0));
	pointList.push_back(vector3(450,1,0));
	pointList.push_back(vector3(500,0,0));
	pointList.push_back(vector3(500,0,0));

	/*
	///Simple wave (2 crests, 2 troughs)
	pointList.push_back(vector3(10,100,0));
	pointList.push_back(vector3(10,100,0));
	pointList.push_back(vector3(100,0,0));
	pointList.push_back(vector3(200,200,0));
	pointList.push_back(vector3(300,100,0));
	pointList.push_back(vector3(300,100,0));*/

}

void BuildPath(){
	
	lineList.clear();

	printf("Point vector filled: %d\n",pointList.size());
	float granularity = 0.1f;
	printf("Filled line list...");

	vector3 min,max;
	for(int c = 0; c < pointList.size(); c++){
		if(c==0){
			min = max = pointList[c];
		} else {
			min = vector3::Min(min,pointList[c]);
			max = vector3::Max(max,pointList[c]);
		}
	}
	vector3 center = (max + min)/2.0f;
	for(int c = 0; c < pointList.size(); c++){
		pointList[c] -= center;
	}
	for(int c = 0; c < pointList.size()-3; c++){
		vector3 p0,p1,p2,p3;
		p0 = pointList[c];
		p1 = pointList[c+1];
		p2 = pointList[c+2];
		p3 = pointList[c+3];
		for(float i = 0.0; i <= 1.0; i+=granularity){
			vector3 pt;
			pt.x = splineFunction(i,p0.x,p1.x,p2.x,p3.x);
			pt.y = splineFunction(i,p0.y,p1.y,p2.y,p3.y);
			pt.z = splineFunction(i,p0.z,p1.z,p2.z,p3.z);

			if(i==0){
				minPt = maxPt = pt;
			} else {
				minPt = vector3::Min(minPt,pt);
				maxPt = vector3::Max(maxPt,pt);
			}

			lineList.push_back(pt);
		}
		printf(".");
	}
	printf("Complete[%d]\n",lineList.size());

	printf("Sample: %f %f %f\n",lineList[0].x,lineList[0].y,lineList[0].z);
	if(lineList.size() % 2 != 0 && lineList.size() != 0)lineList.pop_back();
}



BOOL Initialize (GL_Window* window, Keys* keys){
	g_window	= window;
	g_keys		= keys;

	FillPointVector();
	BuildPath();
	
	// Start Of User Initialization
    isClicked   = false;								            // NEW: Clicking The Mouse?
    isDragging  = false;							                // NEW: Dragging The Mouse?

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0f);	
	glShadeModel (GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return TRUE;										// Initialization Went OK
}
void Deinitialize (void){
}

void Draw(void)									// Here's Where We Do All The Drawing
{
	float scalar = 100.0f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(trans.x,trans.y,trans.z);
	glMultMatrixf(Transform.M);
	//glRotatef(rotY,0,1,0);

	glColor3f(0,1,0);
	for(int i = 0; i < pointList.size(); i++){
		glPushMatrix();
		
		glTranslatef(pointList[i].x/scalar,pointList[i].y/scalar,pointList[i].z/scalar);
		glutSolidSphere(0.05,10,10);
		glPopMatrix();
	}
	glColor3f(1,0,0);
	if(lineList.size()>=2){
		for(int i = 0; i < lineList.size()-1; i++){
			vector3 pt1 = lineList[i]/100.f;
			vector3 pt2 = lineList[i+1]/100.f;
			glBegin(GL_LINES);
				glVertex3f(pt1.x,pt1.y,pt1.z);
				glVertex3f(pt2.x,pt2.y,pt2.z);
			glEnd();
		}
	}
}

void main(){
	start_window();
}

void Update (DWORD milliseconds)									// Perform Motion Updates Here
{
	if (g_keys->keyDown ['W'])	
		trans.z += 1.0f;
	
	if (g_keys->keyDown ['S'])	
		trans.z -= 1.0f;

	if (g_keys->keyDown ['A'])	
		trans.x += 1.0f;
	if (g_keys->keyDown ['D'])	
		trans.x -= 1.0f;

	if (g_keys->keyDown ['B'])	{
		splineFunction = &bezier;
		BuildPath();
	}
	if (g_keys->keyDown ['C'])	{
		splineFunction = &catmullrom;
		BuildPath();
	}
	if (g_keys->keyDown [VK_ESCAPE] == TRUE)						// Is ESC Being Pressed?
		TerminateApplication (g_window);							// Terminate The Program

	if (g_keys->keyDown [VK_F1] == TRUE)							// Is F1 Being Pressed?
		ToggleFullscreen (g_window);								// Toggle Fullscreen Mode

    if (isRClicked)													// If Right Mouse Clicked, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }

    if (!isDragging)												// Not Dragging
    {
        if (isClicked)												// First Click
        {
			isDragging = true;										// Prepare For Dragging
			LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);								// Update Start Vector And Prepare For Dragging
        }
    }
    else
    {
        if (isClicked)												// Still Clicked, So Still Dragging
        {
            Quat4fT     ThisQuat;

            ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
            Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        }
        else														// No Longer Dragging
            isDragging = false;
    }
	//rotY+=1.0f;
}
