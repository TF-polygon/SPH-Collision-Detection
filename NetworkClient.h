#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Core.h"

#pragma comment(lib, "ws2_32")

class RaspberryPi_NetworkClient {
public:
	RaspberryPi_NetworkClient(const char *ipAddr, int port);
	~RaspberryPi_NetworkClient();

public:
	bool send_signal(const char* data);

private:
	WSADATA			wsaData;
	SOCKET			clientSocket;
	sockaddr_in		serverAddr;
};

typedef RaspberryPi_NetworkClient Network;

