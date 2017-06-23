#pragma once
#include <iostream>

class Serializable
{
public:
	Serializable(){}
	virtual ~Serializable(){}

	virtual const void serialize(std::ostream& stream) = 0;
	virtual void deserialize(std::istream& stream) = 0;
};

