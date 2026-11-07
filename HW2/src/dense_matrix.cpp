#include "../Include/dense_matrix.hpp"
#include <stdexcept>
#include <cstddef>


namespace {
	std::size_t checkedSize(int rows, int cols) {
		if ((rows <= 0) || (cols <= 0)) {
			throw std::invalid_argument("Invalid matrix size");
		}
		return static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
	};
}

//метод нахождения индекса числа в матрице
std::size_t DenseMatrix::index(int i, int j) const{
	if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
		throw std::out_of_range("Matrix index out of range");
	}
	return static_cast<std::size_t>(i) * cols_ + j;
	
}

//метод чтения значения по индексу в матрице
double DenseMatrix::at(int i, int j) const {
	return data_[index(i, j)];
}

//метод замены значения по индексу в матрице
void DenseMatrix::set(int i, int j, double value) {
	data_[index(i, j)] = value;
}

int DenseMatrix::rows() const {
	return rows_;
}

int DenseMatrix::cols() const {
	return cols_;
}

// cоздаём матрицу заданного размера и заполняем её нулями.
DenseMatrix::DenseMatrix(int rows, int cols):
	rows_(rows),
	cols_(cols),
	data_(checkedSize(rows,cols),0.0) {
}

//реализация наивного перемножения матриц
DenseMatrix DenseMatrix::mulNaive(const DenseMatrix& other) const {
	if (cols_ != other.rows()) {
		throw std::invalid_argument("Incompatible matrix sizes");
	}
	DenseMatrix result(rows_, other.cols());
	for (int i = 0; i < rows_; i++) {
		for (int j = 0; j < other.cols(); j++) {
			double sum = 0.0;
			for (int k = 0; k < cols_; k++) {
				sum = sum + at(i, k) * other.at(k, j);
			}
			result.set(i, j, sum);
		}
	}
	return result;
}

//реализация оптимизированного перемножения матриц
DenseMatrix DenseMatrix::mulOptimize(const DenseMatrix& other) const {
	if (cols_ != other.rows_) {
		throw std::invalid_argument("Incompatible matrix sizes");
	}
	const int rightCols = other.cols_;
	DenseMatrix result(rows_, rightCols);
	for (int i = 0; i < rows_; i++) {
		const std::size_t leftRow = static_cast<std::size_t>(i) * cols_;
		const std::size_t resultRow = static_cast<std::size_t>(i) * rightCols;
		for (int k = 0; k < cols_; k++) {
			const double aik = data_[leftRow + k];
			const std::size_t rightRow = static_cast<std::size_t>(k) * rightCols;
			for (int j = 0; j < rightCols; j++) {
				result.data_[resultRow + j] += aik * other.data_[rightRow + j];
			}
		}
	}
	return result;
}

//Реализация виртуального метода перемножения
std::unique_ptr<Matrix> DenseMatrix::mul(const Matrix& other) const {
	const DenseMatrix* right =
		dynamic_cast<const DenseMatrix*>(&other);
	if (right == nullptr) {
		throw std::invalid_argument("Unsupported matrix type");
	}
	return std::make_unique<DenseMatrix>(mulOptimize(*right));
}