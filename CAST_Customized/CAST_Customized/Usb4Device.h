#ifndef _USB4_DEVICE_H_
#define _USB4_DEVICE_H_


// load lib file for USB4 device
#pragma comment(lib,"lib\\usb4.lib")

/////////////////////////////////////////////////////////////////////////////
// include
/////////////////////////////////////////////////////////////////////////////
#include "common.h"			// common header
#include "USB4.h"


// definitions, structures, ...
// for USB4 info.
#define USR_EEPROM_SIZE	64
#define USB4_NAME_LEFT		"USB4_LEFT"
#define USB4_NAME_RIGHT		"USB4_RIGHT"

#define VIRTUAL_USB4_ID   -100

#define ENABLE_MOTOR	1
#define DISABLE_MOTOR	0
#define MIN_DAC_VAL		0
#define MAX_DAC_VAL		4096
#define DAC_ZERO_OFFSET 2048

enum MOTOR_DIR
{
	MOTOR_CCW, MOTOR_CW
};

struct DIO_BITS {				// bits   description
	unsigned char yawDir:1;		// 0 (Presently not used)   
	unsigned char insDir:1;		// 1  
	unsigned char pitDir:1;		// 2 
	unsigned char rotDir:1;		// 3 (Presently not used) 
	unsigned char yawEnable:1;	// 4  
	unsigned char insEnable:1;	// 5
	unsigned char pitEnable:1;	// 6  
	unsigned char rotEnable:1;	// 7 (Presently not used) 
} ;

union DIO_REG {
   unsigned char	all;
   struct DIO_BITS bits;
};

typedef struct
{
	XY_INT preMousePos;
	XY_INT curMousePos;
} VIRTUAL_USB4_VAR;

class CUsb4Device
{
public:
	CUsb4Device();
	~CUsb4Device();

public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// For init
///////////////////////////////////////////////////////////////////////////////////////////////////
	int scanUSB4();
	int initUSB4();
///////////////////////////////////////////////////////////////////////////////////////////////////
// interface functions
///////////////////////////////////////////////////////////////////////////////////////////////////
	int getUSB4ID(FIXTURE fixture);
	CString getUSB4Type(FIXTURE fixture);
	int setupUSB4(FIXTURE fixture);
private:

	void initVariables();

///////////////////////////////////////////////////////////////////////////////////////////////////
// Device Info
///////////////////////////////////////////////////////////////////////////////////////////////////
	// variables
	int numOfActualDevices;
	int usb4ID[NUM_OF_FIXTURES];
	// functions
	int searchUSB4Devices();
	int writeUSB4DeviceName(short id, unsigned char *name);
	
///////////////////////////////////////////////////////////////////////////////////////////////////
// Actual encoders
///////////////////////////////////////////////////////////////////////////////////////////////////
	// functions
public:
	int readyAllEncoders(FIXTURE fixture);
	int resetAllEncoderValues(FIXTURE fixture);
	int updateEncoderValues(FIXTURE fixture);
	unsigned long getCurrentEncoderValue(FIXTURE fixture, AXIS axis);
	void getAllCurrentEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS]);
	void getAllPpreEncoderValues(FIXTURE fixture, long(&encCnt)[NUM_OF_ENCODERS]);
	int setEncoderValue(FIXTURE fixture, AXIS axis, long val);
private:
	int configEncoders(FIXTURE fixture);
	int readAllEncoderValues(FIXTURE fixture);
	int readEncoderValue(FIXTURE fixture, AXIS axis);
	
// variables
private:
	long encCount[NUM_OF_FIXTURES][NUM_OF_ENCODERS];
	long preEncCount[NUM_OF_FIXTURES][NUM_OF_ENCODERS];
	long ppreEncCount[NUM_OF_FIXTURES][NUM_OF_ENCODERS];

///////////////////////////////////////////////////////////////////////////////////////////////////
// virtual encoder
//  - share encCount variable with real encoder counter
///////////////////////////////////////////////////////////////////////////////////////////////////
	// functions
private:
	int readAllVirtualEncoderValues(FIXTURE fixture);
private:
	VIRTUAL_USB4_VAR virtualUsb4Var[NUM_OF_FIXTURES];


///////////////////////////////////////////////////////////////////////////////////////////////////
// For Digital I/O
///////////////////////////////////////////////////////////////////////////////////////////////////
	// functions
public:
	// for ADS50/5
	int enableController(FIXTURE fixture, AXIS axis);
	int enableAllControllers(FIXTURE fixture);

	int disableController(FIXTURE fixture, AXIS axis);
	int disableAllControllers(FIXTURE fixture);
	
	int setMotorDirection(FIXTURE fixture, AXIS axis, MOTOR_DIR dir); 	// ESCON 50/5
private:
	int configDigitalOutput(FIXTURE fixutre);
	// variables
private:
	volatile DIO_REG digitalInOutReg[NUM_OF_FIXTURES];


///////////////////////////////////////////////////////////////////////////////////////////////////
// For DA converter
///////////////////////////////////////////////////////////////////////////////////////////////////	
// functions
public:
	void getAllDAOuts(FIXTURE fixture, int(&daOuts)[NUM_OF_ENCODERS]);
	int setDAOut(FIXTURE fixture, AXIS axis, double val);
// variables
private:
	int curDacValue[NUM_OF_FIXTURES][NUM_OF_ENCODERS];

};

#endif