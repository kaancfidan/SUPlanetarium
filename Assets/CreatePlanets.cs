using UnityEngine;
using System.Collections;

public class CreatePlanets : MonoBehaviour 
{
    public GameObject Sun;
    public GameObject Earth;
    public Camera MainCamera;

    // Use this for initialization
    void Start () 
    {
        InitSun();
        InitEarth();
        
    }
    
    // Update is called once per frame
    void Update () 
    {
        
    }


    void InitSun()
    {
        Sun.transform.position = new Vector3(0.0f, 0.0f, 0.0f);
    }

    void InitEarth()
    {
        Earth.transform.position = new Vector3(5.0f, 5.0f, 0.0f);
    }

}
