#ifndef _TRACKER_TYPES_H
#define _TRACKER_TYPES_H

#include <array>

template <class T> using Position = std::array < T, 3 >;
template <class T> using Euler = std::array < T, 3 >;
template <class T> using Quaternion = std::array < T, 4 > ;

#define nullposition Position<float>{ { 0, 0, 0 } }
#define nulleuler Euler<float>{ { 0, 0, 0 } }
#define nullquaternion Quaternion<float>{ { 0, 0, 0, 0 } }

//template <class T>
//class Position : public std::array < T, 3 > {
//public:
//	Position(const std::array<T, 3> a)  {
//		this(a);
//	}
//};
//
//template <class T>
//class Euler : public std::array < T, 3 > {
//public:
//	Euler(const std::array<T, 3>& a)  {}
//};
//
//template <class T>
//class Quaternion : public std::array < T, 4 > {
//public:
//	Quaternion(const std::array<T, 4>& a) {}
//};

const std::array<float, 3> operator+ (const std::array<float, 3>& a, const std::array<float, 3>& b);
std::array<float, 3> operator/ (const std::array<float, 3>& a, const float& b);
std::array<float, 3> operator- (const std::array<float, 3>& a, const std::array<float, 3>& b);
bool equals_ (const std::array<float, 3>& a, const std::array<float, 3>& b);

float distance_(const Position<float>& a, const Position<float>& b);
#endif