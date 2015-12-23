using UnityEngine;
using System.Collections;


public class trailNode		// Linked list structure for trail nodes (old positions)
{
    public double x;
    public double y;
    public double z;

    public trailNode next;
};

public class Planet : MonoBehaviour
{
    void Start()
    {
        m_Pi = 3.14159265358979323846;
        m_RaDeg = 180.0 / m_Pi;
        m_DegRad = m_Pi / 180.0;

        N = N_inc = i = i_inc = w = w_inc = a = a_inc = e = e_inc = M = M_inc = 0;

        radius = 0.00464912633;
        mass = 1.9891e30;

        angvel = 14713;
        axis_angle = 0;

        startPtr = new trailNode();
        startPtr.next = null;
        trailCount = 0;
    }

    double SinD(double x)
    {
        return System.Math.Sin(x * m_DegRad);
    }
    double CosD(double x)
    {
        return System.Math.Cos(x * m_DegRad);
    }
    double TanD(double x)
    {
        return System.Math.Tan(x * m_DegRad);
    }
    double ASinD(double x)
    {
        return System.Math.Asin(x * m_RaDeg);
    }
    double ACosD(double x)
    {
        return System.Math.Acos(x * m_RaDeg);
    }
    double ATanD(double x)
    {
        return System.Math.Atan(x * m_RaDeg);
    }
    double ATan2D(double y, double x)
    {
        return System.Math.Atan2(y, x);
    }

    public double[] GetPosVec()
    {
        double[] posVec = new double[3];

        posVec[0] = x;
        posVec[1] = y;
        posVec[2] = z;

        return posVec;
    }

    public double GetX()
    {
        return x;
    }

    public double GetY()
    {
        return y;
    }

    public double GetZ()
    {
        return z;
    }

    public double GetMass()
    {
        return mass;
    }

    public double GetRadius()
    {
        return radius;
    }

    public void InitKepler(double p_N, double p_N_inc, 
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

    public void InitPhysical(double p_mass, double p_radius,
                             double p_angvel, double p_axis_angle)
    {
        mass = p_mass;
        radius = p_radius;
        angvel = p_angvel;
        axis_angle = p_axis_angle;
    }

    public void UpdatePosition(System.DateTime date, int hour)
    {
        double E, E_old; // for eccentric anomaly computation iterations

        double d;
    
        double x_op, y_op, r, v;

        d = DayNumber(date, hour);

        UpdateKeplerian(d);

        E = M_act + (180 / m_Pi) * e_act * SinD(M_act) * (1 + e_act * CosD(M_act));

        E_old = 0;

        while (System.Math.Abs(E - E_old) > 1e-4)
        {
            E_old = E;

            E = E_old - (E_old - (180 / m_Pi) * e_act * SinD(E_old) - M_act) / (1 - e_act * CosD(E_old));
        }

        // Rectangular coordinates on the orbital plane
        x_op = a_act * (CosD(E) - e_act);
        y_op = a_act * System.Math.Sqrt(1 - e_act * e_act) * SinD(E);

        // Polar coordinates on the orbital plane 
        r = System.Math.Sqrt(x_op * x_op + y_op * y_op);
        v = ATan2D(y_op, x_op);

        if (!(x == 0 && y == 0 && z == 0))
        {
            UpdateTrail();
        }

        // Heliocentric coordinates
        x = r * (CosD(N_act) * CosD(v + w_act) - SinD(N_act) * SinD(v + w_act) * CosD(i_act));
        y = r * (SinD(N_act) * CosD(v + w_act) + CosD(N_act) * SinD(v + w_act) * CosD(i_act));
        z = r * SinD(v + w_act) * SinD(i_act);
    }

    void DrawTrail(int width, float[] color)
    {
        trailNode temp1, temp2;

        float[] tempcolor = new float[3];
        float alpha = 1.0f;
        temp1 = startPtr;

        if (temp1.next != null)
        { 
            temp2 = (trailNode)temp1.next;
        }
        else
        {
            temp2 = temp1;
        }

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDisable(GL_LIGHTING);

        //glLineWidth(width);

        //glBegin(GL_LINES);

        //while (temp2->next != NULL)
        //{
        //    glColor4f(color[0], color[1], color[2], alpha);

        //    alpha *= 1 - (1 / (a_act * 20 * 12));

        //    temp1 = temp1->next;
        //    temp2 = temp2->next;

        //    glVertex3f(temp1->y, temp1->z, temp1->x);
        //    glVertex3f(temp2->y, temp2->z, temp2->x);
        //}

        //glEnd();

        //glDisable(GL_BLEND);
        //glEnable(GL_LIGHTING);
    } 

    public void UpdateKeplerian(double d)
    {
        N_act = Rev(N + d*N_inc);
        i_act = Rev(i + d*i_inc);
        w_act = Rev(w + d*w_inc);
        a_act = Rev(a + d*a_inc);
        e_act = Rev(e + d*e_inc);
        M_act = Rev(M + d*M_inc);
    }

    public double DayNumber(System.DateTime date, int hour)
    {
        return 367 * date.Year - (7 * (date.Year + ((date.Month + 9) / 12))) / 4 + (275 * date.Month) / 9 + date.Day - 730530 + hour / 24.0;
    }

    public double Rev(double angle)
    {
        return angle - System.Math.Floor(angle / 360.0) * 360.0;
    }

    public void UpdateTrail()
    {
        trailNode newPos;

        newPos = new trailNode();

        newPos.x = x;
        newPos.y = y;
        newPos.z = z;

        newPos.next = startPtr.next;

        startPtr.next = newPos;

        if(trailCount < a_act*60*12)
            trailCount++;
        else
        {
            trailNode temp1, temp2;
            
            temp1 = startPtr;
            temp2 = (trailNode)temp1.next;

            while(temp2.next != null)
            {
                temp1 = (trailNode)temp1.next;
                temp2 = (trailNode)temp2.next;
            }

            temp1.next = null;
        }
    }

    double m_Pi;
    double m_RaDeg;
    double m_DegRad;

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

   

    trailNode startPtr;   // Start pointer of the trail linked list
    int trailCount;

    double mass;	  // (kg)
    double radius; // (AU)

    double angvel; // angular velocity of the rotation around its own axis (deg/day)
    double axis_angle; //angle between its rotation axis and the normal of the orbital plane (deg)
}
