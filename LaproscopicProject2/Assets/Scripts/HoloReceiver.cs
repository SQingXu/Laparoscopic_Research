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
public class TrackerTransform
{
    public string id;
    public Vector3 position;
    public Quaternion rotation;

    public TrackerTransform() { id = "unspecified"; position = new Vector3(); rotation = new Quaternion(); }
    public TrackerTransform(string id_, Vector3 pos, Quaternion quat) { id = id_; position = pos; rotation = quat; }
}

public class HoloReceiver : MonoBehaviour
{

    public int port = 7778;
    public bool follow = false;
    public string TrackerID = "tracker1";

    [Range(0f, 100.0f)]
    public float output_rate;// = 0.2f;

    private HoloClient Instance { get; set; }
    // Use this for initialization

      void Start()
    {
        this.Instance = this.gameObject.AddComponent<HoloClient>();
        this.Instance.Init(port, output_rate);

       // this.enabled = false;
    }

    // Update is called once per frame
    void LateUpdate()
    {
        //TrackerTransform tt = new TrackerTransform();
        //string st = JsonUtility.ToJson(tt);
        ////Debug.Log("UPDATE");
        //Debug.Log(st);

        //if (Instance.bTT)
        {
            //
            //Debug.Log("Applying Tracker Update");

            //this.transform.position = Instance.lastTrackerTransform.position;
            //this.transform.rotation = Instance.lastTrackerTransform.rotation;

            //Camera.main.transform.position = Instance.lastTrackerTransform.position;
            //Camera.main.transform.rotation = Instance.lastTrackerTransform.rotation;

            //Debug.Log("Cam  " + Camera.main.transform.position.ToString());
            //Debug.Log("CamF " + FakeCamera.transform.position.ToString());
            //Debug.Log("Root " + transform.position.ToString());
        }

        if (Instance.bTT)
        {
            //Vector3 pos = Instance.lastTrackerTransform.position;
            //pos = new Vector3(-pos.x, pos.y, pos.z);
            ////Vector3 hmdPos = Quaternion.Euler(90, 0, 0) * pos;
            //Vector3 hmdPos = pos + new Vector3(0f, .0f, 3.0f);
            ////hmdPos.x *= -1.0f;
            //this.transform.localPosition = hmdPos;
            ////Debug.Log("Pos " + Instance.lastTrackerTransform.position + " - " + hmdPos);

            ////Quaternion rot = Quaternion.Euler(90, 0, 0) * Quaternion.Inverse(Instance.lastTrackerTransform.rotation);
            //Quaternion rot = (Instance.lastTrackerTransform.rotation);
            //rot.x *= - 1.0f;
            //rot.y *= -1.0f;
            ////rot.z *= -1.0f;
            //rot = new Quaternion(rot.x, rot.y, rot.z, rot.w);
            //Quaternion quatB = Quaternion.AngleAxis(90, Vector3.right);
            //Quaternion quatC = Quaternion.AngleAxis(180, Vector3.up);
            //rot =  quatC *rot * quatB * quatC;
            //Vector3 hmdRot = rot.eulerAngles;
            ////hmdRot.x *= - 1.0f;
            //Debug.Log("Rot " + Instance.lastTrackerTransform.rotation.eulerAngles + " - " + hmdRot);
            //this.transform.localRotation = Quaternion.Euler(hmdRot);

            Vector3 pos = Instance.lastTrackerTransform.position;
            pos = new Vector3(-pos.z, pos.y, pos.x);
            //Vector3 hmdPos = Quaternion.Euler(90, 0, 0) * pos;
            this.transform.localPosition = pos;
            //Debug.Log("Pos " + Instance.lastTrackerTransform.position + " - " + hmdPos);

            //Quaternion rot = Quaternion.Euler(90, 0, 0) * Quaternion.Inverse(Instance.lastTrackerTransform.rotation);
            Quaternion rot = (Instance.lastTrackerTransform.rotation);
            rot.x *= -1.0f;
            rot.y *= -1.0f;
            //rot.z *= -1.0f;
            rot = new Quaternion(rot.x, rot.y, rot.z, rot.w);
            this.transform.localRotation = rot;
        }
    }

    //void LateUpdate()
    //{
    //    //Camera.main.transform.Rotate(new Vector3(0.01f, 0f, 0f));
    //    Debug.Log("LateUpdate");
    //    if (Instance.bTT)
    //    {
    //        //
    //        Debug.Log("Applying Tracker LateUpdate " + Instance.lastTrackerTransform.position.ToString());
    //        this.transform.position = Instance.lastTrackerTransform.position;
    //        this.transform.rotation = Instance.lastTrackerTransform.rotation;
    //        //Camera.main.transform.position = Instance.lastTrackerTransform.position;
    //        //Camera.main.transform.rotation = Instance.lastTrackerTransform.rotation;


    //    }
    //}

    class HoloClient : MonoBehaviour
    {
        public GameObject cube;
        private int port = 9000;
        private float output_rate = 0;
        private bool TransformChange = false;
        private bool FirstTransform = true;
        private TrackerTransform previousTrackerTransform = new TrackerTransform();
        private TrackerTransform _lastTrackerTransform = new TrackerTransform();
        public bool bTT = false;

        private object _lock = new object();

        public TrackerTransform lastTrackerTransform
        {
            get
            {
                lock (_lock)
                {
                    return _lastTrackerTransform;
                }
            }
            private set
            {
                lock (_lock)
                {
                    _lastTrackerTransform = value;
                }
            }
        }

#if UNITY_WSA_10_0  && !UNITY_EDITOR 
        DatagramSocket socket;
        IOutputStream outstream;
        //DataReader reader;
        DataWriter writer;
#else
        UdpClient udp;
#endif

        IPEndPoint ep;

        float sps_time = 0;
        int count_sps = 0;

        public void Init(int port_)
        {
            this.port = port_;
#if UNITY_WSA_10_0  && !UNITY_EDITOR
            socket = new DatagramSocket();
            socket.MessageReceived += SocketOnMessageReceived;
            socket.BindServiceNameAsync(port.ToString()).GetResults();
            //outstream = socket.GetOutputStreamAsync(new HostName(ep.Address.ToString()), port.ToString()).GetResults();
            //writer = new DataWriter(outstream);

#else
            udp = new UdpClient(port);
            udp.BeginReceive(new AsyncCallback(receiveMsg), null);
            Debug.Log("Begin Receive");
#endif
        }

        public void Init(int port_, float output_rate_)
        {
            this.output_rate = output_rate_;
            this.Init(port_);
        }

        // Use this for initialization
        void Start()
        {
            sps_time = 0;
            count_sps = 0;
        }


#if UNITY_WSA_10_0 && !UNITY_EDITOR

        private async void SendMessage(string message)
        {
            var socket = new DatagramSocket();

            using (var stream = await socket.GetOutputStreamAsync(new HostName(ep.Address.ToString()), port.ToString()))
            {
                using (var writer = new DataWriter(stream))
                {
                    var data = Encoding.UTF8.GetBytes(message);

                    writer.WriteBytes(data);
                    writer.StoreAsync();
                    //Debug.Log("sent " + data.Length);
                }
            }
        }

        private async void SocketOnMessageReceived(DatagramSocket sender, DatagramSocketMessageReceivedEventArgs args)
        {
            //            Debug.Log("RECEIVED VOID");

            var result = args.GetDataStream();
            var resultStream = result.AsStreamForRead(1400);

            using (var reader = new StreamReader(resultStream))
            {
                var text = await reader.ReadToEndAsync();
                //var text = reader.ReadToEnd();

                handleMsg(text);

                //                Debug.Log("MESSAGE: " + text);
            }
        }
#else

#endif

        // Update is called once per frame
        void Update()
        {
            //if (TransformChange == true)
            //{
            //    Vector3 positionChange = lastTrackerTransform.position - previousTrackerTransform.position;
            //    //Quaternion rotationChange = lastTrackerTransform.rotation - previousTrackerTransform.rotation;
            //    cube.transform.position = positionChange;
            //    cube.transform.rotation = lastTrackerTransform.rotation;
            //    TransformChange = false;
            //}
            if (output_rate > 0)
            {
                sps_time += Time.deltaTime;
                if (sps_time >= 1.0f / output_rate)
                {
                    Debug.Log("Net SPS: " + count_sps / sps_time);// + " - " + output_rate);
                    sps_time = 0;
                    count_sps = 0;
           //         Debug.Log("TT " + lastTrackerTransform.id + " " + lastTrackerTransform.position + " Hr " + lastTrackerTransform.rotation.eulerAngles);
                    //Debug.Log("Rp " + lastHoloTransform.rposition + " Rr " + lastHoloTransform.rrotation.eulerAngles);
                    //Debug.Log("Ap " + lastHoloTransform.aposition + " Ar " + lastHoloTransform.arotation.eulerAngles);
                    //Debug.Log("Pp " + lastHoloTransform.pposition + " Pr " + lastHoloTransform.protation.eulerAngles);
                    //for(int i=0; i<lastHoloTransform.holoViewMatrices.Length; ++i)
                    //for (int i = 0; i < 2; ++i)
                    //{
                    //    //V Debug.Log("HVM_" + i + ": " + lastHoloMatrices.holoViewMatrices[i].ToString());
                    //    Debug.Log("HPM_" + i + ": " + lastHoloMatrices.holoProjMatrices[i].ToString());
                    //}
                    //Debug.Log("isStereo " + (lastHoloMatrices.isStereo ? "true":"false"));
                    //Debug.Log("Separation " + lastHoloMatrices.separation);
                    //Debug.Log("Convergence " + lastHoloMatrices.convergence);
                    //Debug.Log("Near " + lastHoloMatrices.near);
                    //Debug.Log("Far " + lastHoloMatrices.far);
                    //Debug.Log("Fov " + lastHoloMatrices.fov);
                    //Debug.Log("aspect " + lastHoloMatrices.aspect);

                }
            }

        }


        void receiveMsg(IAsyncResult result)
        {
            // while (!endReceive)
            {
                //Debug.Log("RECEIVING");

#if UNITY_WSA_10_0  && !UNITY_EDITOR

#else
                IPEndPoint source = new IPEndPoint(0, 0);
                //byte[] message = udp.EndReceive(result, ref source);
                //Debug.Log("RECV " + Encoding.UTF8.GetString(message) + " from " + source);

                string message = Encoding.UTF8.GetString(udp.EndReceive(result, ref source));
                //Debug.Log("RECV " + message + " from " + source);

                handleMsg(message);

                // schedule the next receive operation once reading is done:
                udp.BeginReceive(new AsyncCallback(receiveMsg), udp);

#endif
            }
        }

        void handleMsg(string message)
        {
            TrackerTransform tt = JsonUtility.FromJson<TrackerTransform>(message);
            if (tt.id.Equals("tracker0"))
            {
                bTT = true;
                lastTrackerTransform = tt;

                //Debug.Log("HOLO " + ht.position.ToString() + " : " + ht.rotation.eulerAngles.ToString() + " from " + source);

                ++count_sps;
            }
            //Debug.Log("HOLO " + tt.id + " " + tt.position.ToString() + " : " + tt.rotation.eulerAngles.ToString() + " from " + source);
            //++count_sps;
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

