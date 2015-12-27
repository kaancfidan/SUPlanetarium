using UnityEngine;
using System.Collections;
using System;

public class UpdateCamera : MonoBehaviour 
{
	//
	// VARIABLES
	//
	
	public float turnSpeed = 4.0f;		// Speed of camera turning when mouse moves in along an axis
	//public float panSpeed = 4.0f;		// Speed of the camera when being panned
	public float zoomSpeed = 4.0f;		// Speed of the camera going back and forth
	
	private Vector3 mouseOrigin;	// Position of cursor when mouse dragging starts
	//private bool isPanning;		// Is the camera being panned?
	private bool isRotating;	// Is the camera being rotated?
	private bool isZooming;     // Is the camera zooming?
	private bool isMoving; // Is the camera moving to follow another object?

	private Vector3 orbitTarget = Vector3.zero;
	private Transform pickedObject;

    private Transform initTransform;
    private float startTime;
    private float journeyLength;

    private float moveSpeed = 0.1f;

	//
	// UPDATE
	//

	void Update () 
	{
		if(Input.GetMouseButtonDown(2))
		{
			mouseOrigin = Input.mousePosition;
			isRotating = true;
		}

		if (Input.GetMouseButtonDown(0))
		{
			RaycastHit hitInfo = new RaycastHit();
			Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);

			pickedObject = hitInfo.transform;

            if (pickedObject != null)
            {
                initTransform = transform;
                journeyLength = Vector3.Distance(transform.position, pickedObject.transform.position);
                startTime = Time.time;
                isMoving = true;
            }
		}
		
		if(Input.GetMouseButtonDown(1))
		{
			mouseOrigin = Input.mousePosition;
			isZooming = true;
		}
		
		// Disable movements on button release
		if (!Input.GetMouseButton(2)) isRotating=false;
		//if (!Input.GetMouseButton(0)) isPanning=false;
		if (!Input.GetMouseButton(1)) isZooming=false;
		
        if(isMoving)
        {
            var newTransform = pickedObject;

            float distCovered = (Time.time - startTime) * moveSpeed;
            float fracJourney = distCovered / journeyLength;

            transform.position = Vector3.Lerp(initTransform.position, newTransform.position + new Vector3(0, 0, 0.2f), fracJourney);

            if (fracJourney > 0.98)
                isMoving = false;

        }
		// Rotate camera along X and Y axis (orbit around the origin)
		else if (isRotating)
		{
			Vector3 pos = Camera.main.ScreenToViewportPoint(Input.mousePosition - mouseOrigin);

			//transform.RotateAround(transform.position, transform.right, -pos.y * turnSpeed);
			//transform.RotateAround(transform.position, Vector3.up, pos.x * turnSpeed);

			transform.RotateAround(orbitTarget, transform.right, -pos.y * turnSpeed);
			transform.RotateAround(orbitTarget, Vector3.up, pos.x * turnSpeed);
		}
		
		// Move the camera linearly along Z axis
		else if (isZooming)
		{
				Vector3 pos = Camera.main.ScreenToViewportPoint(Input.mousePosition - mouseOrigin);

				Vector3 move = pos.y * zoomSpeed * transform.forward; 
				transform.Translate(move, Space.World);
		}
	}
}
