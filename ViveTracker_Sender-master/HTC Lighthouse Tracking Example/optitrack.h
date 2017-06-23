#ifndef OPTITRACK_H_
#define OPTITRACK_H_

//#include "Device.h"
#include "tracker_types.h"
#include <string>
#include <vector>
#include <map>
#include <array>
#include <functional>
//#include <WinSock2.h>
//#include <Windows.h>
#include <algorithm>
#include <mutex>

#include "NatNetTypes.h"
#include "NatNetClient.h"

#include "TrackerPacket.h"

class Marker{
public:
	Position<float> pos{ { 0, 0, 0 } };
	int id = -1;
	Marker(){};
	Marker(const int id_, const Position<float>& pos_) : id(id_), pos(pos_){}
};

class RigidBody{
public:
	int id = -1;
	Position<float> pos{ { 0, 0, 0 } };
	Euler<float> euler{ { 0, 0, 0 } };
	Quaternion<float> quaternion{ { 0, 0, 0, 0 } };
	RigidBody(){}
	RigidBody(const int id_, const Position<float>& p, Euler<float>& e) : id(id_), pos(p), euler(e) { 
		auto q = glm::quat(glm::vec3(e.at(0), e.at(1), e.at(2)));
		quaternion = Quaternion <float>{ {q.w, q.x, q.y, q.z} };
	}
	RigidBody(const int id_, const Position<float>& p, Euler<float>& e, Quaternion<float>& q) : id(id_), pos(p), euler(e), quaternion(q) {}
};

enum Rule { high, low };

Rule stringToRule(const std::string& r);

#define nullmarker { -1, { 0, 0, 0 } }
#define nullrigidbody { -1, { 0, 0, 0 }, { 0, 0, 0 } }
//#define nullrigidbody RigidBody{ -1, {{ 0, 0, 0 }}, {{ 0, 0, 0 }}, {{ 0, 0, 0, 0 }} };

//template <class T>
class Optitrack{
private:
	std::array<std::vector<Marker>, 3> markers;
	std::array<std::vector<Marker>, 3> persistent_markers;
	//std::map<int, Marker> persistent_markers;
	std::map<unsigned int, RigidBody> rigidbodies;

	TrackerPacket pkt;
	std::mutex mPkt;

	bool connected;
	bool run_threads;

	std::vector<std::function<void()>> callback;

	Optitrack(const Optitrack&);

	int CreateClient(const std::string IP_Address, unsigned int cmdPort, unsigned int dataPort, int iConnectionType);

	bool verbose;

	std::mutex data_mutex;

	NatNetClient* theClient;

	friend void DataHandlerS(sFrameOfMocapData* data, void* pUserData); //static wrapper for NatNet library callback
	void DataHandler(sFrameOfMocapData* data, void* pUserData);

public:

	std::string ServerAddress;
	
	Optitrack();
	~Optitrack();

	bool connect(const std::string ip);
	bool isConnected();

	void setVerbose(const bool);
		
	Marker getMarker(const Rule rule, const unsigned int order, const unsigned int axis = 1);
	std::vector<Marker> getMarkers();
	RigidBody getRigidBody(const unsigned int id);
	std::map<unsigned int, RigidBody> getRigidBodies();

	void setPersistentMarker(const int id, const Rule rule, const unsigned int order, const unsigned int axis = 1);
	Marker getPersistentMarker(const Rule rule, const unsigned int order, const unsigned int axis);

	void addCallback(const std::function<void()>& f){ callback.push_back(f); }

	TrackerPacket getPacket();
};


#endif
