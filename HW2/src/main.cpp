#include "dense_matrix.hpp"
#include <iostream>

int main() {
	DenseMatrix A(3, 4);

	std::cout << A.rows() << "\n";
	std::cout << A.cols() << "\n";
	std::cout << A.at(2,3) << "\n";

	A.set(1, 2, 7.5);
	std::cout << A.at(1, 2) << "\n";

	return 0;
}