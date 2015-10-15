#ifndef TMATRIX_IMPL_79CFF806_6A12_46e0_BC89_66C513C8875F
#define TMATRIX_IMPL_79CFF806_6A12_46e0_BC89_66C513C8875F

#include <TSparseMatrix.h>
#include <Eigen/Core>

class TMatrixImpl
{
public:
    TMatrixImpl();

    //! Initialise the matrix with given value
    TMatrixImpl(int rows, int cols, double value);

    void setConstant(double value);

    //! Initialise a diagonal of the matrix with given value
    void initDiag(double value); 
    void resize(int rows, int cols);

    int rows() const { return fMatrix.rows(); }
    int cols() const { return fMatrix.cols(); }

    double & operator()(int row, int col);
    double operator()(int row, int col) const;
    TMatrixImpl & operator+=(const TMatrixImpl & rhs);
    TMatrixImpl & operator-=(const TMatrixImpl & rhs);
    TMatrixImpl & operator*=(const TMatrixImpl & rhs);
    TMatrixImpl operator*(TMatrixImpl & rhs) const;
    TMatrixImpl & operator*=(double value);
    TMatrixImpl & transpose();
    bool invert();
    TMatrixImpl solve(const TMatrixImpl & b) const;
	TMatrixImpl solveLdlt(const TMatrixImpl & b) const;

    // TODO: Temporary access to internal representation - needed for toSparse
    const double * data() const;

	TSparseMatrix toSparse() const;

private:
	// construct a new TMatrixImpl object from an Eigen matrix
    TMatrixImpl(const Eigen::MatrixXd& m);

    Eigen::MatrixXd fMatrix;
};

#endif