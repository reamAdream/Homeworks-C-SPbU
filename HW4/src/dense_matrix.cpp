#include "../Include/dense_matrix.hpp"
#include <stdexcept>
#include <cstddef>
#include <fstream> 
#include <mutex>
#include <thread>
#include <utility>
#include <iomanip>
#include <limits>


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

//Реализация виртуального многопоточного метода перемножения
std::unique_ptr<Matrix> DenseMatrix::mul(const Matrix& other, int threadCount) const {
	const DenseMatrix* right =
		dynamic_cast<const DenseMatrix*>(&other);
	if (right == nullptr) {
		throw std::invalid_argument("Unsupported matrix type");
	}
	if (right->rows() != cols_) {
		throw std::invalid_argument("Unsupported matrix sizes");
	}
	if (threadCount <= 0) {
		throw std::invalid_argument("Unsupported threads quantity");
	}
	const int rightCols = right->cols();
	DenseMatrix Result(rows_, rightCols);
	int nextRow = 0;
	std::mutex rowMutex;

	auto worker = [&] {
		while (true) {
			int row;
			{
				std::lock_guard<std::mutex> lock(rowMutex);
				if (nextRow >= rows_) {
					return;
				}
				row = nextRow++;
			}
			const std::size_t leftRow = static_cast<std::size_t>(row) * cols_;
			const std::size_t resultRow = static_cast<std::size_t>(row) * rightCols;
			for (int k = 0; k < cols_; k++) {
				const std::size_t rightRow = static_cast<std::size_t>(k) * right->cols();
				const double aik = data_[leftRow + k];
				for (int j = 0; j < rightCols; j++) {
					Result.data_[resultRow + j] += aik * right->data_[rightRow + j];
				}
			}
		}
	};

	std::vector<std::thread> workers;
	try {
		for (int i = 0; i < threadCount; i++) {
			workers.emplace_back(worker);
		}
	}
	catch (...) {
		for (std::thread& thread: workers) {
			thread.join();	
		}
		throw;
	}
	for (std::thread& thread : workers) {
		thread.join();
	}

	return std::make_unique<DenseMatrix>(std::move(Result));
}

std::unique_ptr<Matrix> DenseMatrix::mulFrequenceLock(const Matrix& other, int threadCount) const {
	const DenseMatrix* right =
		dynamic_cast<const DenseMatrix*>(&other);
	if (right == nullptr) {
		throw std::invalid_argument("Unsupported matrix type");
	}
	if (right->rows() != cols_) {
		throw std::invalid_argument("Unsupported matrix sizes");
	}
	if (threadCount <= 0) {
		throw std::invalid_argument("Unsupported threads quantity");
	}
	const int rightCols = right->cols();
	DenseMatrix Result(rows_, rightCols);
	int nextRow = 0;
	std::mutex rowMutex;
	std::mutex elementMutex;

	auto worker = [&] {
		while (true) {
			int row;
			{
				std::lock_guard<std::mutex> lock(rowMutex);
				if (nextRow >= rows_) {
					return;
				}
				row = nextRow++;
			}
			const std::size_t leftRow = static_cast<std::size_t>(row) * cols_;
			const std::size_t resultRow = static_cast<std::size_t>(row) * rightCols;
			for (int k = 0; k < cols_; k++) {
				const std::size_t rightRow = static_cast<std::size_t>(k) * right->cols();
				const double aik = data_[leftRow + k];
				for (int j = 0; j < rightCols; j++) {
					std::lock_guard<std::mutex> lock(elementMutex);
					Result.data_[resultRow + j] += aik * right->data_[rightRow + j];
				}
			}
		}
		};

	std::vector<std::thread> workers;
	try {
		for (int i = 0; i < threadCount; i++) {
			workers.emplace_back(worker);
		}
	}
	catch (...) {
		for (std::thread& thread : workers) {
			thread.join();
		}
		throw;
	}
	for (std::thread& thread : workers) {
		thread.join();
	}

	return std::make_unique<DenseMatrix>(std::move(Result));
}

//Импорт матрицы из файла
DenseMatrix DenseMatrix::fromFile(const std::string& path) {
	std::ifstream inputFile(path);
	if (!inputFile) {
		throw std::runtime_error("Cannot open matrix file:	 " + path);
	}
	
	int rows = 0;
	int cols = 0;
	if (!(inputFile >> rows >> cols)) {
		throw std::runtime_error("Cannot read matrix dimensions: " + path);
	}

	DenseMatrix result(rows, cols);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			double value = 0.0;
			if (!(inputFile >> value)) {
				throw std::runtime_error("Cannot read matrix element: " + path);
			}
			result.set(i, j, value);
		}
	}
	return result;
}

//Сохранение матрицы в файле с перезаписью каждый новый вызов
void DenseMatrix::toFile(const std::string& path) const {
	std::ofstream outputFile(path);
	if (!outputFile) {
		throw std::runtime_error("Cannot open matrix file: " + path);
	}
	int digits = std::numeric_limits<double>::max_digits10;
	outputFile << std::setprecision(digits);
	outputFile << rows_ << ' ' << cols_ << '\n';
	for (int i = 0; i < rows_; i++) {
		for (int j = 0; j < cols_; j++) {
			outputFile << at(i, j) << " ";
		}
		outputFile << "\n";
	}
	outputFile.close();
	if (!outputFile){
		throw std::runtime_error("Cannot close matrix file: " + path);
	}
}