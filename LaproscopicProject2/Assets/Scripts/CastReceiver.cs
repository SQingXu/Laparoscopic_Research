#if UNITY_WSA_10_0 && !UNITY_EDITOR
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
#else
using System.Net.Sockets;
#endif

using System.Net;

using System.Collections;
//using System.Collections.Generic;
using UnityEngine;
using System;
using System.Text;
using System.IO;

[Serializable]

public class FixtureTipTransform
{
    public String direction;
    public Vector3 position;
    public float rotation;
}
public class CastReceiver : MonoBehaviour {
    private CASTClient Instance { get; set; }
	

	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}

class CASTClient : MonoBehaviour
{
    private int port = 8500;
    private FixtureTipTransform _lastFixtureTipTransform_left;
    private FixtureTipTransform _lastFixtureTipTransform_right;
    private object lock_left;
    private object lock_right;
    public FixtureTipTransform lastFixtureTipTransform_left
    {
        get
        {
            lock (lock_left)
            {
                return _lastFixtureTipTransform_left;
            }
        }
        private set
        {
            lock (lock_left)
            {
                _lastFixtureTipTransform_left = value;
            }
        }
    }
    public FixtureTipTransform lastFixtureTipTransform_right
    {
        get
        {
            lock (lock_right)
            {
                return _lastFixtureTipTransform_right;
            }
        }
        private set
        {
            lock (lock_right)
            {
                _lastFixtureTipTransform_right = value;
            }
        }
    }

#if UNITY_WSA_10_0  && !UNITY_EDITOR 

#else
    UdpClient udp;
#endif

    IPEndPoint ep;

    public void Init(int _port)
    {
        this.port = _port;
#if UNITY_WSA_10_0 && !UNITY_EDITOR

#else
        udp = new UdpClient(port);
        udp.BeginReceive(new AsyncCallback(receiveMsg), null);

#endif

    }
    void receiveMsg(IAsyncResult result)
    {
        IPEndPoint source = new IPEndPoint(0, 0);
        //byte[] message = udp.EndReceive(result, ref source);
        //Debug.Log("RECV " + Encoding.UTF8.GetString(message) + " from " + source);

        string message = Encoding.UTF8.GetString(udp.EndReceive(result, ref source));
        //Debug.Log("RECV " + message + " from " + source);

        handleMsg(message);

        // schedule the next receive operation once reading is done:
        udp.BeginReceive(new AsyncCallback(receiveMsg), udp);
    }

    void handleMsg(string msg)
    {
        FixtureTipTransform ft = JsonUtility.FromJson<FixtureTipTransform>(msg);
        if()
    }
}