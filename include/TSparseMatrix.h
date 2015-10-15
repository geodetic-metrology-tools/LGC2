#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include <Eigen/Sparse>
#include <string>

typedef Eigen::SparseMatrix<double> TSparseMatrix;
typedef Eigen::MatrixXd TDenseMatrix;
typedef Eigen::VectorXd TVector;
typedef Eigen::Triplet<double> TTriplet;

namespace TSparseUtils {
TSparseMatrix inverse(const TSparseMatrix & matrix, std::string & error);
// optimized multiplication routine that returns the main diagonal of A*B*A'
TVector& multABATasDiag(TVector& res, const TSparseMatrix& A, const TSparseMatrix& B);
}

#endif
