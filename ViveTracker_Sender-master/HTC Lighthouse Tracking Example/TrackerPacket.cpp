#include "TrackerPacket.h"

void TrackerPacket::addMarker(const Trackable& m)
{
	markers.push_back(m);
}

void TrackerPacket::addRigidBody(const Trackable& r)
{
	rigidBodies.push_back(r);
}

const void TrackerPacket::serialize(std::ostream& stream)
{
	stream << timestamp << " ";
	// Serialization code
	stream << (size_t)markers.size() << " ";
	for (auto m : markers){
		m.serialize(stream);
		stream << " ";
	}
	stream << (size_t)rigidBodies.size() << " ";
	for (auto m : rigidBodies){
		m.serialize(stream);
		stream << " ";
	}
}

void TrackerPacket::deserialize(std::istream& stream)
{
	markers.clear();
	rigidBodies.clear();

	stream >> timestamp;
	// Deserialization code
	size_t msize = 0;
	stream >> msize;
	for (auto i = 0; i < msize; i++){
		Trackable m;
		m.deserialize(stream);
		markers.push_back(m);
	}
	size_t rsize = 0;
	stream >> rsize;
	for (auto i = 0; i < rsize; i++){
		Trackable r;
		r.deserialize(stream);
		rigidBodies.push_back(r);
	}
	
	/*std::cout << "recv: " << markers.size() << ": ";
	if (!markers.empty()){
		std::cout << markers.front().pos.x << " " << markers.front().pos.y << " " << markers.front().pos.z;
	}
	std::cout << std::endl;*/

}