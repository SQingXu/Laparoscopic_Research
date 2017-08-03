#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <iomanip>

int LoadImage(cv::Mat*);
void readCamCalibFile(cv::Mat*, cv::Mat*);
int aruco_detect();