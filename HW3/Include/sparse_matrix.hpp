#pragma once

#include <map>
#include <utility>
#include "matrix.hpp"
#include "dense_matrix.hpp"

class SparseMatrix : public Matrix {
private:
	int rows_;
	int cols_;
	//хранение данных в словаре
	std::map<std::pair<int, int>, double> data_;

public:
	//конструктор
	SparseMatrix(int rows, int cols);

	//перемножение матриц
	//Виртуальный метод
	std::unique_ptr<Matrix> mul(const Matrix& other) const override;
	//Sparse x Sparse
	SparseMatrix mulSparse(const SparseMatrix& other) const;
	//Sparse x Dense
	DenseMatrix mulDense(const DenseMatrix& other) const;
	//Dense x Sparse
	DenseMatrix mulLeftDense(const DenseMatrix& left) const;

	int rows() const override;
	int cols() const override;
	double at(int i, int j) const;
	void set(int i, int j, double value);
};