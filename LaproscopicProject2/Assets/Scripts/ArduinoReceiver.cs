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
        r_g_min = 34000.0f;
        r_g_max = 16800.0f;
        d_g_max = 0.02753f/2.0f;
        i_g = (d_g_max) / (r_g_max - r_g_min);

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
        //Debug.Log("R_g: " + reading.R_g + " R_s: " + reading.R_s);
        CalculateRotation(reading);
        //string[] values = value.Split(' ');
    }

    void CalculateRotation(ResistantReading r)
    {
        float r_g_temp = r.R_g;
        if(r_g_temp > 32000.0f)
        {
            r_g_temp = 32000.0f;
        }else if(r_g_temp < 16800.0){
            r_g_temp = 16800.0f;
        }
        float g_X = (r_g_temp - 32000.0f) * i_g;
        float g_Z = Mathf.Sqrt(0.0004f - Mathf.Pow(g_X, 2));
        Vector3 g_right_clamptip_rot = new Vector3(g_X, 0, g_Z);
        Vector3 g_right_clamptip_norot = new Vector3(0, 0, 0.02f);
        Quaternion g_right_rot = Quaternion.FromToRotation(g_right_clamptip_norot, g_right_clamptip_rot);
        GameObject.Find("Left_RightClampParent").transform.localRotation = g_right_rot;
        GameObject.Find("Left_LeftClampParent").transform.localRotation = Quaternion.Inverse(g_right_rot);
    }
}
