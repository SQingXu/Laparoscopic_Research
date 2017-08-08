using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.Threading;


public class ResistantReading
{
    public float R_g;
    public float R_s;
    public ResistantReading()
    {
        R_g = 0;
        R_s = 0;
    }
    public ResistantReading(float s, float g)
    {
        this.R_s = s;
        this.R_g = g;
    }
}
public class ArduinoReceiver : MonoBehaviour {
    private float r_g_min, r_g_max, d_g_max;
    private float i_g;
    private float r_s_min, r_s_max, d_s_max;
    private float i_s;
    SerialPort stream = new SerialPort("COM3", 38400);
    private ResistantReading _reading;
    private object lock_o = new object();
    private System.Threading.Thread read_port;
    public ResistantReading reading
    {
        get
        {
            lock (lock_o)
            {
                return _reading;
            }
        }
        set
        {
            lock (lock_o)
            {
                _reading = value;
            }
        }
    }
	// Use this for initialization
	void Start () {
        //setting variable
        r_g_min = 33000.0f;
        r_g_max = 18000.0f;
        d_g_max = 0.02753f/2.0f;
        i_g = (d_g_max) / (r_g_max - r_g_min);

        r_s_min = 22000.0f;
        r_s_max = 14900.0f;
        d_s_max = 0.00884f/2.0f;
        i_s = (d_s_max) / (r_s_max - r_s_min);
        
        stream.Open();
        reading = new ResistantReading();
        read_port = new System.Threading.Thread(Run);
        read_port.Start();
        //can_read = true;
	}
    void Abort()
    {
        read_port.Abort();
        stream.Close();
    }
    void Run()
    {
        while (true)
        {
            string value_str = stream.ReadLine();
            reading = JsonUtility.FromJson<ResistantReading>(value_str);
        }
    }
    private void OnDestroy()
    {
        Abort();
        Debug.Log("Exiting Tread");
    }
    // Update is called once per frame
    void Update () {
        Debug.Log("R_g: " + reading.R_g + " R_s: " + reading.R_s);
        CalculateRotation(reading);
    }

    void CalculateRotation(ResistantReading r)
    {
        //Left Grasper
        float r_g_temp = r.R_g;
        if(r_g_temp > r_g_min)
        {
            r_g_temp = r_g_min;
        }else if(r_g_temp < r_g_max){
            r_g_temp = r_g_max;
        }
        float g_Z = -(r_g_temp - r_g_min) * i_g;
        float g_Y = -Mathf.Sqrt(0.0004f - Mathf.Pow(g_Z, 2));
        Vector3 g_right_clamptip_rot = new Vector3(0, g_Y, g_Z);
        Vector3 g_right_clamptip_norot = new Vector3(0, -0.02f, 0);
        Quaternion g_right_rot = Quaternion.FromToRotation(g_right_clamptip_norot, g_right_clamptip_rot);
        GameObject.Find("grasper1").transform.localRotation = g_right_rot;
        GameObject.Find("grasper2").transform.localRotation = Quaternion.Inverse(g_right_rot);

        //Right Scissor
        float r_s_temp = r.R_s;
        if (r_s_temp > r_s_min)
        {
            r_s_temp = r_s_min;
        }
        else if (r_s_temp < r_s_max)
        {
            r_s_temp = r_s_max;
        }
        float s_Z = -(r_s_temp - r_s_min) * i_s;
        float s_Y = -Mathf.Sqrt(Mathf.Pow(0.019f, 2) - Mathf.Pow(s_Z, 2));
        Vector3 s_right_clamptip_rot = new Vector3(0, s_Y, s_Z);
        Vector3 s_right_clamptip_norot = new Vector3(0, -0.019f, 0);
        Quaternion s_right_rot = Quaternion.FromToRotation(s_right_clamptip_norot, s_right_clamptip_rot);
        GameObject.Find("scissor1").transform.localRotation = s_right_rot;
        GameObject.Find("scissor2").transform.localRotation = Quaternion.Inverse(s_right_rot);
    }
}
