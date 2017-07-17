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
    public Vector3 cam_position;
    public Quaternion cam_rotation;

    public TrackerTransform() { id = "unspecified"; position = new Vector3(); rotation = new Quaternion(); cam_position = new Vector3();
        cam_rotation = new Quaternion();
    }
    public TrackerTransform(string id_, Vector3 pos, Quaternion quat) { id = id_; position = pos; rotation = quat; cam_position = new Vector3();
        cam_rotation = new Quaternion();
    }
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
    private Quaternion meter_rot;
    private Quaternion camera_rot;
    private Quaternion rot_calib;//need to be rotated before other rotation.
    private GameObject calibratedCam;
    private GameObject calibratedCamChild;
    private bool calibrating = false;

    private HoloClient Instance { get; set; }
    // Use this for initialization

      void Start()
    {
        this.Instance = this.gameObject.AddComponent<HoloClient>();
        this.Instance.Init(port, output_rate);
       
        from0toTable = new Quaternion(0.666f, -0.230f, 0.230f, 0.671f);
        re_from0toTable = Quaternion.Inverse(from0toTable);

        calibratedCam = new GameObject("calibratedCam");
        calibratedCam.transform.position = new Vector3();
        calibratedCam.transform.rotation = new Quaternion();
        calibratedCamChild = new GameObject("calibratedCamChild");
        calibratedCamChild.transform.parent = calibratedCam.transform;

        Vector3 pos = new Vector3(-0.134f, -0.977f, -0.906f); //origin
        //pos = new Vector3(-0.284f, -0.967f, -1.01f);//point2
        //pos = new Vector3(-0.347f,-0.787f,-1.046f);//point3
        //pos = new Vector3(-0.346f, -0.966f, -0.933f);//point4
        //pos = new Vector3(-0.545f, -0.962f, -0.676f);//point5
        //Vector3 pos2 = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), pointQuat);
        //Debug.Log("pos2 x: " + pos2.x + " y: " + pos2.y + " z: " + pos2.z);


        Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
        Vector3 final_pos = new Vector3(mid_pos.x, mid_pos.z, -mid_pos.y);
        //this.transform.localPosition = final_pos;
        this.transform.parent.Find("ViveMeter").localPosition = final_pos;
        meter_position = final_pos;

        Vector3 init_pos = new Vector3(0.134f, -0.977f, -0.906f);
        Vector3 end_pos = new Vector3(0.6551008f, -0.9704683f, -0.6384149f);
        meter_rot = Quaternion.FromToRotation(init_pos, end_pos);
        this.transform.parent.Find("ViveMeter").localRotation = meter_rot;

        Quaternion re_rot = new Quaternion(from0toTable.x, from0toTable.z, -from0toTable.y, from0toTable.w);
        rot_calib = Quaternion.Inverse(meter_rot * re_rot);

       Debug.Log("HoloReceiver Started");
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

        if (Input.GetKeyDown(KeyCode.C) && !calibrating)
        {
            this.StartCalibrateOrigin();
            Debug.Log("C pressed");
        }
        if (Input.GetKeyDown(KeyCode.H) && !calibrating)
        {
            this.StartCalibrateHolo();
            Debug.Log("H pressed");
        }

        if (Instance.bTT_1 && !calibrating)
        { 
            Vector3 pos = Instance.lastTrackerTransform_1.position;
            Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
            //position relative to vive meter
            Vector3 mid2_pos = new Vector3(-mid_pos.x, -mid_pos.z, mid_pos.y);
            //Vector3 final_pos = meter_position + mid2_pos;
            


            //Transform localToCamera = Camera.main.transform.Find("TrackerLocalToCamera");
            //localToCamera.position = meter_position;
            //localToCamera.localPosition = localToCamera.localPosition + mid2_pos;
            //this.transform.position = localToCamera.position;

            Transform calibratedChildTrans = calibratedCamChild.transform;
            calibratedChildTrans.position = meter_position;
            calibratedChildTrans.localPosition = calibratedChildTrans.localPosition + mid2_pos;
            this.transform.position = calibratedChildTrans.position;


            //Quaternion rot = Quaternion.Euler(90, 0, 0) * Quaternion.Inverse(Instance.lastTrackerTransform.rotation);
            Quaternion rot = (Instance.lastTrackerTransform_1.rotation);

            //version1 by far best (y axis works)
            //rot = new Quaternion(rot.x, -rot.y, -rot.z, rot.w);
            //Quaternion re_from0toTable_left = new Quaternion(re_from0toTable.x, -re_from0toTable.y, -re_from0toTable.z, re_from0toTable.w);

            //version2 not working
            //rot = new Quaternion(rot.x, -rot.z, -rot.y, rot.w);
            //Quaternion re_from0toTable_left = new Quaternion(re_from0toTable.x, -re_from0toTable.z, -re_from0toTable.y, re_from0toTable.w);

            //version3 not working
            //rot = new Quaternion(-rot.x, rot.y, -rot.z, rot.w);
            //Quaternion re_from0toTable_left = new Quaternion(-re_from0toTable.x, re_from0toTable.y, -re_from0toTable.z, re_from0toTable.w);

            //version4
            rot = new Quaternion(rot.x, rot.z, -rot.y, rot.w);
            Quaternion re_rot = new Quaternion(0.666f, 0.230f, 0.230f, 0.671f);
            //Quaternion re_from0toTable_left = new Quaternion(re_from0toTable.x, -re_from0toTable.y, -re_from0toTable.z, re_from0toTable.w);

            //Quaternion change = (Quaternion.Inverse(meter_rot * re_rot) * meter_rot * rot);
            Quaternion change = rot_calib * meter_rot * rot;
            //Quaternion change_left = new Quaternion(-change.x, -change.y, -change.z, change.w);
            //Debug.Log(change);
            this.transform.localRotation = camera_rot * change;
        }
    }

    void StartCalibrateOrigin()
    {
        if (Instance.bTT_0)
        {
            Debug.Log("Start Calibration Procedure");
            calibrating = true;
            //calibrate meter position
            Vector3 pos = Instance.lastTrackerTransform_0.position;
            from0toTable = Instance.lastTrackerTransform_0.rotation;
            re_from0toTable = Quaternion.Inverse(from0toTable);
            Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
            Vector3 final_pos = new Vector3(mid_pos.x, mid_pos.z, -mid_pos.y);
            meter_position = final_pos;

            this.transform.parent.Find("ViveMeter").localPosition = final_pos;
            //calibrate meter rotation
            Vector3 init_pos = pos;
            Vector3 end_pos = new Vector3(-mid_pos.x, -mid_pos.z, mid_pos.y);
            //Vector3 end_pos = new Vector3(0.6551008f, -0.9704683f, -0.6384149f);
            meter_rot = Quaternion.FromToRotation(init_pos, end_pos);

           // Quaternion re_rot = new Quaternion(0.666f, 0.230f, 0.230f, 0.671f);
            Quaternion re_rot = new Quaternion(from0toTable.x, from0toTable.z, -from0toTable.y, from0toTable.w);
            rot_calib = Quaternion.Inverse(meter_rot * re_rot);
            this.transform.parent.Find("ViveMeter").localRotation = meter_rot;

            calibrating = false;
        }
        return;
    }

    public void StartCalibrateHolo()
    {
        if (Instance.bTT_0)
        {
            Debug.Log("Start Holo Calibration Procedure");
            calibrating = true;
            Vector3 stablePosCalib = new Vector3(0, 0.075f, 0.015f);
            //calibrate meter position
            Vector3 pos = Instance.lastTrackerTransform_0.position;
            Vector3 camera_pos = Instance.lastTrackerTransform_0.cam_position; 
            Quaternion tracker_ori = Instance.lastTrackerTransform_0.rotation;
            camera_rot = Instance.lastTrackerTransform_0.cam_rotation;
            from0toTable = tracker_ori;
            re_from0toTable = Quaternion.Inverse(from0toTable);

            Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
            Vector3 final_pos = new Vector3(mid_pos.x, mid_pos.z, -mid_pos.y);
            Transform localToCamera = Camera.main.transform.Find("TrackerLocalToCamera");
            localToCamera.localPosition = final_pos + stablePosCalib;
            meter_position = localToCamera.position;

            calibratedCam.transform.position = camera_pos;
            calibratedCam.transform.rotation = camera_rot;

            this.transform.parent.Find("ViveMeter").position = meter_position;

            //calibrate meter rotation
            Vector3 init_pos = pos;
            Vector3 end_pos = new Vector3(-mid_pos.x, -mid_pos.z, mid_pos.y);
            Quaternion mid_rot = Quaternion.FromToRotation(init_pos, end_pos);
            localToCamera.localRotation = mid_rot;
            //Vector3 end_pos = new Vector3(0.6551008f, -0.9704683f, -0.6384149f);
            meter_rot = localToCamera.rotation;
            this.transform.parent.Find("ViveMeter").rotation = meter_rot;

            // Quaternion re_rot = new Quaternion(0.666f, 0.230f, 0.230f, 0.671f);
            Quaternion re_rot = new Quaternion(from0toTable.x, from0toTable.z, -from0toTable.y, from0toTable.w);
            rot_calib = Quaternion.Inverse(meter_rot * re_rot);

            calibrating = false;
        }
        return;
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
        private int port = 9000;
        private float output_rate = 0;
        private TrackerTransform _lastTrackerTransform_0 = new TrackerTransform();
        private TrackerTransform _lastTrackerTransform_1 = new TrackerTransform();
        public bool bTT_0 = false;
        public bool bTT_1 = false;
        public bool canReadCam_0 = false;
        public bool canReadCam_1 = false;

        private object _lock_0 = new object();
        private object _lock_1 = new object();

        public TrackerTransform lastTrackerTransform_0
        {
            get
            {
                lock (_lock_0)
                {
                    return _lastTrackerTransform_0;
                }
            }
            private set
            {
                lock (_lock_0)
                {
                    _lastTrackerTransform_0 = value;
                }
            }
        }

        public TrackerTransform lastTrackerTransform_1
        {
            get
            {
                lock (_lock_1)
                {
                    return _lastTrackerTransform_1;
                }
            }
            private set
            {
                lock (_lock_1)
                {
                    _lastTrackerTransform_1 = value;
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

        // Update is called once per frame
        void Update()
        {

            if (canReadCam_0)
            {
                lastTrackerTransform_0.cam_position = Camera.main.transform.position;
                lastTrackerTransform_0.cam_rotation = Camera.main.transform.rotation;
                canReadCam_0 = false;
            }
            if (canReadCam_1)
            {
                lastTrackerTransform_1.cam_position = Camera.main.transform.position;
                lastTrackerTransform_1.cam_rotation = Camera.main.transform.rotation;
                canReadCam_1 = false;
            }

            if (output_rate > 0)
            {
                sps_time += Time.deltaTime;
                if (sps_time >= 1.0f / output_rate)
                {
                    //Debug.Log("Net SPS: " + count_sps / sps_time);// + " - " + output_rate);
                    sps_time = 0;
                    count_sps = 0;

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
                bTT_0 = true;
                lastTrackerTransform_0 = tt;
                canReadCam_0 = true;
                //Debug.Log("HOLO " + ht.position.ToString() + " : " + ht.rotation.eulerAngles.ToString() + " from " + source);

                ++count_sps;
            }
            if (tt.id.Equals("tracker1"))
            {
                bTT_1 = true;
                lastTrackerTransform_1 = tt;
                canReadCam_1 = true;
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

