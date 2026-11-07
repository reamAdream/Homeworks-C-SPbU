#include <random>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "../Include/dense_matrix.hpp"
#include <windows.h>

int main() {
	SetConsoleOutputCP(CP_UTF8);
	//Создаём случайные матрицы размерами 100х100, 200х200, 500х500 и 1000х1000
	const int sizes[] = { 100,200,500,1000 };
	double checksumNaive = 0.0;
	double checksumOptimize = 0.0;

	std::cout << std::setw(8) << "Size"
		<< std::setw(16) << "Naive (ms)"
		<< std::setw(20) << "Optimized (ms)"
		<< std::setw(12) << "Speedup" << '\n';
	
	for (int n : sizes) {
		DenseMatrix A(n, n);
		std::mt19937 gen(42);
		std::uniform_real_distribution<double> distA(-10.0, 10.0);
		for (int i = 0; i < A.rows(); i++) {
			for (int j = 0; j < A.cols(); j++) {
				A.set(i, j, distA(gen));
			}
		}
		DenseMatrix B(n, n);
		std::uniform_real_distribution<double> distB(-10.0, 10.0);
		for (int i = 0; i < B.rows(); i++) {
			for (int j = 0; j < B.cols(); j++) {
				B.set(i, j, distB(gen));
			}
		}

		//Замер скорости оптимизированного перемножения
		auto startOptimizeTime = std::chrono::steady_clock::now();
		DenseMatrix D = A.mulOptimize(B);
		auto finishOptimizeTime = std::chrono::steady_clock::now();
		double TimeOptimize = std::chrono::duration<double, std::milli>(finishOptimizeTime - startOptimizeTime).count();

		//Замер скорости наивного перемножения
		auto startNaiveTime = std::chrono::steady_clock::now();
		DenseMatrix C = A.mulNaive(B);
		auto finishNaiveTime = std::chrono::steady_clock::now();
		double TimeNaive = std::chrono::duration<double, std::milli>(finishNaiveTime - startNaiveTime).count();

		// Считаем контрольные суммы обоих результатов вне замеров времени.
		for (int i = 0; i < C.rows(); i++) {
			for (int j = 0; j < C.cols(); j++) {
				checksumNaive += C.at(i, j);
				checksumOptimize += D.at(i, j);
			}
		}

		const double speedup = TimeNaive / TimeOptimize;
		std::cout << std::fixed
			<< std::setw(8) << n
			<< std::setprecision(4)
			<< std::setw(16) << TimeNaive
			<< std::setw(20) << TimeOptimize
			<< std::setprecision(2)
			<< std::setw(12) << speedup << '\n';
	}

	std::cout << std::setprecision(6)
		<< "\nChecksum naive:     " << checksumNaive
		<< "\nChecksum optimized: " << checksumOptimize << '\n';

	return 0;
}