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
    public FixtureTipTransform()
    {
        direction = "";
        position = new Vector3();
        rotation = 0;
    }
    public FixtureTipTransform(String dir, Vector3 pos, float rot)
    {
        direction = dir;
        position = pos;
        rotation = rot;
    }

}
public class CastReceiver : MonoBehaviour
{
    private CASTClient Instance { get; set; }
    private GameObject leftInstruTip;
    private GameObject rightInstruTip;
    private GameObject leftTip_norot;
    private GameObject rightTip_norot;
    private GameObject leftTip_rot;
    private GameObject rightTip_rot;
    private GameObject parentdup_left;
    private GameObject parentdup_right;
    private Transform leftInstruTipTransf;
    private Transform rightInstruTipTransf;
    private Transform leftInstruPivotTransf;
    private Transform rightInstruPivotTransf;
    private float leftInstruLength;
    private float rightInstruLength;

    public int port = 8500;
    void Start()
    {
        this.Instance = this.gameObject.AddComponent<CASTClient>();
        this.Instance.Init(port);
        Debug.Log("CAST UDP receiver start");
        leftInstruTip = new GameObject("LeftInsturTip");
        rightInstruTip = new GameObject("RightInstruTip");
        leftInstruTipTransf = leftInstruTip.transform;
        rightInstruTipTransf = rightInstruTip.transform;
        leftInstruTipTransf.parent = GameObject.Find("Origin").transform;
        rightInstruTipTransf.parent = GameObject.Find("Origin").transform;

        leftInstruPivotTransf = GameObject.Find("LeftInstrumentParent").transform;
        rightInstruPivotTransf = GameObject.Find("RightInstrumentParent").transform;
        //leftInstruLength = GameObject.Find("LeftInstrument").transform.localScale.y * 2; //cylinder's total length is twice what the y is
        //rightInstruLength = GameObject.Find("RightInstrument").transform.localScale.y * 2;
        leftInstruLength = 0.02f;
        rightInstruLength = 0.019f;
        
        parentdup_left = new GameObject("LeftInstrumentDuplicate");
        parentdup_right = new GameObject("RightInstrumentDuplicate");
        parentdup_left.transform.parent = GameObject.Find("Origin").transform;
        parentdup_right.transform.parent = GameObject.Find("Origin").transform;
        parentdup_left.transform.localPosition = leftInstruPivotTransf.localPosition;
        parentdup_left.transform.localRotation = new Quaternion();
        parentdup_right.transform.localPosition = rightInstruPivotTransf.localPosition;
        parentdup_right.transform.localRotation = new Quaternion();

        leftTip_norot = new GameObject("LeftInstruTipWithoutRot");
        rightTip_norot = new GameObject("RightInstruTipWithoutRot");
        leftTip_norot.transform.parent = parentdup_left.transform;
        rightTip_norot.transform.parent = parentdup_right.transform;

        leftTip_rot = new GameObject("LeftInstruTipWithRot");
        rightTip_rot = new GameObject("RightInstruTipWithRot");
        leftTip_rot.transform.parent = parentdup_left.transform;
        rightTip_rot.transform.parent = parentdup_right.transform;
    }

    // Update is called once per frame
    void Update()
    {
        if(this.Instance.ReceiveDataLeft && this.Instance.ReceiveDataRight)
        {
            //Debug.Log(Instance.lastFixtureTipTransform_left.position);
            //Debug.Log(Instance.lastFixtureTipTransform_right.position);
            //this.transform.parent.Find("LeftInstrumentTip").localPosition = Instance.lastFixtureTipTransform_left.position;
            //this.transform.parent.Find("RightInstrumentTip").localPosition = Instance.lastFixtureTipTransform_right.position;

            leftInstruTipTransf.localPosition = Instance.lastFixtureTipTransform_left.position;
            rightInstruTipTransf.localPosition = Instance.lastFixtureTipTransform_right.position;

            //calculate the length between pivot and tip
            Vector3 pivotTotip_left = leftInstruTipTransf.localPosition - leftInstruPivotTransf.localPosition;
            Vector3 pivotTotip_right = rightInstruTipTransf.localPosition - rightInstruPivotTransf.localPosition;
            float length_left = pivotTotip_left.magnitude;
            float length_right = pivotTotip_right.magnitude;
            float insertLength_left = length_left - (leftInstruLength);
            float insertlength_right = length_right - (rightInstruLength);
            this.insertPosition(insertLength_left, insertlength_right);

            //calculate rotation
            leftTip_rot.transform.position = leftInstruTipTransf.position;
            rightTip_rot.transform.position = rightInstruTipTransf.position;
            Quaternion rot_left = Quaternion.FromToRotation(leftTip_norot.transform.localPosition, leftTip_rot.transform.localPosition);
            Quaternion rot_right = Quaternion.FromToRotation(rightTip_norot.transform.localPosition, rightTip_rot.transform.localPosition);
            GameObject.Find("LeftInstrumentParent").transform.localRotation = rot_left;
            GameObject.Find("LeftInstrumentParent").transform.Rotate(new Vector3(0, 0, -Instance.lastFixtureTipTransform_left.rotation), Space.Self);
            GameObject.Find("RightInstrumentParent").transform.localRotation = rot_right;
            GameObject.Find("RightInstrumentParent").transform.Rotate(new Vector3(0, 0, -Instance.lastFixtureTipTransform_right.rotation), Space.Self);
            this.Instance.ReceiveDataLeft = false;
            this.Instance.ReceiveDataRight = false;
        }
    }
    void insertPosition(float left, float right)
    {
        //move instrument along z axis with the value
        //GameObject.Find("LeftInstrument").transform.localPosition = new Vector3(0, 0, left-0.02f);
        GameObject.Find("laparo_grasper").transform.localPosition = new Vector3(0, 0, left);
        GameObject.Find("laparo_scissors").transform.localPosition = new Vector3(0, 0, right);
        //GameObject.Find("RightInstrument").transform.localPosition = new Vector3(0, 0, right);
        //GameObject.Find("LeftInstrumentTip").transform.localPosition = new Vector3(0, 0, left + leftInstruLength);
        //GameObject.Find("RightInstrumentTip").transform.localPosition = new Vector3(0, 0, right + (rightInstruLength / 2));
        //GameObject.Find("LeftInstrumentTip").transform.localRotation = Quaternion.Euler(0,0,-Instance.lastFixtureTipTransform_left.rotation);
        //GameObject.Find("RightInstrumentTip").transform.localRotation = Quaternion.Euler(0, 0, -Instance.lastFixtureTipTransform_right.rotation);
        leftTip_norot.transform.localPosition = new Vector3(0, 0, left + (leftInstruLength / 2));
        rightTip_norot.transform.localPosition = new Vector3(0, 0, right + (rightInstruLength / 2));

    }

    class CASTClient : MonoBehaviour
    {
        private int port = 8500;
        private FixtureTipTransform _lastFixtureTipTransform_left;
        private FixtureTipTransform _lastFixtureTipTransform_right;
        private bool _ReceiveDataLeft = false;
        private bool _ReceiveDataRight = false;
        private object lock_left = new object();
        private object lock_right = new object();
        private object lock_receive_left = new object();
        private object lock_receive_right = new object();
        public bool ReceiveDataLeft
        {
            get
            {
                lock (lock_receive_left)
                {
                    return _ReceiveDataLeft;
                }
            }
            set
            {
                lock (lock_receive_left)
                {
                    _ReceiveDataLeft = value;
                }
            }
        }
        public bool ReceiveDataRight
        {
            get
            {
                lock (lock_receive_right)
                {
                    return _ReceiveDataRight;
                }
            }
            set
            {
                lock (lock_receive_right)
                {
                    _ReceiveDataRight = value;
                }
            }
        }
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

#if UNITY_WSA_10_0 && !UNITY_EDITOR
        DatagramSocket socket;
        IOutputStream outstream;
        //DataReader reader;
        DataWriter writer;
#else
        UdpClient udp;
#endif

        IPEndPoint ep;

        public void Init(int _port)
        {
            this.port = _port;
#if UNITY_WSA_10_0 && !UNITY_EDITOR
        socket = new DatagramSocket();
        socket.MessageReceived += SocketOnMessageReceived;
        socket.BindServiceNameAsync(port.ToString()).GetResults();
        //outstream = socket.GetOutputStreamAsync(new HostName(ep.Address.ToString()), port.ToString()).GetResults();
        //writer = new DataWriter(outstream);
#else
            udp = new UdpClient(port);
            udp.BeginReceive(new AsyncCallback(receiveMsg), null);

#endif
        }

#if UNITY_WSA_10_0 && !UNITY_EDITOR

        //private async void SendMessage(string message)
        //{
        //    var socket = new DatagramSocket();

        //    using (var stream = await socket.GetOutputStreamAsync(new HostName(ep.Address.ToString()), port.ToString()))
        //    {
        //        using (var writer = new DataWriter(stream))
        //        {
        //            var data = Encoding.UTF8.GetBytes(message);

        //            writer.WriteBytes(data);
        //            writer.StoreAsync();
        //            //Debug.Log("sent " + data.Length);
        //        }
        //    }
        //}

        private async void SocketOnMessageReceived(DatagramSocket sender, DatagramSocketMessageReceivedEventArgs args)
        {
            Debug.Log("RECEIVED VOID");

            var result = args.GetDataStream();
            var resultStream = result.AsStreamForRead(1400);

            using (var reader = new StreamReader(resultStream))
            {
                var text = await reader.ReadToEndAsync();
                //var text = reader.ReadToEnd();

                handleMsg(text);

                Debug.Log("MESSAGE: " + text);
            }
        }
#else

#endif

        void receiveMsg(IAsyncResult result)
        {
            //Debug.Log("RECEIVING");
#if UNITY_WSA_10_0 && !UNITY_EDITOR

#else
            IPEndPoint source = new IPEndPoint(0, 0);
            //byte[] message = udp.EndReceive(result, ref source);
            //Debug.Log("RECV " + Encoding.UTF8.GetString(message) + " from " + source);

            string message = Encoding.UTF8.GetString(udp.EndReceive(result, ref source));

            handleMsg(message);
            // schedule the next receive operation once reading is done:
            udp.BeginReceive(new AsyncCallback(receiveMsg), udp);
           
#endif
        }

        void handleMsg(string msg)
        {
            FixtureTipTransform ft = JsonUtility.FromJson<FixtureTipTransform>(msg);
            if (ft.direction.Equals("Left"))
            {
                if (!ReceiveDataLeft)
                {
                    ReceiveDataLeft = true;
                }
                lastFixtureTipTransform_left = ft;
                //Debug.Log("Left match");
            }
            if (ft.direction.Equals("Right"))
            {
                if (!ReceiveDataRight)
                {
                    ReceiveDataRight = true;
                }
                lastFixtureTipTransform_right = ft;
                //Debug.Log("Right match");
            }
            return;
        }

        public void Stop()
        {
#if UNITY_WSA_10_0 && !UNITY_EDITOR

#else
            if (udp != null)
                udp.Close();
#endif
        }
    }
}