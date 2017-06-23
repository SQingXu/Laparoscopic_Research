#include "Trackable.h"
#include <string>

Trackable::Trackable(glm::vec3 p, glm::vec3 e, glm::quat q, std::array<float, 2> s, int id, double ts) :
position{ p }, euler{ e }, quaternion{ q }, ID{ id }, timestamp{ 0 }, sensors{ { 0, 0 } }
{
}


Trackable::~Trackable()
{
}

std::ostream& operator<< (std::ostream &out, const glm::vec3 &pos)
{
	out << pos.x << " " << pos.y << " " << pos.z;
	return out;
}

std::ostream& operator<< (std::ostream &out, const glm::quat &q)
{
	out << q.x << " " << q.y << " " << q.z << " " << q.w;
	return out;
}

std::ostream& operator<< (std::ostream &out, const std::array<float, 3> &vec) {
	out << vec.at(0) << " " << vec.at(1) << " " << vec.at(2);
	return out;
}

std::ostream& operator<< (std::ostream &out, const std::array<float, 2> &sen)
{
	out << sen.at(0) << " " << sen.at(1);
	return out;
}

std::istream& operator>> (std::istream &in, glm::vec3 &pos)
{
	in >> pos.x >> pos.y >> pos.z;
	return in;
}

std::istream& operator>> (std::istream &in, glm::quat &q)
{
	in >> q.x >> q.y >> q.z >> q.w;
	return in;
}

std::istream& operator>> (std::istream &in, std::array<float, 2> &sen)
{
	in >> sen.at(0) >> sen.at(1);
	return in;
}

//std::string serial(const glm::vec3& vec){
//	return (std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z) + " ");
//}
//
//std::string serial(const glm::quat& q){
//	return (std::to_string(q.x) + " " + std::to_string(q.y) + " " + std::to_string(q.z) + " " + std::to_string(q.w) + " ");
//}

const void Trackable::serialize(std::ostream& stream)
{
	// Serialization code
	//stream << serial(position) << serial(euler) << serial(quaternion);
	stream << ID << " " << timestamp << " " << position << " " << euler << " " << quaternion << " " << sensors;
}

void Trackable::deserialize(std::istream& stream)
{
	// Deserialization code
	//stream >> position >> euler >> quaternion;
	stream >> ID >> timestamp >> position >> euler >> quaternion >> sensors;
}
