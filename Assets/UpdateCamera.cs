using UnityEngine;
using System.Collections;
using System;

public class UpdateCamera : MonoBehaviour
{
    //
    // VARIABLES
    //

    public float turnSpeed = 4.0f;      // Speed of camera turning when mouse moves in along an axis
    public float zoomSpeed = 4.0f;      // Speed of the camera going back and forth
    public float followSpeed = 1f;

    private Vector3 mouseOrigin;    // Position of cursor when mouse dragging starts

    private bool isRotating;    // Is the camera being rotated?
    private bool isZooming;     // Is the camera zooming?

    private Transform followedBody;

    private float lookAtDistance = 2f;
    private Vector3 lookAtVector = Vector3.forward;

    void Awake()
    {
        followedBody = GameObject.FindGameObjectWithTag("Sun").transform;
    }

    //
    // UPDATE
    //
    void Update()
    {
        if (Input.GetMouseButtonDown(2))
        {
            mouseOrigin = Input.mousePosition;
            isRotating = true;
        }

        if (Input.GetMouseButtonDown(0))
        {
            var hit = new RaycastHit();
            Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hit);

            var pickedObject = hit.transform;
            
            if (pickedObject != null)
            {
                followedBody = pickedObject;
                lookAtVector = Vector3.forward;
            }
        }

        if (Input.GetMouseButtonDown(1))
        {
            mouseOrigin = Input.mousePosition;
            isZooming = true;
        }

        // Disable movements on button release
        if (!Input.GetMouseButton(2)) isRotating = false;
        //if (!Input.GetMouseButton(0)) isPanning=false;
        if (!Input.GetMouseButton(1)) isZooming = false;

        // Rotate camera along X and Y axis (orbit around the origin)
        if (isRotating)
        {
            Vector3 pos = Camera.main.ScreenToViewportPoint(Input.mousePosition - mouseOrigin);

            var tempTransform = transform;

            tempTransform.RotateAround(followedBody.position, transform.right, -pos.y * turnSpeed);
            tempTransform.RotateAround(followedBody.position, Vector3.up, pos.x * turnSpeed);

            lookAtVector = tempTransform.forward;
        }
        // Move the camera linearly along Z axis
        else if (isZooming)
        {
            Vector3 pos = Camera.main.ScreenToViewportPoint(Input.mousePosition - mouseOrigin);
            lookAtDistance -= pos.y * zoomSpeed;
        }

        // Update camera pos and rot accordingly
        transform.position = Vector3.Lerp(transform.position, followedBody.position - lookAtDistance * lookAtVector, followSpeed * Time.deltaTime);

        var targetRotation = Quaternion.LookRotation(followedBody.position - transform.position, Vector3.up);
        transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, turnSpeed * Time.deltaTime);
    }
}
