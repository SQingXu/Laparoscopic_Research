#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

int setupSocket(const char* ip, const char* port);

int sendData(const char* msg);

void SocketClose();