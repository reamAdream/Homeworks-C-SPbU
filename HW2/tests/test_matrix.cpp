#include <cassert>
#include "../Include/dense_matrix.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <random>


// Проверяет, что при недопустимых размерах конструктор выбрасывает std::invalid_argument.
void testInvalidSize(int rows, int cols) {
	bool check = false;
	try { DenseMatrix A(rows, cols); }
	catch (const std::invalid_argument&) { check = true; }
	assert(check);
}

// Проверяет, что at() и set() выбрасывают std::out_of_range при недопустимых индексах.
void testInvalidIndex(int i, int j) {
	DenseMatrix A(3, 4);
	bool checkAt = false;
	try { A.at(i, j); }
	catch (const std::out_of_range&) { checkAt = true; }
	assert(checkAt);

	bool checkSet = false;
	try { A.set(i, j, 1.0); }
	catch (const std::out_of_range&) { checkSet = true; }
	assert(checkSet);
}

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

//Функция для заполнения проверки корректности наивного и оптимизированного перемножений матриц
void TestMulExample() {
	DenseMatrix A(2, 3);
	int value = 1;
	for (int i = 0; i < A.rows(); i++) {
		for (int j = 0; j < A.cols(); j++) {
			A.set(i, j, value);
			value++;
		}
	}

	DenseMatrix B(3, 2);
	value = 7;
	for (int i = 0; i < B.rows(); i++) {
		for (int j = 0; j < B.cols(); j++) {
			B.set(i, j, value);
			value++;
		}
	}
	DenseMatrix C = A.mulNaive(B);
	assert(C.rows() == A.rows());
	assert(C.cols() == B.cols());
	assert(C.at(0,0) == 58);
	assert(C.at(0,1) == 64);
	assert(C.at(1,0) == 139);
	assert(C.at(1,1) == 154);
	DenseMatrix D = A.mulOptimize(B);
	assertMatrixNear(D, C);
	const Matrix& left = A;
	
	std::unique_ptr<Matrix> result = left.mul(B);
	assert(result != nullptr);
	const DenseMatrix* denseResult = dynamic_cast<const DenseMatrix*> (result.get());
	assert(denseResult != nullptr);
	assertMatrixNear(*denseResult, C);
}

// Проверяет, что mulNaive выбрасывает std::invalid_argument при несовместимых размерах.
void TestIncompatibleMul() {
	DenseMatrix A(2, 4);
	DenseMatrix B(3, 2);
	bool checkNaive = false;
	try {A.mulNaive(B);}
	catch (const std::invalid_argument&) { checkNaive = true;}
	assert(checkNaive);
	
	bool checkOptimize = false;
	try { A.mulOptimize(B); }
	catch (const std::invalid_argument&) { checkOptimize = true; }
	assert(checkOptimize);
}

//Функция умножения матрицы на единичную
void TestIdentityMul() {
	DenseMatrix A(2, 3);
	int value = 1;
	for (int i = 0; i < A.rows(); i++) {
		for (int j = 0; j < A.cols(); j++) {
			A.set(i, j, value);
			value++;
		}
	}
	DenseMatrix identity(3, 3);
	for (int i = 0; i < identity.cols(); i++) {
		identity.set(i, i, 1);
	}
	DenseMatrix C = A.mulNaive(identity);
	DenseMatrix D = A.mulOptimize(identity);
	assertMatrixNear(C, A);
	assertMatrixNear(D, A);
}

//Функция умножения на нулевую матрицу
void TestZeroMul() {
	DenseMatrix Zero(3, 4);
	DenseMatrix A(2, 3);
	int value = 1;
	for (int i = 0; i < A.rows();i++) {
		for (int j = 0; j < A.cols(); j++) {
			A.set(i, j, value);
			value++;
		}
	}
	DenseMatrix C = A.mulNaive(Zero);
	DenseMatrix D = A.mulOptimize(Zero);
	DenseMatrix expected(2, 4);
	assertMatrixNear(expected, C);
	assertMatrixNear(expected, D);
}

void TestRandomMul(int m, int n, int p) {
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
	DenseMatrix C = A.mulNaive(B);
	DenseMatrix D = A.mulOptimize(B);
	assert(C.rows() == m);
	assert(D.rows() == m);
	assert(C.cols() == p);
	assert(D.cols() == p);

	assertMatrixNear(C, D);
}

int main() {
	// Проверка размеров, начального значения элемента и чтения после записи.
	DenseMatrix A(3, 4);
	assert(A.rows() == 3);
	assert(A.cols() == 4);
	assert(A.at(1, 2) == 0.0);
	A.set(1, 2, 7.5);
	assert(A.at(1, 2) == 7.5);

	// Проверка нулевых и отрицательных размеров матрицы.
	testInvalidSize(0, 0);
	testInvalidSize(0, 4);
	testInvalidSize(4, 0);
	testInvalidSize(1, -1);
	testInvalidSize(-1, 1);

	// Проверка отрицательных индексов и выхода за верхнюю границу по каждой координате.
	testInvalidIndex(-1, 0); 
	testInvalidIndex(0, -1); 
	testInvalidIndex(3, 0);  
	testInvalidIndex(0, 4);  

	//Проверка корректности операции наивного и оптимизированного перемножения матриц
	TestMulExample();
	TestIncompatibleMul();

	//Проверка умножения на единичную матрицу
	TestIdentityMul();

	//Проверка умножения на нулевую матрицу
	TestZeroMul();

	//Проверка корректности перемножения случайных матриц
	TestRandomMul(1, 1, 1);
	TestRandomMul(2, 3, 5);
	TestRandomMul(7, 4, 2);
	TestRandomMul(30, 30, 30);

	std::cout << "Success!";
	return 0;
}