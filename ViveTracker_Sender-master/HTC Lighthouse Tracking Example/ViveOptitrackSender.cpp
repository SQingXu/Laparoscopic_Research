//
// HTC Vive Lighthouse Tracking Example
// By Peter Thor 2016
//

#include "stdafx.h"
#include "Windows.h"

#include "LighthouseTracking.h"
#include "Optitrack.h"

#include <memory>
#include <thread>
// windows keyboard input
#include <conio.h>
#include <sstream>
//#include <iomanip>


//std::string optitrackIP = "127.0.0.1";
std::string optitrackIP = "10.100.35.254";

std::string formatV(glm::vec3 v, int width = 5, int precision = 3) {
	std::ostringstream strOS;
	strOS << "[";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << v.x;
	strOS << ",";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << v.y;
	strOS << ",";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << v.z;
	strOS << "]";
	return strOS.str();
}

std::string formatQ(glm::quat q, int width = 5, int precision = 3) {
	std::ostringstream strOS;
	strOS << "[";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << q.x;
	strOS << ",";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << q.y;
	strOS << ",";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << q.z;
	strOS << ",";
	strOS << std::fixed << std::setprecision(precision) << std::setfill(' ') << std::setw(width + precision) << q.w;
	strOS << "]";
	return strOS.str();
}

class TrackerWriter {

	std::string optitrackIP = "127.0.0.1";
	//std::string optitrackIP = "10.100.35.254";
	int rigidBodyID = 1;

	std::unique_ptr<LighthouseTracking> lighthouseTracking;
	std::unique_ptr<Optitrack> optitrack;
	
public:
	bool isRunning = true;

	TrackerWriter(std::string optitrackIP) {
		
		this->optitrackIP = optitrackIP;

		//Initializing Network
		WSADATA WsaData;
		if (WSAStartup(0x202, &WsaData) == SOCKET_ERROR)
		{
			std::cerr << "WSAStartup failed (" << WSAGetLastError() << ")" << std::endl;
		}

		// Create a new LighthouseTracking instance and parse as needed
		std::unique_ptr<LighthouseTracking>  lt(new LighthouseTracking());
		std::unique_ptr<Optitrack> ot(new Optitrack());
		lighthouseTracking = std::move(lt);
		optitrack = std::move(ot);

		if (optitrack) {
			//std::cout << "Optitrack is true" << std::endl;
			optitrack->addCallback(std::bind(&TrackerWriter::on_optitrack_update, this));
			optitrack->connect(optitrackIP);
			
		}

		if (lighthouseTracking) {
			lighthouseTracking->addCallback(std::bind(&TrackerWriter::on_vive_update, this));
		}

		std::thread listen_tread(&TrackerWriter::listenThread, this);
		listen_tread.detach();
	}

	void centerViveOrigin() {
		lighthouseTracking->centerOrigin();
	}

	void listenThread() {
		while (isRunning) {
			auto orb = optitrack->getRigidBody(rigidBodyID);
			glm::vec3 opos(orb.pos.at(0), orb.pos.at(1), orb.pos.at(2));
			glm::quat orot(orb.quaternion.at(0), orb.quaternion.at(1), orb.quaternion.at(2), orb.quaternion.at(3));
			auto lp = lighthouseTracking->GetTrackerPosition();
			auto lr = lighthouseTracking->GetTrackerRotation();
			glm::vec3 lpos(lp);
			glm::quat lrot(lr);

			//std::cout << "pos:" << formatV(opos,3,3) << ", " << formatV(lpos,3,3) << " ";

			//std::cout << "rot:" << formatQ(orot,3,3) << ", " << formatQ(lrot,3,3) << std::endl;
			//std::cout << "rot:" << formatV(glm::eulerAngles(orot), 3, 3) << ", " << formatV(glm::eulerAngles(lrot), 3, 3) << std::endl;

			//std::cout << "rot:" << formatV(glm::eulerAngles(orot), 3, 3) << ", " << formatQ(lrot, 4, 3) << std::endl;

			//std::cout << "rot:" << formatV(glm::vec3(orb.euler.at(0), orb.euler.at(1), orb.euler.at(2)), 3, 3) << ", " << formatV(glm::eulerAngles(lrot), 3, 3) << std::endl;
			// a delay to not overheat your computer... :)
			Sleep(100);
		}
	}
	
	void on_optitrack_update() {
		//std::cout << "optitrack update" << std::endl;
		/*if (!enable_streaming)
		return;
		update_positions();
		update_orientations();
		send_tracker_data();*/
	}

	void on_vive_update() {
		//std::cout << "vive update" << std::endl;

		//auto orb = optitrack->getRigidBody(rigidBodyID);
		//glm::vec3 opos(orb.pos.at(0), orb.pos.at(1), orb.pos.at(2));
		//glm::quat orot(orb.quaternion.at(0), orb.quaternion.at(1), orb.quaternion.at(2), orb.quaternion.at(3));
		//auto lp = lighthouseTracking->GetTrackerPosition();
		//auto lr = lighthouseTracking->GetTrackerRotation();
		//glm::vec3 lpos(lp);
		//glm::quat lrot(lr);
		//char buf[1024];
		//sprintf_s(buf, sizeof(buf), "{ \"id\":\"tracker%i\",\"position\" : {\"x\":%.4f,\"y\" : %.4f,\"z\" : %.4f},\"rotation\" : {\"x\":%.8f,\"y\" : %.8f,\"z\" : %.8f,\"w\" : %.8f} }",
		//	-1, position.v[0], position.v[1], position.v[2], quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		////printf_s(buf);
		//sendData(buf);
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	std::unique_ptr<TrackerWriter> writer(new TrackerWriter(optitrackIP));

	while (writer->isRunning) {
		Sleep(1);

		// Windows quit routine - adapt as you need
		if (_kbhit()) {
			char ch = _getch();
			if ('q' == ch) {

				char buf[1024];
				sprintf_s(buf, sizeof(buf), "User pressed 'q' - exiting...");
				printf_s(buf);

				break;
			}
			else if ('v' == ch) {

				char buf[1024];
				sprintf_s(buf, sizeof(buf), "\nResetting Vive Origin\n");
				printf_s(buf);
				writer->centerViveOrigin();
				//break;
			}
		}
	}
}
