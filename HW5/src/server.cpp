#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstddef>
#include <filesystem>
#include <system_error>
#include <thread>
#include <vector>
#include <exception>
#include <windows.h>
#include "http.hpp"

int main(int argc, char* argv[]) {
	//ПРОВЕРЯЕМ АРГУМЕНТЫ ЗАПУСКА
	if (argc != 2) {
		std::cout << "Usage: server <root_directory>\n";
		return 1;
	}
	const std::string rootDirectory = argv[1];

	// wsaData{} - ПЕРЕМЕННАЯ, wsaData() - ОБЪЯВЛЕНИЕ ФУНКЦИИ
	WSADATA wsaData{};

	//ЗАПУСКАЕМ WinSock
	WORD version = MAKEWORD(2, 2);
	int error = WSAStartup(version, &wsaData) ;
	if (error != 0) {
		std::cerr << "WinSock can't start! Code: " << error << "\n";
		return 1;
	}
	std::cout << "WinSock started\n";

	//СОЗДАЕМ СОКЕТ
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		std::cerr << "Error! Code: " << WSAGetLastError() << '\n';
		WSACleanup();
		return 1;
	}

	//ГОТОВИМ АДРЕС 127.0.0.1:8080
	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	//ПРИВЯЗЫВАЕМ СОКЕТ К АДРЕСУ И ПОРТУ
	int bindResult = bind(
		listenSocket,
		reinterpret_cast<const sockaddr*>(&address),
		sizeof(address)
	);
	if (bindResult == SOCKET_ERROR) {
		std::cerr << "Error! Code: " << WSAGetLastError() << "\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//ПЕРЕВОДИМ СОКЕТ В СЛУШАЮЩИЙ РЕЖИМ
	int listenResult = listen(listenSocket, SOMAXCONN);
	if (listenResult == SOCKET_ERROR) {
		std::cerr << "Error! Code: " << WSAGetLastError() << "\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//ХРАНИМ ПОТОКИ ОБРАБОТКИ КЛИЕНТОВ
	std::vector<std::thread> workers;

	//ПРИНИМАЕМ ПОДКЛЮЧЕНИЕ КЛИЕНТА
	while (true) {
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Error! Code: " << WSAGetLastError() << "\n";
			break;
		}
		std::cout << "Client connected!\n";

		//УБИРАЕМ ЗАВЕРШИВШИЕСЯ ПОТОКИ
		for (auto it = workers.begin(); it != workers.end();) {
			const auto state = WaitForSingleObject(it->native_handle(), 0);
			if (state == WAIT_OBJECT_0) {
				it->join();
				it = workers.erase(it);
			}
			else {
				if (state == WAIT_FAILED) {
					std::cerr << "Thread check failed: " << GetLastError() << "\n";
				}
				++it;
			}
		}

		try {
			workers.emplace_back(handleClient, clientSocket, rootDirectory);
		}
		catch (const std::exception& threadError) {
			std::cerr << "Can't create thread: " << threadError.what() << "\n";
			closesocket(clientSocket);
			break;
		}
	}

	//ЗАКРЫВАЕМ ПРОСЛУШИВАЮЩИЙ СОКЕТ
	closesocket(listenSocket);

	//ДОЖИДАЕМСЯ ЗАВЕРШЕНИЯ РАБОТНИКОВ
	for (auto& worker : workers){
		worker.join();
	}

	//ЗАКРЫВАЕМ WinSock
	WSACleanup();
	return 1;
}