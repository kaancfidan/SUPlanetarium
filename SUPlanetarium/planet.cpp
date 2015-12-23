#define PI          3.14159265358979323846
#define RADEG       (180.0/PI)
#define DEGRAD      (PI/180.0)
#define sind(x)     sin((x)*DEGRAD)
#define cosd(x)     cos((x)*DEGRAD)
#define tand(x)     tan((x)*DEGRAD)
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atand(x)    (RADEG*atan(x))
#define atan2d(y,x) (RADEG*atan2((y),(x)))

#include "planet.h"
#include "date.h"

#include <glut.h>

using namespace std;

Planet::Planet()
{
	N = N_inc = i = i_inc = w = w_inc = a = a_inc = e = e_inc = M = M_inc = 0;

	radius = 0.00464912633;
	mass =  1.9891e30;
	
	angvel = 14713;
	axis_angle = 0;

	startPtr = new trailNode;
	startPtr->next = NULL;
	trailCount = 0;
}

double Planet::getX()
{
	return x;
}	
double Planet::getY()
{
	return y;
}			
double Planet::getZ()
{
	return z;
}

double *Planet::getPosVec()
{
	double posVec[3];

	posVec[0] = x;
	posVec[1] = y;
	posVec[2] = z;

	return posVec;
}

double Planet::getMass()
{
	return mass;
}

double Planet::getRadius()
{
	return radius;
}

void Planet::InitKepler(double p_N, double p_N_inc, 
						double p_i, double p_i_inc, 
						double p_w, double p_w_inc, 
						double p_a, double p_a_inc,
						double p_e, double p_e_inc, 
						double p_M, double p_M_inc)
{
	N = p_N;
	i = p_i;
	w = p_w;
	a = p_a;
	e = p_e;
	M = p_M;
	
	N_inc = p_N_inc;
	i_inc = p_i_inc;
	w_inc = p_w_inc;	
	a_inc = p_a_inc;
	e_inc = p_e_inc;
	M_inc = p_M_inc;
}

void Planet::InitPhysical(double p_mass, double p_radius,
						  double p_angvel, double p_axis_angle)
{
	mass = p_mass;
	radius = p_radius;
	angvel = p_angvel;
	axis_angle = p_axis_angle;
}

void Planet::UpdatePosition(Date date, int hour)
{
	double E, E_old; // for eccentric anomaly computation iterations

	double d;
	
	double x_op, y_op, r, v;
	
	d = DayNumber(date, hour);

	UpdateKeplerian(d);
	
	E = M_act + (180/PI) * e_act * sind(M_act) * (1 + e_act * cosd(M_act));	
	
	E_old = 0;

	while (fabs(E-E_old) > 1e-4)
	{
		E_old = E;
	
		E = E_old - (E_old - (180/PI) * e_act * sind(E_old) - M_act) / (1 - e_act * cosd(E_old));		
	}
	
	// Rectangular coordinates on the orbital plane
	x_op = a_act * (cosd(E) - e_act);					
	y_op = a_act * sqrt(1 - e_act*e_act) * sind(E);
	
	// Polar coordinates on the orbital plane 
	r = sqrt(x_op*x_op + y_op*y_op);
    v = atan2d(y_op, x_op);

	if (!(x == 0 && y == 0 && z == 0))
		UpdateTrail();
	
	// Heliocentric coordinates
	x = r * (cosd(N_act) * cosd(v+w_act) - sind(N_act) * sind(v+w_act) * cosd(i_act));
    y = r * (sind(N_act) * cosd(v+w_act) + cosd(N_act) * sind(v+w_act) * cosd(i_act));
    z = r * sind(v+w_act) * sind(i_act);
}

void Planet::DrawTrail(int width, float *color)
{
	trailNode *temp1, *temp2;

	float tempcolor[3];
	float alpha = 1.0;

	tempcolor[0] = color[0];
	tempcolor[1] = color[1];
	tempcolor[2] = color[2];

	temp1 = startPtr;

	if(temp1->next != NULL)
		temp2 = temp1->next;
	else 
		temp2 = temp1;

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

    glLineWidth(width);	

    glBegin(GL_LINES);

	while(temp2->next != NULL)
	{
		glColor4f(color[0], color[1], color[2], alpha);

		alpha *= 1-(1/(a_act*20*12));

		temp1 = temp1->next;
		temp2 = temp2->next;

		glVertex3f(temp1->y, temp1->z, temp1->x);
		glVertex3f(temp2->y, temp2->z, temp2->x);
	}

    glEnd();

    glDisable(GL_BLEND); 
	glEnable(GL_LIGHTING);
}

void Planet::UpdateKeplerian(double d)
{
	N_act = rev(N + d*N_inc);
	i_act = rev(i + d*i_inc);
	w_act = rev(w + d*w_inc);
	a_act = rev(a + d*a_inc);
	e_act = rev(e + d*e_inc);
	M_act = rev(M + d*M_inc);
}

double Planet::DayNumber(Date date, int hour)
{
	return 367*date.Year() - (7*(date.Year() +((date.Month()+9)/12)))/4 + (275*date.Month())/9 + date.Day() - 730530 + hour/24.0;
}

double Planet::rev( double angle )
{
	return  angle - floor(angle/360.0)*360.0;
}

void Planet::UpdateTrail()
{
	trailNode *newPos;

	newPos = new trailNode;

	newPos->x = x;
	newPos->y = y;
	newPos->z = z;

	newPos->next = startPtr->next;

	startPtr->next = newPos;

	if(trailCount < a_act*60*12)
		trailCount++;
	else
	{
		trailNode *temp1, *temp2;
		
		temp1 = startPtr;
		temp2 = temp1->next;

		while(temp2->next != NULL)
		{
			temp1 = temp1 -> next;
			temp2 = temp2 -> next;
		}

		temp1->next = NULL;

		delete temp2;
	}
}