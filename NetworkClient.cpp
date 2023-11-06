#include "NetworkClient.h"

RaspberryPi_NetworkClient::RaspberryPi_NetworkClient(const char* ipAddr, int port) {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cerr << "Failed to initialize Winsock, " << endl;
		return;
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Failed to create socket." << endl;
		WSACleanup();
		return;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ipAddr, &(serverAddr.sin_addr));
	cout << "Success to connect network. " << endl;
}

RaspberryPi_NetworkClient::~RaspberryPi_NetworkClient() {
	closesocket(clientSocket);
	WSACleanup();
}

bool RaspberryPi_NetworkClient::send_signal(const char* data) {
	int result = send(clientSocket, data, strlen(data), 0);
	if (result == SOCKET_ERROR) {
		//cerr << "Failed to send data." << endl;
		return false;
	}
	return true;
}