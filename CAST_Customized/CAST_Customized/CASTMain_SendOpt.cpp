#include "stdafx.h"
#include <iostream>
#include "Hardware.h"

#include <Dense>
using namespace Eigen;

int main()
{
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
	while(true){
		Sleep(1000);
		gHw.updatePosition(RIGHT);
		VectorXd rightpos = gHw.getPosition(RIGHT);
		std::cout << "Right Pos x: " << rightpos[0] << " y: "<< rightpos[1] << " z: " << rightpos[2] << " ori " << rightpos[3] << "\n"; 
		gHw.updatePosition(LEFT);
		VectorXd leftpos = gHw.getPosition(LEFT);
		std::cout << "Left Pos x: " << leftpos[0] << " y: "<< leftpos[1] << " z: " << leftpos[2] << " ori " << leftpos[3] << "\n";
	}
	
	
	return 0;
}