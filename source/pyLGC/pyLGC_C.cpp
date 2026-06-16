// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pyLGC_C.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <LGCAdjustablePoint.h>
#include <TAdjustableHelmertTransformation.h>
#include <TLGCData.h>
#include <TLSEvaluator.h>
#include <TSparseMatrix.h>
#include <TVAdjustableObject.h>
#include <UEOIndices.h>

static thread_local std::string lastError;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void eigenVecToArray(const Eigen::VectorXd &vec, double **out, int *len)
{
	*len = static_cast<int>(vec.size());
	*out = new double[*len];
	std::copy(vec.data(), vec.data() + *len, *out);
}

static int extractSparse(const TSparseMatrix &mat, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols)
{
	int nnz = static_cast<int>(mat.nonZeros());
	*outNnz = nnz;
	*outNrows = static_cast<int>(mat.rows());
	*outNcols = static_cast<int>(mat.cols());

	std::unique_ptr<int[]> rows(new int[nnz]);
	std::unique_ptr<int[]> cols(new int[nnz]);
	std::unique_ptr<double[]> vals(new double[nnz]);

	int idx = 0;
	for (int k = 0; k < mat.outerSize(); ++k)
		for (TSparseMatrix::InnerIterator it(mat, k); it; ++it)
		{
			rows[idx] = static_cast<int>(it.row());
			cols[idx] = static_cast<int>(it.col());
			vals[idx] = it.value();
			++idx;
		}
	*outRows = rows.release();
	*outCols = cols.release();
	*outVals = vals.release();
	return 0;
}

#define EV(handle) (*static_cast<TLSEvaluator *>(handle))
#define ADJ(handle) (*static_cast<TVAdjustableObject *>(handle))

#define CATCH_ERR \
	catch (const std::exception &ex) \
	{ \
		lastError = ex.what(); \
		return -1; \
	} \
	catch (...) \
	{ \
		lastError = "Unknown error"; \
		return -1; \
	}
#define CATCH_NULL \
	catch (const std::exception &ex) \
	{ \
		lastError = ex.what(); \
		return nullptr; \
	} \
	catch (...) \
	{ \
		lastError = "Unknown error"; \
		return nullptr; \
	}

// ---------------------------------------------------------------------------
// C API
// ---------------------------------------------------------------------------

extern "C"
{
	const char *lgcGetLastError(void)
	{
		return lastError.c_str();
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

	LGCEvaluator lgcEvaluatorCreate(const char *filePath)
	{
		try
		{
			std::ifstream file(filePath);
			if (!file.is_open())
			{
				lastError = std::string("Cannot open file: ") + filePath;
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

	int lgcEvaluatorGetEstimatedParameters(LGCEvaluator ev, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(EV(ev).getEstParams(), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Indices ---------------------------------------------------------------

	int lgcEvaluatorGetProblemDimensions(LGCEvaluator ev, int *u, int *e, int *o, int *c)
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

	int lgcEvaluatorGetMisclosure(LGCEvaluator ev, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(EV(ev).getMisclosure(), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetConstraintMisclosure(LGCEvaluator ev, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(EV(ev).getConstraintMisclosure(), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Sparse matrices -------------------------------------------------------

	int lgcEvaluatorGetFirstDesignMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols)
	{
		try
		{
			return extractSparse(EV(ev).getAMatrix(), outRows, outCols, outVals, outNnz, outNrows, outNcols);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetSecondDesignMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols)
	{
		try
		{
			return extractSparse(EV(ev).getBMatrix(), outRows, outCols, outVals, outNnz, outNrows, outNcols);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetConstraintDesignMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols)
	{
		try
		{
			return extractSparse(EV(ev).getA2Matrix(), outRows, outCols, outVals, outNnz, outNrows, outNcols);
		}
		CATCH_ERR
	}

	int lgcEvaluatorGetWeightMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols)
	{
		try
		{
			return extractSparse(EV(ev).getPMatrix(), outRows, outCols, outVals, outNnz, outNrows, outNcols);
		}
		CATCH_ERR
	}

	// --- Solve -----------------------------------------------------------------

	int lgcEvaluatorSolve(LGCEvaluator ev, int *outOk, double **outSolution, int *outLen)
	{
		try
		{
			Eigen::VectorXd solution;
			bool ok = EV(ev).tryLGCSolve(solution);
			*outOk = ok ? 1 : 0;
			eigenVecToArray(solution, outSolution, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Obs index mapping -----------------------------------------------------

	int lgcEvaluatorGetObsIndexToLineNumber(LGCEvaluator ev, int **outKeys, int **outValues, int *outLen)
	{
		try
		{
			const auto &mapping = EV(ev).getObsIndexToLineNumber();
			*outLen = static_cast<int>(mapping.size());
			std::unique_ptr<int[]> keys(new int[*outLen]);
			std::unique_ptr<int[]> values(new int[*outLen]);
			int i = 0;
			for (const auto &kv : mapping)
			{
				keys[i] = kv.first;
				values[i] = kv.second;
				++i;
			}
			*outKeys = keys.release();
			*outValues = values.release();
			return 0;
		}
		CATCH_ERR
	}

	// --- Adjustable-object access (points & frames) ---------------------------

	LGCAdjObj lgcEvaluatorGetPoint(LGCEvaluator ev, const char *name)
	{
		try
		{
			const std::string sname(name);
			if (!EV(ev).getData().getPoints().doesObjectExist(sname))
			{
				lastError = "Point " + sname + " not found";
				return nullptr;
			}
			return const_cast<LGCAdjustablePoint *>(&EV(ev).getData().getPoints().getObject(sname));
		}
		CATCH_NULL
	}

	LGCAdjObj lgcEvaluatorGetFrame(LGCEvaluator ev, const char *name)
	{
		try
		{
			auto iter = EV(ev).getData().locateNode(std::string(name));
			return &(iter.node->data.get()->frame);
		}
		CATCH_NULL
	}

	const char *lgcAdjObjGetName(LGCAdjObj obj)
	{
		return ADJ(obj).getName().c_str();
	}

	int lgcAdjObjGetFirstUidx(LGCAdjObj obj)
	{
		return ADJ(obj).getFirstUidx();
	}

	int lgcAdjObjGetRelativeUnknIndices(LGCAdjObj obj, int **outData, int *outLen)
	{
		try
		{
			auto indices = ADJ(obj).getRelativeUnknIndices();
			*outLen = static_cast<int>(indices.size());
			*outData = new int[*outLen];
			std::copy(indices.begin(), indices.end(), *outData);
			return 0;
		}
		CATCH_ERR
	}

	int lgcAdjObjGetEstVector(LGCAdjObj obj, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(ADJ(obj).getEstVector(), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

} // extern "C"
