#include "../Include/sparse_matrix.hpp"
#include <stdexcept>

SparseMatrix::SparseMatrix(int rows, int cols): rows_(rows), cols_(cols) {
	if ((rows <= 0) || (cols <= 0)) {
		throw std::invalid_argument("Invalid matrix size");
	}
}

int SparseMatrix::rows() const {
	return rows_;
}

int SparseMatrix::cols() const {
	return cols_;
}

double SparseMatrix::at(int i, int j) const{
	if (i >= 0 && i < rows_ && j >= 0 && j < cols_) {
		if (data_.count({ i,j }) == 0.0) {
			return 0.0;
		}
		else {
			return data_.at({i, j});
		}
	}
	else {
		throw std::out_of_range("Invalid matrix index");
	}
}

void SparseMatrix::set(int i, int j, double value) {
	if (i >= 0 && i < rows_ && j >= 0 && j < cols_) {
		if (value == 0.0) {
			data_.erase({ i,j });
		}
		else {
			data_[{i, j}] = value;
		}
	}
	else {
		throw std::out_of_range("Invalid matrix index");
	}
}

//Виртуальная реализация перемножения
std::unique_ptr<Matrix> SparseMatrix::mul(const Matrix& other) const {
	const SparseMatrix* right = dynamic_cast<const SparseMatrix*>(&other);
	if (right != nullptr) {
		return std::make_unique<SparseMatrix>(mulSparse(*right));
	}	
	const DenseMatrix* dense = dynamic_cast<const DenseMatrix*>(&other);
	if (dense != nullptr) {
		return std::make_unique<DenseMatrix>(mulDense(*dense));
	}
	throw std::invalid_argument("Unsupported matrix type");
}

//Перемножение разреженных матриц
SparseMatrix SparseMatrix::mulSparse(const SparseMatrix& other) const {
	if (cols_ != other.rows() ) { throw std::invalid_argument("Invalid sizes"); }
	SparseMatrix Result(rows_, other.cols());

	for (const auto& [position, aik] : data_) {
		const int i = position.first;
		const int k = position.second;
		auto it = other.data_.lower_bound({ k,0 });
		while (it != other.data_.end() && it -> first.first == k) {
			const int j = it->first.second;
			const double Bkj = it->second;
			double& sum = Result.data_[{i, j}];
			sum += Bkj * aik;
			if (sum == 0.0) {
				Result.data_.erase({i, j});
			}
			it++;
		}
	}
	return Result;
}

//Перемножение разреженной и плотной матриц
DenseMatrix SparseMatrix::mulDense(const DenseMatrix& other) const {
	if (cols_ != other.rows()) { throw std::invalid_argument("Invalid argument"); }
	DenseMatrix Result(rows_, other.cols());
	for (const auto& [position, aik] : data_) {
		const int i = position.first;
		const int k = position.second;
		for (int j = 0; j < other.cols(); j++) {
			Result.set(i,j,Result.at(i,j) + aik*other.at(k, j));
		}
	}
	return Result;
}

//Перемножение плотной и разреженной матриц
DenseMatrix SparseMatrix::mulLeftDense(const DenseMatrix& left) const {
	if (left.cols() != rows_) { throw std::invalid_argument("Invalid argument"); }
	DenseMatrix Result(left.rows(), cols_);
	for (int i = 0; i < left.rows(); i++) {
		for (const auto& [position, Bkj] : data_) {
			const int k = position.first;
			const int j = position.second;
			Result.set(i, j, Result.at(i, j) + Bkj * left.at(i, k));
		}
	}
	return Result;
}