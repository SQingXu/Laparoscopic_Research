using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace PosterAlignment
{
    public class MultiPosterManager : MonoBehaviour
    {
        public CalibrationZone zone1XY;
        public CalibrationZone zone2YZ;
        public CalibrationZone zone3XZ;
        public GameObject ReferenceCube;
        // Use this for initialization
        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }
        private Quaternion getZoneRotation(CalibrationZone zone)
        {
            Quaternion zoneRotation = zone.transform.GetChild(0).rotation * Quaternion.Inverse(zone.transform.GetChild(0).localRotation);
            Debug.Log(zoneRotation);
            return zoneRotation;
        }

        public void AlignAllPosters()
        {
            Vector3 CalibratedPos = new Vector3((zone1XY.transform.GetChild(0).position.x + zone3XZ.transform.GetChild(0).position.x) / 2.0f,
                                                (zone1XY.transform.GetChild(0).position.y + zone2YZ.transform.GetChild(0).position.y) / 2.0f,
                                                (zone2YZ.transform.GetChild(0).position.z + zone3XZ.transform.GetChild(0).position.z) / 2.0f);
           
            ReferenceCube.transform.position = CalibratedPos;
            Quaternion CalibratedRot = Quaternion.Slerp(Quaternion.Slerp(getZoneRotation(zone1XY), getZoneRotation(zone2YZ), 0.5f), getZoneRotation(zone3XZ), 2.0f / 3.0f);
            ReferenceCube.transform.rotation = CalibratedRot;
            
            

            Debug.Log("CalibratedPosition: " + CalibratedPos + " CalibratedRotation: " + CalibratedRot);

        }
    }
}
