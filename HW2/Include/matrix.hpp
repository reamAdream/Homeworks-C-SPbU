#pragma once

#include <memory>
#include <vector>

class Matrix {
public:
	//деструктор
	virtual ~Matrix() = default;

	//мог объявить cols_ и rows_ сразу тут, но выбрал интерфейс без полей
	virtual int cols() const = 0;
	virtual int rows() const = 0;
	
	//метод перемножения
	virtual std::unique_ptr<Matrix> mul(const Matrix& other) const = 0;
};

