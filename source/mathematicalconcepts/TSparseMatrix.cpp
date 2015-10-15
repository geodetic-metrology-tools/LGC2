#include <Eigen/LU>

#include "TSparseMatrix.h"
#include <iostream>
#include <sstream>
#include <vector>

namespace TSparseUtils {
TSparseMatrix inverse(const TSparseMatrix & sparse, std::string & error)
{
	typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
	Eigen::FullPivLU<TMat> lu(sparse);

	if (! lu.isInvertible()) {
		error = "Matrix could not be inverted.";
        return TSparseMatrix();
	}

	TMat inv(lu.inverse());

	// create a sparse matrix from the non-zero coefficients
	std::vector<TTriplet> coeffs;
	for (int i = 0; i < inv.rows(); i++) {
		for (int j = 0; j < inv.cols(); j++) {
			double v(inv(i,j));
			if (fabs(v) > 1e-12)
				coeffs.push_back(std::move(TTriplet(i, j, v)));
		}
	}

	TSparseMatrix result(sparse.rows(), sparse.cols());
    result.setFromTriplets(coeffs.begin(), coeffs.end());
	return result;
}

inline double ABij(const TSparseMatrix& A, const TSparseMatrix& B, int i, int j)
{
	const int u(A.cols());
	double sum(0.0);
	
	#pragma omp parallel for reduction(+ : sum)
	for (int r = 0; r < u; r++) {
		sum +=  A.coeff(i,r)*B.coeff(r,j);
	}

	return sum;
}

TVector& multABATasDiag(TVector& res, const TSparseMatrix& A, const TSparseMatrix& B)
{
	const int obs(A.rows());
	const int ukn(A.cols());

	res.resize(obs);
	res.setZero();

	//TSparseMatrix AB(A*B);
	#pragma omp parallel for
	for (int n = 0; n < obs; n++) {
		for (int u = 0; u < ukn; u++) {
			// for unknowns (u= num unknowns)
			//double abij = AB.coeff(n,u) ;
			double abij = ABij(A, B, n, u);
			res(n) += abij*A.coeff(n,u);
		}
	}
	return res;
}

} // namespace

//#include <Eigen/Cholesky>
//
//TSparseMatrix::TSparseMatrix(int rows, int cols)
//	: fMatrix(Eigen::MatrixXd::Constant(rows, cols, 0))
//{
//}
//
//TSparseMatrix::TSparseMatrix(const Eigen::MatrixXd & m)
//	: fMatrix(m)
//{
//}
//
//TSparseMatrix::TSparseMatrix(int rows, int cols, TReal * vals, int * rowInds, int * colPtr)
//{
//	// TODO
//}
//
//TSparseMatrix TSparseMatrix::transposed() const 
//{ 
//	TSparseMatrix tmp(*this); 
//	tmp.fMatrix.transposeInPlace(); 
//	return tmp; 
//}
//
//TSparseMatrix TSparseMatrix::symmetric_lower_inverse() const
//{
//	TSparseMatrix tmp(*this);
//	tmp.fMatrix.marked<Eigen::LowerTriangular>().solveTriangularInPlace(tmp.fMatrix);
//	return tmp;
//}
//
//TSparseMatrix TSparseMatrix::symmetric_lower_inverse_saving_L(TSparseMatrix & L) const
//{
//	//TODO L
//	//L = this->cholesky_decompose_lower_triangular_returning_lower_triangular();
//	//TSparseMatrix* inverse = L->invert_lower_triangular_cholesky_decomposed_returning_lower_triangular();
//
//	Eigen::VectorXd fVector;
//
//	L.fMatrix.llt().solveInPlace(fVector); // using a LLT factorization
//	L.fMatrix.inverse();
//
//
//	///////////////////////////////////////////////////////////////////////////////////////////
//
//	TSparseMatrix tmp(*this);
//	tmp.fMatrix.marked<Eigen::LowerTriangular>().solveTriangularInPlace(tmp.fMatrix);
//	return tmp;
//}
//TSparseMatrix TSparseMatrix::multiply_F(const TSparseMatrix& second) const
//{
	//TODO
	// multiplies a general sparse matrix with another. The F stands for Fast. The routines with LM take Less Memory but are slower.

//}
//TSparseMatrix multiply_LM(const TSparseMatrix& second) const
//{

//}
//
//TSparseMatrix::TSparseMatrix(int rows, int columns, TReal* vals, int* rowInds, int* colPtr)
//{
//	this->rows = rows;
//	cols = columns;
//
//	this->vals = vals;
//	colptr = colPtr;
//	rowind = rowInds;
//}
//
//TSparseMatrix::~TSparseMatrix()
//{
//    if (colptr != NULL)
//    {
//		if (colptr == rowind)
//		{
//			rowind = NULL;
//		}
//        delete[] colptr;
//    }
//
//    if (vals != NULL)
//    {
//        delete[] vals;
//    }
//
//    if (rowind != NULL)
//    {
//        delete[] rowind;
//    }
//}
//
//TSparseMatrix* TSparseMatrix::transposed() const
//{
//	int* temp = new int[rows];
//	int i, j, q;
//
//	TReal* values = new TReal[colptr[cols]];
//	int* newColptr = new int[rows + 1];
//	int* newRowind = new int[colptr[cols]];
//	for (i = 0; i < rows; i++)
//	{
//		temp[i] = 0;
//	}
//
//	for (i = 0; i < colptr[cols]; i++)
//	{
//		temp[rowind[i]]++;
//	}
//
//	int total = 0;
//	for (i = 0; i < rows; i++)
//	{
//		newColptr[i] = total;
//		total += temp[i];
//		temp[i] = newColptr[i];
//	}
//	newColptr[rows] = total;
//
//	for (i = 0; i < cols; i++)
//    {
//        for (j = colptr[i]; j < colptr[i + 1]; j++)
//        {
//			q = temp[rowind[j]]++;
//            newRowind[q] = i;
//            values[q] = vals[j];
//        }
//    }
//
//	delete[] temp;
//
//	return new TSparseMatrix(cols, rows, values, newRowind, newColptr);
//}
//
//TSparseMatrix* TSparseMatrix::symmetric_lower_inverse() const
//{
//	TSparseMatrix* L;
//	TSparseMatrix* res = symmetric_lower_inverse_saving_L(L);
//	if (res != NULL)
//	{
//		delete L;
//		return res;
//	}
//	return NULL;
//}
//
//TSparseMatrix* TSparseMatrix::symmetric_lower_inverse_saving_L(TSparseMatrix*& L) const
//{
//	L = this->cholesky_decompose_lower_triangular_returning_lower_triangular();
//    if (L != NULL)
//    {
//        TSparseMatrix* inverse = L->invert_lower_triangular_cholesky_decomposed_returning_lower_triangular();
//        TSparseMatrix* inverseTrans = inverse->transposed();
//        TSparseMatrix* result = inverseTrans->multiply_F(*inverse);
//        delete inverse;
//        delete inverseTrans;
//        return result;
//    }
//    else
//    {
//        return NULL;
//    }
//}
//
//TSparseMatrix* TSparseMatrix::multiply_F(const TSparseMatrix& second) const
//{
//    TReal *resultColumn = new TReal[rows];
//    Vector<TReal> results(rows * second.cols / 2);
//    Vector<int> rowInds(rows * second.cols / 2);
//    int i, k, l;
//    TSparseMatrix *result = new TSparseMatrix(rows, second.cols);
//    result->colptr[0] = 0;
//    for (i = 0; i < rows; i++)
//    {
//        resultColumn[i] = 0;
//    }
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k]]; l < colptr[second.rowind[k] + 1]; l++)
//            {
//                resultColumn[rowind[l]] += second.vals[k] * vals[l];
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                results.add(resultColumn[k]);
//                rowInds.add(k);
//                resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = results.size();
//    }
//
//    delete[] resultColumn;
//
//    result->setNNZ(results.size());
//    for (i = 0; i < results.size(); i++)
//    {
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//    }
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_LM(const TSparseMatrix& second) const
//{
//	TReal *resultColumn = new TReal[rows];
//    int i, k, l;
//    TSparseMatrix *result;
//    int nnz = 0;
//    for (i = 0; i < rows; i++)
//    {
//		resultColumn[i] = 0;
//    }
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k]]; l < colptr[second.rowind[k] + 1]; l++)
//            {
//				resultColumn[rowind[l]] += second.vals[k] * vals[l];
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                nnz++;
//                resultColumn[k] = 0;
//            }
//        }
//    }
//    result = new TSparseMatrix(rows, second.cols, nnz);
//    int count = 0;
//    result->colptr[0] = 0;
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k]]; l < colptr[second.rowind[k] + 1]; l++)
//            {
//				resultColumn[rowind[l]] += second.vals[k] * vals[l];
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                result->vals[count] = resultColumn[k];
//                result->rowind[count++] = k;
//				resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = count;
//    }
//
//	delete[] resultColumn;
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_diagonal_dense(const TReal* second, int secondCols) const
//{
//    int i, l;
//    TSparseMatrix *result = new TSparseMatrix(rows, secondCols, colptr[cols]);
//    result->colptr[0] = 0;
//	int count = 0;
//    for (i = 0; i < secondCols; i++)
//    {
//        for (l = colptr[i]; l < colptr[i + 1]; l++)
//        {
//			result->vals[count] = second[i] * vals[l];
//			result->rowind[count++] = rowind[l];
//        }
//        result->colptr[i + 1] = count;
//    }
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_diagonal_sparse(const TReal* second, int secondCols) const
//{
//    int i, l, nnz = 0;
//	for (i = 0; i < secondCols; i++)
//	{
//		if (second[i] != 0)
//		{
//			nnz += colptr[i + 1] - colptr[i];
//		}
//	}
//    TSparseMatrix *result = new TSparseMatrix(rows, secondCols, nnz);
//    result->colptr[0] = 0;
//	int count = 0;
//    for (i = 0; i < secondCols; i++)
//    {
//		if (second[i] != 0)
//		{
//			for (l = colptr[i]; l < colptr[i + 1]; l++)
//			{
//				result->vals[count] = second[i] * vals[l];
//				result->rowind[count++] = rowind[l];
//			}
//		}
//        result->colptr[i + 1] = count;
//    }
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_returning_unordered_F(const TSparseMatrix& second) const
//{
//	int* cache = new int[rows];
//    Vector<TReal> results(rows * second.cols / 2);
//    Vector<int> rowInds(rows * second.cols / 2);
//    int i, k, l;
//    TSparseMatrix *result = new TSparseMatrix(rows, second.cols);
//    result->colptr[0] = 0;
//	for (i = 0; i < rows; i++)
//	{
//		cache[i] = 0;
//	}
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k]]; l < colptr[second.rowind[k] + 1]; l++)
//            {
//				if (cache[rowind[l]] == 0)
//				{
//					cache[rowind[l]] = results.size() + 1;
//            		results.add(second.vals[k] * vals[l]);
//            		rowInds.add(rowind[l]);
//				}
//				else
//				{
//					results[cache[rowind[l]] - 1] += second.vals[k] * vals[l];
//				}
//            }
//        }
//        result->colptr[i + 1] = results.size();
//        for (k = result->colptr[i]; k < result->colptr[i + 1]; k++)
//        {
//			cache[rowInds[k]] = 0;
//        }
//    }
//
//    delete[] cache;
//
//    result->setNNZ(results.size());
//    for (i = 0; i < results.size(); i++)
//    {
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//    }
//
//    return result;
//}
//
//TReal TSparseMatrix::operator ()(int row, int column) const
//{
//	if (column <= cols / 2)
//	{
//		int i = colptr[column];
//		while (i < colptr[column + 1] && rowind[i] < row)
//		{
//			i++;
//		}
//		if (i < colptr[column + 1] && rowind[i] == row)
//		{
//			return vals[i];
//		}
//	}
//	else
//	{
//		int i = colptr[column + 1] - 1;
//		while (i >= colptr[column] && rowind[i] > row)
//		{
//			i--;
//		}
//		if (i >= colptr[column] && rowind[i] == row)
//		{
//			return vals[i];
//		}
//	}
//	return 0;
//}
//
//TReal* TSparseMatrix::operator *(const TReal* right) const
//{
//	int i, j;
//
//	TReal* result = new TReal[rows];
//
//	for (i = 0; i < rows; i++)
//	{
//		result[i] = 0;
//	}
//
//    for (i = 0; i < cols; i++)
//	{
//		if (right[i] != 0)
//		{
//			for (j = colptr[i]; j < colptr[i + 1]; j++)
//			{
//				result[rowind[j]] += right[i] * vals[j];
//			}
//		}
//	}
//
//	return result;
//}
//
//TReal* TSparseMatrix::operator *(const TColumnVector& right) const
//{
//	int i, j;
//
//	TReal* result = new TReal[rows];
//
//	for (i = 0; i < rows; i++)
//	{
//		result[i] = 0;
//	}
//
//    for (i = 0; i < cols; i++)
//	{
//		if (right(i) != 0)
//		{
//			for (j = colptr[i]; j < colptr[i + 1]; j++)
//			{
//				result[rowind[j]] += right(i) * vals[j];
//			}
//		}
//	}
//
//	return result;
//}
//
//bool TSparseMatrix::operator ==(const TSparseMatrix& second) const
//{
//	if (cols != second.cols || rows != second.rows ||
//		colptr[cols] != second.colptr[second.cols])
//	{
//		return false;
//	}
//	for (int i = 0; i < cols; i++)
//	{
//		if (colptr[i] != second.colptr[i])
//		{
//			return false;
//		}
//	}
//
//	for (int i = 0; i < colptr[cols]; i++)
//	{
//		TReal temp = vals[i] / second.vals[i];
//		if (rowind[i] != second.rowind[i] ||
//			temp < 1 - THRESHOLD || temp > 1 + THRESHOLD)
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
//
//bool TSparseMatrix::operator !=(const TSparseMatrix& second) const
//{
//	return !(*this == second);
//}
//
//TSparseMatrix* TSparseMatrix::deep_copy(const TSparseMatrix* matrix)
//{
//	TSparseMatrix* m = new TSparseMatrix(matrix->rows, matrix->cols, matrix->colptr[matrix->cols]);
//
//	for (int i = 0; i <= matrix->cols; i++)
//	{
//		m->colptr[i] = matrix->colptr[i];
//	}
//	for (int i = 0; i < matrix->colptr[matrix->cols]; i++)
//	{
//		m->vals[i] = matrix->vals[i];
//		m->rowind[i] = matrix->rowind[i];
//	}
//
//	return m;
//}
//
//TSparseMatrix* TSparseMatrix::cholesky_decompose_lower_triangular_returning_lower_triangular() const
//{
//	TReal* resultColumn = new TReal[cols];
//	Vector<TReal> results(cols * cols / 2);
//	Vector<int> rowInds(cols * cols / 2);
//
//	int* colWhereTo = new int[cols];
//
//	TSparseMatrix *result = new TSparseMatrix(cols, cols);
//
//	result->colptr[0] = 0;
//
//	for (int i = 0; i < cols; i++)
//	{
//		resultColumn[i] = 0;
//	}
//
//	for (int i = 0; i < cols; i++)
//	{
//		colWhereTo[i] = results.size() + 1;
//
//		int column = 0;
//		int col = result->colptr[column + 1];
//
//		int count;
//
//		while (column < i) // going through all the computed columns
//		{
//			count = colWhereTo[column];
//
//			// if this element in the column is not zero
//			if (count != -1 && rowInds[count] == i)
//			{
//				TReal columnMainValue = results[count++];
//				resultColumn[i] -= columnMainValue * columnMainValue;
//
//				// we multiply each element of the rest of the column with the "main value" which is
//				// on the i -th row and then subtract that from the current row's sum
//				while (count < col)
//				{
//					resultColumn[rowInds[count]] -= results[count] * columnMainValue;
//					count++;
//				}
//
//				if (++colWhereTo[column] >= result->colptr[column + 1])
//				{
//					colWhereTo[column] = -1;
//				}
//			}
//			col = result->colptr[++column + 1];
//		}
//
//		col = colptr[i];
//
//		resultColumn[i] += vals[col];
//		if (resultColumn[i] <= 0)
//		{
//			delete[] resultColumn;
//			delete[] colWhereTo;
//			delete result;
//			return NULL;
//		}
//		resultColumn[i] = sqrtq(resultColumn[i]);
//		rowInds.add(i);
//		results.add(resultColumn[i]);
//		col++;
//
//		for (int j = i + 1; j < cols; j++)
//		{
//			TReal val = resultColumn[j];
//			if (j == rowind[col] && col < colptr[i + 1])
//			{
//				val += vals[col++];
//			}
//			if (val < -THRESHOLD || val > THRESHOLD)
//			{
//				rowInds.add(j);
//				results.add(val / resultColumn[i]);
//			}
//			resultColumn[j] = 0;
//		}
//		resultColumn[i] = 0;
//
//        result->colptr[i + 1] = results.size();
//	}
//
//	delete[] colWhereTo;
//	delete[] resultColumn;
//
//	result->setNNZ(results.size());
//	for (int i = 0; i < results.size(); i++)
//	{
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//	}
//
//	return result;
//}
//
//TSparseMatrix* TSparseMatrix::ldlt_decompose_lower_triangular_returning_lower_triangular(TReal*& D) const
//{
//	TReal* resultColumn = new TReal[cols];
//	Vector<TReal> results(cols * cols / 4);
//	Vector<int> rowInds(cols * cols / 4);
//
//	int* colWhereTo = new int[cols];
//
//	TSparseMatrix *result = new TSparseMatrix(cols, cols);
//	result->colptr[0] = 0;
//	D = new TReal[cols];
//
//	for (int i = 0; i < cols; i++)
//	{
//		resultColumn[i] = 0;
//	}
//
//	for (int i = 0; i < cols; i++)
//	{
//		colWhereTo[i] = results.size() + 1;
//
//		// computing the diagonal element
//		TReal diag;
//		if (colptr[i] != colptr[cols])
//		{
//			diag = vals[colptr[i]];
//		}
//		else
//		{
//			diag = 0;
//		}
//		for (int j = 0; j < i; j++)
//		{
//			if (colWhereTo[j] != -1 && rowInds[colWhereTo[j]] == i)
//			{
//				TReal multiplier = D[j] * results[colWhereTo[j]];
//				diag -= multiplier * results[colWhereTo[j]];
//
//				if (++colWhereTo[j] >= result->colptr[j + 1])
//				{
//					colWhereTo[j] = -1;
//				}
//				else
//				{				
//					for (int k = colWhereTo[j]; k < result->colptr[j + 1]; k++)
//					{
//						resultColumn[rowInds[k]] += multiplier * results[k];
//					}
//				}
//			}
//		}
//		if (diag == 0)
//		{
//			delete[] resultColumn;
//			delete[] colWhereTo;
//			delete[] D;
//			delete result;
//			return NULL;
//		}
//		rowInds.add(i);
//		results.add(1);
//		D[i] = diag;
//
//		diag = 1 / diag;
//		// computing the rest of the column...
//		for (int j = i + 1, k = colptr[i] + 1; j < cols; j++)
//		{
//			TReal element = diag * ((k < colptr[i + 1] && rowind[k] == j ? vals[k++] : 0) - resultColumn[j]);
//			if (element != 0)
//			{
//				rowInds.add(j);
//				results.add(element);
//			}
//			resultColumn[j] = 0;
//		}
//
//		result->colptr[i + 1] = results.size();
//	}
//
//	delete[] colWhereTo;
//	delete[] resultColumn;
//
//	result->setNNZ(results.size());
//	for (int i = 0; i < results.size(); i++)
//	{
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//	}
//
//	return result;
//}
//
//TReal* TSparseMatrix::solve_eqn(const TReal* b) const
//{
//	TReal* result = new TReal[rows];
//
//	TSparseMatrix* LT = this->transposed();
//
//	int col;
//	TReal sum;
//	for (int i = 0; i < cols; i++)
//	{
//		sum = b[i];
//		for (col = LT->colptr[i]; LT->rowind[col] < i; col++)
//		{
//			sum -= LT->vals[col] * result[LT->rowind[col]];
//		}
//		result[i] = sum / LT->vals[col];
//	}
//	for (int i = cols - 1; i >= 0; i--)
//	{
//		sum = result[i];
//		for (col = colptr[i + 1] - 1; rowind[col] > i; col--)
//		{
//			sum -= vals[col] * result[rowind[col]];
//		}
//		result[i] = sum / vals[col];
//	}
//
//	return result;
//}
//
//TReal* TSparseMatrix::solve_ldlt(const TReal* D, const TReal* b) const
//{
//	TReal* result = new TReal[rows];
//
//	TSparseMatrix* LT = this->transposed();
//
//	int col;
//	TReal sum;
//	for (int i = 0; i < cols; i++)
//	{
//		sum = b[i];
//		for (col = LT->colptr[i]; LT->rowind[col] < i; col++)
//		{
//			sum -= LT->vals[col] * result[LT->rowind[col]];
//		}
//		result[i] = sum;
//	}
//	for (int i = cols - 1; i >= 0; i--)
//	{
//		sum = result[i] / D[i];
//		for (col = colptr[i + 1] - 1; rowind[col] > i; col--)
//		{
//			sum -= vals[col] * result[rowind[col]];
//		}
//		result[i] = sum;
//	}
//
//	return result;
//}
//
//void TSparseMatrix::solve_ldlt_in_place(const TSparseMatrix* LT, const TReal* D, TReal* b, int up_to) const
//{
//	int col;
//	TReal sum;
//	for (int i = 0; i < cols; i++)
//	{
//		sum = b[i];
//		for (col = LT->colptr[i]; LT->rowind[col] < i; col++)
//		{
//			sum -= LT->vals[col] * b[LT->rowind[col]];
//		}
//		b[i] = sum;
//	}
//	for (int i = cols - 1; i >= up_to; i--)
//	{
//		sum = b[i] / D[i];
//		for (col = colptr[i + 1] - 1; rowind[col] > i; col--)
//		{
//			sum -= vals[col] * b[rowind[col]];
//		}
//		b[i] = sum;
//	}
//}
//
//TSparseMatrix* TSparseMatrix::invert_diagonal_matrix() const
//{
//	TSparseMatrix* inverse = new TSparseMatrix(cols, cols, cols);
//	for (int i = 0; i < cols; i++)
//	{
//		inverse->vals[i] = 1 / vals[colptr[i]];
//		inverse->rowind[i] = i;
//	}
//	for (int i = 0; i <= cols; i++)
//	{
//		inverse->colptr[i] = i;
//	}
//	return inverse;
//}
//
//TSparseMatrix* TSparseMatrix::invert_lower_triangular_cholesky_decomposed() const
//{
//	TSparseMatrix* transposed = this->transposed();
//
//	Vector<TReal> results(colptr[cols] * 4);
//	Vector<int> rowInds(colptr[cols] * 4);
//
//	int i, j, k, l;
//	TReal sum = 0;
//
//	TSparseMatrix *result = new TSparseMatrix(cols, cols);
//	result->colptr[0] = 0;
//
//    for (i = 0; i < cols; i++)
//    {
//        for (j = 0; j < i; j++)
//        {
//			k = result->colptr[j];
//			while (rowInds[k] < j && k < result->colptr[j + 1])
//			{
//				k++;
//			}
//            for (l = transposed->colptr[i];
//                rowInds[k] < i &&
//				k < result->colptr[j + 1] &&
//                l < transposed->colptr[i + 1] &&
//                transposed->rowind[l] < i; )
//            {
//                if (transposed->rowind[l] < rowInds[k])
//                {
//                    l++;
//                }
//                else if (rowInds[k] == transposed->rowind[l])
//                {
//                    sum -= results[k] * transposed->vals[l];
//                    k++;
//                    l++;
//                }
//                else
//                {
//                    k++;
//                }
//            }
//
//            if (sum != 0)
//            {
//                rowInds.add(j);
//                results.add(sum / vals[colptr[i]]);
//                sum = 0;
//            }
//        }
//
//        results.add(1 / vals[colptr[i]]);
//        rowInds.add(i);
//
//        for (j = i + 1; j < cols; j++)
//        {
//			int size = results.size();
//			k = result->colptr[i];
//			while (rowInds[k] < i && k < size)
//			{
//				k++;
//			}
//            for (l = transposed->colptr[j];
//                k < size &&
//                rowInds[k] < j &&
//                l < transposed->colptr[j + 1] &&
//                transposed->rowind[l] < j; )
//            {
//                if (transposed->rowind[l] < rowInds[k])
//                {
//                    l++;
//                }
//                else if (rowInds[k] == transposed->rowind[l])
//                {
//                    sum -= results[k] * transposed->vals[l];
//                    k++;
//                    l++;
//                }
//                else
//                {
//                    k++;
//                }
//            }
//
//            if (sum != 0)
//            {
//                rowInds.add(j);
//                results.add(sum / vals[colptr[j]]);
//                sum = 0;
//            }
//        }
//        result->colptr[i + 1] = results.size();
//	}
//
//	delete transposed;
//
//	result->setNNZ(results.size());
//	for (i = 0; i < results.size(); i++)
//	{
//		result->rowind[i] = rowInds[i];
//		result->vals[i] = results[i];
//	}
//
//	return result;
//}
//
//TSparseMatrix* TSparseMatrix::invert_lower_triangular_cholesky_decomposed_returning_lower_triangular() const
//{
//	TSparseMatrix* transposed = this->transposed();
//
//	Vector<TReal> results(colptr[cols] * 4);
//	Vector<int> rowInds(colptr[cols] * 4);
//
//	int i, j, k, l;
//	TReal sum = 0;
//
//	TSparseMatrix *result = new TSparseMatrix(cols, cols);
//	result->colptr[0] = 0;
//
//    for (i = 0; i < cols; i++)
//    {
//        results.add(1 / vals[colptr[i]]);
//        rowInds.add(i);
//
//        for (j = i + 1; j < cols; j++)
//        {
//			int size = results.size();
//            for (k = result->colptr[i], l = transposed->colptr[j];
//                k < size &&
//                rowInds[k] < j &&
//                l < transposed->colptr[j + 1] &&
//                transposed->rowind[l] < j; )
//            {
//                if (transposed->rowind[l] < rowInds[k])
//                {
//                    l++;
//                }
//                else if (rowInds[k] == transposed->rowind[l])
//                {
//                    sum -= results[k] * transposed->vals[l];
//                    k++;
//                    l++;
//                }
//                else
//                {
//                    k++;
//                }
//            }
//
//            if (sum != 0)
//            {
//                rowInds.add(j);
//                results.add(sum / vals[colptr[j]]);
//                sum = 0;
//            }
//        }
//        result->colptr[i + 1] = results.size();
//	}
//
//	delete transposed;
//
//	result->setNNZ(results.size());
//	for (i = 0; i < results.size(); i++)
//	{
//		result->rowind[i] = rowInds[i];
//		result->vals[i] = results[i];
//	}
//
//	return result;
//}
//
//TSparseMatrix* TSparseMatrix::invert_lower_triangular_ldlt_decomposed(const TReal* D) const
//{
//	TSparseMatrix* Ainv = this->invert_lower_triangular_ldlt_decomposed_returning_lower_triangular(D);
//	TSparseMatrix* trans = Ainv->transposed();
//
//	TReal* ds = new TReal[cols];
//	int* rs = new int[cols + 1];
//
//	for (int i = 0; i < cols; i++)
//	{
//		ds[i] = -Ainv->vals[Ainv->colptr[i]];
//		rs[i] = i;
//	}
//	rs[cols] = cols;
//	TSparseMatrix* diag = new TSparseMatrix(cols, cols, ds, rs, rs);
//	TSparseMatrix* middle = Ainv->add(*diag);
//	delete Ainv;
//	delete diag;
//	TSparseMatrix* result = middle->add(*trans);
//	delete middle;
//	delete trans;
//	return result;
//}
//
//TSparseMatrix* TSparseMatrix::invert_lower_triangular_ldlt_decomposed_returning_lower_triangular(const TReal* D) const
//{
//	TSparseMatrix* LT = this->transposed();
//
//	TReal* temp = new TReal[cols];
//
//	Vector<TReal> results(colptr[cols]);
//	Vector<TReal> rowInds(colptr[cols]);
//
//	TSparseMatrix* result = new TSparseMatrix(cols, cols);
//	result->colptr[0] = 0;
//
//	for (int i = 0; i < cols; i++)
//	{
//		temp[i] = 0;
//	}
//
//	for (int i = 0; i < cols; i++)
//	{
//		temp[i] = 1;
//
//		solve_ldlt_in_place(LT, D, temp, i);
//
//		for (int j = i; j < cols; j++)
//		{
//			if (temp[j] != 0)
//			{
//				results.add(temp[j]);
//				rowInds.add(j);
//				temp[j] = 0;
//			}
//		}
//
//		result->colptr[i + 1] = results.size();
//	}
//
//	delete LT;
//	delete[] temp;
//
//	result->setNNZ(results.size());
//	for (int i = 0; i < results.size(); i++)
//	{
//		result->rowind[i] = rowInds[i];
//		result->vals[i] = results[i];
//	}
//
//	return result;
//}
//
//TReal* TSparseMatrix::invert_lower_triangular_ldlt_decomposed_returning_diagonal(const TReal* D) const
//{
//	// TODO: not working!
//	int i, j;
//
//	Vector<TReal> results(colptr[cols]);
//	Vector<int> rowInds(colptr[cols]);
//
//	int* newColptr = new int[cols + 1];
//
//	newColptr[cols] = 0;
//	results.add(1 / D[cols - 1]);
//	rowInds.add(cols - 1);
//	newColptr[cols - 1] = 1;
//
//	for (i = cols - 2; i >= 0; i--)
//	{
//		TReal diag = 1 / D[i];
//		for (j = colptr[i + 1] - 1; j > colptr[i]; j--)
//		{
//			TReal res = -results[newColptr[rowind[j] + 1]] * vals[j];
//			results.add(res);
//			rowInds.add(rowind[j]);
//			diag -= res * vals[j];
//		}
//		results.add(diag);
//		rowInds.add(i);
//		newColptr[i] = results.size();
//	}
//
//	TReal* result = new TReal[cols];
//
//	int sum = 0;
//	for (i = 0, j = cols - 1; i < cols; i++, j--)
//	{
//		result[i] = results[newColptr[j] - 1];
//	}
//
//	delete[] newColptr;
//
//	return result;
//}
//
//TReal* TSparseMatrix::multiply_returning_diagonal(const TSparseMatrix& second) const
//{
//	TReal* result = new TReal[second.cols];
//    int i, k, l, secondN = second.cols / 2;
//    for (i = 0; i < secondN; i++)
//    {
//		result[i] = 0;
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//			l = colptr[second.rowind[k]];
//			while (rowind[l] < i && l < colptr[second.rowind[k] + 1])
//			{
//				l++;
//			}
//			if (rowind[l] == i && l < colptr[second.rowind[k] + 1])
//			{
//				result[i] += vals[l] * second.vals[k];
//			}
//        }
//    }
//    for ( ; i < second.cols; i++)
//    {
//		result[i] = 0;
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//			l = colptr[second.rowind[k] + 1] - 1;
//			while (rowind[l] > i && l >= colptr[second.rowind[k]])
//			{
//				l--;
//			}
//			if (rowind[l] == i && l >= colptr[second.rowind[k]])
//			{
//				result[i] += vals[l] * second.vals[k];
//			}
//		}
//    }
//
//	return result;
//}
//
//TReal* TSparseMatrix::multiply_three_returning_diagonal(const TSparseMatrix& second, const TSparseMatrix& third) const
//{
//	TReal* result = new TReal[third.cols];
//	int i, k, j, l, thirdN = third.cols / 2;
//    for (i = 0; i < thirdN; i++)
//    {
//		result[i] = 0;
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				j = colptr[second.rowind[l]];
//				while (rowind[j] < i && j < colptr[second.rowind[l] + 1])
//				{
//					j++;
//				}
//				if (rowind[j] == i && j < colptr[second.rowind[l] + 1])
//				{
//					result[i] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//		}
//	}
//    for ( ; i < third.cols; i++)
//    {
//		result[i] = 0;
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				j = colptr[second.rowind[l] + 1] - 1;
//				while (rowind[j] > i && j >= colptr[second.rowind[l]])
//				{
//					j--;
//				}
//				if (rowind[j] == i && j >= colptr[second.rowind[l]])
//				{
//					result[i] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//		}
//	}
//	return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_three_F(const TSparseMatrix& second, const TSparseMatrix& third) const
//{
//	TReal *resultColumn = new TReal[rows];
//	Vector<TReal> results(rows * third.cols / 2);
//	Vector<int> rowInds(rows * third.cols / 2);
//    int i, k, l, j;
//    TSparseMatrix *result = new TSparseMatrix(rows, third.cols);
//    result->colptr[0] = 0;
//    for (i = 0; i < rows; i++)
//    {
//		resultColumn[i] = 0;
//    }
//    for (i = 0; i < third.cols; i++)
//    {
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				for (j = colptr[second.rowind[l]]; j < colptr[second.rowind[l] + 1]; j++)
//				{
//					resultColumn[rowind[j]] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                results.add(resultColumn[k]);
//                rowInds.add(k);
//				resultColumn[k] = 0;
//            }
//        }
//		result->colptr[i + 1] = results.size();
//    }
//
//	delete[] resultColumn;
//
//    result->setNNZ(results.size());
//	for (i = 0; i < results.size(); i++)
//	{
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//	}
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_three_LM(const TSparseMatrix& second, const TSparseMatrix& third) const
//{
//	TReal *resultColumn = new TReal[rows];
//    int i, k, l, j;
//    TSparseMatrix *result;
//    int nnz = 0;
//    for (i = 0; i < rows; i++)
//    {
//		resultColumn[i] = 0;
//    }
//    for (i = 0; i < third.cols; i++)
//    {
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				for (j = colptr[second.rowind[l]]; j < colptr[second.rowind[l] + 1]; j++)
//				{
//					resultColumn[rowind[j]] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                nnz++;
//				resultColumn[k] = 0;
//            }
//        }
//    }
//    result = new TSparseMatrix(rows, third.cols, nnz);
//    result->colptr[0] = 0;
//    int count = 0;
//    for (i = 0; i < third.cols; i++)
//    {
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				for (j = colptr[second.rowind[l]]; j < colptr[second.rowind[l] + 1]; j++)
//				{
//					resultColumn[rowind[j]] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                result->vals[count] = resultColumn[k];
//                result->rowind[count++] = k;
//				resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = count;
//    }
//
//	delete[] resultColumn;
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_three_returning_lower_triangular_F(const TSparseMatrix& second, const TSparseMatrix& third) const
//{
//    TReal *resultColumn = new TReal[rows];
//    Vector<TReal> results((rows * third.cols) / 2 + rows);
//    Vector<int> rowInds((rows * third.cols) / 2 + rows);
//    int i, k, l, j;
//    TSparseMatrix *result = new TSparseMatrix(rows, third.cols);
//    result->colptr[0] = 0;
//    for (i = 0; i < rows; i++)
//    {
//        resultColumn[i] = 0;
//    }
//    for (i = 0; i < third.cols; i++)
//    {
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//                for (j = colptr[second.rowind[l] + 1] - 1; j >= colptr[second.rowind[l]] && rowind[j] >= i; j--)
//                {
//                    resultColumn[rowind[j]] += third.vals[k] * second.vals[l] * vals[j];
//                }
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                results.add(resultColumn[k]);
//                rowInds.add(k);
//                resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = results.size();
//    }
//
//    delete[] resultColumn;
//
//    result->setNNZ(results.size());
//    for (i = 0; i < results.size(); i++)
//    {
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//    }
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_three_returning_lower_triangular_LM(const TSparseMatrix& second, const TSparseMatrix& third) const
//{
//	TReal *resultColumn = new TReal[rows];
//    int i, k, l, j;
//    TSparseMatrix *result;
//    int nnz = 0;
//    for (i = 0; i < rows; i++)
//    {
//		resultColumn[i] = 0;
//    }
//    for (i = 0; i < third.cols; i++)
//    {
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				for (j = colptr[second.rowind[l] + 1] - 1; j >= colptr[second.rowind[l]] && rowind[j] >= i; j--)
//				{
//					resultColumn[rowind[j]] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                nnz++;
//				resultColumn[k] = 0;
//            }
//        }
//    }
//    result = new TSparseMatrix(rows, third.cols, nnz);
//    result->colptr[0] = 0;
//    int count = 0;
//    for (i = 0; i < third.cols; i++)
//    {
//        for (k = third.colptr[i]; k < third.colptr[i + 1]; k++)
//        {
//            for (l = second.colptr[third.rowind[k]]; l < second.colptr[third.rowind[k] + 1]; l++)
//            {
//				for (j = colptr[second.rowind[l] + 1] - 1; j >= colptr[second.rowind[l]] && rowind[j] >= i; j--)
//				{
//					resultColumn[rowind[j]] += third.vals[k] * second.vals[l] * vals[j];
//				}
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                result->vals[count] = resultColumn[k];
//                result->rowind[count++] = k;
//				resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = count;
//    }
//
//	delete[] resultColumn;
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_returning_lower_triangular_F(const TSparseMatrix& second) const
//{
//    TReal *resultColumn = new TReal[rows];
//    Vector<TReal> results((rows * second.cols) / 2 + rows);
//    Vector<int> rowInds((rows * second.cols) / 2 + rows);
//    int i, k, l;
//    TSparseMatrix *result = new TSparseMatrix(rows, second.cols);
//    result->colptr[0] = 0;
//    for (i = 0; i < rows; i++)
//    {
//        resultColumn[i] = 0;
//    }
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k] + 1] - 1; l >= colptr[second.rowind[k]] && rowind[l] >= i; l--)
//            {
//                resultColumn[rowind[l]] += second.vals[k] * vals[l];
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                results.add(resultColumn[k]);
//                rowInds.add(k);
//                resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = results.size();
//    }
//
//    delete[] resultColumn;
//
//    result->setNNZ(results.size());
//    for (i = 0; i < results.size(); i++)
//    {
//        result->vals[i] = results[i];
//        result->rowind[i] = rowInds[i];
//    }
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::multiply_returning_lower_triangular_LM(const TSparseMatrix& second) const
//{
//	TReal *resultColumn = new TReal[rows];
//    int i, k, l;
//    TSparseMatrix *result;
//    int nnz = 0;
//    for (i = 0; i < rows; i++)
//    {
//		resultColumn[i] = 0;
//    }
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k] + 1] - 1; l >= colptr[second.rowind[k]] && rowind[l] >= i; l--)
//            {
//                resultColumn[rowind[l]] += second.vals[k] * vals[l];
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                nnz++;
//                resultColumn[k] = 0;
//            }
//        }
//    }
//    result = new TSparseMatrix(rows, second.cols, nnz);
//    result->colptr[0] = 0;
//    int count = 0;
//    for (i = 0; i < second.cols; i++)
//    {
//        for (k = second.colptr[i]; k < second.colptr[i + 1]; k++)
//        {
//            for (l = colptr[second.rowind[k] + 1] - 1; l >= colptr[second.rowind[k]] && rowind[l] >= i; l--)
//            {
//                resultColumn[rowind[l]] += second.vals[k] * vals[l];
//            }
//        }
//        for (k = 0; k < rows; k++)
//        {
//            if (resultColumn[k] != 0)
//            {
//                result->vals[count] = resultColumn[k];
//                result->rowind[count++] = k;
//				resultColumn[k] = 0;
//            }
//        }
//        result->colptr[i + 1] = count;
//    }
//
//	delete[] resultColumn;
//
//    return result;
//}
//
//TSparseMatrix* TSparseMatrix::add(const TSparseMatrix& second) const
//{
//    int first = 0, col = 1, sec = 0;
//    int total = 0;
//
//    while (col <= cols)
//    {
//        while (first < colptr[col] && sec < second.colptr[col])
//        {
//            if (rowind[first] == second.rowind[sec] && vals[first++] + second.vals[sec++] != 0)
//            {
//	            total++;
//            }
//            else if (rowind[first] < second.rowind[sec])
//            {
//				first++;
//				total++;
//            }
//            else
//            {
//				sec++;
//				total++;
//            }
//        }
//        total += colptr[col] - first + second.colptr[col] - sec;
//        first = colptr[col];
//        sec = second.colptr[col++];
//    }
//
//    TSparseMatrix* result = new TSparseMatrix(rows, cols, total);
//    first = 0;
//    col = 1;
//    sec = 0;
//    total = 0;
//
//	int colp = 1;
//
//	result->colptr[0] = 0;
//    while (col <= cols)
//    {
//        while (first < colptr[col] && sec < second.colptr[col])
//        {
//			TReal res;
//            if (rowind[first] == second.rowind[sec])
//            {
//				res = vals[first] + second.vals[sec++];
//				if (res != 0)
//				{
//	                result->rowind[total] = rowind[first];
//	                result->vals[total++] = res;
//				}
//				first++;
//            }
//            else if (rowind[first] < second.rowind[sec])
//            {
//                result->rowind[total] = rowind[first];
//                result->vals[total++] = vals[first++];
//            }
//            else
//            {
//                result->rowind[total] = second.rowind[sec];
//                result->vals[total++] = second.vals[sec++];
//            }
//        }
//		while (first < colptr[col])
//		{
//            result->rowind[total] = rowind[first];
//            result->vals[total++] = vals[first++];
//		}
//		while (sec < second.colptr[col])
//		{
//            result->rowind[total] = second.rowind[sec];
//            result->vals[total++] = second.vals[sec++];
//		}
//		col++;
//		result->colptr[colp++] = total;
//    }
//
//    return result;
//}
//
//void TSparseMatrix::multiply_by_number(TReal n)
//{
//	for (int i = 0; i < colptr[cols]; i++)
//	{
//		vals[i] *= n;
//	}
//}
//
//void TSparseMatrix::write_matrix_file(const char *filename) const
//{
//	FILE *f = fopen(filename, "wt");
//	fprintf(f, "%d %d %d\n", rows, cols, colptr[cols]);
//	for (int i = 0; i < cols; i++)
//	{
//		for (int j = colptr[i]; j < colptr[i + 1]; j++)
//		{
//			fprintf(f, "%d %d %.20e\n", rowind[j] + 1, i + 1, (double) vals[j]);
//		}
//	}
//	fclose(f);
//}
//
//TSparseMatrix* TSparseMatrix::read_matrix_file(const char* filename)
//{
//	FILE *f = fopen(filename, "rt");
//
//	int row = 0, column = 0, nnz = 0;
//	fscanf(f, "%d %d %d\n", &row, &column, &nnz);
//	TSparseMatrix* matrix = new TSparseMatrix(row, column, nnz);
//
//	int count = 0;
//	int colptr = 1;
//	matrix->colptr[0] = 0;
//	int oldcol = 1;
//	while (!feof(f))
//	{
//		double temp;
//		fscanf(f, "%d %d %lf\n", &row, &column, &temp);
//
//		while (oldcol != column)
//		{
//			matrix->colptr[colptr++] = count;
//			oldcol++;
//		}
//
//		matrix->rowind[count] = row - 1;
//		matrix->vals[count++] = temp;
//	}
//	while (colptr <= matrix->columnsCount())
//	{
//		matrix->colptr[colptr++] = count;
//	}
//	fclose(f);
//
//	return matrix;
//}
//
//TMatrix* TSparseMatrix::to_dense() const
//{
//	TMatrix* res = new TMatrix(rows, cols);
//	*res = 0;
//
//	for (int i = 0; i < cols; i++)
//	{
//		for (int j = colptr[i]; j < colptr[i + 1]; j++)
//		{
//			(*res)(rowind[j], i) = vals[j];
//		}
//	}
//
//	return res;
//}
