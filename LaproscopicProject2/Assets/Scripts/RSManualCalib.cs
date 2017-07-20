using System.Collections;
using System.Collections.Generic;
using UnityEngine;

enum RotationType { AroundYLeft, AroundYRight, AroundXUp, AroundXDown };
enum MovingType { MoveUp, MoveDown, MoveLeft, MoveRight, MoveForward, MoveBackward};
public class RSManualCalib : MonoBehaviour {
    public float MoveUnit;
    public float RotateUnit;
    private GameObject rotatingCoord;
	// Use this for initialization
	void Start () {
        MoveUnit = 0.002f; //2mm
        RotateUnit = 0.5f; //0.5 degree
        rotatingCoord = new GameObject("RotCoord");
        rotatingCoord.transform.parent = this.gameObject.transform;
	}
	
	// Update is called once per frame
	void Update () {
        if (Input.GetKeyDown(KeyCode.UpArrow))
        {
            //Debug.Log("Up Arrow Key pressed");
            Rotate(RotationType.AroundXUp);
        }
        if (Input.GetKeyDown(KeyCode.DownArrow))
        {
            //Debug.Log("Down Arrow Key pressed");
            Rotate(RotationType.AroundXDown);
        }
        if (Input.GetKeyDown(KeyCode.RightArrow))
        {
            //Debug.Log("Right Arrow Key pressed");
            Rotate(RotationType.AroundYRight);
        }
        if (Input.GetKeyDown(KeyCode.LeftArrow))
        {
            //Debug.Log("Left Arrow Key pressed");
            Rotate(RotationType.AroundYLeft);
        }
        if (Input.GetKeyDown(KeyCode.W))
        {
            //Debug.Log("Forward pressed");
            Move(MovingType.MoveForward);
        }
        if (Input.GetKeyDown(KeyCode.S))
        {
            //Debug.Log("Backward pressed");
            Move(MovingType.MoveBackward);
        }
        if (Input.GetKeyDown(KeyCode.A))
        {
            //Debug.Log("Leftward pressed");
            Move(MovingType.MoveLeft);
        }
        if (Input.GetKeyDown(KeyCode.D))
        {
            //Debug.Log("Rightward pressed");
            Move(MovingType.MoveRight);
        }
        if (Input.GetKeyDown(KeyCode.R))
        {
            //Debug.Log("Higher pressed");
            Move(MovingType.MoveUp);
        }
        if (Input.GetKeyDown(KeyCode.F))
        {
            //Debug.Log("Lower pressed");
            Move(MovingType.MoveDown);
        }
    }

    void Rotate(RotationType rt)
    {
        Vector3 rotateAxis;
        float rotateAmount = RotateUnit;
        Transform rotCoordTrans = rotatingCoord.transform;
        Transform originTrans = this.transform;
        if (rt == RotationType.AroundXDown)
        {
            rotCoordTrans.localPosition = Vector3.right;
            rotateAxis = rotCoordTrans.position - originTrans.position;
        }
        else if (rt == RotationType.AroundXUp)
        {
            rotCoordTrans.localPosition = Vector3.right;
            rotateAxis = rotCoordTrans.position - originTrans.position;
            rotateAmount = rotateAmount * -1;
        }
        else if (rt == RotationType.AroundYLeft)
        {
            rotCoordTrans.localPosition = Vector3.up;
            rotateAxis = rotCoordTrans.position - originTrans.position;
            rotateAmount = rotateAmount * -1;
        }
        else
        {
            rotCoordTrans.localPosition = Vector3.up;
            rotateAxis = rotCoordTrans.position - originTrans.position;
        }
        this.transform.Rotate(rotateAxis, rotateAmount);
    }
    void Move(MovingType mt)
    {
        Vector3 moveAmount;
        if(mt == MovingType.MoveForward)
        {
            moveAmount = Vector3.forward;
        }
        else if(mt == MovingType.MoveBackward)
        {
            moveAmount = Vector3.back;
        }
        else if (mt == MovingType.MoveLeft)
        {
            moveAmount = Vector3.left;
        }
        else if (mt == MovingType.MoveRight)
        {
            moveAmount = Vector3.right;
        }
        else if (mt == MovingType.MoveUp)
        {
            moveAmount = Vector3.up;
        }
        else
        {
            moveAmount = Vector3.down;
        }
        moveAmount = MoveUnit * moveAmount;
        this.transform.localPosition += moveAmount;
    } 
}
