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

    public int port = 9000;
    public bool follow = false;
    public string TrackerID = "tracker1";


    [Range(0f, 100.0f)]
    public float output_rate;// = 0.2f;
    private Quaternion from0toTable;
    Quaternion re_from0toTable;
    private Vector3 meter_position;

    private HoloClient Instance { get; set; }
    // Use this for initialization

      void Start()
    {
        this.Instance = this.gameObject.AddComponent<HoloClient>();
        this.Instance.Init(port, output_rate);
        //right-handed quaternion
        from0toTable = new Quaternion(0.666f, -0.230f, 0.230f, 0.671f);
        Quaternion quatFromMToT = new Quaternion(-0.006f, -0.692f, 0.722f, 0.001f);
        //Quaternion demo = new Quaternion(0.69288583f, 0.20189612f, -0.20377779f, -0.66152990f);
        //Quaternion quatFromMToT = new Quaternion(0.017f, 0.655f, -0.755f, 0.006f);
        Quaternion quatFromTToM = Quaternion.Inverse(quatFromMToT);
        Quaternion pointQuat = Quaternion.Inverse(quatFromTToM * from0toTable);
        Vector3 pos = new Vector3(-0.134f, -0.977f, -0.906f); //origin
        //pos = new Vector3(-0.284f, -0.967f, -1.01f);//point2
        //pos = new Vector3(-0.347f,-0.787f,-1.046f);//point3
        //pos = new Vector3(-0.346f, -0.966f, -0.933f);//point4
        //pos = new Vector3(-0.545f, -0.962f, -0.676f);//point5
        Vector3 pos2 = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), pointQuat);
        Debug.Log("pos2 x: " + pos2.x + " y: " + pos2.y + " z: " + pos2.z);

        Vector3 demo_euler = from0toTable.eulerAngles;

        //Vector3 xyz = new Vector3(3, 6, 7);
        //Quaternion XYZ = Quaternion.Euler(30, 50, 21);
        //Vector3 xyz1 = this.rotateAroundAxis(xyz, new Vector3(0, 0, 0), XYZ);
        //Quaternion in_XYZ = Quaternion.Inverse(XYZ);
        //Vector3 xyz2 = this.rotateAroundAxis(xyz1, new Vector3(0, 0, 0), in_XYZ);
        //Debug.Log("Inverse test x: " + xyz2.x + " y: " + xyz2.y + " z: " + xyz2.z);

        //Debug.Log("x: " + demo_euler.x + " y: " + demo_euler.y + " z: " + demo_euler.z);
        //change to left handed quaternion
        //Vector3 fllipedRotation = new Vector3(demo_euler.x, -demo_euler.y, -demo_euler.z);
        //Quaternion qx = Quaternion.AngleAxis(fllipedRotation.x, Vector3.right);
        //Quaternion qy = Quaternion.AngleAxis(fllipedRotation.y, Vector3.up);
        //Quaternion qz = Quaternion.AngleAxis(fllipedRotation.z, Vector3.forward);
        //Quaternion demo_left = qz * qy * qx;

        //Vector3 demo_euler_left = demo_left.eulerAngles;
        //Debug.Log("x: " + demo_euler_left.x + " y: " + demo_euler_left.y + " z: " + demo_euler_left.z);


        re_from0toTable = Quaternion.Inverse(from0toTable);
        Vector3 position_new = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
        //Debug.Log("new position after rotation x: " + position_new.x + " y: " + position_new.y + " z: " + position_new.z);

        Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
        Vector3 final_pos = new Vector3(mid_pos.x, mid_pos.z, -mid_pos.y);
        //this.transform.localPosition = final_pos;
        Quaternion change = Quaternion.FromToRotation(mid_pos, final_pos);
        Quaternion re_from0toTable_left = new Quaternion(re_from0toTable.x,-re_from0toTable.y,-re_from0toTable.z,-re_from0toTable.w);
        this.transform.parent.Find("ViveMeter").localPosition = final_pos;
        meter_position = final_pos;
        this.transform.parent.Find("ViveMeter").localRotation = Quaternion.Inverse(re_from0toTable * change);

        // this.enabled = false;
    }
    Vector3 rotateAroundAxis(Vector3 point, Vector3 pivot, Quaternion quat)
    {
        Vector3 finalPos = point - pivot;
        finalPos = quat * finalPos;
        finalPos += pivot;
        return finalPos;
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
            Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
            //position relative to vive meter
            Vector3 mid2_pos = new Vector3(-mid_pos.x, -mid_pos.z, mid_pos.y);
            Vector3 final_pos = meter_position + mid2_pos;
            //Vector3 hmdPos = Quaternion.Euler(90, 0, 0) * pos;
            this.transform.localPosition = final_pos;
            //Debug.Log("Pos " + Instance.lastTrackerTransform.position + " - " + hmdPos);

            //Quaternion rot = Quaternion.Euler(90, 0, 0) * Quaternion.Inverse(Instance.lastTrackerTransform.rotation);
            Quaternion rot = (Instance.lastTrackerTransform.rotation);
            rot.x *= -1.0f;
            rot.y *= -1.0f;
            rot.z *= -1.0f;
            rot = new Quaternion(rot.x, rot.y, rot.z, rot.w);
            Quaternion change = Quaternion.RotateTowards(from0toTable, rot, 180);
            this.transform.localRotation = change;
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

