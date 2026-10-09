#include "../Include/my_sort.hpp"
#include "../Include/point.hpp"
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

int main() {
	std::mt19937 gen(42);
	std::uniform_int_distribution<int> dist(-1000, 1000);

	//ПЕРВАЯ СТАДИЯ: РАНДОМНЫЕ МАССИВЫ ЦЕЛЫХ ЧИСЕЛ РАМЗЕРАМИ В 1000,10000,100000,1000000
	//ДЕЛАЕМ 5 ИЗМЕРЕНИЙ, ЧТОБЫ ВЫДАТЬ СРЕДНЕЕ ВРЕМЯ (ОНО ТОЧНЕЕ)
	std::cout << "4 SIZES RANDOM INT:\n";
	for (int n: { 1000, 10000, 100000, 1000000 }) {
		std::vector<int> original;
		for (int i = 0; i < n; ++i) { original.push_back(dist(gen)); };
		
		//Переменные для суммы замеров времени
		double myBaseTotal = 0.0;
		double stdBaseTotal = 0.0;
		for (int k = 0; k < 5; ++k) {
			std::vector<int> myBase = original;
			std::vector<int> stdBase = original;
			//Замерим время нашей сортировки в милисекундах
			auto myStart = std::chrono::steady_clock::now();
			mySort(myBase.begin(), myBase.end());
			auto myFinish = std::chrono::steady_clock::now();
			double myTime = std::chrono::duration<double, std::milli>(
				myFinish - myStart
			).count();
			//Обнаружение неправильных результатов
			if (!std::is_sorted(myBase.begin(), myBase.end())) {
				std::cerr << "mySort failed!\n";
				return 1;
			}

			//Замерим время std::sort в милисекундах
			auto stdStart = std::chrono::steady_clock::now();
			std::sort(stdBase.begin(), stdBase.end());
			auto stdFinish = std::chrono::steady_clock::now();
			double stdTime = std::chrono::duration<double, std::milli>(
				stdFinish - stdStart
			).count();
			//Обнаружение неправильных результатов
			if (!std::is_sorted(stdBase.begin(), stdBase.end())) {
				std::cerr << "std::sorted failed!\n";
				return 1;
			}
			myBaseTotal = myBaseTotal + myTime;
			stdBaseTotal = stdBaseTotal + stdTime;
		}
		std::cout << "For n = " << n << "\n";
		std::cout << "mySort average time: " << myBaseTotal/5 << " ms\n";
		std::cout << "std::sort average time " << stdBaseTotal/5 << " ms\n";
		std::cout << "Ratio: " << myBaseTotal / stdBaseTotal << "\n\n";
	}


	//ВТОРАЯ СТАДИЯ: УЖЕ ОТСОРТИРОВАННЫЙ МАССИВ РАЗМЕРОВ В 100000 
	std::cout << "\nALREADY SORTED INT:\n";
	std::vector<int> original2;
	for (int i = 0; i < 100000; ++i) { original2.push_back(i); };

	double mysortedTotal = 0.0;
	double stdsortedTotal = 0.0;

	for (int k = 0; k < 5; ++k) {
		std::vector<int> stdsortedBase = original2;
		std::vector<int> sortedBase = original2;

		auto mysortedStart = std::chrono::steady_clock::now();
		mySort(sortedBase.begin(), sortedBase.end());
		auto mysortedFinish = std::chrono::steady_clock::now();
		double sortedTime = std::chrono::duration<double, std::milli>(
			mysortedFinish - mysortedStart
		).count();
		//Обнаружение неправильных результатов
		if (!std::is_sorted(sortedBase.begin(), sortedBase.end())) {
			std::cerr << "mySort failed!\n";
			return 1;
		}

		auto stdsortedStart = std::chrono::steady_clock::now();
		std::sort(stdsortedBase.begin(), stdsortedBase.end());
		auto stdsortedFinish = std::chrono::steady_clock::now();
		double stdsortedTime = std::chrono::duration<double, std::milli>(
			stdsortedFinish - stdsortedStart
		).count();
		//Обнаружение неправильных результатов
		if (!std::is_sorted(stdsortedBase.begin(), stdsortedBase.end())) {
			std::cerr << "std::sort failed!\n";
			return 1;
		}
		mysortedTotal = mysortedTotal + sortedTime;
		stdsortedTotal = stdsortedTotal + stdsortedTime;
	}
	std::cout << "mySort average time: " << mysortedTotal/5 << " ms\n";
	std::cout << "std::sort average time " << stdsortedTotal/5 << " ms\n";
	std::cout << "Ratio: " << mysortedTotal / stdsortedTotal << "\n\n";


	std::cout << "\n4 SIZES RANDOM POINTS:\n";
	std::uniform_real_distribution<double> dist1(-1000.0, 1000.0);
	auto pointComp = [](const Point& a, const Point& b) {return a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y; };
	//ТРЕТЬЯ СТАДИЯ: РАНДОМНЫЕ МАССИВЫ ТОЧЕК РАЗМЕРАМИ В 1000,10000,100000,1000000
	for (int n : { 1000, 10000, 100000, 1000000 }) {
		std::vector<Point> original3;
		for (int i = 0; i < n; ++i) { original3.push_back({ dist1(gen), dist1(gen) }); };

		double myPointTotal = 0.0;
		double stdPointTotal = 0.0;

		for (int k = 0; k < 5; ++k) {
			std::vector<Point> stdpointBase = original3;
			std::vector<Point> pointBase = original3;
			//Замерим время нашей сортировки в милисекундах
			auto pointStart = std::chrono::steady_clock::now();
			mySort(pointBase.begin(), pointBase.end(), pointComp);
			auto pointFinish = std::chrono::steady_clock::now();
			double pointTime = std::chrono::duration<double, std::milli>(
				pointFinish - pointStart
			).count();
			//Обнаружение неправильных результатов
			if (!std::is_sorted(pointBase.begin(), pointBase.end(), pointComp)) {
				std::cerr << "mySort failed!\n";
				return 1;
			}

			//Замерим время std::sort в милисекундах
			auto stdpointStart = std::chrono::steady_clock::now();
			std::sort(stdpointBase.begin(), stdpointBase.end(), pointComp);
			auto stdpointFinish = std::chrono::steady_clock::now();
			double stdpointTime = std::chrono::duration<double, std::milli>(
				stdpointFinish - stdpointStart
			).count();
			//Обнаружение неправильных результатов
			if (!std::is_sorted(stdpointBase.begin(), stdpointBase.end(), pointComp)) {
				std::cerr << "std::sort failed!\n";
				return 1;
			}
			myPointTotal = myPointTotal + pointTime;
			stdPointTotal = stdPointTotal + stdpointTime;
		}
		std::cout << "For n = " << n << "\n";
		std::cout << "mySort average time: " << myPointTotal/5 << " ms\n";
		std::cout << "std::sort average time " << stdPointTotal/5 << " ms\n";
		std::cout << "Ratio: " << myPointTotal / stdPointTotal << "\n\n";
	}
}