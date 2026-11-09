#include <cassert>
#include "../Include/sparse_matrix.hpp"
#include <iostream>
#include <stdexcept>
#include <random>
#include <cmath>


//Проверка корректности размера матрицы
void TestInvalidSize(int rows,int cols) {
    bool check = false;
    try { SparseMatrix(rows, cols); }
    catch (const std::invalid_argument&) { check = true; }
    assert(check);
}

//Проверка корректности индексов для чтения и замены элементов
void TestInvalidIndex(int i, int j) {
    bool checkAt = false;
    SparseMatrix A(3,4);
    try { A.at(i, j); }
    catch (const std::out_of_range&) { checkAt = true; }
    assert(checkAt);

    bool checkSet = false;
    try { A.set(i, j, 1.0); }
    catch (const std::out_of_range&) { checkSet = true; }
    assert(checkSet);
}

//Перемножение матриц небольших размеров (2x3 и 3x2)
void TestSparseMulExample() {
    SparseMatrix A(2, 3);
    A.set(0, 0, 1.0);
    A.set(0, 2, 2.0);
    A.set(1, 1, 3.0);

    SparseMatrix B(3, 2);
    B.set(0, 0, 4.0);
    B.set(1, 1, 5.0);
    B.set(2, 0, 6.0);

    SparseMatrix C = A.mulSparse(B);
    assert(C.rows() == A.rows());
    assert(C.cols() == B.cols());
    assert(C.at(0, 0) == 16.0);
    assert(C.at(0, 1) == 0.0);
    assert(C.at(1, 0) == 0.0);
    assert(C.at(1, 1) == 15.0);

    const Matrix& left = A;
    auto result = left.mul(B);
    assert(result != nullptr);
    assert(result->rows() == A.rows());
    assert(result->cols() == B.cols());

    const SparseMatrix* sparseResult = dynamic_cast<const SparseMatrix*>(result.get());
    assert(sparseResult != nullptr);

    for (int i = 0; i < sparseResult->rows();i++) {
        for (int j = 0; j < sparseResult->cols();j++) {
            assert(C.at(i, j) == sparseResult->at(i,j));
        }
    }
}

//Проверка корректности перемножения SPARSE x DENSE
void TestSparseDenseMulExample() {
    SparseMatrix A(2, 3);
    A.set(0, 0, 1.0);
    A.set(0, 2, 2.0);
    A.set(1, 1, 3.0);

    DenseMatrix B(3, 2);
    B.set(0, 0, 4.0);
    B.set(1, 1, 5.0);
    B.set(2, 0, 6.0);

    DenseMatrix C = A.mulDense(B);
    assert(C.rows() == A.rows());
    assert(C.cols() == B.cols());
    assert(C.at(0,0) == 16.0);
    assert(C.at(0,1) == 0.0);
    assert(C.at(1,0) == 0.0);
    assert(C.at(1,1) == 15.0);

    //Проверка виртуального метода
    const Matrix& left = A;
    auto result = left.mul(B);
    const DenseMatrix* denseResult = dynamic_cast<const DenseMatrix*>(result.get());
    assert(denseResult != nullptr);
}

//Проверка корректности перемножения DENSE x SPARSE
void TestDenseSparseMulExample() {
    DenseMatrix A(2, 3);
    A.set(0, 0, 1.0);
    A.set(0, 2, 2.0);
    A.set(1, 1, 3.0);

    SparseMatrix B(3, 2);
    B.set(0, 0, 4.0);
    B.set(1, 1, 5.0);
    B.set(2, 0, 6.0);

    DenseMatrix C = B.mulLeftDense(A);

    assert(C.rows() == A.rows());
    assert(C.cols() == B.cols());
    assert(C.at(0, 0) == 16.0);
    assert(C.at(0, 1) == 0.0);
    assert(C.at(1, 0) == 0.0);
    assert(C.at(1, 1) == 15.0);

    const Matrix& left = A;
    auto result = left.mul(B);
    const DenseMatrix* denseResult = dynamic_cast<const DenseMatrix*>(result.get());
    assert(denseResult != nullptr);
}

//Проверка корректности умножения на единичную матрицу
void mulIdentitySparse() {
    SparseMatrix A(2, 3);
    double k = 1.0;
    for (int i = 0; i < A.rows(); i++) {
        for (int j = 0; j < A.cols(); j++) {
            A.set(i, j, k);
            k += 1.0;
        }
    }
    //Единичная матрица
    SparseMatrix Identity(3, 3);
    for (int i = 0; i < Identity.cols(); i++) {
        Identity.set(i, i, 1.0);
    }
    SparseMatrix C = A.mulSparse(Identity);
    assert(C.rows() == A.rows());
    assert(C.cols() == Identity.cols());
    for (int i = 0; i < C.rows(); i++) {
        for (int j = 0; j < C.cols(); j++) {
            assert(C.at(i,j) == A.at(i,j));
        }
    }
}

//Проверка корректности умножения на нулевую матрицу
void mulNullSparse() {
    SparseMatrix A(2, 3);
    double k = 1.0;
    for (int i = 0; i < A.rows(); i++) {
        for (int j = 0; j < A.cols(); j++) {
            A.set(i, j, k);
            k += 1.0;
        }
    }
    SparseMatrix Nul(3, 4);
    SparseMatrix C = A.mulSparse(Nul);
    assert(C.rows() == A.rows());
    assert(C.cols() == Nul.cols());
    for (int i = 0; i < C.rows(); i++) {
        for (int j = 0; j < C.cols(); j++) {
            assert(C.at(i,j) == 0.0);
        }
    }
}

//Проверка корректности умножения матриц несовместимых размеров
void InvalidMulSparse() {
    SparseMatrix A(2, 3);
    SparseMatrix B(4, 2);

    bool check = false;
    try { SparseMatrix C = A.mulSparse(B); }
    catch (const std::invalid_argument&) { check = true; }
    assert(check);
}

//Проверка совпадения результатов перемножений разных типов одинаковых матриц и совпадения результатов перемножения смешанных типов одинаковых матриц
void TestRandomCrossMul() {
    std::mt19937 gen(42);
    std::bernoulli_distribution nonzero(0.25);
    std::uniform_real_distribution<double> number(-10.0, 10.0);
    DenseMatrix Ad(7, 5);
    SparseMatrix As(7, 5);
    for (int i = 0; i < Ad.rows(); i++) {
        for (int j = 0; j < Ad.cols(); j++) {
            if (nonzero(gen)) {
                const double valueA = number(gen);
                Ad.set(i, j, valueA);
                As.set(i, j, valueA);
            }
        }
    }
    std::uniform_real_distribution<double> numberB(-10, 10);
    DenseMatrix Bd(5, 9);
    SparseMatrix Bs(5, 9);
    for (int i = 0; i < Bd.rows(); i++) {
        for (int j = 0; j < Bd.cols(); j++) {
            if (nonzero(gen)) {
                const double valueB = numberB(gen);
                Bd.set(i, j, valueB);
                Bs.set(i, j, valueB);
            }
        }
    }
    DenseMatrix expected = Ad.mulOptimize(Bd);
    SparseMatrix actual = As.mulSparse(Bs);

    //Виртуальные вызовы
    const Matrix& sparseLeft = As;
    auto sparseDense = sparseLeft.mul(Bd);
    const DenseMatrix* sparseDenseResult = dynamic_cast<const DenseMatrix*>(sparseDense.get());
    assert(sparseDenseResult != nullptr);

    const Matrix& denseLeft = Ad;
    auto denseSparse = denseLeft.mul(Bs);
    const DenseMatrix* denseSparseResult = dynamic_cast<const DenseMatrix*>(denseSparse.get());
    assert(denseSparseResult != nullptr);

    for (int i = 0; i < expected.rows(); i++) {
        for (int j = 0; j < expected.cols();j++) {
            assert(std::abs(expected.at(i, j) - actual.at(i, j)) < 1e-9);
            assert(std::abs(expected.at(i, j) - denseSparseResult->at(i, j)) < 1e-9);
            assert(std::abs(expected.at(i, j) - sparseDenseResult->at(i, j)) < 1e-9);
        }
    }
}

int main() {
    //Базовые проверки (размеры, чтение, запись и обнуление элемента)
    SparseMatrix A(3, 4);
    assert(A.rows() == 3);
    assert(A.cols() == 4);

    assert(A.at(1, 2) == 0.0);
    A.set(1, 2, 7.5);
    assert(A.at(1, 2) == 7.5);
    A.set(1, 2, 0.0);
    assert(A.at(1, 2) == 0.0);

    ////Проверка исключения при нулевых и отрицательных размера
    TestInvalidSize(0, 4);
    TestInvalidSize(3, 0);
    TestInvalidSize(-1, 4);
    TestInvalidSize(3, -1);

    //Проверка исключений при чтении и записи по неверным индексам
    TestInvalidIndex(-1, 0);
    TestInvalidIndex(0, -1);
    TestInvalidIndex(3, 0);
    TestInvalidIndex(0, 4);

    //Проверка корректности умножения на маленьких матрицах 2x3 и 3x2
    TestSparseMulExample();

    //Проверка корректности умножения на единичную матрицу
    mulIdentitySparse();

    //Проверка корректности умножения на нулевую матрицу
    mulNullSparse();

    //Проверка исключения при умножении матриц несовместимых размеров
    InvalidMulSparse();

    //Проверка корректности перемножения разреженной матрицы НА плотную матрицу
    TestSparseDenseMulExample();

    //Проверка корректности перемножения плотной матрицы НА разреженную матрицу
    TestDenseSparseMulExample();

    //Сравнение всех вариантов умножения на одинаковых случайных данных
    TestRandomCrossMul();

    std::cout << "Success!";
    return 0;
}
