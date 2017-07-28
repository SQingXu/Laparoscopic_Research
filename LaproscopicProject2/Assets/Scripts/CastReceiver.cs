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

//[Serializable]

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

    public int port = 8500;
    void Start()
    {
        this.Instance = this.gameObject.AddComponent<CASTClient>();
        this.Instance.Init(port);
        Debug.Log("CAST UDP receiver start");

    }

    // Update is called once per frame
    void Update()
    {
        if(this.Instance.ReceiveDataLeft && this.Instance.ReceiveDataRight)
        {
            //Debug.Log(Instance.lastFixtureTipTransform_left.position);
            //Debug.Log(Instance.lastFixtureTipTransform_right.position);
            this.transform.parent.Find("LeftInstrumentTip").localPosition = Instance.lastFixtureTipTransform_left.position;
            this.transform.parent.Find("RightInstrumentTip").localPosition = Instance.lastFixtureTipTransform_right.position;
        }
    }


    class CASTClient : MonoBehaviour
    {
        private int port = 8500;
        private FixtureTipTransform _lastFixtureTipTransform_left;
        private FixtureTipTransform _lastFixtureTipTransform_right;
        public bool ReceiveDataLeft = false;
        public bool ReceiveDataRight = false;
        private object lock_left = new object();
        private object lock_right = new object();
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
                ReceiveDataLeft = true;
                lastFixtureTipTransform_left = ft;
                //Debug.Log("Left match");
            }
            if (ft.direction.Equals("Right"))
            {
                ReceiveDataRight = true;
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