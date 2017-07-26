#include "stdafx.h"

#include "common.h"
#include "usb4Device.h"

using namespace std;

CCriticalSection g_csEncoderCount;
CCriticalSection g_csDacValue;

CUsb4Device::CUsb4Device()
{
	initVariables();
}

CUsb4Device::~CUsb4Device()
{
	//disableAllControllers();
	//if (usb4ID != NONE)	USB4_Shutdown();
}

void CUsb4Device::initVariables()
{
	FIXTURE fixture = LEFT;
	usb4ID[fixture] = NONE;
	for (int i=0 ; i<NUM_OF_ENCODERS ; i++)
	{
		setEncoderValue(fixture, (AXIS)i, 0);
	}
	virtualUsb4Var[fixture].preMousePos.x = -9999;
	virtualUsb4Var[fixture].preMousePos.y = -9999;

	digitalInOutReg[fixture].all = 0;

	g_csDacValue.Lock();
	curDacValue[fixture][YAW] = 0; curDacValue[fixture][INS] = 0;
	curDacValue[fixture][PIT] = 0; curDacValue[fixture][ROL] = 0;
	g_csDacValue.Unlock();

	fixture = RIGHT;
	usb4ID[fixture] = NONE;
	for (int i=0 ; i<NUM_OF_ENCODERS ; i++)
	{
		setEncoderValue(fixture, (AXIS)i, 0);
	}
	virtualUsb4Var[fixture].preMousePos.x = -9999;
	virtualUsb4Var[fixture].preMousePos.y = -9999;

	digitalInOutReg[fixture].all = 0;

	g_csDacValue.Lock();
	curDacValue[fixture][YAW] = 0; curDacValue[fixture][INS] = 0;
	curDacValue[fixture][PIT] = 0; curDacValue[fixture][ROL] = 0;
	g_csDacValue.Unlock();
}

int CUsb4Device::scanUSB4()
{
	int iSuccess;
	iSuccess = searchUSB4Devices();
	return iSuccess;
}

int CUsb4Device::initUSB4()
{
	int iSuccess;
	iSuccess = setupUSB4(LEFT);
	iSuccess = resetAllEncoderValues(LEFT);
	iSuccess = setupUSB4(RIGHT);
	iSuccess = resetAllEncoderValues(RIGHT);
	
	return iSuccess;
}

int CUsb4Device::getUSB4ID(FIXTURE fixture)
{
	return usb4ID[fixture];
}

CString CUsb4Device::getUSB4Type(FIXTURE fixture)
{
	CString strType;
	if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		strType = _T("Virtual");
	}
	else
	{
		strType = _T("Actual");
	}

	return strType;
}

int CUsb4Device::setupUSB4(FIXTURE fixture)
{
	if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else
	{
		int iSuccess;
		iSuccess = USB4_RestoreFactoryParameters(usb4ID[fixture]);
		iSuccess = configEncoders(fixture);
		iSuccess = configDigitalOutput(fixture);
	
		// disable all motors
		iSuccess = disableAllControllers(fixture);
		// set dac output
		setDAOut(fixture, YAW, 0.0);
		setDAOut(fixture, INS, 0.0);
		setDAOut(fixture, PIT, 0.0);

		if (iSuccess == USB4_SUCCESS) return _OK_;
		else return _FAIL_;
	}
}

int CUsb4Device::searchUSB4Devices()
{
	int retVal = _OK_;
	short iDeviceCount = 0;
	// init usb4ID
	usb4ID[LEFT] = VIRTUAL_USB4_ID;
	usb4ID[RIGHT] = VIRTUAL_USB4_ID;

	int iSuccess = USB4_Initialize(&iDeviceCount);
	
	if (iSuccess == USB4_SUCCESS)
	{
		unsigned char ucStartAddress = 0;
		for (short i=0 ; i<iDeviceCount ; i++)
		{
			unsigned char deviceName[USR_EEPROM_SIZE];
			// 1. get Device Name
			iSuccess = USB4_ReadUserEEPROM(i, ucStartAddress, USR_EEPROM_SIZE, deviceName);
			if (iSuccess != USB4_SUCCESS)
			{
				retVal = _FAIL_;
				break;
			}
			// 2. get Device ID
			if (strncmp((const char *)(deviceName), USB4_NAME_LEFT, USR_EEPROM_SIZE) == 0)
			{
				usb4ID[LEFT] = i;
			}
			else if (strncmp((const char *)(deviceName), USB4_NAME_RIGHT, USR_EEPROM_SIZE) == 0)
			{
				usb4ID[RIGHT] = i;
			}
		}
	}

	numOfActualDevices = iDeviceCount;

	return retVal;
}

// to write a device name for a new USB4 device
int CUsb4Device::writeUSB4DeviceName(short id, unsigned char *name)
{
	unsigned char ucStartAddress = 0;
	int iSuccess = USB4_WriteUserEEPROM(id, ucStartAddress, USR_EEPROM_SIZE, name);
	if (iSuccess == USB4_SUCCESS) return _OK_;
	else return _FAIL_;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Actual encoders
///////////////////////////////////////////////////////////////////////////////////////////////////
int CUsb4Device::readyAllEncoders(FIXTURE fixture)
{
	int iSuccess;

	if (usb4ID[fixture] != VIRTUAL_USB4_ID)
	{
		iSuccess = readAllEncoderValues(fixture);
	}
	else
	{
		iSuccess = readAllVirtualEncoderValues(fixture);
		ppreEncCount[fixture][YAW] = encCount[fixture][YAW];
		ppreEncCount[fixture][INS] = encCount[fixture][INS];
		ppreEncCount[fixture][PIT] = encCount[fixture][PIT];
		ppreEncCount[fixture][ROL] = encCount[fixture][ROL];

		preEncCount[fixture][YAW] = encCount[fixture][YAW];
		preEncCount[fixture][INS] = encCount[fixture][INS];
		preEncCount[fixture][PIT] = encCount[fixture][PIT];
		preEncCount[fixture][ROL] = encCount[fixture][ROL];
	}
	return iSuccess;
}

int CUsb4Device::resetAllEncoderValues(FIXTURE fixture)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		for (short encID=0 ; encID<NUM_OF_ENCODERS ; encID++)
		{
			setEncoderValue(fixture, (AXIS)encID, 0);
		}
		virtualUsb4Var[fixture].preMousePos.x = -9999;
		virtualUsb4Var[fixture].preMousePos.y = -9999;
	}
	else
	{
		int iSuccess;
		// Reset all Encoder counter values
		for (short encID=0 ; encID<USB4_MAX_ENCODERS ; encID++)
		{
			iSuccess = USB4_ResetCount(usb4ID[fixture],encID);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;
			setEncoderValue(fixture, (AXIS)encID, 0);
		}
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
	}

	return _OK_;
}

int CUsb4Device::updateEncoderValues(FIXTURE fixture)
{
	int iSuccess;
	if (usb4ID[fixture] == NONE) iSuccess = _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
		iSuccess = readAllVirtualEncoderValues(fixture);
	else
		iSuccess = readAllEncoderValues(fixture);
	
	return iSuccess;
}

unsigned long CUsb4Device::getCurrentEncoderValue(FIXTURE fixture, AXIS axis)
{
	unsigned long cnt;
	g_csEncoderCount.Lock();
		cnt = encCount[fixture][axis];
	g_csEncoderCount.Unlock();

	return cnt;
}

void CUsb4Device::getAllCurrentEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS])
{
	g_csEncoderCount.Lock();
		encCnt[YAW] = encCount[fixture][YAW];
		encCnt[INS] = encCount[fixture][INS];
		encCnt[PIT] = encCount[fixture][PIT];
		encCnt[ROL] = encCount[fixture][ROL];
	g_csEncoderCount.Unlock();
}

void CUsb4Device::getAllPpreEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS])
{
	g_csEncoderCount.Lock();
		encCnt[YAW] = ppreEncCount[fixture][YAW];
		encCnt[INS] = ppreEncCount[fixture][INS];
		encCnt[PIT] = ppreEncCount[fixture][PIT];
		encCnt[ROL] = ppreEncCount[fixture][ROL];
	g_csEncoderCount.Unlock();
}

int CUsb4Device::configEncoders(FIXTURE fixture)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else
	{
		int iSuccess;
		// common settings
		for (short encID=0 ; encID<USB4_MAX_ENCODERS ; encID++)
		{
			// Set the preset register to the CPR-1
			// CPR = 2047 (CPR-1 = 0x7FE)
			iSuccess = USB4_SetPresetValue(usb4ID[fixture], encID, 2046);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;
		
			//iSuccess = USB4_PresetCount(usb4ID,encID);
			//if (iSuccess != USB4_SUCCESS) return iSuccess;

			// Set quadrature mode to X4.
			iSuccess = USB4_SetMultiplier(usb4ID[fixture],encID,3);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;

			// Set counter mode to 24bit counter mode
			iSuccess = USB4_SetCounterMode(usb4ID[fixture],encID,0); 
			if (iSuccess != USB4_SUCCESS) return _FAIL_;

			// Set direction bit
			// [FixME]
			iSuccess = USB4_SetForward(usb4ID[fixture],encID,true);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;

			// Enable the counter
			iSuccess = USB4_SetCounterEnabled(usb4ID[fixture],encID,true);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;
		
			// Reset the counter to 0
			iSuccess = USB4_ResetCount(usb4ID[fixture],encID);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;
		}
		
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}

int CUsb4Device::readAllEncoderValues(FIXTURE fixture)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		readAllVirtualEncoderValues(fixture);
		return _FAIL_;
	}
	else
	{
		int iSuccess;
		iSuccess = readEncoderValue(fixture, YAW);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		iSuccess = readEncoderValue(fixture, INS);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		iSuccess = readEncoderValue(fixture, PIT);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		iSuccess = readEncoderValue(fixture, ROL);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
	}
	
	return _OK_;
}


int CUsb4Device::readEncoderValue(FIXTURE fixture, AXIS axis)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _FAIL_;
	}
	else
	{
		int iSuccess;
		unsigned long tmpEncCnt;
		iSuccess = USB4_GetCount(usb4ID[fixture], axis, &tmpEncCnt);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;

		if ( (tmpEncCnt & 0x800000) == 0x800000 )
		{
			g_csEncoderCount.Lock();
				ppreEncCount[fixture][axis] = preEncCount[fixture][axis];
				preEncCount[fixture][axis] = encCount[fixture][axis];
				encCount[fixture][axis] = 0xff000000 + (long)tmpEncCnt;
			g_csEncoderCount.Unlock();
		}
		else
		{
			g_csEncoderCount.Lock();
				ppreEncCount[fixture][axis] = preEncCount[fixture][axis];
				preEncCount[fixture][axis] = encCount[fixture][axis];
				encCount[fixture][axis] = (long)tmpEncCnt;
			g_csEncoderCount.Unlock();
		}
	}
	return _OK_;
}

int CUsb4Device::setEncoderValue(FIXTURE fixture, AXIS axis, long val)
{
	g_csEncoderCount.Lock();
		encCount[fixture][axis] = val;
	g_csEncoderCount.Unlock();

	return _OK_;
}


int CUsb4Device::readAllVirtualEncoderValues(FIXTURE fixture)
{
	encCount[fixture][YAW] = 0;
	encCount[fixture][INS] = 0;
	encCount[fixture][PIT] = 0;
	encCount[fixture][ROL] = 0;
	/*
	POINT point;
	GetCursorPos(&point);
	virtualUsb4Var[fixture].curMousePos.x = point.x;
	virtualUsb4Var[fixture].curMousePos.y = point.y;
	if (virtualUsb4Var[fixture].preMousePos.x == -9999)
		virtualUsb4Var[fixture].preMousePos.x = point.x;
	if (virtualUsb4Var[fixture].preMousePos.y == -9999)
		virtualUsb4Var[fixture].preMousePos.y = point.y;


	// check for right instrument
	SHORT status;
	if (usb4ID[fixture] == VIRTUAL_USB4_ID_RIGHT)
	{
		status = GetKeyState(VK_RBUTTON);
		if (status & 0x8000)
		{
			encCount[fixture][YAW] += 10*(virtualUsb4Var[fixture].curMousePos.x - virtualUsb4Var[fixture].preMousePos.x);
			if (encCount[fixture][YAW] < 0) encCount[fixture][YAW] = 0;
			if (encCount[fixture][YAW] > (long)(fixtureInfo.encCntRange[YAW])) encCount[fixture][YAW] = (long)(fixtureInfo.encCntRange[YAW]);
			encCount[fixture][PIT] += 5*(virtualUsb4Var[fixture].curMousePos.y - virtualUsb4Var[fixture].preMousePos.y);
			if (encCount[fixture][PIT] < 0) encCount[fixture][PIT] = 0;
			if (encCount[fixture][PIT] > (long)(fixtureInfo.encCntRange[PIT])) encCount[fixture][PIT] = (long)(fixtureInfo.encCntRange[PIT]);

			status = GetKeyState(VK_UP);
			if (status & 0x8000)
			{
				encCount[fixture][INS] += 100;
				if (encCount[fixture][INS] < 0) encCount[fixture][INS] = 0;
				if (encCount[fixture][INS] > (long)(fixtureInfo.encCntRange[INS])) encCount[fixture][INS] = (long)(fixtureInfo.encCntRange[INS]);
			}
			status = GetKeyState(VK_DOWN);
			if (status & 0x8000)
			{
				encCount[fixture][INS] -= 100;
				if (encCount[fixture][INS] < 0) encCount[fixture][INS] = 0;
				if (encCount[fixture][INS] > (long)(fixtureInfo.encCntRange[INS])) encCount[fixture][INS] = (long)(fixtureInfo.encCntRange[INS]);
			}
		}
		
	}
		
	// check for left instrument
	if (usb4ID == VIRTUAL_USB4_ID_LEFT)
	{
		status = GetKeyState(VK_LBUTTON);
		if (status & 0x8000)
		{
			encCount[fixture][YAW] += 10*(virtualUsb4Var[fixture].curMousePos.x - virtualUsb4Var.preMousePos.x); 
			if (encCount[fixture][YAW] > 0) encCount[fixture][YAW] = 0;
			if (encCount[fixture][YAW] < (long)(fixtureInfo.encCntRange[YAW])) encCount[fixture][YAW] = (long)(fixtureInfo.encCntRange[YAW]);
			encCount[fixture][PIT] -= 5*(virtualUsb4Var.curMousePos.y - virtualUsb4Var.preMousePos.y);
			if (encCount[fixture][PIT] > 0) encCount[PIT] = 0;
			if (encCount[fixture][PIT] < (long)(fixtureInfo.encCntRange[PIT])) encCount[fixture][PIT] = (long)(fixtureInfo.encCntRange[PIT]);
			status = GetKeyState(VK_UP);
			if (status & 0x8000)
			{
				encCount[fixture][INS] -= 100;
				if (encCount[fixture][INS] > 0) encCount[fixture][INS] = 0;
				if (encCount[fixture][INS] < (long)(fixtureInfo.encCntRange[INS])) encCount[fixture][INS] = (long)(fixtureInfo.encCntRange[INS]);
			}
			status = GetKeyState(VK_DOWN);
			if (status & 0x8000)
			{
				encCount[fixture][INS] += 100;
				if (encCount[fixture][INS] > 0) encCount[fixture][INS] = 0;
				if (encCount[fixture][INS] < (long)(fixtureInfo.encCntRange[INS])) encCount[fixture][INS] = (long)(fixtureInfo.encCntRange[INS]);
			}
		}
	}	
	virtualUsb4Var[fixture].preMousePos.x = point.x;
	virtualUsb4Var[fixture].preMousePos.y = point.y;
	*/
	return _OK_;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Digital I/O
///////////////////////////////////////////////////////////////////////////////////////////////////
int CUsb4Device::enableController(FIXTURE fixture, AXIS axis)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return USB4_SUCCESS;
	}
	else 
	{
		int iSuccess = USB4_SUCCESS;
		if (axis == YAW)	digitalInOutReg[fixture].bits.yawEnable = ENABLE_MOTOR;
		else if (axis == INS) digitalInOutReg[fixture].bits.insEnable = ENABLE_MOTOR;
		else if (axis == PIT) digitalInOutReg[fixture].bits.pitEnable = ENABLE_MOTOR;
		//else if (axis == ROT) digitalInOutReg.bit.rotEnable = ENABLE_MOTOR;

		iSuccess = USB4_WriteOutputPortRegister(usb4ID[fixture], digitalInOutReg[fixture].all);

		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}

int CUsb4Device::enableAllControllers(FIXTURE fixture)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return USB4_SUCCESS;
	}
	else
	{
		int iSuccess = USB4_SUCCESS;

		if ( (digitalInOutReg[fixture].bits.yawEnable == ENABLE_MOTOR) &&
			(digitalInOutReg[fixture].bits.insEnable == ENABLE_MOTOR) &&
			(digitalInOutReg[fixture].bits.pitEnable == ENABLE_MOTOR) )
		{
			iSuccess = USB4_SUCCESS;
		}
		else
		{
			digitalInOutReg[fixture].bits.yawEnable = ENABLE_MOTOR;
			digitalInOutReg[fixture].bits.insEnable = ENABLE_MOTOR;
			digitalInOutReg[fixture].bits.pitEnable = ENABLE_MOTOR;
			//digitalInOutReg.bit.rotEnable = ENABLE_MOTOR;
			iSuccess = USB4_WriteOutputPortRegister(usb4ID[fixture], digitalInOutReg[fixture].all);
		}

		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}



int CUsb4Device::disableAllControllers(FIXTURE fixture)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else 
	{
		int iSuccess = USB4_SUCCESS;

		digitalInOutReg[fixture].bits.yawEnable = DISABLE_MOTOR;
		digitalInOutReg[fixture].bits.insEnable = DISABLE_MOTOR;
		digitalInOutReg[fixture].bits.pitEnable = DISABLE_MOTOR;

		iSuccess = USB4_WriteOutputPortRegister(usb4ID[fixture], digitalInOutReg[fixture].all);

		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}


int CUsb4Device::disableController(FIXTURE fixture, AXIS axis)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else 
	{
		int iSuccess = USB4_SUCCESS;
		if (axis == YAW)	digitalInOutReg[fixture].bits.yawEnable = DISABLE_MOTOR;
		else if (axis == INS) digitalInOutReg[fixture].bits.insEnable = DISABLE_MOTOR;
		else if (axis == PIT) digitalInOutReg[fixture].bits.pitEnable = DISABLE_MOTOR;
		else if (axis == ROL) digitalInOutReg[fixture].bits.rotEnable = DISABLE_MOTOR;

		iSuccess = USB4_WriteOutputPortRegister(usb4ID[fixture], digitalInOutReg[fixture].all);

		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}


int CUsb4Device::setMotorDirection(FIXTURE fixture, AXIS axis, MOTOR_DIR dir)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else 
	{
		int iSuccess = USB4_SUCCESS;

		if (axis == PIT) digitalInOutReg[fixture].bits.pitDir = dir;
		else if (axis == ROL) digitalInOutReg[fixture].bits.rotDir = dir;
		else return _OK_;

		iSuccess = USB4_WriteOutputPortRegister(usb4ID[fixture], digitalInOutReg[fixture].all);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}

int CUsb4Device::configDigitalOutput(FIXTURE fixture)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else 
	{
		int iSuccess;
		digitalInOutReg[fixture].all = 0;
		iSuccess = USB4_WriteRegister(usb4ID[fixture], OUTPUT_PORT_SETUP_REGISTER, 0x0000);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// For DA converter
///////////////////////////////////////////////////////////////////////////////////////////////////
void CUsb4Device::getAllDAOuts(FIXTURE fixture, int(&daOuts)[USB4_MAX_ENCODERS])
{
	g_csDacValue.Lock();
	daOuts[YAW] = curDacValue[fixture][YAW];
	daOuts[INS] = curDacValue[fixture][INS];
	daOuts[PIT] = curDacValue[fixture][PIT];
	daOuts[ROL] = curDacValue[fixture][ROL];
	g_csDacValue.Unlock();
}

int CUsb4Device::setDAOut(FIXTURE fixture, AXIS axis, double val)
{
	if (usb4ID[fixture] == NONE) return _FAIL_;
	else if (usb4ID[fixture] == VIRTUAL_USB4_ID)
	{
		return _OK_;
	}
	else 
	{
		int iSuccess = USB4_SUCCESS;
		int roundGain;

		if (axis == YAW)	val += DAC_ZERO_OFFSET;
		else if (axis == INS)
		{
			MOTOR_DIR dir;
			if (val < 0.0)
			{
				dir = MOTOR_CW;
			}
			else
			{
				dir = MOTOR_CCW;
			}

			iSuccess = setMotorDirection(fixture, axis, dir);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;
			val = fabs(val);
		}
		else if (axis == PIT)
		{
			MOTOR_DIR dir;
			if (val < 0.0)
			{
				dir = MOTOR_CW;
			}
			else
			{
				dir = MOTOR_CCW;
			}

			iSuccess = setMotorDirection(fixture, axis, dir);
			if (iSuccess != USB4_SUCCESS) return _FAIL_;
			val = fabs(val);
		}
		else if (axis == ROL)
		{
			val = fabs(val);
		}

		roundGain = (int)(val+0.5);
	

		if(roundGain < MIN_DAC_VAL)
			roundGain = MIN_DAC_VAL;
		if(roundGain >= MAX_DAC_VAL)
			roundGain = MAX_DAC_VAL;
	
		// save actual da out
		g_csDacValue.Lock();
		curDacValue[fixture][axis] = roundGain;
		g_csDacValue.Unlock();

		// Set the DA Value
		iSuccess = USB4_SetD2A(usb4ID[fixture], axis, roundGain, TRUE);
		if (iSuccess != USB4_SUCCESS) return _FAIL_;
		else return _OK_;
	}
}




