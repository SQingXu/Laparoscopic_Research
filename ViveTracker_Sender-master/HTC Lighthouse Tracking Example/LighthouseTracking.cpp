//
// HTC Vive Lighthouse Tracking Example
// By Peter Thor 2016
//
// Shows how to extract basic tracking data
//

#include "LighthouseTracking.h"

#include "UDPSender.h"

#include <thread>
#include "glm\gtc\matrix_transform.hpp"


//static Vector3 FromQ2(vr::HmdQuaternion_t q1)
//{
//
//	float PI = 3.1415926535f;
//	float Rad2Deg = 57.2958f;
//
//	float sqw = q1.w * q1.w;
//	float sqx = q1.x * q1.x;
//	float sqy = q1.y * q1.y;
//	float sqz = q1.z * q1.z;
//	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
//	float test = q1.x * q1.w - q1.y * q1.z;
//	Vector3 v;
//
//	if (test>0.4995f*unit) { // singularity at north pole
//		v.y = 2.0f * atan2f(q1.y, q1.x);
//		v.x = PI / 2;
//		v.z = 0;
//		return NormalizeAngles(v *  Rad2Deg);
//	}
//	if (test<-0.4995f*unit) { // singularity at south pole
//		v.y = -2.0f * atan2f(q1.y, q1.x);
//		v.x = -PI / 2;
//		v.z = 0;
//		return NormalizeAngles(v * Mathf.Rad2Deg);
//	}
//	vr::HmdQuaternion_t q = new vr::HmdQuaternion_t(q1.w, q1.z, q1.x, q1.y);
//	v.y = (float)atan2f(2.0f * q.x * q.w + 2.0f * q.y * q.z, 1 - 2.0f * (q.z * q.z + q.w * q.w));     // Yaw
//	v.x = (float)asinf(2.0f * (q.x * q.z - q.w * q.y));                             // Pitch
//	v.z = (float)atan2f(2.0f * q.x * q.y + 2.0f * q.z * q.w, 1 - 2.0f * (q.y * q.y + q.z * q.z));      // Roll
//	return NormalizeAngles(v * Rad2Deg);
//}
//
//static Vector3 NormalizeAngles(Vector3 angles)
//{
//	angles.x = NormalizeAngle(angles.x);
//	angles.y = NormalizeAngle(angles.y);
//	angles.z = NormalizeAngle(angles.z);
//	return angles;
//}
//
//static float NormalizeAngle(float angle)
//{
//	while (angle>360)
//		angle -= 360;
//	while (angle<0)
//		angle += 360;
//	return angle;
//}



bool bUpdateHMD = false;
bool bSendHMD = false;
bool bPrintHMD = false;

bool bUpdateControllers = false;
bool bSendControllers = false;
bool bPrintControllers = false;

bool bUpdateTrackers = true;
bool bSendTrackers = true;
bool bprintTrackers = true;

// Destructor
LighthouseTracking::~LighthouseTracking() {
	if (m_pHMD != NULL)
	{
		vr::VR_Shutdown();
		m_pHMD = NULL;
	}

	SocketClose();
}

// Constructor
LighthouseTracking::LighthouseTracking() {
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Background);

	if (eError != vr::VRInitError_None)
	{
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		printf_s(buf);
		exit(EXIT_FAILURE);
	}

	setupSocket("152.2.130.69","9000");
	//setupSocket("152.23.19.157", "9000"); //Hololens
	printf_s("Set up socket");
	Sleep(2000);

	std::thread listen_tread(&LighthouseTracking::listenThread, this);
	listen_tread.detach();
}

// If false we'll parse tracking data continously, if true we parse when an openvr event fires
bool bAcquireTrackingDataByWaitingForVREvents = false;

void LighthouseTracking::listenThread() {
	char buf[1024];
	sprintf_s(buf, sizeof(buf), "Press 'q' to quit. Starting capture of tracking data...\n");
	printf_s(buf);

	while (this->RunProcedure(bAcquireTrackingDataByWaitingForVREvents)) {

		for (auto call : callback)
			call();

		// a delay to not overheat your computer... :)
		Sleep(sleepDuration);
	}
}

/*
* Loop-listen for events then parses them (e.g. prints the to user)
* Returns true if success or false if openvr has quit
*/
bool LighthouseTracking::RunProcedure(bool bWaitForEvents) {

	// Either A) wait for events, such as hand controller button press, before parsing...
	if (bWaitForEvents) {
		// Process VREvent
		vr::VREvent_t event;
		while (m_pHMD->PollNextEvent(&event, sizeof(event)))
		{
			// Process event
			if (!ProcessVREvent(event)) {
				char buf[1024];
				sprintf_s(buf, sizeof(buf), "(OpenVR) service quit\n");
				printf_s(buf);
				//return false;
			}

			ParseTrackingFrame();
		}
	}
	else {
		// ... or B) continous parsint of tracking data irrespective of events
		//std::cout << std::endl << "Parsing next frame...";

		ParseTrackingFrame();
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Processes a single VR event
//-----------------------------------------------------------------------------

bool LighthouseTracking::ProcessVREvent(const vr::VREvent_t & event)
{
	switch (event.eventType)
	{
		case vr::VREvent_TrackedDeviceActivated:
		{
			//SetupRenderModelForTrackedDevice(event.trackedDeviceIndex);
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Device : %d attached\n", event.trackedDeviceIndex);
			printf_s(buf);
		}
		break;

		case vr::VREvent_TrackedDeviceDeactivated:
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Device : %d detached\n", event.trackedDeviceIndex);
			printf_s(buf);
		}
		break;

		case vr::VREvent_TrackedDeviceUpdated:
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Device : %d updated\n", event.trackedDeviceIndex);
			printf_s(buf);
		}
		break;

		case (vr::VREvent_DashboardActivated) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Dashboard activated\n");
			printf_s(buf);
		}
		break;

		case (vr::VREvent_DashboardDeactivated) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Dashboard deactivated\n");
			printf_s(buf);

		}
		break;

		case (vr::VREvent_ChaperoneDataHasChanged) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Chaperone data has changed\n");
			printf_s(buf);

		}
		break;

		case (vr::VREvent_ChaperoneSettingsHaveChanged) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Chaperone settings have changed\n");
			printf_s(buf);
		}
		break;

		case (vr::VREvent_ChaperoneUniverseHasChanged) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Chaperone universe has changed\n");
			printf_s(buf);

		}
		break;

		case (vr::VREvent_ApplicationTransitionStarted) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Application Transition: Transition has started\n");
			printf_s(buf);

		}
		break;

		case (vr::VREvent_ApplicationTransitionNewAppStarted) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Application transition: New app has started\n");
			printf_s(buf);

		}
		break;

		case (vr::VREvent_Quit) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Received SteamVR Quit (%d", vr::VREvent_Quit, ")\n");
			printf_s(buf);

			return false;
		}
		break;

		case (vr::VREvent_ProcessQuit) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) SteamVR Quit Process (%d", vr::VREvent_ProcessQuit, ")\n");
			printf_s(buf);

			return false;
		}
		break;

		case (vr::VREvent_QuitAborted_UserPrompt) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) SteamVR Quit Aborted UserPrompt (%d", vr::VREvent_QuitAborted_UserPrompt, ")\n");
			printf_s(buf);

			return false;
		}
		break;

		case (vr::VREvent_QuitAcknowledged) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) SteamVR Quit Acknowledged (%d", vr::VREvent_QuitAcknowledged, ")\n");
			printf_s(buf);

			return false;
		}
												  break;

		case (vr::VREvent_TrackedDeviceRoleChanged) :
		{

			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) TrackedDeviceRoleChanged: %d\n", event.trackedDeviceIndex);
			printf_s(buf);
			break;
		}

		case (vr::VREvent_TrackedDeviceUserInteractionStarted) :
		{
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) TrackedDeviceUserInteractionStarted: %d\n", event.trackedDeviceIndex);
			printf_s(buf);
			break;
		}

		default:
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "(OpenVR) Event: %d\n", event.eventType);
			printf_s(buf);
			break;
	}

	return true;
}


// Get the quaternion representing the rotation
vr::HmdQuaternion_t LighthouseTracking::GetRotation(vr::HmdMatrix34_t matrix) {
	vr::HmdQuaternion_t q;

	q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
	return q;
}

glm::quat LighthouseTracking::GetRotationQ(vr::HmdMatrix34_t m) {
	glm::mat4 m4(m.m[0][0], m.m[1][0], m.m[2][0], 0,
		m.m[0][1], m.m[1][1], m.m[2][1], 0,
		m.m[0][2], m.m[1][2], m.m[2][2], 0,
		m.m[0][3], m.m[1][3], m.m[2][3], 1.0f);
	return glm::quat_cast(m4);
}


// Get the vector representing the position
vr::HmdVector3_t LighthouseTracking::GetPosition(vr::HmdMatrix34_t matrix) {
	vr::HmdVector3_t vector;

	vector.v[0] = matrix.m[0][3];
	vector.v[1] = matrix.m[1][3];
	vector.v[2] = matrix.m[2][3];

	return vector;
}

glm::mat4 getMatrix(glm::vec3 pos, glm::quat rot) {
	glm::mat4 identityMat = glm::mat4(1.0f);
	glm::mat4 rotMatrix = glm::mat4_cast(rot);   //rotation is glm::quat
	glm::mat4 transMatrix = glm::translate(identityMat, pos);
	//glm::mat4 viewMatrix = rotMatrix * glm::inverse(transMatrix);
	glm::mat4 viewMatrix = rotMatrix *transMatrix;
	return viewMatrix;
}

void LighthouseTracking::centerOrigin() {
	std::lock_guard<std::mutex> lock(data_mutex);
	/*glm::vec3 v(tracker_position.v[0], tracker_position.v[1], tracker_position.v[2]);
	glm::quat q(tracker_quaternion.w, tracker_quaternion.x, tracker_quaternion.y, tracker_quaternion.z);*/
	
	glm::mat4 viewMatrix = getMatrix(tracker_position, tracker_quaternion);

	origin = glm::inverse(viewMatrix);
	//origin = viewMatrix;
	//origin = glm::mat4(1.0f);
	bCenterOrigin = true;
}

glm::vec3 LighthouseTracking::GetReferencePosition(glm::vec3 p) {
	glm::vec4 v(p, 1.0f);
	v += origin[3];
	//glm::vec4 v(0,0,0, 1.0f);
	//return glm::mat3(origin) * glm::vec3(v);
	
	//return glm::vec3(GetReferenceRotation(tracker_quaternion) * v); //reference fixed to camera axes
	return glm::vec3(v * GetReferenceRotation(tracker_quaternion)); 
}

glm::quat LighthouseTracking::GetReferenceRotation(glm::quat q) {
	return q * glm::quat_cast(origin);
}

/*
* Parse a Frame with data from the tracking system
*
* Handy reference:
* https://github.com/TomorrowTodayLabs/NewtonVR/blob/master/Assets/SteamVR/Scripts/SteamVR_Utils.cs
*
* Also:
* Open VR Convention (same as OpenGL)
* right-handed system
* +y is up
* +x is to the right
* -z is going away from you
* http://www.3dgep.com/understanding-the-view-matrix/
*
*/
void LighthouseTracking::ParseTrackingFrame() {

	int deviceSendId = -1;

	//vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount);

	// Process SteamVR device states
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	{
		
		// if not connected just skip the rest of the routine
		if (!m_pHMD->IsTrackedDeviceConnected(unDevice)) {
			/*char buf[1024];
			sprintf_s(buf, sizeof(buf), "\nSkipping device %i\n", vr::VRSystem()->GetTrackedDeviceClass(unDevice));
			printf_s(buf);*/
			continue;
		}
		else {
			char buf[1024];
			char devclassname[1024];
			vr::ETrackedDeviceClass devclass = vr::VRSystem()->GetTrackedDeviceClass(unDevice);
			switch (devclass) {
			case vr::ETrackedDeviceClass::TrackedDeviceClass_Invalid:			// the ID was not valid.
				sprintf_s(devclassname, sizeof(devclassname), "Invalid");
				break;
			case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:					// Head-Mounted Displays
				sprintf_s(devclassname, sizeof(devclassname), "HMD");
				break;
			case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:			// Tracked controllers
				sprintf_s(devclassname, sizeof(devclassname), "Controller");
				break;
			case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:		// Generic trackers, similar to controllers
				sprintf_s(devclassname, sizeof(devclassname), "GenericTracker");
				break;
			case vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference:	// Camera and base stations that serve as tracking reference points
				sprintf_s(devclassname, sizeof(devclassname), "TrackingReference");
				break;
			case vr::ETrackedDeviceClass::TrackedDeviceClass_DisplayRedirect:
				sprintf_s(devclassname, sizeof(devclassname), "DisplayRedirect");
				break;
			}

			//sprintf_s(buf, sizeof(buf), "\ndevice %s\n", devclassname);//, vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, vr::ETrackedDeviceProperty::Prop_AttachedDeviceId_String);
			//printf_s(buf);

			

			//int m_iValidPoseCount = 0;
			//vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
			//m_strPoseClasses = "";
			//for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
			//{
			//	if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
			//	{
			//		m_iValidPoseCount++;
			//		//m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			//		if (m_rDevClassChar[nDevice] == 0)
			//		{
			//			switch (m_pHMD->GetTrackedDeviceClass(nDevice))
			//			{
			//			case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
			//			case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
			//			case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
			//			case vr::TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
			//			case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
			//			default:                                       m_rDevClassChar[nDevice] = '?'; break;
			//			}
			//		}
			//		m_strPoseClasses += m_rDevClassChar[nDevice];
			//	}
			//}

			//if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
			//{
			//	m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
			//	m_mat4HMDPose.invert();
			//}
		}

		vr::TrackedDevicePose_t trackedDevicePose;
		vr::TrackedDevicePose_t *devicePose = &trackedDevicePose;

		vr::VRControllerState_t controllerState;
		vr::VRControllerState_t *ontrollerState_ptr = &controllerState;

		vr::HmdVector3_t position;
		vr::HmdQuaternion_t quaternion;
		glm::quat quaterniong;

		if (vr::VRSystem()->IsInputFocusCapturedByAnotherProcess()) {
			char buf[1024];

			sprintf_s(buf, sizeof(buf), "\nInput Focus by Another Process\n");
			printf_s(buf);
		}

		bool bPoseValid = trackedDevicePose.bPoseIsValid;
		vr::HmdVector3_t vVel;
		vr::HmdVector3_t vAngVel;
		vr::ETrackingResult eTrackingResult;

		// Get what type of device it is and work with its data
		vr::ETrackedDeviceClass trackedDeviceClass = vr::VRSystem()->GetTrackedDeviceClass(unDevice);
		switch (trackedDeviceClass) {
		case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
			if (!bSendHMD)
				continue;

			++deviceSendId;

			// print stuff for the HMD here, see controller stuff in next case block

			// get pose relative to the safe bounds defined by the user
			//vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, &trackedDevicePose, 1);
			trackedDevicePose = m_rTrackedDevicePose[unDevice];

			// get the position and rotation
			position = GetPosition(devicePose->mDeviceToAbsoluteTracking);
			quaternion = GetRotation(devicePose->mDeviceToAbsoluteTracking);
			
			// get some data
			vVel = trackedDevicePose.vVelocity;
			vAngVel = trackedDevicePose.vAngularVelocity;
			eTrackingResult = trackedDevicePose.eTrackingResult;
			bPoseValid = trackedDevicePose.bPoseIsValid;

			// print the tracking data
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "\nHMD\nx: %.2f y: %.2f z: %.2f\n", position.v[0], position.v[1], position.v[2]);
			if(bPrintHMD) printf_s(buf);
			sprintf_s(buf, sizeof(buf), "qw: %.2f qx: %.2f qy: %.2f qz: %.2f\n", quaternion.w, quaternion.x, quaternion.y, quaternion.z);
			if (bPrintHMD) printf_s(buf);

			// and print some more info to the user about the state of the device/pose
			switch (eTrackingResult) {
			case vr::ETrackingResult::TrackingResult_Uninitialized:
				sprintf_s(buf, sizeof(buf), "Invalid tracking result\n");
				if (bPrintHMD) printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Calibrating_InProgress:
				sprintf_s(buf, sizeof(buf), "Calibrating in progress\n");
				if (bPrintHMD) printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Calibrating_OutOfRange:
				sprintf_s(buf, sizeof(buf), "Calibrating Out of range\n");
				if (bPrintHMD) printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Running_OK:
				sprintf_s(buf, sizeof(buf), "Running OK\n");
				if (bPrintHMD) printf_s(buf);
				break;
			case vr::ETrackingResult::TrackingResult_Running_OutOfRange:
				sprintf_s(buf, sizeof(buf), "WARNING: Running Out of Range\n");
				if (bPrintHMD) printf_s(buf);

				break;
			default:
				sprintf_s(buf, sizeof(buf), "Default\n");
				if (bPrintHMD) printf_s(buf);
				break;
			}

			// print if the pose is valid or not
			if (bPoseValid)
				sprintf_s(buf, sizeof(buf), "Valid pose\n");
			else
				sprintf_s(buf, sizeof(buf), "Invalid pose\n");
			if (bPrintHMD) printf_s(buf);

			break;


		case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
			// Simliar to the HMD case block above, please adapt as you like
			// to get away with code duplication and general confusion

			if (!bSendControllers)
				continue;

			++deviceSendId;

			vr::VRSystem()->GetControllerStateWithPose(vr::TrackingUniverseStanding, unDevice, &controllerState, sizeof(controllerState), &trackedDevicePose);

			position = GetPosition(devicePose->mDeviceToAbsoluteTracking);
			quaternion = GetRotation(devicePose->mDeviceToAbsoluteTracking);

			vVel = trackedDevicePose.vVelocity;
			vAngVel = trackedDevicePose.vAngularVelocity;
			eTrackingResult = trackedDevicePose.eTrackingResult;
			bPoseValid = trackedDevicePose.bPoseIsValid;
			
			switch (vr::VRSystem()->GetControllerRoleForTrackedDeviceIndex(unDevice)) {
			case vr::TrackedControllerRole_Invalid:
				// invalid hand...
				break;

				//
			case vr::TrackedControllerRole_LeftHand:
			
			if(false){
				char buf[1024];

				sprintf_s(buf, sizeof(buf), "\nLeft Controller\nx: %.2f y: %.2f z: %.2f\n", position.v[0], position.v[1], position.v[2]);
				if (bPrintControllers) printf_s(buf);

				sprintf_s(buf, sizeof(buf), "qw: %.2f qx: %.2f qy: %.2f qz: %.2f\n", quaternion.w, quaternion.x, quaternion.y, quaternion.z);
				if (bPrintControllers) printf_s(buf);

				switch (eTrackingResult) {
				case vr::ETrackingResult::TrackingResult_Uninitialized:
					sprintf_s(buf, sizeof(buf), "Invalid tracking result\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Calibrating_InProgress:
					sprintf_s(buf, sizeof(buf), "Calibrating in progress\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Calibrating_OutOfRange:
					sprintf_s(buf, sizeof(buf), "Calibrating Out of range\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Running_OK:
					sprintf_s(buf, sizeof(buf), "Running OK\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Running_OutOfRange:
					sprintf_s(buf, sizeof(buf), "WARNING: Running Out of Range\n");
					if (bPrintControllers) printf_s(buf);

					break;
				default:
					sprintf_s(buf, sizeof(buf), "Default\n");
					if (bPrintControllers) printf_s(buf);
					break;
				}

				if (bPoseValid)
					sprintf_s(buf, sizeof(buf), "Valid pose\n");
				else
					sprintf_s(buf, sizeof(buf), "Invalid pose\n");
				if (bPrintControllers) printf_s(buf);
			}
				break;

			case vr::TrackedControllerRole_RightHand:
				if (false) {
				
				char buf[1024];

				sprintf_s(buf, sizeof(buf), "\Right Controller\nx: %.2f y: %.2f z: %.2f\n", position.v[0], position.v[1], position.v[2]);
				if (bPrintControllers) printf_s(buf);

				sprintf_s(buf, sizeof(buf), "qw: %.2f qx: %.2f qy: %.2f qz: %.2f\n", quaternion.w, quaternion.x, quaternion.y, quaternion.z);
				if (bPrintControllers) printf_s(buf);

				switch (eTrackingResult) {
				case vr::ETrackingResult::TrackingResult_Uninitialized:
					sprintf_s(buf, sizeof(buf), "Invalid tracking result\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Calibrating_InProgress:
					sprintf_s(buf, sizeof(buf), "Calibrating in progress\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Calibrating_OutOfRange:
					sprintf_s(buf, sizeof(buf), "Calibrating Out of range\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Running_OK:
					sprintf_s(buf, sizeof(buf), "Running OK\n");
					if (bPrintControllers) printf_s(buf);
					break;
				case vr::ETrackingResult::TrackingResult_Running_OutOfRange:
					sprintf_s(buf, sizeof(buf), "WARNING: Running Out of Range\n");
					if (bPrintControllers) printf_s(buf);

					break;
				default:
					sprintf_s(buf, sizeof(buf), "Default\n");
					if (bPrintControllers) printf_s(buf);
					break;
				}

				if (bPoseValid)
					sprintf_s(buf, sizeof(buf), "Valid pose\n");
				else
					sprintf_s(buf, sizeof(buf), "Invalid pose\n");
				if (bPrintControllers) printf_s(buf);
			}
				break;

			case vr::TrackedDeviceClass_TrackingReference:
			{
				char buf[1024];
				// incomplete code, only here for switch reference
				sprintf_s(buf, sizeof(buf), "Camera / Base Station");
				//printf_s(buf);
			}
				break;
			}

			break;

			case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:
				if (!bUpdateTrackers)
					continue;

				++deviceSendId;

				{
					// print stuff for the HMD here, see controller stuff in next case block

					// get pose relative to the safe bounds defined by the user
					//vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0, &trackedDevicePose, 1);
					trackedDevicePose = m_rTrackedDevicePose[unDevice];

					// get the position and rotation
					position = GetPosition(devicePose->mDeviceToAbsoluteTracking);
					quaternion = GetRotation(devicePose->mDeviceToAbsoluteTracking);		
					quaterniong = GetRotationQ(devicePose->mDeviceToAbsoluteTracking);

					// get some data
					vVel = trackedDevicePose.vVelocity;
					vAngVel = trackedDevicePose.vAngularVelocity;
					eTrackingResult = trackedDevicePose.eTrackingResult;
					bPoseValid = trackedDevicePose.bPoseIsValid;

					// print the tracking data
					char buf[1024];
					sprintf_s(buf, sizeof(buf), "\nTracker%i \nx: %.2f y: %.2f z: %.2f\n", unDevice, position.v[0], position.v[1], position.v[2]);
					if (bprintTrackers) printf_s(buf);
					sprintf_s(buf, sizeof(buf), "qw: %.2f qx: %.2f qy: %.2f qz: %.2f\n", quaternion.w, quaternion.x, quaternion.y, quaternion.z);
					//if (bprintTrackers) printf_s(buf);

					// and print some more info to the user about the state of the device/pose
					switch (eTrackingResult) {
					case vr::ETrackingResult::TrackingResult_Uninitialized:
						sprintf_s(buf, sizeof(buf), "Invalid tracking result\n");
						if (bprintTrackers) printf_s(buf);
						break;
					case vr::ETrackingResult::TrackingResult_Calibrating_InProgress:
						sprintf_s(buf, sizeof(buf), "Calibrating in progress\n");
						if (bprintTrackers) printf_s(buf);
						break;
					case vr::ETrackingResult::TrackingResult_Calibrating_OutOfRange:
						sprintf_s(buf, sizeof(buf), "Calibrating Out of range\n");
						if (bprintTrackers) printf_s(buf);
						break;
					case vr::ETrackingResult::TrackingResult_Running_OK:
						sprintf_s(buf, sizeof(buf), "Running OK\n");
						//if (bprintTrackers) printf_s(buf);
						break;
					case vr::ETrackingResult::TrackingResult_Running_OutOfRange:
						sprintf_s(buf, sizeof(buf), "WARNING: Running Out of Range\n");
						if (bprintTrackers) printf_s(buf);

						break;
					default:
						sprintf_s(buf, sizeof(buf), "Default\n");
						if (bprintTrackers) printf_s(buf);
						break;
					}

					// print if the pose is valid or not
					if (bPoseValid) {
						sprintf_s(buf, sizeof(buf), "{ \"id\":\"tracker%i\",\"position\" : {\"x\":%.4f,\"y\" : %.4f,\"z\" : %.4f},\"rotation\" : {\"x\":%.8f,\"y\" : %.8f,\"z\" : %.8f,\"w\" : %.8f} }",
							deviceSendId, position.v[0], position.v[1], position.v[2], quaternion.x, quaternion.y, quaternion.z, quaternion.w);
						printf_s(buf);
						if(bSendTrackers) sendData(buf);

						{
							std::lock_guard<std::mutex> lock(data_mutex);
							tracker_position = glm::vec3(position.v[0], position.v[1], position.v[2]);
							tracker_quaternion = glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
							tracker_quaternion = quaterniong;

							for (auto c : callback)
								c();
						}
					}
					else
						sprintf_s(buf, sizeof(buf), "Invalid pose\n");
					

				}

				break;

		}
	}
}

void LighthouseTracking::SaveOnFile(char* filename) {
	char buf[1024];
	// incomplete code, only here for switch reference
	sprintf_s(buf, sizeof(buf), "saveNextOnFile\n");
	printf_s(buf);
	saveNextOnFile = true;
}