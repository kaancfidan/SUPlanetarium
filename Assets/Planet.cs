using UnityEngine;
using System.Collections;

public class Planet : MonoBehaviour
{
    void Awake()
    {
        m_Pi = 3.14159265358979323846;
        m_Rad2Deg = 180.0 / m_Pi;
        m_Deg2Rad = m_Pi / 180.0;

        UpdatePosition(m_Date, m_Hour);
        transform.position = new Vector3((float)y, (float)z, (float)x);
    }

    void FixedUpdate()
    {
        m_Hour += m_TimeScale;

        if (m_Hour >= 24)
        {
            m_Date = m_Date.AddDays(m_Hour/24);
            m_Hour = m_Hour%24;
        }

        UpdatePosition(m_Date, m_Hour);

        transform.position = new Vector3((float)y, (float)z, (float)x);
    }

    public void ChangeTimeScale(float timeScale)
    {
        m_TimeScale = timeScale;
    }

    double SinD(double x)
    {
        return System.Math.Sin(x * m_Deg2Rad);
    }
    double CosD(double x)
    {
        return System.Math.Cos(x * m_Deg2Rad);
    }
    double TanD(double x)
    {
        return System.Math.Tan(x * m_Deg2Rad);
    }
    double ASinD(double x)
    {
        return System.Math.Asin(x) * m_Rad2Deg;
    }
    double ACosD(double x)
    {
        return System.Math.Acos(x) * m_Rad2Deg;
    }
    double ATanD(double x)
    {
        return System.Math.Atan(x) * m_Rad2Deg;
    }
    double ATan2D(double y, double x)
    {
        return System.Math.Atan2(y, x) * m_Rad2Deg;
    }

    public void UpdatePosition(System.DateTime date, float hour)
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

        // Heliocentric coordinates
        x = r * (CosD(N_act) * CosD(v + w_act) - SinD(N_act) * SinD(v + w_act) * CosD(i_act));
        y = r * (SinD(N_act) * CosD(v + w_act) + CosD(N_act) * SinD(v + w_act) * CosD(i_act));
        z = r * SinD(v + w_act) * SinD(i_act);
    }

    private void UpdateKeplerian(double d)
    {
        N_act = Rev(N + d*N_inc);
        i_act = Rev(i + d*i_inc);
        w_act = Rev(w + d*w_inc);
        a_act = Rev(a + d*a_inc);
        e_act = Rev(e + d*e_inc);
        M_act = Rev(M + d*M_inc);
    }

    private double DayNumber(System.DateTime date, float hour)
    {
        return 367 * date.Year - (7 * (date.Year + ((date.Month + 9) / 12))) / 4 + (275 * date.Month) / 9 + date.Day - 730530 + hour / 24.0;
    }

    private double Rev(double angle)
    {
        return angle - System.Math.Floor(angle / 360.0) * 360.0;
    }

    double m_Pi;
    double m_Rad2Deg;
    double m_Deg2Rad;

    float m_Hour;
    System.DateTime m_Date = System.DateTime.Today;

    float m_TimeScale = 1;

    // initial, actual and increment values of the Keplerian elements (AU, deg)
    public double N, N_inc;
    public double i, i_inc;
    public double w, w_inc;
    public double a, a_inc;
    public double e, e_inc;
    public double M, M_inc;

    double N_act;
    double i_act;
    double w_act;
    double a_act;
    double e_act;
    double M_act;

    // heliocentric coordinates (AU)
    double x;
    double y;
    double z;   

    public double mass;	  // (kg)
    public double radius; // (AU)

    double angvel; // angular velocity of the rotation around its own axis (deg/day)
    double axis_angle; //angle between its rotation axis and the normal of the orbital plane (deg)
}
