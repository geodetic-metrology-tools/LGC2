// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pyLGC_C.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <LGCAdjustablePoint.h>
#include <TAdjustableHelmertTransformation.h>
#include <TLGCData.h>
#include <TLSEvaluator.h>
#include <TSparseMatrix.h>
#include <UEOIndices.h>

static thread_local std::string g_lastError;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void eigenVecToArray(const Eigen::VectorXd &vec, double **out, int *len)
{
	*len = static_cast<int>(vec.size());
	*out = new double[*len];
	std::copy(vec.data(), vec.data() + *len, *out);
}

static int extractSparse(const TSparseMatrix &mat, int **out_rows, int **out_cols, double **out_vals, int *out_nnz, int *out_nrows, int *out_ncols)
{
	int nnz = static_cast<int>(mat.nonZeros());
	*out_nnz = nnz;
	*out_nrows = static_cast<int>(mat.rows());
	*out_ncols = static_cast<int>(mat.cols());
	*out_rows = new int[nnz];
	*out_cols = new int[nnz];
	*out_vals = new double[nnz];

	int idx = 0;
	for (int k = 0; k < mat.outerSize(); ++k)
		for (TSparseMatrix::InnerIterator it(mat, k); it; ++it)
		{
			(*out_rows)[idx] = static_cast<int>(it.row());
			(*out_cols)[idx] = static_cast<int>(it.col());
			(*out_vals)[idx] = it.value();
			++idx;
		}
	return 0;
}

#define EV(handle) (*static_cast<TLSEvaluator *>(handle))
#define PT(handle) (*static_cast<LGCAdjustablePoint *>(handle))
#define FR(handle) (*static_cast<TAdjustableHelmertTransformation *>(handle))

#define CATCH_ERR \
	catch (const std::exception &ex) \
	{ \
		g_lastError = ex.what(); \
		return -1; \
	} \
	catch (...) \
	{ \
		g_lastError = "Unknown error"; \
		return -1; \
	}
#define CATCH_NULL \
	catch (const std::exception &ex) \
	{ \
		g_lastError = ex.what(); \
		return nullptr; \
	} \
	catch (...) \
	{ \
		g_lastError = "Unknown error"; \
		return nullptr; \
	}

// ---------------------------------------------------------------------------
// C API
// ---------------------------------------------------------------------------

extern "C"
{
	const char *lgcGetLastError(void)
	{
		return g_lastError.c_str();
	}

	void lgcFreeDoubleArray(double *ptr)
	{
		delete[] ptr;
	}
	void lgcFreeIntArray(int *ptr)
	{
		delete[] ptr;
	}

	// --- Evaluator lifecycle ---------------------------------------------------

	LGCEvaluator lgcEvaluatorCreate(const char *file_path)
	{
		try
		{
			std::ifstream file(file_path);
			if (!file.is_open())
			{
				g_lastError = std::string("Cannot open file: ") + file_path;
				return nullptr;
			}
			std::stringstream ss;
			ss << file.rdbuf();
			return new TLSEvaluator(ss);
		}
		CATCH_NULL
	}

	void lgcEvaluatorDestroy(LGCEvaluator ev)
	{
		delete static_cast<TLSEvaluator *>(ev);
	}

	// --- Evaluation & parameters -----------------------------------------------

	int lgcEvaluatorEvaluate(LGCEvaluator ev)
	{
		try
		{
			return EV(ev).evaluate() ? 1 : 0;
		}
		CATCH_ERR
	}

	int lgcEvaluatorSetParameters(LGCEvaluator ev, const double *data, int len)
	{
		try
		{
			Eigen::VectorXd params = Eigen::Map<const Eigen::VectorXd>(data, len);
			EV(ev).setParameters(params);
			return 0;
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetEstParams(LGCEvaluator ev, double **out_data, int *out_len)
	{
		try
		{
			eigenVecToArray(EV(ev).getEstParams(), out_data, out_len);
			return 0;
		}
		CATCH_ERR
	}

	// --- Indices ---------------------------------------------------------------

	int lgcEvaluatorGetIndices(LGCEvaluator ev, int *u, int *e, int *o, int *c)
	{
		try
		{
			UEOIndices idx = EV(ev).getIndices();
			*u = idx.UIndex;
			*e = idx.EIndex;
			*o = idx.OIndex;
			*c = idx.CIndex;
			return 0;
		}
		CATCH_ERR
	}

	// --- Vectors ---------------------------------------------------------------

	int lgcEvaluatorGetMisclosure(LGCEvaluator ev, double **out_data, int *out_len)
	{
		try
		{
			eigenVecToArray(EV(ev).getMisclosure(), out_data, out_len);
			return 0;
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetConstraintMisclosure(LGCEvaluator ev, double **out_data, int *out_len)
	{
		try
		{
			eigenVecToArray(EV(ev).getConstraintMisclosure(), out_data, out_len);
			return 0;
		}
		CATCH_ERR
	}

	// --- Sparse matrices -------------------------------------------------------

	int lgcEvaluatorGetAMatrix(LGCEvaluator ev, int **r, int **c, double **v, int *nnz, int *nr, int *nc)
	{
		try
		{
			return extractSparse(EV(ev).getAMatrix(), r, c, v, nnz, nr, nc);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetBMatrix(LGCEvaluator ev, int **r, int **c, double **v, int *nnz, int *nr, int *nc)
	{
		try
		{
			return extractSparse(EV(ev).getBMatrix(), r, c, v, nnz, nr, nc);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetInvBMatrix(LGCEvaluator ev, int **r, int **c, double **v, int *nnz, int *nr, int *nc)
	{
		try
		{
			return extractSparse(EV(ev).getInvBMatrix(), r, c, v, nnz, nr, nc);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetA2Matrix(LGCEvaluator ev, int **r, int **c, double **v, int *nnz, int *nr, int *nc)
	{
		try
		{
			return extractSparse(EV(ev).getA2Matrix(), r, c, v, nnz, nr, nc);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetPMatrix(LGCEvaluator ev, int **r, int **c, double **v, int *nnz, int *nr, int *nc)
	{
		try
		{
			return extractSparse(EV(ev).getPMatrix(), r, c, v, nnz, nr, nc);
		}
		CATCH_ERR
	}

	// --- Dense matrix ----------------------------------------------------------

	int lgcEvaluatorGetFiniteDifferenceA(LGCEvaluator ev, double epsilon, double **out_data, int *out_nrows, int *out_ncols)
	{
		try
		{
			Eigen::MatrixXd A = EV(ev).getFiniteDifferenceA(epsilon);
			*out_nrows = static_cast<int>(A.rows());
			*out_ncols = static_cast<int>(A.cols());
			int n = (*out_nrows) * (*out_ncols);
			*out_data = new double[n];
			// row-major for easy consumption from Python
			for (int i = 0; i < *out_nrows; ++i)
				for (int j = 0; j < *out_ncols; ++j)
					(*out_data)[i * (*out_ncols) + j] = A(i, j);
			return 0;
		}
		CATCH_ERR
	}

	// --- Solve -----------------------------------------------------------------

	int lgcEvaluatorTrySolve(LGCEvaluator ev, int *out_ok, double **out_solution, int *out_len)
	{
		try
		{
			Eigen::VectorXd solution;
			bool ok = EV(ev).tryLGCSolve(solution);
			*out_ok = ok ? 1 : 0;
			eigenVecToArray(solution, out_solution, out_len);
			return 0;
		}
		CATCH_ERR
	}

	// --- Obs index mapping -----------------------------------------------------

	int lgcEvaluatorGetObsIndexToLineNumber(LGCEvaluator ev, int **out_keys, int **out_values, int *out_len)
	{
		try
		{
			const auto &mapping = EV(ev).getObsIndexToLineNumber();
			*out_len = static_cast<int>(mapping.size());
			*out_keys = new int[*out_len];
			*out_values = new int[*out_len];
			int i = 0;
			for (const auto &kv : mapping)
			{
				(*out_keys)[i] = kv.first;
				(*out_values)[i] = kv.second;
				++i;
			}
			return 0;
		}
		CATCH_ERR
	}

	// --- Point access ----------------------------------------------------------

	LGCPoint lgcEvaluatorGetPoint(LGCEvaluator ev, const char *name)
	{
		try
		{
			const std::string sname(name);
			if (!EV(ev).getData().getPoints().doesObjectExist(sname))
			{
				g_lastError = "Point " + sname + " not found";
				return nullptr;
			}
			return const_cast<LGCAdjustablePoint *>(&EV(ev).getData().getPoints().getObject(sname));
		}
		CATCH_NULL
	}

	const char *lgcPointGetName(LGCPoint pt)
	{
		return PT(pt).getName().c_str();
	}

	int lgcPointGetFirstUidx(LGCPoint pt)
	{
		return PT(pt).getFirstUidx();
	}

	int lgcPointGetRelativeUnknIndices(LGCPoint pt, int **out_data, int *out_len)
	{
		try
		{
			auto indices = PT(pt).getRelativeUnknIndices();
			*out_len = static_cast<int>(indices.size());
			*out_data = new int[*out_len];
			std::copy(indices.begin(), indices.end(), *out_data);
			return 0;
		}
		CATCH_ERR
	}

	int lgcPointGetEstVector(LGCPoint pt, double **out_data, int *out_len)
	{
		try
		{
			eigenVecToArray(PT(pt).getEstVector(), out_data, out_len);
			return 0;
		}
		CATCH_ERR
	}

	// --- Frame access ----------------------------------------------------------

	LGCFrame lgcEvaluatorGetFrame(LGCEvaluator ev, const char *name)
	{
		try
		{
			auto iter = EV(ev).getData().locateNode(std::string(name));
			return &(iter.node->data.get()->frame);
		}
		CATCH_NULL
	}

	const char *lgcFrameGetName(LGCFrame fr)
	{
		return FR(fr).getName().c_str();
	}

	int lgcFrameGetFirstUidx(LGCFrame fr)
	{
		return FR(fr).getFirstUidx();
	}

	int lgcFrameGetRelativeUnknIndices(LGCFrame fr, int **out_data, int *out_len)
	{
		try
		{
			auto indices = FR(fr).getRelativeUnknIndices();
			*out_len = static_cast<int>(indices.size());
			*out_data = new int[*out_len];
			std::copy(indices.begin(), indices.end(), *out_data);
			return 0;
		}
		CATCH_ERR
	}

	int lgcFrameGetEstVector(LGCFrame fr, double **out_data, int *out_len)
	{
		try
		{
			eigenVecToArray(FR(fr).getEstVector(), out_data, out_len);
			return 0;
		}
		CATCH_ERR
	}

} // extern "C"
