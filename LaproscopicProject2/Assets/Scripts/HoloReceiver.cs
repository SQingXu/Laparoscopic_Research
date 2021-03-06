﻿#if UNITY_WSA_10_0 && !UNITY_EDITOR
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
[Serializable]
public class StoredTransform
{
    //relative to Vive meter 
    public Vector3 StoredPosition;
    public Quaternion StoredRotation;
    public StoredTransform(Vector3 v, Quaternion q)
    {
        this.StoredPosition = v;
        this.StoredRotation = q;
    } 

}

public class HoloReceiver : MonoBehaviour
{

    public int port = 9000;
    private int drift = 10;
    public bool follow = false;
    public string TrackerID = "tracker1";


    [Range(0f, 100.0f)]
    public float output_rate;// = 0.2f;
    public int driftadjust_rate = 10;
    public string filename = "StoredTable.json";
    private Quaternion from0toTable;
    //Quaternion re_from0toTable;
    //private Vector3 meter_position;
    //private Vector3 counter_drift_total;
    //private Vector3 previous_drift_amount;
    //private int drift_accum_curr;
    //private Quaternion meter_rot;
    private Quaternion camera_rot;
    private Quaternion rot_calib;//need to be rotated before other rotation.
    private GameObject calibratedCam;
    private GameObject calibratedCamChild;
    private GameObject tableParent;
    private GameObject tableChild;
    private GameObject storedTable;
    private Vector3 storedTablePos;
    private Quaternion storedTableRot;
    private bool calibrating = false;
    Vector3 HoloToTrackerDisplacement = new Vector3(0, 0.058f, 0.056f);
    //private bool calib_confirmed = false;
    private bool first_tracking = true;

    public GameObject ViveEmitter;
    public GameObject ViveHoloTracker;
    public GameObject ViveCASTTracker;
    public GameObject CASTOrigin;
    public string fileName = "StoredTableCalibration.json";
    public string filePath;
    bool ViveEmitter_calibrated = false;
    Vector3 ViveToHoloPos = new Vector3(0, 0, 0);
    Quaternion ViveToHoloRot = new Quaternion(0, 0, 0, 1);
    bool ViveToHolo_calibrated = false;

    private HoloClient Instance { get; set; }
    // Use this for initialization

      void Start()
    {
        this.Instance = this.gameObject.AddComponent<HoloClient>();
        this.Instance.Init(port, output_rate);
       
        //from0toTable = new Quaternion(0.666f, -0.230f, 0.230f, 0.671f);
        //re_from0toTable = Quaternion.Inverse(from0toTable);

        calibratedCam = new GameObject("calibratedCam");
        calibratedCam.transform.position = new Vector3();
        calibratedCam.transform.rotation = new Quaternion();
        calibratedCamChild = new GameObject("calibratedCamChild");
        calibratedCamChild.transform.parent = calibratedCam.transform;
        tableParent = new GameObject("calibratedTableParent");
        tableParent.transform.position = new Vector3();
        tableParent.transform.rotation = new Quaternion();
        tableChild = new GameObject("calibratedTableOrigin");
        tableChild.transform.parent = tableParent.transform;
        storedTable = new GameObject("StoredTableRelativeToViveMeter");
        storedTable.transform.parent = this.transform.parent.Find("ViveMeter");
        Vector3 pos = new Vector3(-0.134f, -0.977f, -0.906f);
        //origin
        //pos = new Vector3(-0.284f, -0.967f, -1.01f);//point2
        //pos = new Vector3(-0.347f,-0.787f,-1.046f);//point3
        //pos = new Vector3(-0.346f, -0.966f, -0.933f);//point4
        //pos = new Vector3(-0.545f, -0.962f, -0.676f);//point5
        //Vector3 pos2 = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), pointQuat);
        //Debug.Log("pos2 x: " + pos2.x + " y: " + pos2.y + " z: " + pos2.z);

        //Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
        //Vector3 final_pos = new Vector3(mid_pos.x, mid_pos.z, -mid_pos.y);
        ////this.transform.localPosition = final_pos;
        //this.transform.parent.Find("ViveMeter").localPosition = final_pos;
        //meter_position = final_pos;

        //Vector3 init_pos = new Vector3(0.134f, -0.977f, -0.906f);
        //Vector3 end_pos = new Vector3(0.6551008f, -0.9704683f, -0.6384149f);
        //meter_rot = Quaternion.FromToRotation(init_pos, end_pos);
        //this.transform.parent.Find("ViveMeter").localRotation = meter_rot;

        //Quaternion re_rot = new Quaternion(from0toTable.x, from0toTable.z, -from0toTable.y, from0toTable.w);
        //rot_calib = Quaternion.Inverse(meter_rot * re_rot);

        filePath = Path.Combine(Application.streamingAssetsPath, fileName);
        /* GIOVA*/
        if (!ViveEmitter)
            ViveEmitter = GameObject.Find("ViveEmitter");
        if (!ViveCASTTracker)
            ViveCASTTracker = GameObject.Find("ViveTracker");
        if (!ViveHoloTracker)
            ViveHoloTracker = GameObject.Find("HoloTracker");
        //if (!ViveCASTTrackerDebug)
        //    ViveCASTTrackerDebug = GameObject.Find("ViveTrackerFromHolo");
        if (!CASTOrigin)
            CASTOrigin = GameObject.Find("Origin");

        camera_rot = new Quaternion(0, 0, 0, 1);
        //Debug.Log("HoloReceiver Started");
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
            //this.StartCalibrateOrigin();
            //this.CalibrateViveWithHololens();
            if (Instance.bTT_0 && !calibrating)
            {
                if (ViveEmitter_calibrated)
                {
                    CalibrateViveWithHololens();
                }
            }
            Debug.Log("C pressed");
        }
        if (Input.GetKeyDown(KeyCode.H) && !calibrating)
        {
            this.CalibrateRotation();
            Debug.Log("H pressed");
        }
        if(Input.GetKeyDown(KeyCode.S) && !calibrating)
        {
            Debug.Log("Confirm calibration of table, store the position and orientation of table");
            //this.StoreTableCalibration();
            this.StoreViveEmitterCalibration();
        }
        if(Input.GetKeyDown(KeyCode.L) && !calibrating)
        {
            Debug.Log("Load calibration data from previous one");
            //this.LoadTableCalibration();
            this.LoadViveEmitterVCalibration();
        }
        if (Input.GetKeyDown(KeyCode.E) && !calibrating)
        {
            Debug.Log("E pressed");
            this.CalibrateViveEmitter();
        }
        if (Input.GetKeyDown(KeyCode.UpArrow))
        {
            //Debug.Log("Up Arrow Key pressed");
            HoloToTrackerDisplacement += new Vector3(0, 0.002f, 0);
            Debug.Log(HoloToTrackerDisplacement.ToString("F4"));
        }
        if (Input.GetKeyDown(KeyCode.DownArrow))
        {
            //Debug.Log("Down Arrow Key pressed");
            HoloToTrackerDisplacement += new Vector3(0, -0.002f, 0);
            Debug.Log(HoloToTrackerDisplacement.ToString("F4"));
        }
        if (Input.GetKeyDown(KeyCode.RightArrow))
        {
            //Debug.Log("Right Arrow Key pressed");
            //backward
            HoloToTrackerDisplacement += new Vector3(0, 0, 0.002f);
            Debug.Log(HoloToTrackerDisplacement.ToString("F4"));
        }
        if (Input.GetKeyDown(KeyCode.LeftArrow))
        {
            //Debug.Log("Left Arrow Key pressed");
            //forward
            HoloToTrackerDisplacement += new Vector3(0, 0, -0.002f);
            Debug.Log(HoloToTrackerDisplacement.ToString("F4"));
        }
        //if (Instance.bTT_1 && !calibrating)
        //{
        //    Vector3 pos = Instance.lastTrackerTransform_1.position;
        //    Vector3 mid_pos = this.rotateAroundAxis(pos, new Vector3(0, 0, 0), re_from0toTable);
        //    //position relative to vive meter
        //    Vector3 mid2_pos = new Vector3(-mid_pos.x, -mid_pos.z, mid_pos.y);
        //    //Vector3 final_pos = meter_position + mid2_pos;



        //    //Transform localToCamera = Camera.main.transform.Find("TrackerLocalToCamera");
        //    //localToCamera.position = meter_position;
        //    //localToCamera.localPosition = localToCamera.localPosition + mid2_pos;
        //    //this.transform.position = localToCamera.position;

        //    Transform calibratedChildTrans = calibratedCamChild.transform;
        //    calibratedChildTrans.position = meter_position;
        //    calibratedChildTrans.localPosition = calibratedChildTrans.localPosition + mid2_pos;
        //    this.transform.position = calibratedChildTrans.position;


        //    //Quaternion rot = Quaternion.Euler(90, 0, 0) * Quaternion.Inverse(Instance.lastTrackerTransform.rotation);
        //    Quaternion rot = (Instance.lastTrackerTransform_1.rotation);

        //    //version1 by far best (y axis works)
        //    //rot = new Quaternion(rot.x, -rot.y, -rot.z, rot.w);
        //    //Quaternion re_from0toTable_left = new Quaternion(re_from0toTable.x, -re_from0toTable.y, -re_from0toTable.z, re_from0toTable.w);

        //    //version2 not working
        //    //rot = new Quaternion(rot.x, -rot.z, -rot.y, rot.w);
        //    //Quaternion re_from0toTable_left = new Quaternion(re_from0toTable.x, -re_from0toTable.z, -re_from0toTable.y, re_from0toTable.w);

        //    //version3 not working
        //    //rot = new Quaternion(-rot.x, rot.y, -rot.z, rot.w);
        //    //Quaternion re_from0toTable_left = new Quaternion(-re_from0toTable.x, re_from0toTable.y, -re_from0toTable.z, re_from0toTable.w);

        //    //version4
        //    rot = new Quaternion(rot.x, rot.z, -rot.y, rot.w);
        //    Quaternion change = rot_calib * meter_rot * rot;
        //    this.transform.localRotation = camera_rot * change;
        //}
        if (Instance.bTT_0 && Instance.bTT_1 && !calibrating)
        {
            TrackerTransform TT0 = Instance.lastTrackerTransform_0;
            TrackerTransform TT1 = Instance.lastTrackerTransform_1;
            if (TT0.position.x == 0 && TT0.position.y == 0 && TT0.position.z == 0 && TT0.rotation.x == 0 && TT0.rotation.y == 0 && TT0.rotation.z == 0 && TT0.rotation.w == 1.0f)
            {
                //Tracker0 Hololens one lose tracking
                Debug.Log("Tracker 0 lost tracking");
                //this.GetComponent<Renderer>().material.color = Color.blue;
                first_tracking = true;
                return;
            }
            else
            {
                //this.GetComponent<Renderer>().material.color = Color.white;
            }
            if (TT1.position.x == 0 && TT1.position.y == 0 && TT1.position.z == 0 && TT1.rotation.x == 0 && TT1.rotation.y == 0 && TT1.rotation.z == 0 && TT1.rotation.w == 1.0f)
            {
                //Tracker1 lose tracking
                Debug.Log("Tracker 1 lost tracking");
                //this.GetComponent<Renderer>().material.color = Color.red;
                first_tracking = true;
                return;
            }
            else
            {
                //this.GetComponent<Renderer>().material.color = Color.white;
            }
        }

        if (Instance.bTT_1 && !calibrating)
        {
            if (!ViveEmitter_calibrated)
            {
                UpdateViveEmitterCalibration();
            }

            UpdateViveHoloTracker();
        }
        if (Instance.bTT_0 && !calibrating)
        {
            if (ViveEmitter_calibrated)
            {
                CalibrateViveWithHololens();
            }
        }

    }

    void UpdateViveEmitterCalibration()
    {
        //LEFT-handed (converted from RIGHT-handed when receiving data)
        TrackerTransform TrackerTableT = Instance.lastTrackerTransform_1;
        //Debug.Log (TrackerTableT.position);
        //Vive emitter transform according to tracker 1
        ViveEmitter.transform.rotation = ViveCASTTracker.transform.rotation * Quaternion.Inverse(TrackerTableT.rotation);
        ViveEmitter.transform.position = ViveCASTTracker.transform.position + ViveEmitter.transform.TransformVector(-TrackerTableT.position);
    }

    void UpdateViveHoloTracker()
    {
        //LEFT-handed (converted from RIGHT-handed when receiving data)
        TrackerTransform TrackerHoloT = Instance.lastTrackerTransform_0;
        ViveHoloTracker.transform.position = ViveEmitter.transform.position + ViveEmitter.transform.TransformVector(TrackerHoloT.position);
        ViveHoloTracker.transform.rotation = ViveEmitter.transform.rotation * TrackerHoloT.rotation;
    }

    void CalibrateViveWithHololens()
    {
        //LEFT-handed Unity reference system
        Transform HoloCameraT = Camera.main.transform;
        Transform ViveHoloTrackerT = ViveHoloTracker.transform;
        Vector3 ViveTrackerFromHoloPos = HoloCameraT.position + HoloCameraT.TransformVector(HoloToTrackerDisplacement);
        //Vector3 ViveToHololensTranslation = ViveTrackerFromHoloPos - ViveHoloTrackerT.position;
        Quaternion ViveTrackersRotation = ViveHoloTracker.transform.rotation * Quaternion.Inverse(ViveCASTTracker.transform.rotation); ;
        Quaternion ViveToHololensRotation = Quaternion.Euler(0, Quaternion.Inverse(ViveTrackersRotation * Quaternion.Inverse(HoloCameraT.rotation)).eulerAngles.y, 0);

        //LEFT-handed (converted from RIGHT-handed when receiving data)
        //TrackerTransform TrackerHoloT = Instance.lastTrackerTransform_0;
        //Vector3 EmitterFromVivePos = ViveHoloTracker.transform.position + ViveHoloTracker.transform.TransformVector (-TrackerHoloT.position);
        //Quaternion EmitterFromViveRot = ViveHoloTracker.transform.rotation * Quaternion.Inverse (TrackerHoloT.rotation);
       // Vector3 OriginFromVivePos = ViveCASTTracker.transform.position - ViveHoloTracker.transform.position;
        //Vector3 OriginFromHoloPos = ViveCASTTracker.transform.position - ViveTrackerFromHoloPos;
        //Quaternion OriginFromViveRot = ViveCASTTracker.transform.rotation * Quaternion.Inverse(ViveHoloTracker.transform.rotation);
        //ViveCASTTrackerDebug.transform.position = ViveTrackerFromHoloPos +  Camera.main.transform.TransformVector (OriginFromVivePos);

        Quaternion FinalCASTOriginRot = ViveToHololensRotation;
        Vector3 FinalCASTOriginPos = ViveTrackerFromHoloPos + ViveToHololensRotation * (ViveCASTTracker.transform.position - ViveHoloTrackerT.position);
        //ViveCASTTrackerDebug.transform.rotation = FinalCASTOriginRot;// HoloCameraT.rotation;// ViveToHololensRotation;
        //ViveCASTTrackerDebug.transform.position = FinalCASTOriginPos;
        //ViveCASTTracker.transform.position + OriginFromVivePos - OriginFromHoloPos;

        CASTOrigin.transform.rotation = FinalCASTOriginRot;
        CASTOrigin.transform.position = FinalCASTOriginPos - ViveToHololensRotation * ViveCASTTracker.transform.position;
        //ViveCASTTrackerDebug.transform.rotation = ViveCASTTracker.transform.rotation * Camera.main.transform.rotation * Quaternion.Inverse(OriginFromViveRot);
    }

    public void CalibrateViveEmitter()
    {
        UpdateViveEmitterCalibration();
        ViveEmitter_calibrated = true;
    }
    public void StoreViveEmitterCalibration()
    {
        //LEFT-handed (converted from RIGHT-handed when receiving data)
        TrackerTransform TrackerTableT = Instance.lastTrackerTransform_1;
        //Debug.Log (TrackerTableT.position);
        //Vive emitter transform according to tracker 1
        ViveEmitter.transform.rotation = ViveCASTTracker.transform.rotation * Quaternion.Inverse(TrackerTableT.rotation);
        ViveEmitter.transform.position = ViveCASTTracker.transform.position + ViveEmitter.transform.TransformVector(-TrackerTableT.position);
        string jsonData = JsonUtility.ToJson(TrackerTableT);
        File.WriteAllText(filePath, jsonData);
        ViveEmitter_calibrated = true;
    }
    public void LoadViveEmitterVCalibration()
    {
        if (File.Exists(filePath))
        {
            StreamReader reader = new StreamReader(filePath);
            string jsonData = reader.ReadToEnd();
            TrackerTransform tt = JsonUtility.FromJson<TrackerTransform>(jsonData);
            ViveEmitter.transform.rotation = ViveCASTTracker.transform.rotation * Quaternion.Inverse(tt.rotation);
            ViveEmitter.transform.position = ViveCASTTracker.transform.position + ViveEmitter.transform.TransformVector(-tt.position);
            ViveEmitter_calibrated = true;
        }
        else
        {
            Debug.Log("Calibration File does not exist.");
        }
    }

    void StartCalibrateOrigin()
    {
        if (Instance.bTT_1)
        {
            Debug.Log("Start Calibration Procedure");
            calibrating = true;
            //calib_confirmed = false;
            tableParent.transform.position = this.transform.position;
            tableParent.transform.rotation = this.transform.rotation;
            Vector3 tableCalib = new Vector3(0, -0.155f, 0);
            tableChild.transform.localPosition = new Vector3(0,0,0);
            tableChild.transform.localPosition = tableChild.transform.localPosition + tableCalib;
            this.transform.parent.Find("Origin").rotation = this.transform.rotation;
            this.transform.parent.Find("Origin").position = tableChild.transform.position;

            calibrating = false;
        }
        return;
    }

    public void CalibrateRotation()
    {
        if (Instance.bTT_0)
        {
            Debug.Log("Start Rotation Calibration Procedure");
            calibrating = true;
            Quaternion tracker_ori = Instance.lastTrackerTransform_0.rotation;
            camera_rot = Instance.lastTrackerTransform_0.cam_rotation;
            from0toTable = tracker_ori;
            // Quaternion re_rot = new Quaternion(0.666f, 0.230f, 0.230f, 0.671f);
            Quaternion re_rot = new Quaternion(from0toTable.x, from0toTable.z, -from0toTable.y, from0toTable.w);
            rot_calib = Quaternion.Inverse(re_rot);

            calibrating = false;
        }
        return;
    }
    public void StoreTableCalibration()
    {
        //set storedTable as child of ViveMeter object to stable local position and rotation of Table relative to ViveMeter;
        storedTable.transform.position = this.transform.parent.Find("Origin").position;
        storedTable.transform.rotation = this.transform.parent.Find("Origin").rotation;
        
        storedTablePos = storedTable.transform.localPosition;
        storedTableRot = storedTable.transform.localRotation;
        StoredTransform storedT = new StoredTransform(storedTablePos, storedTableRot);
        String jsonStored = JsonUtility.ToJson(storedT);
        String filePath = Path.Combine(Application.streamingAssetsPath, filename);
        File.WriteAllText(filePath, jsonStored);
        //calib_confirmed = true;
    }

    public void LoadTableCalibration()
    {
        String filePath = Path.Combine(Application.streamingAssetsPath, filename);
        if (File.Exists(filePath))
        {
            StreamReader reader = new StreamReader(filePath);
            string jsonString = reader.ReadToEnd();
            reader.Close();
            StoredTransform storedT = JsonUtility.FromJson<StoredTransform>(jsonString);
            storedTablePos = storedT.StoredPosition;
            storedTableRot = storedT.StoredRotation;
            //calib_confirmed = true;
        }
        else
        {
            Debug.Log("Calibration storation json file does not exist.");
        }
    } 

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
            //RIGHT-Handed coordinate system coming from vive
            //Converting to Unity's LEFT-handed ccords system
            tt.position.x *= -1.0f;
            tt.rotation = new Quaternion(-tt.rotation.x, tt.rotation.y, tt.rotation.z, -tt.rotation.w);

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

