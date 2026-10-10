#include <iostream>
#include <cassert> 
#include <vector>
#include "../Include/my_sort.hpp"
#include "../Include/point.hpp"
#include <algorithm>
#include <random>

int main() {
	//тест для пустого вектора целых чисел
	std::vector<int> empty; 
	mySort(empty.begin(), empty.end());
	assert(empty.empty());

	//тест для произвольного вектора целых чисел
	std::vector<int> a = { 5,5,4,7,3 };
	std::vector<int> expected = a;
	std::sort(expected.begin(), expected.end());
	mySort(a.begin(), a.end());
	assert(std::is_sorted(a.begin(),a.end()));
	assert(a == expected);

	//тест для уже отсортированного вектора целых чисел
	std::vector<int> sorted = { 1, 2, 3, 4, 5 };
	std::vector<int> expected2 = sorted; 
	mySort(sorted.begin(), sorted.end());
	assert(sorted == expected2);

	//тест для вектора с одним целым числом
	std::vector<int> single = { 42 };
	mySort(single.begin(), single.end());
	assert(single[0] == 42);

	//тест для вектора целых чисел, отсортированного в обратном порядке
	std::vector<int> reversed = { 5,4,3,2,1 };
	std::vector<int> expected3 = reversed;
	mySort(reversed.begin(), reversed.end());
	std::sort(expected3.begin(), expected3.end());
	assert(std::is_sorted(reversed.begin(), reversed.end()));
	assert(reversed == expected3);

	//тест для вектора целых чисел с одинаковыми элементами
	std::vector<int> equal = { 7,7,7,7,7 };
	std::vector<int> expected4 = equal;
	mySort(equal.begin(), equal.end());
	std::sort(expected4.begin(), expected4.end());
	assert(std::is_sorted(equal.begin(), equal.end()));
	assert(equal == expected4);

	//тест для вектора целых чисел с отрицательными элементами
	std::vector<int> negative = { -3, 5, -10, 0, -3, 8, -1 };
	std::vector<int> expected5 = negative;
	mySort(negative.begin(), negative.end());
	std::sort(expected5.begin(), expected5.end());
	assert(std::is_sorted(negative.begin(), negative.end()));
	assert(negative == expected5);

	//тест для случайных векторов целых чисел длиной 10,1000 и 100000
	std::mt19937 gen(42);
	std::uniform_int_distribution<int> dist(-1000, 1000);
	for (int n : {10, 1000, 100000}) {
		std::vector<int> randomNumbers;
		for (int i = 0; i < n; ++i) { randomNumbers.push_back(dist(gen));}
		std::vector<int> expected6 = randomNumbers;

		mySort(randomNumbers.begin(), randomNumbers.end());
		std::sort(expected6.begin(), expected6.end());

		assert(std::is_sorted(randomNumbers.begin(), randomNumbers.end()));
		assert(randomNumbers == expected6);
	}

	//тест для вектора точек
	std::vector<Point> points = {
		{3,4},
		{1,2},
		{0,1},
		{6,8},
		{0,5}
	};
	//компаратор для сортировки расстояний точек до (0,0) по возрастанию
	auto pointComp = [](const Point& a, const Point& b) {return a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y; };
	std::vector<Point> expected7 = points;
	mySort(points.begin(), points.end(),pointComp);
	assert(std::is_sorted(points.begin(), points.end(), pointComp));
	//компаратор для сравнения точек (точки равны, если их соответсвующие координаты совпадают)
	auto pointEqual = [](const Point& a, const Point& b) {return a.x == b.x && a.y == b.y; };
	assert(std::is_permutation(
		points.begin(), points.end(),
		expected7.begin(),expected7.end(),
		pointEqual
	));

	//тест для пустого вектора точек
	std::vector<Point> emptyVector;
	mySort(emptyVector.begin(), emptyVector.end(), pointComp);
	assert(emptyVector.empty());

	//тест для вектора точек с одним элементом
	std::vector<Point> singlePoint = { {50,43} };
	mySort(singlePoint.begin(), singlePoint.end(), pointComp);
	assert(singlePoint[0].x == 50 && singlePoint[0].y == 43);
	
	//тест для уже отсортированного вектора точек (есть совпадающие точки)
	std::vector<Point> sortedPoint = {
		{1,2},
		{3,4},
		{5,6},
		{5,6},
		{7,8}
	};
	std::vector<Point> expected8 = sortedPoint;
	mySort(sortedPoint.begin(), sortedPoint.end(), pointComp);
	assert(std::is_sorted(sortedPoint.begin(), sortedPoint.end(), pointComp));
	assert(std::is_permutation(
		sortedPoint.begin(), sortedPoint.end(),
		expected8.begin(), expected8.end(),
		pointEqual
	));

	//тест для отсортированного в обратную сторону вектора точек (есть совпадающие точки)
	std::vector<Point> reversedPoint = {
		{7,8},
		{5,6},
		{5,6},
		{3,4},
		{1,2}
	};
	std::vector<Point> expected9 = reversedPoint;
	mySort(reversedPoint.begin(), reversedPoint.end(), pointComp);
	assert(std::is_sorted(reversedPoint.begin(), reversedPoint.end(), pointComp));
	assert(std::is_permutation(
		reversedPoint.begin(), reversedPoint.end(),
		expected9.begin(), expected9.end(),
		pointEqual
	));

	//тест для вектора точек с одинаковыми элементами 
	std::vector<Point> duplicatePoint = {
		{1,1},
		{1,1},
		{1,1},
		{1,1},
		{1,1}
	};
	std::vector<Point> expected10 = duplicatePoint;
	mySort(duplicatePoint.begin(), duplicatePoint.end(), pointComp);
	assert(std::is_permutation(
		duplicatePoint.begin(), duplicatePoint.end(),
		expected10.begin(), expected10.end(),
		pointEqual
	));

	//компаратор проверки сохранности исходных точек
	auto coordComp = [](const Point& a, const Point& b) {
		if (a.x != b.x) {
			return a.x < b.x;
		}
		return a.y < b.y;
	};

	std::uniform_real_distribution<double> randPoint(-1000.0, 1000.0);
	//тест для векторов точек со случайными элементами размером 10,1000,100000 элементов
	for (int n : {10, 1000, 100000}) {
		std::vector<Point> randomPoint;
		for (int i = 0; i < n; ++i) {
			randomPoint.push_back({ randPoint(gen),randPoint(gen) });
		};
		std::vector<Point> expected11 = randomPoint;
		mySort(randomPoint.begin(), randomPoint.end(), pointComp);
		//Проверка на верность сортировки по расстоянию
		assert(std::is_sorted(randomPoint.begin(), randomPoint.end(), pointComp));
		//Сортируем по координатам, чтобы в дальнейшем сравнить оба вектора
		std::sort(expected11.begin(), expected11.end(), coordComp);
		std::sort(randomPoint.begin(), randomPoint.end(), coordComp);
		assert(std::equal(
			randomPoint.begin(),randomPoint.end(),
			expected11.begin(),expected11.end(),
			pointEqual
		));
	}

	std::cout << "Success!";
}