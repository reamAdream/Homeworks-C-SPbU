#pragma once

#include <vector>
#include "matrix.hpp"
#include <cstddef>

//Публично наследуем от базового класса матрицы
class DenseMatrix : public Matrix {
private:
	std::size_t index(int i, int j) const;
	int rows_;
	int cols_;
	//для хранения элементов матрицы
	std::vector<double> data_;

public:
	//конструктор
	DenseMatrix(int rows, int cols);
	//Наивное и оптимизированное перемножение матриц
	DenseMatrix mulNaive(const DenseMatrix& other) const;
	DenseMatrix mulOptimize(const DenseMatrix& other) const;

	double at(int i, int j) const;
	void set(int i, int j, double value);
	int rows() const override;
	int cols() const override;
	std::unique_ptr<Matrix> mul(const Matrix& other) const override;
};