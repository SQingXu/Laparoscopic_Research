#include "Optitrack.h"

//#include <string>
#include <iostream>
#include <thread>
//#include <algorithm>

#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

bool IPAddress_StringToAddr(const char *szNameOrAddress, struct in_addr *Address);
int GetLocalIPAddresses(unsigned long Addresses[], int nMax);
void GetEulers(float x, float y, float z, float w, float *angle1, float *angle2, float *angle3);

unsigned int  PORT_COMMAND = 1510; //default one
unsigned int  PORT_DATA = 1511; //default one
//unsigned int  PORT_COMMAND = 9000; //default one
//unsigned int  PORT_DATA = 9000; //default one

void DataHandlerS(sFrameOfMocapData* data, void* pUserData);		// receives data from the server
void MessageHandlerS(int msgType, char* msg);		            // receives NatNet error mesages

int GetLocalIPAddresses(unsigned long Addresses[], int nMax)
{
	unsigned long  NameLength = 128;
	char szMyName[1024];
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	struct sockaddr_in addr;
	int retVal = 0;
	char* port = "0";

	if (GetComputerName(szMyName, &NameLength) != TRUE)
	{
		std::cout << "get computer name  failed (" << WSAGetLastError() << ")" << std::endl;
		return 0;
	};

	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_DGRAM;
	aiHints.ai_protocol = IPPROTO_UDP;
	if ((retVal = getaddrinfo(szMyName, port, &aiHints, &aiList)) != 0)
	{
		std::cout << "getaddrinfo failed (" << WSAGetLastError() << ")" << std::endl;
		return 0;
	}

	memcpy(&addr, aiList->ai_addr, aiList->ai_addrlen);
	freeaddrinfo(aiList);
	Addresses[0] = addr.sin_addr.S_un.S_addr;

	return 1;
}

bool Optitrack::connect(const std::string ip){

	connected = false;

	int iResult;
	//int iConnectionType = ConnectionType_Multicast;
	int iConnectionType = ConnectionType_Unicast;

	// Create NatNet Client
	iResult = CreateClient(ip, PORT_COMMAND, PORT_DATA, iConnectionType);
	if (iResult != ErrorCode_OK)
	{
		//printf("Error initializing client.  See log for details.  Exiting");
		return 1;
	}
	else
	{
		//printf("Client initialized and ready.\n");
	}


	// send/receive test request
//	printf("[SampleClient] Sending Test Request\n");
	void* response;
	int nBytes;
	iResult = theClient->SendMessageAndWait("TestRequest", &response, &nBytes);
	if (iResult == ErrorCode_OK)
	{
//		printf("[SampleClient] Received: %s", (char*)response);
		connected = true;
	}

	// Retrieve Data Descriptions from server
//	printf("\n\n[SampleClient] Requesting Data Descriptions...");
	sDataDescriptions* pDataDefs = NULL;
	int nBodies = theClient->GetDataDescriptions(&pDataDefs);
	if (!pDataDefs)
	{
//		printf("[SampleClient] Unable to retrieve Data Descriptions.");
	}
	else
	{
		connected = true;
	}

	if (connected)
		ServerAddress = ip;

	return connected;
}

// Establish a NatNet Client connection
int Optitrack::CreateClient(const std::string IP_Address, unsigned int cmdPort, unsigned int dataPort, int iConnectionType)
{
	// release previous server
	if (theClient)
	{
		theClient->Uninitialize();
	}

	// create NatNet client
	theClient = new NatNetClient(iConnectionType);

	// [optional] use old multicast group
	//theClient->SetMulticastAddress("224.0.0.1");

	// print version info
	unsigned char ver[4];
	theClient->NatNetVersion(ver);
//	printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);

	// Set callback handlers
	theClient->SetMessageCallback(MessageHandlerS);
	theClient->SetVerbosityLevel(Verbosity_Debug);
	theClient->SetDataCallback(DataHandlerS, this);	// this function will receive data from the server

	in_addr MyAddress;
	char szMyIPAddress[128] = "";

	GetLocalIPAddresses((unsigned long *)&MyAddress, 1);
	sprintf_s(szMyIPAddress, "%d.%d.%d.%d", MyAddress.S_un.S_un_b.s_b1, MyAddress.S_un.S_un_b.s_b2, MyAddress.S_un.S_un_b.s_b3, MyAddress.S_un.S_un_b.s_b4);

	// Init Client and connect to NatNet server
	// to use NatNet default port assigments
	int retCode = theClient->Initialize(&szMyIPAddress[0], (char*) IP_Address.c_str(), cmdPort, dataPort);
	// to use a different port for commands and/or data:
	//int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort);
	if (retCode != ErrorCode_OK)
	{
		std::cout << "Unable to connect to server.  Error code: " << retCode << ". Exiting" << std::endl;
		return ErrorCode_Internal;
	}
	else
	{
		// print server info
		sServerDescription ServerDescription;
		memset(&ServerDescription, 0, sizeof(ServerDescription));
		theClient->GetServerDescription(&ServerDescription);
		if (!ServerDescription.HostPresent)
		{
			std::cout << "Unable to connect to server. Host not present. Exiting." << std::endl;
			return 1;
		}
	}

	return ErrorCode_OK;

}

void DataHandlerS(sFrameOfMocapData* data, void* pUserData){ //static wrapper
	return ((Optitrack*) pUserData)->DataHandler(data, pUserData);
}
void MessageHandlerS(int msgType, char* msg){
	//printf("\n%s\n", msg);
}

// DataHandler receives data from the server
void Optitrack::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	//return;
	if (!this->connected) 
		return;

	NatNetClient* pClient = theClient;

	int i = 0;

	//printf("FrameID : %d\n", data->iFrame);
	//printf("Timestamp :  %3.2lf\n", data->fTimestamp);
	//printf("Latency :  %3.2lf\n", data->fLatency);

	// FrameOfMocapData params
	//bool bIsRecording = data->params & 0x01;
	//bool bTrackedModelsChanged = data->params & 0x02;
	//if (bIsRecording)
	//	printf("RECORDING\n");
	//if (bTrackedModelsChanged)
	//	printf("Models Changed.\n");


	// timecode - for systems with an eSync and SMPTE timecode generator - decode to values
	//int hour, minute, second, frame, subframe;
	//bool bValid = pClient->DecodeTimecode(data->Timecode, data->TimecodeSubframe, &hour, &minute, &second, &frame, &subframe);
	//// decode to friendly string
	//char szTimecode[128] = "";
	//pClient->TimecodeStringify(data->Timecode, data->TimecodeSubframe, szTimecode, 128);
//	printf("Timecode : %s\n", szTimecode);

	// Other Markers
	if (verbose)
		std::cout << "Unidentified Marker Count : " << data->nOtherMarkers << std::endl;

	{
		std::lock_guard<std::mutex> lock(data_mutex);

		std::lock_guard<std::mutex> lockPkt{ mPkt };
		pkt.markers.clear();
		pkt.rigidBodies.clear();
		pkt.timestamp = data->fTimestamp;

		//persistent_markers = markers;
		for (int axis = 0; axis < 3; ++axis)
			markers[axis].clear();

		for (i = 0; i < data->nOtherMarkers; i++)
		{
			float x = data->OtherMarkers[i][0];
			float y = data->OtherMarkers[i][1];
			float z = data->OtherMarkers[i][2];

			pkt.addMarker(Trackable{ glm::vec3{ x, y, z }, glm::vec3{ 0, 0, 0 }, glm::quat{ 0, 0, 0, 1 }, { 0, 0 }, 0, data->fTimestamp });
			
			for (int axis = 0; axis < 3; ++axis)
				markers[axis].push_back(Marker{ -1, { x, y, z } });
		}

		for (int axis = 0; axis < 3; ++axis)
			std::sort(markers[axis].begin(), markers[axis].end(), [axis](const Marker& m1, const Marker& m2) { return m1.pos[axis] < m2.pos[axis]; });

		for (int axis = 0; axis < 3; ++axis){
			if (persistent_markers[axis].size() == 0){
				persistent_markers[axis] = markers[axis];
			}
			else{
				auto m_copy = markers[axis];
				auto pm_copy = persistent_markers[axis];
				for (int i = 0; i < markers[axis].size(); i++){
					float min_dist = 1000;
					int min_m = 0;
					int min_pm = 0;
					int m_id = 0;
					int pm_id = 0;
					for (const auto& m : m_copy){
						if (equals_(m.pos, { { 1000, 1000, 1000 } })){
							++m_id;
							continue;
						}
						pm_id = 0;
						for (const auto& pm : pm_copy){
							if (equals_(pm.pos, { { 1000, 1000, 1000 } })){
								++pm_id;
								continue;
							}
							if (distance_(m.pos, pm.pos) < min_dist){
								min_dist = distance_(m.pos, pm.pos);
								min_m = m_id;
								min_pm = pm_id;
							}
							++pm_id;
						}
						++m_id;
					}
					if (min_dist != 1000){
						persistent_markers[axis][min_pm].pos = m_copy[min_m].pos;
						m_copy[min_m].pos = { { 1000, 1000, 1000 } };
						pm_copy[min_pm].pos = { { 1000, 1000, 1000 } };

					}
				}
			}
		}
	}
//	printf("Other Markers [Count=%d]\n", data->nOtherMarkers);
	for (i = 0; i < data->nOtherMarkers; i++)
	{
		//printf("Other Marker %d : %3.2f\t%3.2f\t%3.2f\n",
		//	i,
		//	data->OtherMarkers[i][0],
		//	data->OtherMarkers[i][1],
		//	data->OtherMarkers[i][2]);
	}


	// Rigid Bodies
	{
		std::lock_guard<std::mutex> lock(data_mutex);
		rigidbodies.clear();

		//	printf("Rigid Bodies [Count=%d]\n", data->nRigidBodies);
		for (i = 0; i < data->nRigidBodies; i++)
		{
			// params
			// 0x01 : bool, rigid body was successfully tracked in this frame
			bool bTrackingValid = data->RigidBodies[i].params & 0x01;

			//printf("Rigid Body [ID=%d  Error=%3.2f  Valid=%d]\n", data->RigidBodies[i].ID, data->RigidBodies[i].MeanError, bTrackingValid);
			//printf("\tx\ty\tz\tqx\tqy\tqz\tqw\n");
			//printf("\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
			//	data->RigidBodies[i].x,
			//	data->RigidBodies[i].y,
			//	data->RigidBodies[i].z,
			//	data->RigidBodies[i].qx,
			//	data->RigidBodies[i].qy,
			//	data->RigidBodies[i].qz,
			//	data->RigidBodies[i].qw);

			int ID = data->RigidBodies[i].ID;
			float x = data->RigidBodies[i].x;
			float y = data->RigidBodies[i].y;
			float z = data->RigidBodies[i].z;
			float qx = data->RigidBodies[i].qx;
			float qz = data->RigidBodies[i].qy;
			float qy = data->RigidBodies[i].qz;
			float qw = data->RigidBodies[i].qw;
			glm::quat q{ qw, qx, qy, qz };
			pkt.addRigidBody(Trackable{ { x, y, z }, { glm::pitch(q), glm::yaw(q), glm::roll(q) }, { qx, qy, qz, qw }, { 0, 0 }, ID, data->fTimestamp });

			float pitch;
			float yaw;
			float roll;

			//GetEulers(qx, qy, qz, qw, &pitch, &yaw, &roll);
			//rigidbodies[ID] = RigidBody(ID, Position < float > {{ x, y, z }}, Euler < float > {{ pitch, yaw, roll }});
			rigidbodies[ID] = RigidBody(ID, Position < float > { { x, y, z }}, Euler < float > { { glm::pitch(q), glm::yaw(q), glm::roll(q) }}, Quaternion <float> { { qw, qx, qz, qy }});


			//		printf("\tRigid body markers [Count=%d]\n", data->RigidBodies[i].nMarkers);
			//	for (int iMarker = 0; iMarker < data->RigidBodies[i].nMarkers; iMarker++)
			//	{
			//		printf("\t\t");
			//		if (data->RigidBodies[i].MarkerIDs)
			//			printf("MarkerID:%d", data->RigidBodies[i].MarkerIDs[iMarker]);
			//		if (data->RigidBodies[i].MarkerSizes)
			//			printf("\tMarkerSize:%3.2f", data->RigidBodies[i].MarkerSizes[iMarker]);
			//		if (data->RigidBodies[i].Markers)
			//			printf("\tMarkerPos:%3.2f,%3.2f,%3.2f\n",
			//			data->RigidBodies[i].Markers[iMarker][0],
			//			data->RigidBodies[i].Markers[iMarker][1],
			//			data->RigidBodies[i].Markers[iMarker][2]);
			//	}
			//}

			// skeletons
			//printf("Skeletons [Count=%d]\n", data->nSkeletons);
			//for (i = 0; i < data->nSkeletons; i++)
			//{
			//	sSkeletonData skData = data->Skeletons[i];
			//	printf("Skeleton [ID=%d  Bone count=%d]\n", skData.skeletonID, skData.nRigidBodies);
			//	for (int j = 0; j< skData.nRigidBodies; j++)
			//	{
			//		sRigidBodyData rbData = skData.RigidBodyData[j];
			//		printf("Bone %d\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
			//			rbData.ID, rbData.x, rbData.y, rbData.z, rbData.qx, rbData.qy, rbData.qz, rbData.qw);

			//		printf("\tRigid body markers [Count=%d]\n", rbData.nMarkers);
			//		for (int iMarker = 0; iMarker < data->RigidBodies[i].nMarkers; iMarker++)
			//		{
			//			printf("\t\t");
			//			if (rbData.MarkerIDs)
			//				printf("MarkerID:%d", rbData.MarkerIDs[iMarker]);
			//			if (rbData.MarkerSizes)
			//				printf("\tMarkerSize:%3.2f", rbData.MarkerSizes[iMarker]);
			//			if (rbData.Markers)
			//				printf("\tMarkerPos:%3.2f,%3.2f,%3.2f\n",
			//				data->RigidBodies[i].Markers[iMarker][0],
			//				data->RigidBodies[i].Markers[iMarker][1],
			//				data->RigidBodies[i].Markers[iMarker][2]);
			//		}
			//	}
			//}

			//// labeled markers
			//bool bOccluded;     // marker was not visible (occluded) in this frame
			//bool bPCSolved;     // reported position provided by point cloud solve
			//bool bModelSolved;  // reported position provided by model solve
			//printf("Labeled Markers [Count=%d]\n", data->nLabeledMarkers);
			//for (i = 0; i < data->nLabeledMarkers; i++)
			//{
			//	bOccluded = data->LabeledMarkers[i].params & 0x01;
			//	bPCSolved = data->LabeledMarkers[i].params & 0x02;
			//	bModelSolved = data->LabeledMarkers[i].params & 0x04;
			//	sMarker marker = data->LabeledMarkers[i];
			//	printf("Labeled Marker [ID=%d, Occluded=%d, PCSolved=%d, ModelSolved=%d] [size=%3.2f] [pos=%3.2f,%3.2f,%3.2f]\n",
			//		marker.ID, bOccluded, bPCSolved, bModelSolved, marker.size, marker.x, marker.y, marker.z);

		}
	}

	for (auto call : callback)
		call();
}


void Optitrack::setVerbose(bool v){
	verbose = v;
}

Optitrack::Optitrack(){
	verbose = false;
	connected = false;
	run_threads = false;
	theClient = nullptr;
}

void setPersistentMarker(const int id, const Rule rule, const unsigned int order, const unsigned int axis = 1){

}

Marker Optitrack::getMarker(const Rule rule, const unsigned int order, const unsigned int axis){
	std::lock_guard<std::mutex> lock(data_mutex);
	//markers is sorted! (DONE when markers is filled)
	if (order < 1 || order > markers[axis].size() || axis < 0 || axis >2)
		return nullmarker;

	switch (rule)
	{
	case high:
		return markers[axis][markers[axis].size() - order];
		break;
	case low:
		return markers[axis][order - 1];
		break;
	default:
		return nullmarker;
		break;
	}
}

Marker Optitrack::getPersistentMarker(const Rule rule, const unsigned int order, const unsigned int axis){
	std::lock_guard<std::mutex> lock(data_mutex);
	//markers is sorted! (DONE when markers is filled)
	if (order < 1 || order > persistent_markers[axis].size() || axis < 0 || axis >2)
		return nullmarker;

	switch (rule)
	{
	case high:
		return persistent_markers[axis][persistent_markers[axis].size() - order];
		break;
	case low:
		return persistent_markers[axis][order - 1];
		break;
	default:
		return nullmarker;
		break;
	}
}

Optitrack::~Optitrack(){
	//theClient->Uninitialize();

	callback.clear();
	run_threads = false;
	if (isConnected()){
		int iSuccess = theClient->Uninitialize();
		if (iSuccess != 0)
			printf("error un-initting Client\n");
		delete theClient;
	}
}

std::vector<Marker>  Optitrack::getMarkers(){
	std::lock_guard<std::mutex> lock(data_mutex);
	return markers[1];
}

RigidBody Optitrack::getRigidBody(const unsigned int id){
	std::lock_guard<std::mutex> lock(data_mutex);
	try{
		return rigidbodies.at(id);
	}
	catch (std::out_of_range){
		return RigidBody();
	}
}

std::map<unsigned int, RigidBody> Optitrack::getRigidBodies(){
	std::lock_guard<std::mutex> lock(data_mutex);
	return rigidbodies;
}

bool Optitrack::isConnected(){
	return connected;
}

TrackerPacket Optitrack::getPacket(){
	std::lock_guard<std::mutex> lockpkt{ mPkt };
	return pkt;
}

inline void RadiansToDegrees(float *value)
{
	*value = (*value)*(180.0f / 3.14159265f);
}

//PYR
//void GetEulers(float qx, float qy, float qz, float qw, float *angle1, float *angle2, float *angle3)
//{
//	float &heading = *angle1;
//	float &attitude = *angle2;
//	float &bank = *angle3;
//	//float &heading = *angle2;
//	//float &attitude = *angle1;
//	//float &bank = *angle3;
//
//	double test = qx*qy + qz*qw;
//	if (test > 0.499) { // singularity at north pole
//		heading = (float) 2.0f * atan2(qx, qw);
//		attitude = 3.14159265f / 2.0f;
//		bank = 0;
//
//		RadiansToDegrees(&heading);
//		RadiansToDegrees(&attitude);
//		RadiansToDegrees(&bank);
//		return;
//	}
//	if (test < -0.499) { // singularity at south pole
//		heading = (float)-2.0f * atan2(qx, qw);
//		attitude = -3.14159265f / 2.0f;
//		bank = 0;
//
//		RadiansToDegrees(&heading);
//		RadiansToDegrees(&attitude);
//		RadiansToDegrees(&bank);
//		return;
//	}
//	double sqx = qx*qx;
//	double sqy = qy*qy;
//	double sqz = qz*qz;
//	heading = (float)atan2((double)2.0*qy*qw - 2.0*qx*qz, (double)1 - 2.0*sqy - 2.0*sqz);
//	attitude = (float)asin(2.0*test);
//	bank = (float)atan2((double)2.0*qx*qw - 2.0*qy*qz, (double)1.0 - 2.0*sqx - 2.0*sqz);
//
//	RadiansToDegrees(&heading);
//	RadiansToDegrees(&attitude);
//	RadiansToDegrees(&bank);
//}

//YPR
void GetEulers(float x, float y, float z, float w, float *angle1, float *angle2, float *angle3)
{
	*angle3 = atan2(2 * (y*z + w*x), w*w - x*x - y*y + z*z) *(180.0f / 3.14159265f);
	*angle1 = asin(-2 * (x*z - w*y)) *(180.0f / 3.14159265f);
	*angle2 = atan2(2 * (x*y + w*z), w*w + x*x - y*y - z*z) *(180.0f / 3.14159265f);
}

Rule stringToRule(const std::string& r){
	std::string rule(r);
	std::transform(begin(r), end(r), begin(rule), ::tolower);
	if (rule.compare("high") == 0)
		return Rule::high;
	else if (rule.compare("low") == 0)
		return Rule::low;
	else throw "unknown rule";
}