#pragma once
#include "glm\glm.hpp"
#include <chrono>
#include "glm\gtx\quaternion.hpp"
#include <array>

#include "Serializable.h"

using namespace std::chrono;

std::ostream& operator<< (std::ostream &out, const glm::vec3 &pos);
std::ostream& operator<< (std::ostream &out, const std::array<float, 2> &sen);
std::ostream& operator<< (std::ostream &out, const std::array<float, 3> &vec);
std::ostream& operator<< (std::ostream &out, const glm::quat &q);
std::istream& operator>> (std::istream &in, glm::vec3 &pos);
std::istream& operator>> (std::istream &in, glm::quat &q);
std::istream& operator>> (std::istream &in, std::array<float, 2> &sen);

class Trackable : public Serializable
{
public:
	//time_point<high_resolution_clock> timestamp;
	double timestamp;
	int ID;
	glm::vec3 position;
	glm::vec3 euler;
	glm::quat quaternion;
	std::array<float, 2> sensors;
	
	Trackable(glm::vec3 pos = glm::vec3{ 0, 0, 0 }, glm::vec3 euler = glm::vec3{ 0, 0, 0 }, glm::quat quat = glm::quat{ 0, 0, 0, 1 }, std::array<float, 2> sensors = { { 0.0f, 0.0f } }, int id = 0, double timestamp = 0.0f);
	~Trackable();

	const void serialize(std::ostream& stream);
	void deserialize(std::istream& stream);
};

