#include "stdafx.h"
#include <iostream>
#include "Hardware.h"
#include "UDPSender.h"
#include <Dense>
using namespace Eigen;

bool SendData = true;
bool PrintData = true;
int main()
{
	setupSocket("152.2.130.69","8500");
	std::cout << "Socket set up successfully\n";

	CHardware gHw = CHardware();
	int scanRes = gHw.scanUSB4();
	int initRes = _FAIL_;
	if(scanRes == _OK_){
		initRes = gHw.initUSB4();
	}else{
		std::cout << "USB4 not found\n";  
		return 0;
	}
	if(initRes == _OK_){
		std::cout << "USB4 found and initialized successfully.";
	}else{
		return 0;
	}
	int loadStatus = gHw.loadPositionParams();
	if(loadStatus == _FAIL_){
		std::cout << "Load param file failed\n";
		return 0;
	}
	char input;
	std::cout << "Put instruments to home position and press 'C'\n";
	std::cin >> input;
	while(input != 'c'){
		std::cout << "Wrong key, try again\n";
		std::cin >> input;
	}
	//reset encoder (right)
	gHw.resetAllEncoderValues(RIGHT);
	gHw.resetPositionVariables(RIGHT);
	gHw.resetAllEncoderValues(LEFT);
	gHw.resetPositionVariables(LEFT);

	char buf[1024];
	while(true){
		Sleep(50);
		gHw.updatePosition(RIGHT);
		VectorXd rightpos = gHw.getPosition(RIGHT);
		//std::cout << "Right Pos x: " << rightpos[0] << " y: "<< rightpos[1] << " z: " << rightpos[2] << " ori " << rightpos[3] << "\n"; 
		gHw.updatePosition(LEFT);
		VectorXd leftpos = gHw.getPosition(LEFT);
		//std::cout << "Left Pos x: " << leftpos[0] << " y: "<< leftpos[1] << " z: " << leftpos[2] << " ori " << leftpos[3] << "\n";
		sprintf_s(buf, sizeof(buf), "{ \"direction\":\"Left\",\"position\" : {\"x\":%.4f,\"y\" : %.4f,\"z\" : %.4f},\"rotation\" : %.4f }",
		leftpos[0]/100, leftpos[2]/100, leftpos[1]/100, leftpos[3]);
		if(PrintData){
			printf_s(buf);
			printf_s("\n");
		}
		if(SendData) sendData(buf);
		
		sprintf_s(buf, sizeof(buf), "{ \"direction\":\"Right\",\"position\" : {\"x\":%.4f,\"y\" : %.4f,\"z\" : %.4f},\"rotation\" : %.4f }",
		rightpos[0]/100, rightpos[2]/100, rightpos[1]/100, rightpos[3]);
		if(PrintData){
			printf_s(buf);
			printf_s("\n");
		}
		if(SendData) sendData(buf);
	}
	
	SocketClose();
	return 0;
}