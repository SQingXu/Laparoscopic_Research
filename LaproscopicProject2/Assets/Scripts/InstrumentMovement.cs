using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InstrumentMovement : MonoBehaviour {
    GameObject origin;
    Vector3 pivotPosition;
    Vector3 relativeDistance;
	// Use this for initialization
	void Start () {
        origin = GameObject.Find("Origin");
        relativeDistance = new Vector3(-0.125f,0.16f,-0.0125f);
        pivotPosition = origin.transform.position;

        Debug.Log(origin.transform.position);
        Debug.Log(origin.transform.localPosition);


	}
	
    void Yaw()
    {
        transform.Rotate(Vector3.right * Time.deltaTime);
    }
	// Update is called once per frame
	void Update () {
        Yaw();
	}
}
