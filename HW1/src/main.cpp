#include <iostream>
#include <vector>
#include <iterator>
#include <functional>
#include "../Include/my_sort.hpp"
#include "../Include/point.hpp"

//Отсортируем для примера
int main() {
	std::vector<Point> points = { {3,4}, {1,2}, {0,1} };
	//компаратор для точек (сортировка по возрастанию расстояния до начала координат (0;0) )
	auto pointComp = [](const Point& a, const Point& b) { return a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y; };
	std::cout << "Before:" << " ";
	for (int k = 0; k < points.size(); ++k) { std::cout << "(" << points[k].x << "," << points[k].y << ") "; }
	mySort(points.begin(), points.end(), pointComp);
	std::cout << "\nAfter:" << " ";
	for (int k = 0; k < points.size(); ++k) { std::cout << "(" << points[k].x << "," << points[k].y << ") "; }

	//компаратор для целых чисел (сортировка по убыванию)
	auto comp = [](int a, int b) { return a > b; };
	std::vector<int> a = { 9, -3, 0, 9, -10, 5, -3 };
	std::cout << "\nBefore:" << " ";
	for (int k = 0; k < a.size(); ++k) { std::cout << a[k] << " "; }
	mySort(a.begin(), a.end());
	std::cout << "\nAfter:" << " ";
	for (int k = 0; k < a.size(); ++k) { std::cout << a[k] << " "; }
}