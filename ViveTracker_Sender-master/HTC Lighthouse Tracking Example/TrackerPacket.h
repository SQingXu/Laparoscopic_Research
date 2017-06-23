#pragma once
#include "Trackable.h"
#include <vector>
#include <chrono>
#include "Serializable.h"
using namespace std::chrono;

//class TrackerHeader : Serializable
//{
//	size_t size;
//	size_t packets;
//	time_point<high_resolution_clock> timestamp;
//
//	void serialize(std::ostream& stream);
//	void deserialize(std::istream& stream);
//};

class TrackerPacket : Serializable
{
public:
	double timestamp;
	std::vector<Trackable> markers;
	std::vector<Trackable> rigidBodies;

	void addMarker(const Trackable& m);
	void addRigidBody(const Trackable& r);

//	TrackerPacket();
//	virtual ~TrackerPacket();

	const void serialize(std::ostream& stream);
	void deserialize(std::istream& stream);
};

