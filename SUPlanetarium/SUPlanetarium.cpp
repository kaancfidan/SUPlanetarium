#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <glut.h>

#define PI 3.1415926535897932384626433832795
#define m2AU 6.68458134e-12 // conversion between Astronomical Units and meters
#define AU2m 0.149598e12

#define SUN_GRAVFIELDRES 1 // Gravitational field resolution for parts not belonging to planet neihgbourhoods
#define SUN_NBHOOD 50 // Neighborhood for the whole solar system

#define OUTER_GRAVFIELDRES 0.2 // Gravitational field resolution for outer solar system planet neighborhoods
#define OUTER_NBHOOD 2.5 // Neighborhood dimensions for the outer solar system (per planet) in AU

#define INNER_GRAVFIELDRES 0.05 // Gravitational field resolution for inner solar system planet neighborhoods
#define INNER_NBHOOD 1.75 // Neihgborhood dimensions for the inner solar system (whole) in AU

#define GRAV_VISUAL_GAIN 1e6 // Conversion coefficient between g (gravitational acceleration)(m/s^2) and AU for sending the nodes down
#define PLANET_SIZE_GAIN 500
#define SUN_SIZE_GAIN 60

#include "planet.h"
//#include "ou.h"


using namespace std;

static int Width = 800, Height = 600;
static GLfloat nearPlane = 1;
static GLfloat farPlane = 1000;

static bool mouseZoom = false;
static float zoomFactor = 1.0;
static float prevZoomFactor = 1.0;

static float fov = 60.0;

static bool mouseOrbit = false;
static float orbitDegrees_aroundY = 0;
static float orbitDegrees_aroundX = 0;
static float prevorbitDegrees_aroundY = 0;
static float prevorbitDegrees_aroundX = 0;

static int mouseX = 0;
static int mouseY = 0;
static int mouseX_delta = 0;
static int mouseY_delta = 0;
static int mouseX_captured = 0;
static int mouseY_captured = 0;

static float cameraPos[3] = { 0, 10, 1  };
static float lookAt[3] = { 0, 0, 0 };
static float upVector[3] = { 0, 1, 0 };
static float lightPos[4] = { 0, 0, 0, 1 };

Planet planets[9];

GLUquadric* myQuad;

Date myDate(1, 11, 2011);
static int myHour = 0;

GLfloat sunDiffuse[] = {1, 1, 1};
GLfloat sunEmissive[] = {1, 1, 1};

GLfloat mercuryDiffuse[] = {1, 0.4, 0};
GLfloat mercuryEmissive[] = {0, 0, 0};

GLfloat venusDiffuse[] = {1, 0.8, 0};
GLfloat venusEmissive[] = {0, 0, 0};

GLfloat earthDiffuse[] = {0, 0.4, 1};
GLfloat earthEmissive[] = {0, 0, 0};

GLfloat marsDiffuse[] = {1, 0.1, 0};
GLfloat marsEmissive[] = {0, 0, 0};

GLfloat jupiterDiffuse[] = {0.74, 0.65, 0.45};
GLfloat jupiterEmissive[] = {0, 0, 0};

GLfloat saturnDiffuse[] = {0.94, 0.65, 0.65};
GLfloat saturnEmissive[] = {0, 0, 0};

GLfloat uranusDiffuse[] = {0, 0.90, 0.95};
GLfloat uranusEmissive[] = {0, 0, 0};

GLfloat neptuneDiffuse[] = {0, 0.15, 0.85};
GLfloat neptuneEmissive[] = {0, 0, 0};

static bool sunMass = true;

static int nbhoods[5][2]; // 5 neighborhoods, 2 dimension contains the top-left corner and the size of that nbhood.
						  // when the size is zero, nbhood is canceled.

float toDegs(float radians)
{
	return radians*(360/(2*PI));
}

float toRads(float degrees)
{
	return degrees*(2*PI/360);
}

const float lookAtAngle = toDegs(atan2(cameraPos[1],cameraPos[2]));

void Idle();
void Display();
void Reshape(GLint width, GLint height);
void mouseClicks(int button, int state, int x, int y);
void mouseActiveMove (int x,int y);
void mousePassiveMove(int x, int y);
void processNormalKeys(unsigned char key, int x, int y);
void InitGraphics();

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(Width,Height);
	glutCreateWindow("SUPlanetarium"); 
    
	glutDisplayFunc(Display);
	glutReshapeFunc (Reshape);
	glutMouseFunc(mouseClicks);
	glutMotionFunc(mouseActiveMove); 
	glutPassiveMotionFunc(mousePassiveMove); 
	glutKeyboardFunc(processNormalKeys);
	glutIdleFunc(Idle);

	InitGraphics();
	
	glutMainLoop();

	return 0;
}

void Idle()
{
	myHour += 4;

	if(myHour == 24)
	{
		myHour = 0;
		myDate++;
	}

	Display();
}

void Reshape(GLint width, GLint height)
{
	Width = width;
	Height = height;
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (float)Width / Height, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}

void mouseClicks(int button, int state, int x, int y) 
{
	if (state == GLUT_DOWN)
	{
		if(button == GLUT_MIDDLE_BUTTON && !mouseZoom)
		{
			mouseOrbit = true;

			mouseX_captured = x;
			mouseY_captured = y;
		}
		else if(button == GLUT_RIGHT_BUTTON && !mouseOrbit)
		{
			mouseZoom = true;

			mouseX_captured = x;
			mouseY_captured = y;
		}
	}
	else if(state == GLUT_UP && button == GLUT_MIDDLE_BUTTON)
	{
		mouseOrbit = false;	
		
		prevorbitDegrees_aroundX = orbitDegrees_aroundX;
		prevorbitDegrees_aroundY = orbitDegrees_aroundY;
	}
	else if(state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
	{
		mouseZoom = false;

		prevZoomFactor = zoomFactor;
	}
}


void mouseActiveMove (int x,int y)
{
	mouseX = x;
	mouseY = y;

	mouseX_delta = x - mouseX_captured;
	mouseY_delta = y - mouseY_captured;

	if (mouseOrbit)
	{
		orbitDegrees_aroundX = prevorbitDegrees_aroundX + mouseY_delta/2;

		orbitDegrees_aroundY = prevorbitDegrees_aroundY + mouseX_delta/2;

		if(orbitDegrees_aroundX < -lookAtAngle)
			orbitDegrees_aroundX = -lookAtAngle;
		else if(orbitDegrees_aroundX > 180-lookAtAngle)
			orbitDegrees_aroundX = 180-lookAtAngle;

		if(orbitDegrees_aroundY > 360)
			orbitDegrees_aroundY -= 360;
	}
	else if(mouseZoom)
	{
		zoomFactor = prevZoomFactor + (float)(mouseY_delta)/200;

		if(zoomFactor < 0.1)
			zoomFactor = 0.1;
	}
}

void mousePassiveMove(int x, int y)
{
	mouseX = x;
	mouseY = y;
}
void processNormalKeys(unsigned char key, int x, int y) 
{
	if (key==' ') 
	{
		sunMass = !sunMass;
	}
}


void InitPlanets()
{
	// Initializing planets
	// Mercury
	planets[1].InitKepler(48.3313,3.24587e-5,
						  7.0047,5.00e-8,
						  29.1241,1.01444e-5,
						  0.387098, 0,
		  				  0.205635,5.59e-10,
		   				  168.6562,4.0923344368);
	
	planets[1].InitPhysical(5.9736e24, 1.63083731e-5, 0, 0);

	// Venus
	planets[2].InitKepler(76.6799,2.4659e-5,
						  3.3946,2.75e-8,
						  54.8910,1.38374e-5,
						  0.72333, 0,
						  0.006773,-1.302e-9,
						  48.0052,1.602130224);
	
	planets[2].InitPhysical(4.8685e24, 4.04537494e-5, 0, 0);

	// Earth
	planets[3].InitKepler(348.73936,2.24384e-5,
						  0.00005,1.98e-8,
						  114.20783,2.15325e-5,
						  1.00000261, 0,
						  0.01671022,0.607e-9,
						  357.51716,1.061165);
	
	planets[3].InitPhysical(5.9736e24, 4.25874677e-5, 0, 0);

	// Mars
	planets[4].InitKepler(49.5574,2.11081e-5,
						  1.8497,-1.78e-8,
						  286.5016,2.92961e-5,
						  1.523688, 0,
						  0.093405,2.516e-9,
						  18.6021,0.5240207766);
	
	planets[4].InitPhysical(6.4185e23, 2.27021752e-5, 0, 0);

	// Jupiter
	planets[5].InitKepler(100.4542,2.76854e-5,
						  1.3030,-1.557e-7,
						  273.8777,1.64505e-5,
						  5.20256, 0,
						  0.048498, 4.469e-9,
						  19.8950,0.0830853001);
	
	planets[5].InitPhysical(1.8986e27, 47.7894089e-5, 0, 0);

	// Saturn
	planets[6].InitKepler(113.6634,2.38980e-5,
						  2.4886,-1.081e-7,
						  339.3939,2.97661e-5,
						  9.55475, 0,
						  0.055546, -9.499e-9,
						  316.967,0.0334442282);
	
	planets[6].InitPhysical(5.6846e26, 40.2866348e-5, 0, 0);

	// Uranus
	planets[7].InitKepler(74.0005,1.3978e-5,
						  0.7733,1.9e-8,
						  96.6612,3.0565e-5,
						  19.18171, -1.55e-8,
						  0.047318, 7.45e-9,
						  142.5905,0.011725806);
	
	planets[7].InitPhysical(8.6810e25, 17.0851215e-5, 0, 0);

	// Neptune
	planets[8].InitKepler(131.7806,3.0173e-5,
						  1.7700,-2.55e-7,
						  272.8461,-2.55e-7,
						  30.05826, 3.313e-8,
						  0.008606, 2.15e-9,
						  260.2471,0.005995147);
	
	planets[8].InitPhysical(1.0243e26, 16.5536972e-5, 0, 0);
}

void InitGraphics()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	myQuad = gluNewQuadric();

	InitPlanets();
}

void drawPlanet(GLfloat* diffuse, GLfloat* emissive, Planet &p, int sizeGain)
{
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissive);

	p.UpdatePosition(myDate, myHour);	

	p.DrawTrail(2, diffuse);

	glPushMatrix();
		glTranslatef(p.getY(), p.getZ(), p.getX());

		gluSphere(myQuad, sizeGain*p.getRadius(), 32, 32);
	glPopMatrix();
}

void drawCoords()
{
	glDisable(GL_LIGHTING);
	
	
	// Z
	glColor3f(0.0,0.0,1.0);
	gluCylinder(myQuad, 0.02, 0.02, 0.5, 16, 16);

	// Y
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		glColor3f(0.0, 1.0, 0.0);
		gluCylinder(myQuad, 0.02, 0.02, 0.5, 16 ,16);
	glPopMatrix();

	// X
	glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glColor3f(1.0, 0.0, 0.0);
		gluCylinder(myQuad, 0.02, 0.02, 0.5, 16 ,16);
	glPopMatrix();	

	glEnable(GL_LIGHTING);
}

double mag(double *vector)
{
	return sqrt((vector[0]*vector[0])+(vector[1]*vector[1])+(vector[2]*vector[2]));
}

void unitVec(double *vector, double *unitVector)
{
	double vectorMag = mag(vector);

	unitVector[0] = vector[0]/vectorMag;
	unitVector[1] = vector[1]/vectorMag;
	unitVector[2] = vector[2]/vectorMag;
}

void drawNBGravityField(Planet p, const float nbhood_size, const float nbhood_res)
{
	const int numNodes = (nbhood_size*2)/nbhood_res;

	double G = 6.674e-11; // Gravitational constant

	double g[3] = {0}; // total gravitational acceleration at the point of interest

	double planetPosVec[9][3]; // Position vectors for planets relative to the point of interest

	double unitPlanetPosVec[3];

	GLfloat **fieldGrid = NULL;

	fieldGrid = new GLfloat*[numNodes];

	for (int i = 0; i < numNodes; i++) fieldGrid[i] = new GLfloat[numNodes];

	int firstPlanet;

	if (sunMass) firstPlanet = 0;
	else firstPlanet = 1;

	for(int i = 0; i< numNodes; i++)
	{
		for(int j = 0; j< numNodes; j++)
		{
			g[0] = 0;
			g[1] = 0;
			g[2] = 0;	

			for(int m = firstPlanet; m < 9; m++) // Computes the planet position vectors relative to this point.
			{
				planetPosVec[m][0] = (planets[m].getY()- (i*nbhood_res - nbhood_size) - p.getY());
				planetPosVec[m][1] = (planets[m].getZ());
				planetPosVec[m][2] = (planets[m].getX()- (j*nbhood_res - nbhood_size) - p.getX());

				double posVecMag = mag(planetPosVec[m])*AU2m;

				unitVec(planetPosVec[m], unitPlanetPosVec);
				
				if (posVecMag <= 2e10) posVecMag = 2e10; // clipping if the node is closer to a center of a planet than xxx meters.
				
				// Sums the gravitational acceleration created by each planet
				g[0] = g[0] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[0];
				g[1] = g[1] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[1];
				g[2] = g[2] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[2];
			}

			fieldGrid[i][j] = GRAV_VISUAL_GAIN*mag(g);
		}
	}
	
	// This is the part for drawing the gravitational field
	for(int i = 0; i< numNodes -1; i++)
	{
		for(int j = 0; j< numNodes -1; j++)
		{
			glBegin(GL_LINES);
				glVertex3f(i*nbhood_res-nbhood_size + p.getY(), fieldGrid[i][j], j*nbhood_res-nbhood_size + p.getX());
				glVertex3f((i+1)*nbhood_res-nbhood_size + p.getY(), fieldGrid[i+1][j], j*nbhood_res-nbhood_size + p.getX());
			glEnd();

			glBegin(GL_LINES);
				glVertex3f(i*nbhood_res-nbhood_size + p.getY(), fieldGrid[i][j], j*nbhood_res-nbhood_size + p.getX());
				glVertex3f(i*nbhood_res-nbhood_size + p.getY(), fieldGrid[i][j+1], (j+1)*nbhood_res-nbhood_size + p.getX());
			glEnd();
		}
	}

	// despereately trying to prevent memory leak. seems to work. maybe overkill.
	for (int i = 0; i < numNodes; i++) delete fieldGrid[i];

	delete fieldGrid;

	fieldGrid = NULL;
}

void drawGravityField()
{
	const int numNodes = (SUN_NBHOOD*2)/SUN_GRAVFIELDRES + 1; 	// +-30AU range

	double G = 6.674e-11; // Gravitational constant

	double g[3] = {0}; // total gravitational acceleration at the point of interest

	double planetPosVec[9][3]; // Position vectors for planets relative to the point of interest

	double unitPlanetPosVec[3];

	GLfloat fieldGrid[numNodes][numNodes] = {0};

	bool dontDraw = false;

	glDisable(GL_LIGHTING);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // alpha blending
	glEnable(GL_BLEND);

	glColor4f(0, 0.7, 1, 0.3);

	// Draw the neighbourhood gravity fields
	drawNBGravityField(planets[0], INNER_NBHOOD, INNER_GRAVFIELDRES);
	drawNBGravityField(planets[5], OUTER_NBHOOD, OUTER_GRAVFIELDRES);
	drawNBGravityField(planets[6], OUTER_NBHOOD, OUTER_GRAVFIELDRES);
	drawNBGravityField(planets[7], OUTER_NBHOOD, OUTER_GRAVFIELDRES);
	drawNBGravityField(planets[8], OUTER_NBHOOD, OUTER_GRAVFIELDRES);

	int firstPlanet;

	if (sunMass) firstPlanet = 0;
	else firstPlanet = 1;

	for(int i = 0; i< numNodes; i++)
	{
		for(int j = 0; j< numNodes; j++)
		{
			dontDraw = false;

			g[0] = 0;
			g[1] = 0;
			g[2] = 0;	

			for(int m = firstPlanet; m < 9; m++) // Computes the planet position vectors relative to this point.
			{
				planetPosVec[m][0] = (planets[m].getY()- (i*SUN_GRAVFIELDRES - SUN_NBHOOD));
				planetPosVec[m][1] = (planets[m].getZ());
				planetPosVec[m][2] = (planets[m].getX()- (j*SUN_GRAVFIELDRES - SUN_NBHOOD));

				double posVecMag = mag(planetPosVec[m])*AU2m;

				unitVec(planetPosVec[m], unitPlanetPosVec);
				
				if (m != 0 && posVecMag <= AU2m) 
				{
					 dontDraw = true;
				}
				else if(posVecMag <= 2e11)
				{
					posVecMag = 2e11; // clipping if the node is closer to a center of a planet than xxx meters. (here only for the Sun, the others are not drawn)
				}
				
				// Sums the gravitational acceleration created by each planet
				g[0] = g[0] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[0];
				g[1] = g[1] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[1];
				g[2] = g[2] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[2];
			}

			if(!dontDraw)
			{
				fieldGrid[i][j] = GRAV_VISUAL_GAIN*mag(g);
			}
		}
	}
	
	// This is the part for drawing the gravitational field
	for(int i = 0; i< numNodes -1; i++)
	{
		for(int j = 0; j< numNodes -1; j++)
		{
			if(fieldGrid[i][j] != 0 && fieldGrid[i+1][j] != 0 && fieldGrid[i][j+1] != 0)
			{
				glBegin(GL_LINES);
					glVertex3f(i*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i][j], j*SUN_GRAVFIELDRES-SUN_NBHOOD);
					glVertex3f((i+1)*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i+1][j], j*SUN_GRAVFIELDRES-SUN_NBHOOD);
				glEnd();

				glBegin(GL_LINES);
					glVertex3f(i*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i][j], j*SUN_GRAVFIELDRES-SUN_NBHOOD);
					glVertex3f(i*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i][j+1], (j+1)*SUN_GRAVFIELDRES-SUN_NBHOOD);
				glEnd();
			}
		}
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

// The old, non-adaptive gravity field
//void drawGravityField()
//{
//	const int numNodes = (SUN_NBHOOD*2)/SUN_GRAVFIELDRES + 1; 	// +-30AU range
//
//	double G = 6.674e-11; // Gravitational constant
//
//	double g[3] = {0}; // total gravitational acceleration at the point of interest
//
//	double planetPosVec[9][3]; // Position vectors for planets relative to the point of interest
//
//	double unitPlanetPosVec[3];
//
//	GLfloat fieldGrid[numNodes][numNodes];
//
//	glDisable(GL_LIGHTING);
//
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // alpha blending
//	glEnable(GL_BLEND);
//
//	glColor4f(1, 1, 1, 0.4);
//
//	for(int i = 0; i< numNodes; i++)
//	{
//		for(int j = 0; j< numNodes; j++)
//		{
//			g[0] = 0;
//			g[1] = 0;
//			g[2] = 0;	
//
//			for(int m = 0; m < 9; m++) // Computes the planet position vectors relative to this point.
//			{
//				planetPosVec[m][0] = (planets[m].getY()- (i*SUN_GRAVFIELDRES - SUN_NBHOOD));
//				planetPosVec[m][1] = (planets[m].getZ());
//				planetPosVec[m][2] = (planets[m].getX()- (j*SUN_GRAVFIELDRES - SUN_NBHOOD));
//
//				double posVecMag = mag(planetPosVec[m])*AU2m;
//
//				unitVec(planetPosVec[m], unitPlanetPosVec);
//				
//				if (posVecMag <= 1e7) posVecMag = 1e7; // clipping if the node is closer to a center of a planet than xxx meters.
//				
//				// Sums the gravitational acceleration created by each planet
//				g[0] = g[0] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[0];
//				g[1] = g[1] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[1];
//				g[2] = g[2] + G*(planets[m].getMass()/pow(posVecMag,2))*unitPlanetPosVec[2];
//			}
//
//			fieldGrid[i][j] = GRAV_VISUAL_GAIN*mag(g);
//		}
//	}
//	
//	// This is the part for drawing the gravitational field
//	for(int i = 0; i< numNodes -1; i++)
//	{
//		for(int j = 0; j< numNodes -1; j++)
//		{
//			glBegin(GL_LINES);
//				glVertex3f(i*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i][j], j*SUN_GRAVFIELDRES-SUN_NBHOOD);
//				glVertex3f((i+1)*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i+1][j], j*SUN_GRAVFIELDRES-SUN_NBHOOD);
//			glEnd();
//
//			glBegin(GL_LINES);
//				glVertex3f(i*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i][j], j*SUN_GRAVFIELDRES-SUN_NBHOOD);
//				glVertex3f(i*SUN_GRAVFIELDRES-SUN_NBHOOD, fieldGrid[i][j+1], (j+1)*SUN_GRAVFIELDRES-SUN_NBHOOD);
//			glEnd();
//		}
//	}
//
//
//	glDisable(GL_BLEND);
//	glEnable(GL_LIGHTING);
//}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(cameraPos[0],cameraPos[1],cameraPos[2],
			  lookAt[0], lookAt[1], lookAt[2],
		      upVector[0], upVector[1], upVector[2]);
	
	glScalef(-zoomFactor, -zoomFactor, -zoomFactor);
	glEnable(GL_NORMALIZE);

	glRotatef(orbitDegrees_aroundX, 1, 0, 0);
	glRotatef(orbitDegrees_aroundY, 0, 1, 0);

	//drawCoords();

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);	

	drawPlanet(sunDiffuse, sunEmissive, planets[0], SUN_SIZE_GAIN);
	drawPlanet(mercuryDiffuse, mercuryEmissive, planets[1], PLANET_SIZE_GAIN);
	drawPlanet(venusDiffuse, venusEmissive, planets[2], PLANET_SIZE_GAIN);
	drawPlanet(earthDiffuse, earthEmissive, planets[3], PLANET_SIZE_GAIN);
	drawPlanet(marsDiffuse, marsEmissive, planets[4], PLANET_SIZE_GAIN);
	drawPlanet(jupiterDiffuse, jupiterEmissive, planets[5], PLANET_SIZE_GAIN);
	drawPlanet(saturnDiffuse, saturnEmissive, planets[6], PLANET_SIZE_GAIN);
	drawPlanet(uranusDiffuse, uranusEmissive, planets[7], PLANET_SIZE_GAIN);
	drawPlanet(neptuneDiffuse, neptuneEmissive, planets[8], PLANET_SIZE_GAIN);

	glDisable(GL_NORMALIZE);

	drawGravityField();

	glutSwapBuffers();	
}

