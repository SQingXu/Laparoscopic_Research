using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Runtime.InteropServices;

public class CameraCalibrationData
{
    public Vector3 up_column;
    public Vector3 forward_column;
    public Vector3 translation_vector;
    public CameraCalibrationData()
    {
        up_column = new Vector3();
        forward_column = new Vector3();
        translation_vector = new Vector3();
    }
    public CameraCalibrationData(Vector3 up, Vector3 forward, Vector3 transl)
    {
        this.up_column = up;
        this.forward_column = forward;
        this.translation_vector = transl;
    }
}

public class CameraCalibration : MonoBehaviour {

	// Use this for initialization
	void Start () {
        //Vector3 column1 = new Vector3(0.999737300214088f, -0.02037085203571542f, 0.01050518671826108f);
        Vector3 column2 = new Vector3(-0.003243157076691674f, -0.5794556074046202f, -0.8149973502898759f);//yaxis
        Vector3 column3 = new Vector3(0.02268947978298411f, 0.8147491806897885f, -0.5793694504135474f);
        Vector3 translation = new Vector3(-0.01907420517764934f,0.2015660434235227f, -0.1775903495761549f);//invert y and z axis
        //Vector3 rot = new Vector3(-2.188783172580953f, -0.02300284725188262f, 0.01636375664737853f);

        Vector3 upVector = new Vector3(-column2.x, -column2.z, -column2.y);
        Vector3 forwardVector = new Vector3(column3.x, column3.z, column3.y);
        Quaternion lookRotation = Quaternion.LookRotation(forwardVector, upVector);

        //this.transform.localPosition = new Vector3(translation.x, translation.y, translation.z + 0.125f);
        //this.transform.localRotation = lookRotation;
	}
	public void calibrateCameraPosition(CameraCalibrationData data)
    {
        Vector3 up_col = data.up_column;
        Vector3 forward_col = data.forward_column;
        Vector3 transl_vec = data.translation_vector;
        Vector3 up_vec = new Vector3(-up_col.x, -up_col.z, -up_col.y);
        Vector3 forward_vec = new Vector3(forward_col.x, forward_col.z, forward_col.y);
        Quaternion lookRotation = Quaternion.LookRotation(forward_vec, up_vec);
        this.transform.localPosition = new Vector3(transl_vec.x, transl_vec.z, transl_vec.y + 0.125f);
        this.transform.localRotation = lookRotation;
    }
	// Update is called once per frame
	void Update () {
		
	}
}
