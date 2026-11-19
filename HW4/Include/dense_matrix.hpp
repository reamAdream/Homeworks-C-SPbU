#pragma once

#include <vector>
#include "matrix.hpp"
#include <cstddef>
#include <string>

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
	//чтение матрицы из файла
	static DenseMatrix fromFile(const std::string& path);
	//генерация файла с матрицей
	void toFile(const std::string& path) const;
	//Наивное и оптимизированное перемножение матриц
	DenseMatrix mulNaive(const DenseMatrix& other) const;
	DenseMatrix mulOptimize(const DenseMatrix& other) const;

	double at(int i, int j) const;
	void set(int i, int j, double value);
	int rows() const override;
	int cols() const override;
	std::unique_ptr<Matrix> mul(const Matrix& other, int threadCount) const override;
	std::unique_ptr<Matrix> mulFrequenceLock(const Matrix& other, int threadCount) const;
};