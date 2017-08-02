using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraCalibration : MonoBehaviour {

	// Use this for initialization
	void Start () {
        Vector3 column1 = new Vector3(0.999737300214088f, -0.02037085203571542f, 0.01050518671826108f);
        Vector3 column2 = new Vector3(-0.003243157076691674f, -0.5794556074046202f, -0.8149973502898759f);//yaxis
        Vector3 column3 = new Vector3(0.02268947978298411f, 0.8147491806897885f, -0.5793694504135474f);
        Vector3 translation = new Vector3(-0.01907420517764934f,0.2015660434235227f, -0.1775903495761549f);//invert y and z axis
        Vector3 rot = new Vector3(-2.188783172580953f, -0.02300284725188262f, 0.01636375664737853f);
        float theta = (float)(rot.magnitude * 180 / Mathf.PI);
        Quaternion rot_q = Quaternion.AngleAxis(theta, rot);

        Vector3 upVector = new Vector3(-column2.x, -column2.z, -column2.y);
        Vector3 forwardVector = new Vector3(column3.x, column3.z, column3.y);
        Quaternion lookRotation = Quaternion.LookRotation(forwardVector, upVector);

        this.transform.localPosition = new Vector3(translation.x, translation.y, translation.z + 0.125f);
        this.transform.localRotation = lookRotation;
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
