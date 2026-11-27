#include "http.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <cstddef>
#include <exception>

//РАЗБИРАЕМ ПЕРВУЮ СТРОКУ ЗАПРОСА
bool parseRequestLine(
	const std::string& firstLine,
	std::string& path
) {
	std::istringstream lineStream(firstLine);
	
	std::string method;
	std::string parsedPath;
	std::string httpVersion;
	std::string extra;

	if (!(lineStream >> method >> parsedPath >> httpVersion)) {
		return false;
	}

	if (method != "GET" || httpVersion != "HTTP/1.1") {
		return false;
	}

	if (parsedPath.empty() || parsedPath[0] != '/') {
		return false;
	}

	if (lineStream >> extra) {
		return false;
	}

	path = parsedPath;
	return true;
}

std::string makeRelativePath(const std::string& path) {
	std::string urlPath = path.substr(0, path.find("?"));

	if (urlPath == "/") {
		urlPath = "/index.html";
	}
	return urlPath.substr(1);
}

//ПРОВЕРЯЕМ, ЧТО ПУТЬ ВНУТРИ КОРНЕВОЙ ПАПКИ
bool isPathInsideRoot(
	const std::filesystem::path& rootPath,
	const std::filesystem::path& fullPath
) {
	const auto pathFromRoot = fullPath.lexically_relative(rootPath);

	if (pathFromRoot.empty()) {
		return false;
	}
	for (const auto& part : pathFromRoot) {
		if (part == "..") {
			return false;
		}
	}
	return true;
}

//СОБИРАЕМ HTTP-ОТВЕТ
std::string buildResponse(
	const std::string& statusLine,
	const std::string& body,
	const std::string& contentType
) {
	std::string response = statusLine;
	response += "Content-Type: " + contentType + "\r\n";
	response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";
	response += body;
	return response;
}

//ОБРАБАТЫВАЕМ ЗАПРОС ОДНОГО КЛИЕНТА
void handleClient(
	SOCKET clientSocket,
	const std::string& rootDirectory
) {
	try {
		//ПОЛУЧАЕМ ЗАГОЛОВКИ HTTP-ЗАПРОСА КЛИЕНТА
		std::string request;
		while (request.find("\r\n\r\n") == std::string::npos) {
			char buffer[4096];
			int received = recv(
				clientSocket,
				buffer,
				static_cast<int>(sizeof(buffer)),
				0
			);
			if (received == SOCKET_ERROR) {
				std::cerr << "Error! Code: " << WSAGetLastError() << "\n";
				closesocket(clientSocket);
				return;
			}
			if (received == 0) {
				std::cout << "Client has stoped send data!";
				closesocket(clientSocket);
				return;
			}

			request.append(buffer, received);
		}

		//ВЫДЕЛЯЕМ ПЕРВУЮ СТРОКУ ЗАПРОСА
		const auto lineEnd = request.find("\r\n");
		std::string firstLine = request.substr(0, lineEnd);

		std::string path;

		if (!parseRequestLine(firstLine, path)) {
			std::cerr << "Invalid request line\n";
			closesocket(clientSocket);
			return;
		}

		std::cout << "First line: " << firstLine << "\n";

		//УБИРАЕМ ПАРАМЕТРЫ URL И ГОТОВИМ ИМЯ ФАЙЛА
		std::string	relativePath = makeRelativePath(path);
		std::cout << "Relative path: " << relativePath << "\n";

		const std::filesystem::path requestedPath(relativePath);
		if (requestedPath.has_root_path()) {
			std::cerr << "Forbidden path: root component is not allowed\n";
			closesocket(clientSocket);
			return;
		}

		//ПОЛУЧАЕМ НОРМАЛИЗОВАННЫЕ ПУТИ К ПАПКЕ И ФАЙЛУ
		std::error_code pathError;
		const auto rootPath = std::filesystem::weakly_canonical(rootDirectory, pathError);
		if (pathError) {
			std::cerr << "Path Error!" << pathError.value() << ":" << pathError.message() << "\n";
			closesocket(clientSocket);
			return;
		}
		const auto fullPath = std::filesystem::weakly_canonical(rootPath / requestedPath, pathError);
		if (pathError) {
			std::cerr << "Path Error!" << pathError.value() << ":" << pathError.message() << "\n";
			closesocket(clientSocket);
			return;
		}

		std::cout << "Root: " << rootPath.string() << "\n";
		std::cout << "Full: " << fullPath.string() << "\n";

		//ПРОВЕРЯЕМ, ЧТО ПУТЬ НЕ ВЫХОДИТ ИЗ КОРНЕВОЙ ПАПКИ
		if (!isPathInsideRoot(rootPath, fullPath)) {
			std::cerr << "Forbidden path: outside root directory";
			closesocket(clientSocket);
			return;
		}

		std::string statusLine = "HTTP/1.1 200 OK\r\n";
		std::string body;
		std::string contentType = "text/html; charset=utf-8";

		//ОТКРЫВАЕМ HTML-ФАЙЛ
		std::ifstream inputFile(fullPath, std::ios::binary);
		if (!inputFile) {
			std::cout << "File can't be opened\n";
			statusLine = "HTTP/1.1 404 NOT FOUND\r\n";
			body = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>404 Not Found | LOOPBACK TAPES</title>
<style>
* { box-sizing: border-box; }
body {
    margin: 0;
    min-height: 100vh;
    display: grid;
    place-items: center;
    padding: 24px;
    background: #151513;
    color: #f4efe4;
    font: 16px system-ui, sans-serif;
    text-align: center;
}
main {
    width: min(100%, 720px);
    padding: clamp(24px, 7vw, 80px) 16px;
    border: 1px solid #48463f;
}
.label { color: #dfff48; font: bold .85rem monospace; letter-spacing: .15em; }
h1 { margin: 20px 0; font-size: clamp(2rem, 10vw, 7rem); line-height: .9; letter-spacing: -.07em; }
h1 span { display: block; color: #dfff48; }
a { display: inline-block; margin-top: 12px; padding: 12px 18px; background: #dfff48; color: #151513; font-weight: 800; text-decoration: none; }
a:hover { background: #f4efe4; }
a:focus-visible { outline: 2px solid #f4efe4; outline-offset: 5px; }
</style>
</head>
<body>
<main>
    <p class="label">HELLAREAM / LOOPBACK TAPES</p>
    <h1><span>404</span>NOT FOUND</h1>
    <a href="/">Back to the cover</a>
</main>
</body>
</html>)HTML";
		}
		else {
			//ЧИТАЕМ ФАЙЛ ПО ЧАСТЯМ В ТЕЛО ОТВЕТА
			if (fullPath.extension() != ".html" && fullPath.extension() != ".htm") {
				contentType = "application/octet-stream";
			}

			char fileBuffer[4096];

			while (inputFile) {
				inputFile.read(fileBuffer, sizeof(fileBuffer));
				const auto byteRead = inputFile.gcount();
				body.append(fileBuffer, byteRead);
			}

			if (inputFile.bad() || !inputFile.eof()) {
				std::cout << "Error of load file!\n";
				closesocket(clientSocket);
				return;
			}
			std::cout << body << "\nBytes: " << body.size() << "\n";
		}

		//СОБИРАЕМ ЗАГОЛОВКИ И ТЕЛО HTTP-ОТВЕТА
		std::string response = buildResponse(statusLine, body, contentType);
		std::cout << response << "\n";

		//ОТПРАВЛЯЕМ HTTP-ОТВЕТ ЦЕЛИКОМ
		std::size_t totalSent = 0;

		while (totalSent < response.size()) {
			const auto remaining = response.size() - totalSent;

			int bytesToSend = 4096;
			if (remaining < 4096) {
				bytesToSend = static_cast<int>(remaining);
			}

			int sent = send(
				clientSocket,
				response.data() + totalSent,
				bytesToSend,
				0
			);

			if (sent <= 0) {
				std::cerr << "Response send failed\n";
				closesocket(clientSocket);
				return;
			}
			totalSent += static_cast<std::size_t>(sent);
		}

		//ЗАКРЫВАЕМ СОКЕТ КЛИЕНТА
		closesocket(clientSocket);
	}
	catch (const std::exception& error) {
		closesocket(clientSocket);
		std::cerr << "Client handler error: " << error.what() << "\n";
	}
	catch (...) {
		closesocket(clientSocket);
		std::cerr << "Unknown client handler error\n";
	}
}