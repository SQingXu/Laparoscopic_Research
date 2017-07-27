#include "stdafx.h"
#include "Hardware.h"

CCriticalSection g_csPosition;

CHardware::CHardware()
{
	/*memset(&nodeJs_ExecuteInfo, 0, sizeof(nodeJs_ExecuteInfo));*/
	initPlatformInfo();
}

CHardware::~CHardware()
{
	/*closeLocalHost();*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform infomation
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CHardware::initPlatformInfo()
{
	gPlatformInfo.school = 0;
	gPlatformInfo.folderPath.clear();

	gPlatformInfo.hwStatus.usb4ID[LEFT] = 0;
	gPlatformInfo.hwStatus.usb4ID[RIGHT] = 0;
	gPlatformInfo.hwStatus.usb4Str[LEFT] = _T("");
	gPlatformInfo.hwStatus.usb4Str[RIGHT] = _T("");
	gPlatformInfo.hwStatus.numOfCams = 0;

	gPlatformInfo.serverStatus.status = _FAIL_;
	gPlatformInfo.serverStatus.statusStr = _T("Fail");
}

void CHardware::updateHwStatus()
{
	gPlatformInfo.hwStatus.usb4ID[LEFT] = gUsb4.getUSB4ID(LEFT);
	gPlatformInfo.hwStatus.usb4ID[RIGHT] = gUsb4.getUSB4ID(RIGHT);
	gPlatformInfo.hwStatus.usb4Str[LEFT] =  gUsb4.getUSB4Type(LEFT);
	gPlatformInfo.hwStatus.usb4Str[RIGHT] = gUsb4.getUSB4Type(RIGHT);
	//gPlatformInfo.hwStatus.numOfCams = gCam.getNumOfCameras();
}


PLATFORM_INFO_VAR CHardware::getPlatformInfo()
{
	return gPlatformInfo;
}

void CHardware::setSchoolInPlatformInfo(int school)
{
	gPlatformInfo.school = school;
	if (gPlatformInfo.school == UA_PLATFORM)
	{
		gPlatformInfo.folderPath = UA_PLATFORM_PATH;
	}
	else if (gPlatformInfo.school == UNC_PLATFORM)
	{
		gPlatformInfo.folderPath = UNC_PLATFORM_PATH;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Local host
/////////////////////////////////////////////////////////////////////////////////////////////////////
//int CHardware::openLocalHost()
//{
//	memset(&nodeJs_ExecuteInfo, 0, sizeof(nodeJs_ExecuteInfo));
//    
//    nodeJs_ExecuteInfo.cbSize       = sizeof(nodeJs_ExecuteInfo);
//    nodeJs_ExecuteInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;                
//    nodeJs_ExecuteInfo.hwnd         = NULL;                
//    nodeJs_ExecuteInfo.lpVerb       = _T("open");                      // Operation to perform
//    nodeJs_ExecuteInfo.lpFile       = _T("node");  // Application name
//    nodeJs_ExecuteInfo.lpParameters = _T("localhost.js");           // Additional parameters
//    nodeJs_ExecuteInfo.lpDirectory  = _T(".\\server");                           // Default directory
//    nodeJs_ExecuteInfo.nShow        = SW_SHOW;
//    nodeJs_ExecuteInfo.hInstApp     = NULL;
//    
//
//	if(ShellExecuteEx(&nodeJs_ExecuteInfo) == FALSE)
//	{
//		AfxMessageBox(_T("cannot open local host"));
//		return _FAIL_;
//	}
//	else
//	{
//		gPlatformInfo.serverStatus.status = _OK_;
//		gPlatformInfo.serverStatus.statusStr = _T("OK");
//		return _OK_;
//	}
//}
//
//int CHardware::closeLocalHost()
//{
//	if (nodeJs_ExecuteInfo.hProcess != NULL)
//	{
//		BOOL retVal = TerminateProcess(nodeJs_ExecuteInfo.hProcess, 1);
//		if (retVal == 0)
//		{
//			AfxMessageBox(_T("cannot close nodejs"));
//			return _FAIL_;
//		}
//		else
//			return _OK_;
//	}
//	else
//	{
//		return _OK_;
//	}
//}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// USB4
/////////////////////////////////////////////////////////////////////////////////////////////////////
int CHardware::scanUSB4()
{
	int iSuccess;
	iSuccess = gUsb4.scanUSB4();
	return iSuccess;
}

int CHardware::initUSB4()
{
	int iSuccess;
	iSuccess = gUsb4.initUSB4();
	return iSuccess;
}

int CHardware::updateEncoderValues(FIXTURE fixture)
{
	int iSuccess;
	iSuccess = gUsb4.updateEncoderValues(fixture);
	return iSuccess;
}

void CHardware::getAllCurrentEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS])
{
	gUsb4.getAllCurrentEncoderValues(fixture, encCnt);
}


void CHardware::getAllPpreEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS])
{
	gUsb4.getAllPpreEncoderValues(fixture, encCnt);
}

int CHardware::resetAllEncoderValues(FIXTURE fixture)
{
	return gUsb4.resetAllEncoderValues(fixture);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// camera
/////////////////////////////////////////////////////////////////////////////////////////////////////
//int CHardware::scanCamera()
//{
//	int iSuccess;
//	iSuccess = gCam.searchAllCameras();
//	return iSuccess;
//}
//
//int CHardware::getImageFromCamera()
//{
//	return gCam.getImageFromCamera();
//}
//
//Mat* CHardware::getCurrentImage()
//{
//	return gCam.getCurrentImage();
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// camera calibration
/////////////////////////////////////////////////////////////////////////////////////////////////////
//int CHardware::readAllCamCalibInfo()
//{
//	string folderPath;
//	cCamCalibInfoReader.initCamCalibInfoVariable();
//	folderPath = gPlatformInfo.folderPath;
//	folderPath += CAM_CALIB_FOLDER;
//	int iSuccess;
//	iSuccess = cCamCalibInfoReader.readAllCamCalibInfo(folderPath.c_str());
//	iSuccess = cCamCalibInfoReader.generateJsonFile();
//	return iSuccess;
//}
//
//CAM_CALIB_INFO* CHardware::getCamCalibInfoPtr(int index)
//{
//	cCamCalibInfoReader.setCurInfoIndex(index);
//	return cCamCalibInfoReader.getCamCalibInfoPtr();
//}
//
//int CHardware::generateCameraMatrix()
//{
//	return cCamCalibAlgo.runLlsAlgo(cCamCalibInfoReader.getCamCalibInfoPtr());
//}
//
//int CHardware::saveCameraMatrix()
//{
//	return cCameraConfigEditor.genCamConfigJsonFile(gPlatformInfo.folderPath, cCamCalibAlgo.getCamMatrix());
//}
//
//int  CHardware::readCamMatrix(MatrixXd &camMatrix)
//{
//	return cCameraConfigEditor.readCamMatrix(gPlatformInfo.folderPath, camMatrix);
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Fixture calibration
/////////////////////////////////////////////////////////////////////////////////////////////////////
//int CHardware::genFixConfigJsonFile(MatrixXd curFixParams)
//{
//	cFixtureConfigEditor.genFixConfigJsonFile(gPlatformInfo.folderPath, cFixCalibAlgo.getCurFixture(), curFixParams);
//	return _OK_;
//}
//
//int CHardware::initFixCalibAlgo(HWND curParentDlgHwnd, FIXTURE fixture)
//{
//	int iSuccess;
//	cFixCalibAlgo.setParentHwnd(curParentDlgHwnd);
//	MatrixXd curCamMatrix;
//	iSuccess = readCamMatrix(curCamMatrix);
//	cFixCalibAlgo.initVariables(fixture, curCamMatrix);
//
//	return iSuccess;
//}
//
//int CHardware::runFixCalibAlgo()
//{
//	int iSuccess;
//	iSuccess = cFixCalibAlgo.findBestParams(gBestFixParams);
//	return iSuccess;
//}
//
//
//int CHardware::saveFixCalibResult()
//{
//	int iSuccess;
//	iSuccess = genFixConfigJsonFile(gBestFixParams);
//	return iSuccess;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////
// For Position
///////////////////////////////////////////////////////////////////////////////////////////////////
int CHardware::resetPositionVariables(FIXTURE fixture)
{
	int iSuccess;

	iSuccess = gUsb4.readyAllEncoders(fixture);
	if (iSuccess) return _FAIL_;

	long curEnc[NUM_OF_ENCODERS];
	gUsb4.getAllCurrentEncoderValues(fixture, curEnc);
	
	gPosition[fixture] = enc2XYZO(fixture, curEnc);
	g_csPosition.Lock();
		gPrePosition[fixture] = gPosition[fixture];
		gPprePosition[fixture] = gPosition[fixture];
	g_csPosition.Unlock();
	return _OK_;
	
}

int CHardware::updatePosition(FIXTURE fixture)
{
	int iSuccess;
	iSuccess = gUsb4.updateEncoderValues(fixture);
	if (iSuccess) return _FAIL_;
	long curEnc[NUM_OF_ENCODERS];
	g_csPosition.Lock();
		gPprePosition[fixture] = gPrePosition[fixture];
		gPrePosition[fixture] = gPosition[fixture];
	g_csPosition.Unlock();
	gUsb4.getAllCurrentEncoderValues(fixture, curEnc);
	gPosition[fixture] = enc2XYZO(fixture, curEnc);

	return _OK_;	
}

VectorXd CHardware::getPprePosition(FIXTURE fixture)
{
	return gPprePosition[fixture];
}
	
VectorXd CHardware::getPrePosition(FIXTURE fixture)
{
	return gPrePosition[fixture];
}


VectorXd CHardware::getPosition(FIXTURE fixture)
{
	VectorXd xyz;
	g_csPosition.Lock();
		xyz = gPosition[fixture];
	g_csPosition.Unlock();
	return xyz;
}


int CHardware::loadPositionParams()
{
	int iSuccess;
	// 1. read fixture configuration file
	MatrixXd fixtureParams;
	/*iSuccess = cFixtureConfigEditor.readFixtureParams(gPlatformInfo.folderPath, fixtureParams);*/
	iSuccess = cFixtureConfigEditor.readFixtureParams(".\\", fixtureParams);
	if (iSuccess) return _FAIL_;
	gFixParams[LEFT] = fixtureParams.row(LEFT);
	gFixParams[RIGHT] = fixtureParams.row(RIGHT);
	
	// 2. read camera configuration file
	//iSuccess = cCameraConfigEditor.readCamMatrix(gPlatformInfo.folderPath, gCamMatrix);
	return iSuccess;
}

VectorXd CHardware::getFixParams(FIXTURE fixture)
{
	return gFixParams[fixture];
}

Vector2d CHardware::xyz2XY(Vector3d xyz)
{
	Vector2d xy;
	if (gCamMatrix.cols() == 14)
	{
		// use params
		VectorXd tVec;
		tVec.setZero(3);
		tVec << gCamMatrix(0,10), gCamMatrix(0,11),gCamMatrix(0,12);
		VectorXd pos;
		Matrix3d rotX1, rotX0;
		rotX0 = getRotX(90.0);
		pos = rotX0*xyz+tVec;
		rotX1 = getRotX(gCamMatrix(0,13));
		pos = rotX1*pos;

		Vector2d xn;
		xn << pos(0)/pos(2), pos(1)/pos(2);
		double rr;
		rr = xn(0)*xn(0) + xn(1)*xn(1);
		double param1;
		param1 = 1+gCamMatrix(0,0)*rr+gCamMatrix(0,1)*rr*rr+gCamMatrix(0,4)*rr*rr*rr;
		Vector2d dx;
		dx << (2*gCamMatrix(0,2)*xn(0)*xn(1) + gCamMatrix(0,3)*(rr+2*xn(0)*xn(0))),
			  (gCamMatrix(0,2)*(rr+2*xn(1)*xn(1))+2*gCamMatrix(0,3)*xn(0)*xn(1));
		Vector2d xd;
		xd = param1*xn +dx;
		double xp, yp;
		xp = gCamMatrix(0,5)*(xd(0)+gCamMatrix(0,7)*xd(1))+gCamMatrix(0,8);
		yp = gCamMatrix(0,6)*xd(1)+gCamMatrix(0,9);
		
		xy << (int)xp, (int)yp;
	}
	else
	{
		// use matrix
		VectorXd pos;
		pos.setOnes(4,1);
		pos(0) = xyz(0); pos(1) = xyz(1); pos(2) = xyz(2);
		Vector3d uvw;
		uvw = gCamMatrix*pos;
		xy(0) = (int)(uvw(0) / uvw(2));
		xy(1)= (int)(uvw(1) / uvw(2));
	}

	return xy;
}

Vector3d CHardware::xyz2XYD(Vector3d xyz)
{
	VectorXd pos;
	pos.setOnes(4,1);
	pos(0) = xyz(0); pos(1) = xyz(1); pos(2) = xyz(2);
	Vector3d uvw;
	uvw = gCamMatrix*pos;

	Vector3d xyd;
	xyd(0) = (int)(uvw(0) / uvw(2));
	xyd(1)= (int)(uvw(1) / uvw(2));

	double GMIN_Y = 15.0;
	double GMAX_Y = 0;
	xyd(2) = 2.0*(xyz(1) - GMIN_Y)/(GMAX_Y - GMIN_Y) - 1;
	
	return xyd;
}

Vector2d CHardware::xyzo2XY(VectorXd xyzo)
{
	VectorXd pos;
	pos.setOnes(4,1);
	pos(0) = xyzo(0); pos(1) = xyzo(1); pos(2) = xyzo(2);
	Vector3d uvw;
	uvw = gCamMatrix*pos;

	Vector2d xy;
	xy(0) = (int)(uvw(0) / uvw(2));
	xy(1)= (int)(uvw(1) / uvw(2));

	return xy;
}

Vector3d CHardware::xyzo2XYD(VectorXd xyzo)
{
	VectorXd pos;
	pos.setOnes(4,1);
	pos(0) = xyzo(0); pos(1) = xyzo(1); pos(2) = xyzo(2);
	Vector3d uvw;
	uvw = gCamMatrix*pos;

	Vector3d xyd;
	xyd(0) = (int)(uvw(0) / uvw(2));
	xyd(1)= (int)(uvw(1) / uvw(2));

	double GMIN_Y = 15.0;
	double GMAX_Y = 0;
	xyd(2) = 2.0*(xyzo(1) - GMIN_Y)/(GMAX_Y - GMIN_Y) - 1;
	
	return xyd;
}

VectorXd CHardware::enc2XYZO(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS])
{
	VectorXd params = gFixParams[fixture];
	double theta, phi, insLength, rolAngle, alpha;
    
	int index = 3;
    theta = encCnt[YAW]* (params(index+1)-params(index))/(params(index+9)-params(index+8)) + params(index);
    insLength = encCnt[INS]* (params(index+3)-params(index+2))/(params(index+11)-params(index+10)) + params(index+2);
    phi = encCnt[PIT]* (params(index+5)-params(index+4))/(params(index+13)-params(index+12)) + params(index+4);
    rolAngle = encCnt[ROL]* (params(index+7)-params(index+6))/(params(index+15)-params(index+14)) + params(index+6);
    alpha = params(index+16);

	
	Matrix3d rotX1, rotZ1, rotX0, rotZ0;
	rotZ0 = getRotZ(alpha);
	rotX0 = getRotX(rolAngle);
	rotZ1 = getRotZ(theta);
	rotX1 = getRotX(phi);

	Vector3d initPos;
	initPos << insLength, 0, 0;

	Vector3d newPos;
	newPos = rotX1*rotZ1*rotX0*rotZ0*initPos;

	Vector3d offset;
	offset << params(0), params(1), params(2);
	newPos = newPos + offset;

	double orientation = rolAngle - params(index+6);//-90.0+rolAngle;
	Vector4d finalXyz;
	finalXyz << newPos(0), newPos(1), newPos(2), orientation;
	return finalXyz;
}

Matrix3d CHardware::getRotX(double degAngle)
{
	Matrix3d rotX;
	double radAngle = degAngle / 180.0 * M_PI;
	rotX << 1.0, 0.0, 0.0,
		    0.0, cos(radAngle), -sin(radAngle),
		    0.0, sin(radAngle),  cos(radAngle);

	return rotX;
}

Matrix3d CHardware::getRotY(double degAngle)
{
	Matrix3d rotY;
	double radAngle = degAngle / 180.0 * M_PI;

	rotY << cos(radAngle), 0.0, sin(radAngle),
		    0.0, 1.0, 0.0,
		   -sin(radAngle), 0.0, cos(radAngle);

	return rotY;
}

Matrix3d CHardware::getRotZ(double degAngle)
{
	Matrix3d rotZ;
	double radAngle = degAngle / 180.0 * M_PI;
	rotZ << cos(radAngle), -sin(radAngle), 0.0,
		    sin(radAngle),  cos(radAngle), 0.0,
		    0.0          ,  0.0          , 1.0;

	return rotZ;
}
