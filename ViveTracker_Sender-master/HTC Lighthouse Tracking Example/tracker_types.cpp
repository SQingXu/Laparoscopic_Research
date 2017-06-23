#include "tracker_types.h"

const std::array<float, 3> operator+ (const std::array<float, 3>& a, const std::array<float, 3>& b){
	return{ { a[0] + b[0], a[1] + b[1], a[2] + b[2] } };
}

std::array<float, 3> operator/ (const std::array<float, 3>& a, const float& b){
	return{ { a[0] / b, a[1] / b, a[2] / b } };
}

std::array<float, 3> operator- (const std::array<float, 3>& a, const std::array<float, 3>& b){
	return{ { a[0] - b[0], a[1] - b[1], a[2] - b[2] } };
}

bool equals_(const std::array<float, 3>& a, const std::array<float, 3>& b){
	return ((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
}

float distance_(const Position<float>& a, const Position<float>& b){
	return std::sqrtf((a[0] - b[0])*(a[0] - b[0]) + (a[1] - b[1])*(a[1] - b[1]) + (a[2] - b[2])*(a[2] - b[2]));
}