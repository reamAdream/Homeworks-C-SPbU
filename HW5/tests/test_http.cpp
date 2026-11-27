#include "http.hpp"

#include <cassert>
#include <iostream>
#include <string>

int main() {
	std::string path;

	//ПРОВЕРЯЕМ КОНКРЕТНЫЙ ЗАПРОС
	assert(parseRequestLine("GET /about.html HTTP/1.1", path));
	assert(path == "/about.html");

	//ПРОВЕРЯЕМ НЕКОРРЕКТНЫЕ ЗАПРОСЫ
	assert(!parseRequestLine("",path));
	assert(!parseRequestLine("GET /about.html", path));
	assert(!parseRequestLine("POST /about.html HTTP/1.1", path));
	assert(!parseRequestLine("GET about.html HTTP/1.1", path));
	assert(!parseRequestLine("GET /about.html HTTP/1.1 extra", path));
		
	assert(makeRelativePath("/") == "index.html");
	assert(makeRelativePath("/about.html") == "about.html");
	assert(makeRelativePath("/about.html?version=2") == "about.html");
	assert(makeRelativePath("/?version=2") == "index.html");

	//ПРОВЕРЯЕМ ГРАНИЦЫ КОРНЕВОЙ ПАПКИ
	const std::filesystem::path root = "C:/test/www";
	assert(isPathInsideRoot(root, "C:/test/www/about.html"));
	assert(isPathInsideRoot(root, "C:/test/www/folder/page.html"));
	assert(!isPathInsideRoot(root, "C:/test/secret.txt"));
	assert(!isPathInsideRoot(root, "C:/test/www2/secret.txt"));

	//ПРОВЕРЯЕМ СБОРКУ HTTP-ОТВЕТА
	const std::string body = "Hello";
	const std::string response = buildResponse("HTTP/1.1 200 OK\r\n", body, "text/html; charset=utf-8");

	assert(response.find("HTTP/1.1 200 OK\r\n") == 0);
	assert(response.find("Content-Length: 5\r\n") != std::string::npos);
	assert(response.find("\r\n\r\nHello") != std::string::npos);


	//ПРОВЕРЯЕМ ОТВЕТ С ПУСТЫМ ТЕЛОМ
	const std::string emptyResponse = buildResponse("HTTP/1.1 200 OK\r\n", "", "text/html; charset=utf-8");
	assert(emptyResponse.find("Content-Length: 0\r\n") != std::string::npos);
	assert(emptyResponse.substr(emptyResponse.size()-4) == "\r\n\r\n");

	//ПРОВЕРЯЕМ ТЕЛО С НУЛЕВЫМ БАЙТОМ
	const std::string binaryBody("A\0B", 3);
	const std::string binaryResponse = buildResponse("HTTP/1.1 200 OK\r\n", binaryBody, "application/octet-stream");
	assert(binaryResponse.find("Content-Type: application/octet-stream\r\n") != std::string::npos);
	assert(binaryResponse.find("Content-Length: 3\r\n") != std::string::npos);
	assert(binaryResponse.substr(binaryResponse.size() - 3) == binaryBody);	

	std::cout << "Success!\n";
	return 0;
}