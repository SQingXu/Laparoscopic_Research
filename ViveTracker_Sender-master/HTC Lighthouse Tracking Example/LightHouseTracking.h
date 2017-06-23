// LIGHTHOUSETRACKING.h
#ifndef _LIGHTHOUSETRACKING_H_
#define _LIGHTHOUSETRACKING_H_

// OpenVR
#include <openvr.h>
#include "samples\shared\Matrices.h"
#include <vector>
#include <functional>
#include <mutex>
#include "glm\glm.hpp"
#include "glm\gtc\quaternion.hpp"

class LighthouseTracking {
private:

	// Basic stuff
	vr::IVRSystem *m_pHMD = NULL;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	Matrix4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

	// Position and rotation of pose
	vr::HmdVector3_t GetPosition(vr::HmdMatrix34_t matrix);
	vr::HmdQuaternion_t GetRotation(vr::HmdMatrix34_t matrix);
	glm::quat GetRotationQ(vr::HmdMatrix34_t matrix);

	// If false the program will parse tracking data continously and not wait for openvr events
	bool bWaitForEventsBeforeParsing = false;

	bool saveNextOnFile = false;
	char* filename = "../Config/ETReferenceCalibration.ini";

	float sleepDuration = 1000.0f/200.0f;

	std::vector<std::function<void()>> callback;

	glm::vec3 tracker_position;
	glm::quat tracker_quaternion;

	std::mutex data_mutex;
	
	bool bCenterOrigin = false;
	glm::mat4x4 origin = glm::mat4x4(1.0f);

	glm::vec3 GetReferencePosition(glm::vec3 p);
	glm::quat GetReferenceRotation(glm::quat q);

public:
	~LighthouseTracking();
	LighthouseTracking();

	void centerOrigin();

	void listenThread();

	// Main loop that listens for openvr events and calls process and parse routines, if false the service has quit
	bool RunProcedure(bool bWaitForEvents);

	// Process a VR event and print some general info of what happens
	bool ProcessVREvent(const vr::VREvent_t & event);

	// Parse a tracking frame and print its position / rotation
	void ParseTrackingFrame();

	void SaveOnFile(char* filename);

	void SetSpeed(float hz) { sleepDuration = 1000.0f / hz; }

	void addCallback(const std::function<void()>& f) { callback.push_back(f); }

	glm::vec3 GetTrackerPosition() {
		std::lock_guard<std::mutex> lock(data_mutex);
		if (bCenterOrigin) {
			return GetReferencePosition(tracker_position);
		}
		return tracker_position;
	}

	glm::quat GetTrackerRotation() {
		std::lock_guard<std::mutex> lock(data_mutex);
		if (bCenterOrigin) {
			return GetReferenceRotation(tracker_quaternion);
		}
		return tracker_quaternion;
	}

};

#endif _LIGHTHOUSETRACKING_H_
