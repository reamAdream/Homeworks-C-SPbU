#include "../Include/sparse_matrix.hpp"
#include "../Include/dense_matrix.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

int main() {
	std::cout << std::setw(14) << "Density (%)"
		<< std::setw(16) << "Dense (ms)"
		<< std::setw(16) << "Sparse (ms)"
		<< std::setw(12) << "Speedup" << "\n";

	double sumDense = 0.0;
	double sumSparse = 0.0;
	
	const int n = 200;
	const double densities[] = { 0.5, 0.1, 0.01, 0.001 };
	std::mt19937 gen(42);
	std::uniform_real_distribution<double> number(-10.0, 10.0);

	//Перебор процента ненулых элементов
	for (double density : densities) {
		std::bernoulli_distribution nonzero(density);
		//Заполнение матриц A
		DenseMatrix Ad(n, n);
		SparseMatrix As(n, n);
		for (int i = 0; i < Ad.rows(); i++) {
			for (int j = 0; j < Ad.cols(); j++) {
				if (nonzero(gen)) {
					const double valueA = number(gen);
					Ad.set(i, j, valueA);
					As.set(i, j, valueA);
				}
			}
		}
		//Заполнение матриц B
		DenseMatrix Bd(n, n);
		SparseMatrix Bs(n, n);
		for (int i = 0; i < Bd.rows();i++) {
			for (int j = 0; j < Bd.cols();j++) {
				if (nonzero(gen)) {
					const double valueB = number(gen);
					Bd.set(i, j, valueB);
					Bs.set(i, j, valueB);
				}
			}
		}

		//Замер Dense x Dense
		const auto denseStart = std::chrono::steady_clock::now();
		auto denseResult = Ad.mul(Bd);
		const auto denseEnd = std::chrono::steady_clock::now();
		const double denseTime = std::chrono::duration<double, std::milli>(denseEnd - denseStart).count();

		//Замер Sparse x Sparse
		const auto sparseStart = std::chrono::steady_clock::now();
		auto sparseResult = As.mul(Bs);
		const auto sparseEnd = std::chrono::steady_clock::now();
		const double sparseTime = std::chrono::duration<double, std::milli>(sparseEnd - sparseStart).count();

		// Считаем контрольные суммы обоих результатов вне замеров времени
		const auto* dense = dynamic_cast<const DenseMatrix*>(denseResult.get());
		const auto* sparse = dynamic_cast<const SparseMatrix*>(sparseResult.get());
		for (int i = 0; i < denseResult->rows(); i++) {
			for (int j = 0; j < denseResult->cols(); j++) {
				if (dense != nullptr && sparse != nullptr) {
					sumDense += dense->at(i, j);
					sumSparse += sparse->at(i, j);
				}
			}
		}

		const double speedup = denseTime / sparseTime;

		std::cout << std::fixed << std::setprecision(1)
			<< std::setw(14) << density * 100.0
			<< std::setprecision(4)
			<< std::setw(16) << denseTime
			<< std::setw(16) << sparseTime
			<< std::setprecision(2)
			<< std::setw(12) << speedup << "\n";
	}
	std::cout << std::setprecision(6)
		<< "\nChecksum Dense x Dense: " << sumDense
		<< "\nChecksum Sparse x Sparse: " << sumSparse << '\n';
}