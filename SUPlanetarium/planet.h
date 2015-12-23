#ifndef planet_h
#define planet_h

#include <cmath>
#include "date.h"

using namespace std;

class Planet
{
  public:
    // constructors
	Planet(); // default constructor used for the Sun

    // accessor functions
	double getX(); // returns heliocentric x coordinate	
	double getY(); // returns heliocentric x coordinate			
	double getZ(); // returns heliocentric x coordinate	
	double *getPosVec();
	
	double getMass(); // returns the mass
	double getRadius(); // returns the radius
	
    // mutator functions
	
	void InitKepler(double p_N, double p_N_inc, 
				    double p_i, double p_i_inc, 
				    double p_w, double p_w_inc, 
				    double p_a, double p_a_inc,
				    double p_e, double p_e_inc, 
				    double p_M, double p_M_inc);

	void InitPhysical(double p_mass, double p_radius,
					  double p_angvel, double p_axis_angle);

	void UpdatePosition(Date date, int hour); // updates the heliocentric coordinates according to date
	
	void DrawTrail(int width, float *color);

  private:
    
	// initial, actual and increment values of the Keplerian elements (AU, deg)
    double N, N_act, N_inc;  
	double i, i_act, i_inc;
	double w, w_act, w_inc;
	double a, a_act, a_inc;
	double e, e_act, e_inc;
	double M, M_act, M_inc;
	
	// heliocentric coordinates (AU)
	double x; 
	double y; 
	double z;

	struct trailNode		// Linked list structure for trail nodes (old positions)
	{
		double x;
		double y;
		double z;

		trailNode *next;
	};
	
	trailNode *startPtr;   // Start pointer of the trail linked list

	int trailCount;

	double mass;	  // (kg)
	double radius; // (AU)
	
	double angvel; // angular velocity of the rotation around its own axis (deg/day)
	double axis_angle; //angle between its rotation axis and the normal of the orbital plane (deg)
	
	void UpdateKeplerian(double d); // updates the Keplerian elements according to the day number
	double DayNumber(Date date, int hour); // returns the day number according to the given date
	double rev(double angle);		// normalizes the angle into 0-360 degrees range

	void UpdateTrail();
};

#endif