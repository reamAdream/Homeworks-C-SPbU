#include "../Include/dense_matrix.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

int main() {
	int sizes[] = { 200,1000 };

	for (int size : sizes) {
		std::mt19937 gen(42);
		std::uniform_real_distribution<double> distA(-10.0, 10.0);
		DenseMatrix A(size, size);
		for (int i = 0; i < A.rows(); i++) {
			for (int j = 0; j < A.cols(); j++) {
				A.set(i, j, distA(gen));
			}
		}
		std::cout << std::setw(45) << "Matrix Size: " << size << "\n";

		std::uniform_real_distribution<double> distB(-10.0, 10.0);
		DenseMatrix B(size, size);
		for (int i = 0; i < A.rows(); i++) {
			for (int j = 0; j < A.cols(); j++) {
				B.set(i, j, distB(gen));
			}
		}

		A.toFile(HW4_DATA_DIR "/benchA");
		B.toFile(HW4_DATA_DIR "/benchB");
		
		DenseMatrix newA = DenseMatrix::fromFile(HW4_DATA_DIR "/benchA");
		DenseMatrix newB = DenseMatrix::fromFile(HW4_DATA_DIR "/benchB");

		std::cout << std::setw(14) << "Thread "
			<< std::setw(20) << "OneThreadMul (ms)"
			<< std::setw(14) << "mul (ms)"
			<< std::setw(12) << "Speedup"
			<< std::setw(14) << "Efficiency";
			if (size == 200) {
				std::cout << std::setw(20) << "FrequentLock (ms)"
					<< std::setw(14) << "Slowdown";
			}
			std::cout << '\n';
			

		const auto optimizeStart = std::chrono::steady_clock::now();
		auto optimizeResult = newA.mul(newB, 1);
		const auto optimizeEnd = std::chrono::steady_clock::now();
		const double optimizeTime = std::chrono::duration<double, std::milli>(optimizeEnd - optimizeStart).count();


		int threadsCount[] = { 1,2,4,8 };
		for (int i : threadsCount) {

			const auto threadsStart = std::chrono::steady_clock::now();
			auto threadsResult = newA.mul(newB, i);
			const auto threadsEnd = std::chrono::steady_clock::now();
			const double threadsTime = std::chrono::duration<double, std::milli>(threadsEnd - threadsStart).count();

			const double Diff = optimizeTime / threadsTime;
			const double efficiency = Diff / i;

			std::cout << std::setw(14) << i
				<< std::setw(20) << optimizeTime
				<< std::setw(14) << threadsTime
				<< std::setw(12) << Diff
				<< std::setw(14) << efficiency;
			
			if (size == 200) {
				const auto frequentStart = std::chrono::steady_clock::now();
				auto frequentResult = newA.mulFrequenceLock(newB, i);
				const auto frequentEnd = std::chrono::steady_clock::now();

				const double frequentTime =
					std::chrono::duration<double, std::milli>(
						frequentEnd - frequentStart).count();

				const double slowdown = frequentTime / threadsTime;

				std::cout << std::setw(20) << frequentTime
					<< std::setw(14) << slowdown;
			}
			std::cout << '\n';
		}
		std::cout << "\n";
	}
	return 0;
}