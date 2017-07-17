using UnityEngine;
using UnityEngine.Windows.Speech;
using System.Collections.Generic;
using System.Linq;
namespace PosterAlignment
{
    public class SpeechManager : MonoBehaviour
    {

        KeywordRecognizer keywordRecognizer = null;
        public ZoneCalibrationManager ZoneManager;
        public MultiPosterManager PostersManager;
        public HoloReceiver HoloReceiver;
        Dictionary<string, System.Action> keywords = new Dictionary<string, System.Action>();

        // Use this for initialization
        private void Awake()
        {
            if (!HoloHelper.isHololens())
            {
                this.enabled = false;
                Debug.Log("Not Holo");
            }
        }
        void Start()
        {
            //keywords.Add("Reference Calibration", () =>
            // {
            //     Debug.Log("Voice recognized");
            //     if (!ReferenceCalibration.Instance.isCalibrating())
            //     {
            //         ReferenceCalibration.Instance.SendMessage("OnStartReferenceCalibration");
            //     }
            // });
            keywords.Add("align poster 1", () =>
            {
                Debug.Log("Align command 1 recognized");
                if (ZoneManager != null)
                {
                    ZoneManager.AlignZone(0);
                    
                }
            });
            keywords.Add("align poster 2", () =>
            {
                Debug.Log("Align command 2 recognized");
                if (ZoneManager != null)
                {
                    ZoneManager.AlignZone(1);

                }
            });
            keywords.Add("align poster 3", () =>
            {
                Debug.Log("Align command 3 recognized");
                if (ZoneManager != null)
                {
                    ZoneManager.AlignZone(2);

                }
            });

            keywords.Add("lock alignment", () =>
            {
                if (ZoneManager != null)
                {
                    ZoneManager.LockZone(true);
                }
            });

            keywords.Add("align all posters", () =>
            {
                Debug.Log("Align all command recognized");
                if(PostersManager != null)
                {
                    PostersManager.AlignAllPosters();
                }
            });

            keywords.Add("calibrate", () =>
            {
                Debug.Log("Calibrate with position of Vive Tracker");
                if(HoloReceiver != null)
                {
                    HoloReceiver.StartCalibrateHolo();
                    Debug.Log("Not null");
                }
            });
            keywordRecognizer = new KeywordRecognizer(keywords.Keys.ToArray());
            keywordRecognizer.OnPhraseRecognized += Keyword_OnRecognized;
            keywordRecognizer.Start();
            Debug.Log("Speechmanager Started");
        }

        void Keyword_OnRecognized(PhraseRecognizedEventArgs arg)
        {
            System.Action keywordAction;
            if (keywords.TryGetValue(arg.text, out keywordAction))
            {
                keywordAction.Invoke();
            }

        }
    }
}
