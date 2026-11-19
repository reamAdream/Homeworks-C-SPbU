#include <cassert>
#include "../Include/dense_matrix.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <random>

//Функция сравнения матриц
void assertMatrixNear(const DenseMatrix& A, const DenseMatrix& B) {
	assert(A.rows() == B.rows());
	assert(A.cols() == B.cols());
	for (int i = 0; i < A.rows(); i++) {
		for (int j = 0; j < A.cols(); j++) {
			assert(std::abs(A.at(i, j) - B.at(i, j)) < 1e-9);
		}
	}
}

//Перемножение рандомно сгенерированных матриц заданных размеров
void TestRandomMul(int m, int n, int p, int threadCount) {
	std::mt19937 gen(42);
	std::uniform_real_distribution<double> dist(-10.0, 10.0);
	DenseMatrix A(m, n);
	for (int i = 0; i < A.rows(); i++) {
		for (int j = 0; j < A.cols(); j++) {
			A.set(i, j, dist(gen));
		}
	}

	DenseMatrix B(n, p);
	for (int i = 0; i < B.rows(); i++) {
		for (int j = 0; j < B.cols(); j++) {
			B.set(i, j, dist(gen));
		}
	}
	std::unique_ptr<Matrix> result = A.mul(B, threadCount);
	const DenseMatrix* C = dynamic_cast<const DenseMatrix*>(result.get());
	assert(C != nullptr);

	DenseMatrix D = A.mulOptimize(B);
	assert(C -> rows() == m);
	assert(D.rows() == m);
	assert(C -> cols() == p);
	assert(D.cols() == p);

	assertMatrixNear(*C, D);

	std::unique_ptr<Matrix> frequentResult = A.mulFrequenceLock(B, threadCount);
	const DenseMatrix* frequent = dynamic_cast<const DenseMatrix*>(frequentResult.get());
	
	assert(frequent != nullptr);
	assertMatrixNear(*frequent,D);
}

//Проверка корректности импорта матриц из файлов и корректности их перемножения
void TestImportMatrixMul() {
	DenseMatrix A = DenseMatrix::fromFile(HW4_DATA_DIR "/matrix_a_TEST.txt");
	DenseMatrix B = DenseMatrix::fromFile(HW4_DATA_DIR "/matrix_b_TEST.txt");
	assert(A.at(1, 1) == 5.0);
	assert(B.at(1, 1) == 10.0);
	
	std::unique_ptr<Matrix> result = A.mul(B, 4);
	const DenseMatrix* C = dynamic_cast<const DenseMatrix*>(result.get());
	assert(C != nullptr);

	assert(C ->rows() == 2); 
	assert(C -> cols() == 2);
	assert(C -> at(0, 0) == 58.0);
	assert(C -> at(0, 1) == 64.0);
	assert(C -> at(1, 0) == 139.0);
	assert(C -> at(1, 1) == 154.0);
}

//Тест сохранения и чтения матрицы
void TestSaveAndLoadMatrix() {
	DenseMatrix A(2, 3);
	A.set(0, 0, 1.23456789);
	A.set(1, 1, -2.987654321);
	A.toFile(HW4_DATA_DIR "/example.txt");
	DenseMatrix B = DenseMatrix::fromFile(HW4_DATA_DIR "/example.txt");
	assert(A.rows() == B.rows());
	assert(A.cols() == B.cols());
	for (int i = 0; i < A.rows(); i++) {
		for (int j = 0; j < A.cols(); j++) {
			assert(A.at(i, j) == B.at(i, j));
		}
	}
}

int main() {
	//Проверка корректности параллельного перемножения случайных матриц
	TestRandomMul(1, 1, 1,1);
	TestRandomMul(2, 3, 5,2);
	TestRandomMul(7, 4, 2,4);
	TestRandomMul(30, 30, 30,8);
	TestRandomMul(7, 4, 2, 4);

	//Проверка корректности импорта матриц из файлов и корректности их перемножения
	TestImportMatrixMul();

	//Проверка корректности сохранения матрицы в файл и чтения её из него же
	TestSaveAndLoadMatrix();

	std::cout << "Success!";
	return 0;
}