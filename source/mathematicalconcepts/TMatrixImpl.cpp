#include <TMatrixImpl.h>

#include <Eigen/LU>
#include <Eigen/Cholesky>

TMatrixImpl::TMatrixImpl()
{
}

TMatrixImpl::TMatrixImpl(int rows, int cols, double value)
: fMatrix(Eigen::MatrixXd::Constant(rows, cols, value))
{
}

TMatrixImpl::TMatrixImpl(const Eigen::MatrixXd& m) :
	fMatrix(m)
{
}

void TMatrixImpl::setConstant(double value)
{
    fMatrix.setConstant(value);
}

void TMatrixImpl::initDiag(double value)
{
    fMatrix.setIdentity(rows(), cols());
    fMatrix *= value;
}

void TMatrixImpl::resize(int rows, int cols)
{
    fMatrix.setConstant(rows, cols, 0.0);
}

double & TMatrixImpl::operator()(int row, int col)
{
    return fMatrix(row,col);
}

double TMatrixImpl::operator()(int row, int col) const
{
    return fMatrix(row,col);
}

TMatrixImpl & TMatrixImpl::operator+=(const TMatrixImpl & rhs)
{
    fMatrix += rhs.fMatrix;
    return *this;
}

TMatrixImpl & TMatrixImpl::operator-=(const TMatrixImpl & rhs)
{
    fMatrix -= rhs.fMatrix;
    return *this;
}

TMatrixImpl & TMatrixImpl::operator*=(const TMatrixImpl & rhs)
{
    fMatrix *= rhs.fMatrix;
    return *this;
}

TMatrixImpl TMatrixImpl::operator*(TMatrixImpl & rhs) const
{
    TMatrixImpl res;
    res.fMatrix = this->fMatrix * rhs.fMatrix;
    return res;
}

TMatrixImpl & TMatrixImpl::operator*=(double value)
{
    fMatrix *= value;
    return *this;
}

TMatrixImpl & TMatrixImpl::transpose()
{
    fMatrix.transposeInPlace();
    return *this;
}

bool TMatrixImpl::invert()
{
	Eigen::FullPivLU<Eigen::MatrixXd> lu(fMatrix);
    if(lu.isInvertible())
    {
        fMatrix = lu.inverse();
        return true;
    }
    return false;
}

TMatrixImpl TMatrixImpl::solve(const TMatrixImpl & b) const
{
    // TODO: Report if can be solved
    Eigen::FullPivLU<Eigen::MatrixXd> lu(fMatrix);
    return TMatrixImpl(lu.solve(b.fMatrix));
}

TMatrixImpl TMatrixImpl::solveLdlt(const TMatrixImpl & b) const
{// returns an empty matrix is decomposition not successful
    Eigen::LDLT<Eigen::MatrixXd> ldlt(fMatrix);
	TMatrixImpl ret(ldlt.solve(b.fMatrix));
	if (ldlt.info() == Eigen::Success)
		return ret;
	else
		return TMatrixImpl();
}

const double * TMatrixImpl::data() const
{
    return fMatrix.data();
}

TSparseMatrix TMatrixImpl::toSparse() const
{
	TSparseMatrix result(fMatrix.rows(), fMatrix.cols());
	result.reserve(Eigen::VectorXi::Constant(fMatrix.cols(),10));
	for(auto r=0; r!=fMatrix.rows(); ++r)
		for(auto c=0; c!=fMatrix.cols(); ++c)
			if(fMatrix(r,c)!=0)
				result.insert(r,c) = fMatrix(r,c);
	result.makeCompressed();
	return result;
}