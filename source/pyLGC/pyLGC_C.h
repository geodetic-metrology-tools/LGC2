// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PYLGC_C_H
#define PYLGC_C_H

#ifdef _WIN32
#	ifdef PYLGC_C_EXPORTS
#		define PYLGC_API __declspec(dllexport)
#	else
#		define PYLGC_API __declspec(dllimport)
#	endif
#else
#	define PYLGC_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	typedef void *LGCEvaluator;
	typedef void *LGCAdjObj;

	/* Error handling — returns message from last failed call (thread-local). */
	PYLGC_API const char *lgcGetLastError(void);

	/* Memory management — caller must free arrays returned by the API. */
	PYLGC_API void lgcFreeDoubleArray(double *ptr);
	PYLGC_API void lgcFreeIntArray(int *ptr);

	/* Evaluator lifecycle */
	PYLGC_API LGCEvaluator lgcEvaluatorCreate(const char *filePath);
	PYLGC_API void lgcEvaluatorDestroy(LGCEvaluator ev);

	/* Evaluation & parameters — return 0 on success, -1 on error.
	   lgcEvaluatorEvaluate returns 1=true, 0=false, -1=error. */
	PYLGC_API int lgcEvaluatorEvaluate(LGCEvaluator ev);
	PYLGC_API int lgcEvaluatorSetParameters(LGCEvaluator ev, const double *data, int len);
	PYLGC_API int lgcEvaluatorGetEstimatedParameters(LGCEvaluator ev, double **outData, int *outLen);

	/* Indices (UEOIndices) */
	PYLGC_API int lgcEvaluatorGetProblemDimensions(LGCEvaluator ev, int *u, int *e, int *o, int *c);

	/* Vectors */
	PYLGC_API int lgcEvaluatorGetMisclosure(LGCEvaluator ev, double **outData, int *outLen);
	PYLGC_API int lgcEvaluatorGetConstraintMisclosure(LGCEvaluator ev, double **outData, int *outLen);

	/* Sparse matrices as COO triplets (rows, cols, vals arrays of length nnz) */
	PYLGC_API int lgcEvaluatorGetFirstDesignMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols);
	PYLGC_API int lgcEvaluatorGetSecondDesignMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols);
	PYLGC_API int lgcEvaluatorGetConstraintDesignMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols);
	PYLGC_API int lgcEvaluatorGetWeightMatrix(LGCEvaluator ev, int **outRows, int **outCols, double **outVals, int *outNnz, int *outNrows, int *outNcols);

	/* Solve — returns 0 on success (-1 on error); outOk is 1 if solve converged. */
	PYLGC_API int lgcEvaluatorSolve(LGCEvaluator ev, int *outOk, double **outSolution, int *outLen);

	/* Obs-index-to-line-number mapping (parallel key/value arrays) */
	PYLGC_API int lgcEvaluatorGetObsIndexToLineNumber(LGCEvaluator ev, int **outKeys, int **outValues, int *outLen);

	/* Adjustable-object access (points & frames) — returns NULL on error
	   (check lgcGetLastError). Returned handle is borrowed (valid as long as the
	   evaluator lives). */
	PYLGC_API LGCAdjObj lgcEvaluatorGetPoint(LGCEvaluator ev, const char *name);
	PYLGC_API LGCAdjObj lgcEvaluatorGetFrame(LGCEvaluator ev, const char *name);
	PYLGC_API const char *lgcAdjObjGetName(LGCAdjObj obj);
	PYLGC_API int lgcAdjObjGetFirstUidx(LGCAdjObj obj);
	PYLGC_API int lgcAdjObjGetRelativeUnknIndices(LGCAdjObj obj, int **outData, int *outLen);
	PYLGC_API int lgcAdjObjGetEstVector(LGCAdjObj obj, double **outData, int *outLen);

#ifdef __cplusplus
}
#endif

#endif /* PYLGC_C_H */
