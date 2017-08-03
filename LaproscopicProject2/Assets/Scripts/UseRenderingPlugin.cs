using UnityEngine;
using UnityEditor;
using System;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;


public class UseRenderingPlugin : MonoBehaviour
{
    // Native plugin rendering events are only called if a plugin is used
    // by some script. This means we have to DllImport at least
    // one function in some active script.
    // For this example, we'll call into plugin's SetTimeFromUnity
    // function and pass the current time so the plugin can animate.
    bool takepic = true;
    System.IntPtr result;
    System.IntPtr result_rot;
    System.IntPtr result_tranl;
    double[] rot_matrix = new double[9];
    int[] result_arr = new int[1];
    int[] matrix;
#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
	[DllImport ("RenderingPlugin")]
#endif
	private static extern void SetTimeFromUnity(float t);


	// We'll also pass native pointer to a texture in Unity.
	// The plugin will fill texture data from native code.
#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
	[DllImport ("RenderingPlugin")]
#endif
	private static extern void SetRGBTextureFromUnity(System.IntPtr texture, int w, int h);

#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("RenderingPlugin")]
#endif
    private static extern void SetDepthTextureFromUnity(System.IntPtr texture, int w, int h);

#if UNITY_IPHONE && !UNITY_EDITOR
    [DllImport ("__Internal")]
#else
    [DllImport("RenderingPlugin")]
#endif
    private static extern void ExitAndDestroy();

#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("RenderingPlugin")]
#endif
	private static extern IntPtr GetRenderEventFunc();

#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("RenderingPlugin")]
#endif
    private static extern IntPtr GetRenderEventFunc_Capture();

#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("RenderingPlugin")]
#endif
    //private static extern IntPtr detectArucoMarker(System.IntPtr rot, System.IntPtr tranl);
    private static extern IntPtr detectArucoMarker();
    IEnumerator Start()
	{
		CreateTextureAndPassToPlugin();
        Debug.Log("Start");
		yield return StartCoroutine("CallPluginAtEndOfFrames");
	}
    public void Update()
    {
        if(Input.GetKeyDown(KeyCode.G))
        {
            takepic = true;
            Debug.Log("Take one picture for calibration");
        }
        if (Input.GetKeyDown(KeyCode.H))
        {
            detectArucoMarker();
            //Marshal.Copy(result, result_arr, 0, 1);
            //Marshal.Copy(result_rot, rot_matrix, 0, 9);
            //Debug.Log(result_arr[0]);
        }
        if (Input.GetKeyDown(KeyCode.Y))
        {
            //Debug.Log(Application.dataPath);
            using (StreamReader r = new StreamReader(Path.Combine(Application.dataPath, "../cam_aruco_calib.json")))
            {
                string content = r.ReadToEnd();
                CameraCalibrationData data = JsonUtility.FromJson<CameraCalibrationData>(content);
                Debug.Log(data.up_column);
                Debug.Log(data.forward_column);
                Debug.Log(data.translation_vector);
                this.gameObject.GetComponent<CameraCalibration>().calibrateCameraPosition(data);
            }
                
        }
    }
    private void OnDestroy()
    {
        ExitAndDestroy();
        Debug.Log("Exit camera");
    }

    private void CreateTextureAndPassToPlugin()
	{
		// Create a texture
		Texture2D RGBtex = new Texture2D(640,480,TextureFormat.ARGB32,false);
        Texture2D Depthtex = new Texture2D(640, 480, TextureFormat.R16, false);
        // Set point filtering just so we can see the pixels clearly
        RGBtex.filterMode = FilterMode.Point;
        Depthtex.filterMode = FilterMode.Point;
		// Call Apply() so it's actually uploaded to the GPU
		RGBtex.Apply();
        Depthtex.Apply();

        // Set texture onto our material
        GetComponent<Renderer>().material.SetTexture("_ColorTex", RGBtex);
        GetComponent<Renderer>().material.SetTexture("_DepthTex", Depthtex);
        //this.GetComponent<MeshRenderer>().material.SetTexture("RGB",RGBtex);
        //GetComponent<Renderer>().material.SetTexture("_MainTex", Depthtex);
        // Pass texture pointer to the plugin
        SetRGBTextureFromUnity(RGBtex.GetNativeTexturePtr(), RGBtex.width, RGBtex.height);
        SetDepthTextureFromUnity(Depthtex.GetNativeTexturePtr(), Depthtex.width, Depthtex.height);
    }

	private IEnumerator CallPluginAtEndOfFrames()
	{
        bool wh = true;
        //if (wh)
        //{
        //    yield return new WaitForEndOfFrame();
        //    GL.IssuePluginEvent(GetRenderEventFunc_Capture(), 1);
        //}
        //Debug.Log("Call Function start");
		while (wh) {
			// Wait until all frame rendering is done
			yield return new WaitForEndOfFrame();

            // Set time for the plugin
            //SetTimeFromUnity (Time.timeSinceLevelLoad);

            // Issue a plugin event with arbitrary integer identifier.
            // The plugin can distinguish between different
            // things it needs to do based on this ID.
            // For our simple plugin, it does not matter which ID we pass here.
            if (takepic)
            {
                GL.IssuePluginEvent(GetRenderEventFunc_Capture(), 1);
                //Debug.Log("In while loop");
            }
            else
            {
                GL.IssuePluginEvent(GetRenderEventFunc(), 1);
            }
            takepic = false;
            wh = true;
		}
	}
}
