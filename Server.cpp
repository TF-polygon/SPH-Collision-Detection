#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <wiringPi.h>

#define LED_PIN 25

int main(int argc, char* argv[]) {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		printf("Failed to create socket\n");
		return -1;
	}
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(60010);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		printf("Bind failed: %s\n", strerror(errno));
		close(serverSocket);
		return -1;
	}

	if (listen(serverSocket, 5) == -1) {
		printf("Listen failed\n");
		close(serverSocket);
		return -1;
	}
	printf("Server is listening on port 61000...\n");
	sockaddr_in clientAddress;
	socklen_t clientAddrSize = sizeof(clientAddress);
	int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrSize);
	if (clientSocket == -1) {
		printf("Accept failed\n");
		close(serverSocket);
		return -1;
	}
	printf(":: Success to accept Windows Visual C++ SPH Simulator ::\n");

	// GPIO Extension Board part

	wiringPiSetup();
	pinMode(LED_PIN, OUTPUT);
	int sum = 0, N = 0;
	char buffer[2];
	int bytesRead = 0;
	while (1) {
		memset(buffer, 0, sizeof(buffer));
		bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		sum += buffer[0] - 48; N++;
		if (N <= 0) {
			printf("Receive failed\n");
		}
		else {
			if (N >= 100) {
				if (sum >= 1) {
					digitalWrite(LED_PIN, HIGH);
				}
				else {
					digitalWrite(LED_PIN, LOW);
				}
				sum = 0;
				N %= 100;
			}
		}
	}
	close(clientSocket);
	close(serverSocket);

	return 0;
}