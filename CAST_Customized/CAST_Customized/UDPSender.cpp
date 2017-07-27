#include "stdafx.h"
#include "UDPSender.h"

#include <iostream>

SOCKET sock;
sockaddr_storage addrDest = {};
sockaddr_storage addrDest2 = {};

int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
{
	int result;
	addrinfo* result_list = NULL;
	addrinfo hints = {};
	hints.ai_family = family;
	hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
	result = getaddrinfo(hostname, service, &hints, &result_list);
	if (result == 0)
	{
		//ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
		memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
		freeaddrinfo(result_list);
	}

	return result;
}

int setupSocket(const char* ip, const char* port) {
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	int result = 0;
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	char szIP[100];

	sockaddr_in addrListen = {}; // zero-int, sin_port is 0, which picks a random port for bind.
	addrListen.sin_family = AF_INET;
	result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));
	if (result == -1)
	{
		int lasterror = errno;
		std::cout << "error: " << lasterror;
		exit(1);
	}

	result = resolvehelper(ip, AF_INET, port, &addrDest);
	if (result != 0)
	{
		int lasterror = errno;
		std::cout << "error: " << lasterror;
		exit(1);
	}

	/*result = resolvehelper("127.0.0.1", AF_INET, port, &addrDest);
	if (result != 0)
	{
		int lasterror = errno;
		std::cout << "error: " << lasterror;
		exit(1);
	}*/
	std::cout << "No error occurs" << std::endl;
	return 0;
}

int sendData(const char* msg) {
	//const char* msg = "Jane Doe";
	size_t msg_length = strlen(msg);

	int result = sendto(sock, msg, msg_length, 0, (sockaddr*)&addrDest, sizeof(addrDest));
	//result = sendto(sock, msg, msg_length, 0, (sockaddr*)&addrDest2, sizeof(addrDest2));

	//std::cout << result << " bytes sent" << std::endl;
	return result;
}

void SocketClose() {
	closesocket(sock);
}