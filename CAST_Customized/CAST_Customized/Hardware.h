#ifndef _HARDWARE_H_

#define _HARDWARE_H_

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "common.h"
#include "Usb4Device.h"
//#include "Camera.h"

//#include "CamCalibInfoReader.h"
//#include "CameraConfigEditor.h"
//#include "CamCalibAlgo.h"
//#include "fixCalibAlgo.h"
#include "FixtureConfigEditor.h"

// for eigen library
#include <Dense>
using namespace Eigen;

typedef struct {
	int usb4ID[NUM_OF_FIXTURES];
	CString usb4Str[NUM_OF_FIXTURES];
	int numOfCams;
} HW_STATUS_VAR;

typedef struct {
	int status;
	CString statusStr;
} SERVER_STATUS_VAR;

typedef struct {
	int school;
	string folderPath;
	HW_STATUS_VAR hwStatus;
	SERVER_STATUS_VAR serverStatus;
} PLATFORM_INFO_VAR;

class CHardware
{
public:
	CHardware();
	~CHardware();

/////////////////////////////////////////////////////////////////////////////////////////////////////
// platform
/////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void initPlatformInfo();
	void updateHwStatus();
	PLATFORM_INFO_VAR getPlatformInfo();
	void setSchoolInPlatformInfo(int school);
private:
	PLATFORM_INFO_VAR gPlatformInfo;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Local host
/////////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	int openLocalHost();
//	int closeLocalHost();
//private:
//	// variables
//	SHELLEXECUTEINFO nodeJs_ExecuteInfo;


/////////////////////////////////////////////////////////////////////////////////////////////////////
// usb4 device
/////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	int scanUSB4();
	int initUSB4();
	int updateEncoderValues(FIXTURE fixture);
	void getAllCurrentEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS]);
	void getAllPpreEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS]);
	int resetAllEncoderValues(FIXTURE fixture);
private:
	// variables
	CUsb4Device gUsb4;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Camera
/////////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	int scanCamera();
//	int getImageFromCamera();
//	Mat* getCurrentImage();
//private:
//	CCamera gCam;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Camera Calibration
/////////////////////////////////////////////////////////////////////////////////////////////////////
//public:
//	int readAllCamCalibInfo();
//	CAM_CALIB_INFO* getCamCalibInfoPtr(int index);
//
//	int generateCameraMatrix();
//	int saveCameraMatrix();
//	int readCamMatrix(MatrixXd &camMatrix);
//private:
//	// variables
//	CCameraConfigEditor cCameraConfigEditor;
//	CCamCalibInfoReader cCamCalibInfoReader;
//	CCamCalibAlgo cCamCalibAlgo;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Fixture Calibration
/////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	int genFixConfigJsonFile(MatrixXd curFixParams);
//	int initFixCalibAlgo(HWND curParentDlgHwnd, FIXTURE fixture);
//	int runFixCalibAlgo();
//	int saveFixCalibResult();
private:
//	CFixCalibAlgo cFixCalibAlgo;
	CFixtureConfigEditor cFixtureConfigEditor;
//	MatrixXd gBestFixParams;
///////////////////////////////////////////////////////////////////////////////////////////////////
// For Position
///////////////////////////////////////////////////////////////////////////////////////////////////
public:
	int resetPositionVariables(FIXTURE fixture);
	int updatePosition(FIXTURE fixture);


	VectorXd getPprePosition(FIXTURE fixture);
	VectorXd getPrePosition(FIXTURE fixture);
	VectorXd getPosition(FIXTURE fixture);
private:
	
	VectorXd gPosition[NUM_OF_FIXTURES];			// x,y,z with orientation
	VectorXd gPrePosition[NUM_OF_FIXTURES];			// x,y,z with orientation
	VectorXd gPprePosition[NUM_OF_FIXTURES];		// x,y,z with orientation

	// for parameters
public:
	int loadPositionParams();
	VectorXd getFixParams(FIXTURE fixture);

public:
	Vector2d xyz2XY(Vector3d xyz);
	Vector3d xyz2XYD(Vector3d xyz);
	Vector2d xyzo2XY(VectorXd xyzo);
	Vector3d xyzo2XYD(VectorXd xyzo);
	Matrix3d getRotX(double degAngle);
	Matrix3d getRotY(double degAngle);
	Matrix3d getRotZ(double degAngle);

private:
	VectorXd enc2XYZO(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS]);
		
	
	VectorXd gFixParams[NUM_OF_FIXTURES];
	MatrixXd gCamMatrix;

#if 0
///////////////////////////////////////////////////////////////////////////////////////////////////
// For Position
///////////////////////////////////////////////////////////////////////////////////////////////////		
// functions
public:	
	int resetPositionVariables(FIXTURE fixture);
	XYZ getPprePosition(FIXTURE fixture);
	XYZ getPrePosition(FIXTURE fixture);
	int updatePosition(FIXTURE fixture);
	int updatePosition2(FIXTURE fixture);
	XYZ getPosition(FIXTURE fixture);
	void convertXYZ2Enc(XYZ xyz, long(&encCnt)[USB4_MAX_ENCODERS]);

private:
	void calculateXYZ(FIXTURE fixture);
	int getRotX(double degAngle, Matrix3d &rotX);
	int getRotZ(double degAngle, Matrix3d &rotZ);
// variable
private:
	XYZ gPosition[NUM_OF_FIXTURES];
	XYZ gPrePosition[NUM_OF_FIXTURES];
	XYZ gPprePosition[NUM_OF_FIXTURES];

///////////////////////////////////////////////////////////////////////////////////////////////////
// For Fixture info
///////////////////////////////////////////////////////////////////////////////////////////////////
// functions
public:
	void loadFixtureInfo(FIXTURE_INFO info, FIXTURE fixture);
// variables
private:
	FIXTURE_INFO fixtureInfo[NUM_OF_FIXTURES];

#endif
};

#endif