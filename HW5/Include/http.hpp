#pragma once

#include <winsock2.h>
#include <string>
#include <filesystem>

//ОБРАБАТЫВАЕМ ЗАПРОС ОДНОГО КЛИЕНТА
void handleClient(SOCKET clientSocket, const std::string& rootDirectory);

//РАЗБИРАЕМ ПЕРВУЮ СТРОКУ ЗАПРОСА
bool parseRequestLine(
	const std::string& firstLine,
	std::string& path
);

//ГОТОВИМ ОТНОСИТЕЛЬНЫЙ ПУТЬ К ФАЙЛУ
std::string makeRelativePath(const std::string& path);

//ПРОВЕРЯЕМ, ЧТО ПУТЬ ВНУТРИ КОРНЕВОЙ ПАПКИ
bool isPathInsideRoot(
	const std::filesystem::path& rootPath,
	const std::filesystem::path& fullPath
);

//СОБИРАЕМ HTTP-ОТВЕТ
std::string buildResponse(
	const std::string& statusLine,
	const std::string& body,
	const std::string& contentType
);