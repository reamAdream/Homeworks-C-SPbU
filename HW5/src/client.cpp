#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <cstddef>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Usage: client <host> <port> <path>\n";
		return 1;
	}
	const std::string host = argv[1];
	const std::string port = argv[2];
	const std::string path = argv[3];
	
	// wsaData{} - ПЕРЕМЕННАЯ, wsaData() - ОБЪЯВЛЕНИЕ ФУНКЦИИ
	WSADATA wsaData{};

	//ЗАПУСКАЕМ WinSock
	WORD version = MAKEWORD(2, 2);
	int error = WSAStartup(version, &wsaData);
	if (error != 0) {
		std::cerr << "WinSock can't start! Code: " << error << "\n";
		return 1;
	}
	std::cout << "WinSock started\n";

	//ПОЛУЧАЕМ АДРЕС СЕРВЕРА
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* addresses = nullptr;

	int	addressError = getaddrinfo(
		host.c_str(),
		port.c_str(),
		&hints,
		&addresses
	);

	if (addressError != 0) {
		std::cerr << "Address lookup failed: " << addressError << "\n";
		WSACleanup();
		return 1;
	}

	//ПРОБУЕМ ПОДКЛЮЧИТЬСЯ К СЕРВЕРУ
	SOCKET connectSocket = INVALID_SOCKET;

	for (addrinfo* current = addresses; current != nullptr; current = current->ai_next) {
		connectSocket = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
		if (connectSocket == INVALID_SOCKET) {
			continue;
		}

		int connectResult = connect(connectSocket, current->ai_addr, static_cast<int>(current->ai_addrlen));
		if (connectResult == 0) {
			break;
		}
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(addresses);
	
	if (connectSocket == INVALID_SOCKET) {
		std::cerr << "Connection failed.\n";
		WSACleanup();
		return 1;
	}
	
	std::cout << "Connected!\n";

	//СОБИРАЕМ HTTP-ЗАПРОС
	std::string request = "GET " + path + " HTTP/1.1\r\n";
	request += "Host: " + host + ":" + port + "\r\n";
	request += "Connection: close\r\n";
	request += "\r\n";

	//ОТПРАВЛЯЕМ HTTP-ЗАПРОС ЦЕЛИКОМ
	std::size_t totalSent = 0;

	while (totalSent < request.size()) {
		const auto remaining = request.size() - totalSent;

		int bytesToSend = 4096;
		if (remaining < 4096) {
			bytesToSend = static_cast<int>(remaining);
		}

		int sent = send(
			connectSocket,
			request.data() + totalSent,
			bytesToSend,
			0
		);

		if (sent <= 0) {
			std::cerr << "Request send failed\n";
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
		totalSent += static_cast<std::size_t>(sent);
	}

	//ПОЛУЧАЕМ И ВЫВОДИМ HTTP-ОТВЕТ
	while (true) {
		char buffer[4096];
		int received = recv(
			connectSocket, buffer, static_cast<int>(sizeof(buffer)), 0
		);
		if (received > 0) {
			std::cout.write(buffer, received);
		}
		if (received == 0) {
			break;
		}
		if (received == SOCKET_ERROR) {
			std::cerr << "Socket error!\n";
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
	}
	closesocket(connectSocket);
	WSACleanup();
	return 0;
}