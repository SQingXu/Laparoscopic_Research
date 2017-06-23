using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.VR.WSA;
#if UNITY_WSA_10_0
using UnityEngine.VR.WSA.Persistence;
using UnityEngine.VR.WSA.Input;
#endif
public class ReferenceCalibration : MonoBehaviour {
    public static ReferenceCalibration Instance { get; private set; }

    Vector3 calibratingPosition = new Vector3(0, -0.5f, 1.2f);
    Vector3 calibrationRotation = new Vector3(-20, 0, 0);

    public bool showCalibration = false;

    bool calibrating = false;
    bool anchorLoaded = false;

    public string ObjectAnchorName;

    GameObject CalibrationObject = null;

#if UNITY_WSA_10_0
    WorldAnchorStore anchorStore = null;
    GestureRecognizer recognizer = null;
#endif

    // Use this for initialization
    private void Awake()
    {
        if (!HoloHelper.isHololens())
        {
            this.enabled = false;
            Debug.Log("Not Hololens");
        }
        Instance = this;
    }
    void Start () {
//#if UNITY_WSA_10_0
//        WorldAnchorStore.GetAsync(AnchorStore_GetAsync);
//        recognizer = new GestureRecognizer();
//        recognizer.TappedEvent += (source, tapCount, ray) =>
//        {
//            this.OnSelect();
//        };
//#endif
//        CalibrationObject = transform.Find("TableShape").gameObject;

    }

    public bool isCalibrating()
    {
        return calibrating;
    }
	
    //void AnchorStore_GetAsync(WorldAnchorStore store)
    //{
    //    anchorStore = store;
    //    string[] ids = anchorStore.GetAllIds();
    //    for(int index = 0; index < ids.Length; index++)
    //    {
    //        if (ids[index] == ObjectAnchorName)
    //        {
    //            anchorStore.Load(ids[index], gameObject);
    //            calibrating = false;
    //            anchorLoaded = true;
    //            break;
    //        }
    //    }
    //    if (!anchorLoaded)
    //    {
    //        OnStartReferenceCalibration();
    //    }
    //}
    void destroyAnchor()
    {
#if UNITY_WSA_10_0
        WorldAnchor anchor = gameObject.GetComponent<WorldAnchor>();
        if (anchor != null)
        {
            DestroyImmediate(anchor);
        }

        string[] ids = anchorStore.GetAllIds();
        for (int index = 0; index < ids.Length; index++)
        {
            if(ids[index] == ObjectAnchorName)
            {
                anchorStore.Delete(ids[index]);
                break;
            }
        }
#endif
    }

    void createAnchor()
    {
//#if UNITY_WSA_10_0
//        WorldAnchor attachingAnchor = gameObject.AddComponent<WorldAnchor>();
//        if (attachingAnchor.isLocated)
//        {
//            anchorStore.Save(ObjectAnchorName, attachingAnchor);
//        }
//        else
//        {
//            attachingAnchor.OnTrackingChanged += AttachingAnchor_OnTrackingChanged;
//        }
//#endif
    }
    //void AttachingAnchor_OnTrackingChanged(WorldAnchor self, bool located)
    //{
    //    if (located)
    //    {
    //        anchorStore.Save(ObjectAnchorName, self);
    //        self.OnTrackingChanged -= AttachingAnchor_OnTrackingChanged;
    //    }
    //}
    void OnStartReferenceCalibration()
    {
#if UNITY_WSA_10_0
        if(anchorStore == null)
        {
            return;
        }
        recognizer.StartCapturingGestures();
#endif
        calibrating = true;
        Debug.Log("Start calibrating");

        destroyAnchor();
    }
    void OnStopReferenceCalibration()
    {
#if UNITY_WSA_10_0
       if(anchorStore == null)
        {
            return;
        }
        recognizer.CancelGestures();
        recognizer.StopCapturingGestures();
#endif
        calibrating = false;
        destroyAnchor();
        createAnchor();
    }

    void OnSelect()
    {
        OnStopReferenceCalibration();
    } 

    

    // Update is called once per frame
    void Update () {
        Debug.Log("Updating");
        if (calibrating)
        {
            this.transform.position = Camera.main.transform.position + Camera.main.transform.rotation * (Quaternion.Euler(calibrationRotation) * calibratingPosition);
            this.transform.rotation = Camera.main.transform.rotation * Quaternion.Euler(calibrationRotation);
        }
		
	}
}
