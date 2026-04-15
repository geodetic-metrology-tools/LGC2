// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PYLGC_C_H
#define PYLGC_C_H

#ifdef _WIN32
#   ifdef PYLGC_C_EXPORTS
#       define PYLGC_API __declspec(dllexport)
#   else
#       define PYLGC_API __declspec(dllimport)
#   endif
#else
#   define PYLGC_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LGCEvaluator;
typedef void* LGCPoint;
typedef void* LGCFrame;

/* Error handling — returns message from last failed call (thread-local). */
PYLGC_API const char* lgcGetLastError(void);

/* Memory management — caller must free arrays returned by the API. */
PYLGC_API void lgcFreeDoubleArray(double* ptr);
PYLGC_API void lgcFreeIntArray(int* ptr);

/* Evaluator lifecycle */
PYLGC_API LGCEvaluator lgcEvaluatorCreate(const char* filePath);
PYLGC_API void           lgcEvaluatorDestroy(LGCEvaluator ev);

/* Evaluation & parameters — return 0 on success, -1 on error.
   lgcEvaluatorEvaluate returns 1=true, 0=false, -1=error. */
PYLGC_API int lgcEvaluatorEvaluate(LGCEvaluator ev);
PYLGC_API int lgcEvaluatorSetParameters(LGCEvaluator ev, const double* data, int len);
PYLGC_API int lgcEvaluatorGetEstParams(LGCEvaluator ev, double** outData, int* outLen);

/* Indices (UEOIndices) */
PYLGC_API int lgcEvaluatorGetIndices(LGCEvaluator ev, int* u, int* e, int* o, int* c);

/* Vectors */
PYLGC_API int lgcEvaluatorGetMisclosure(LGCEvaluator ev, double** outData, int* outLen);
PYLGC_API int lgcEvaluatorGetConstraintMisclosure(LGCEvaluator ev, double** outData, int* outLen);

/* Sparse matrices as COO triplets (rows, cols, vals arrays of length nnz) */
PYLGC_API int lgcEvaluatorGetAMatrix(LGCEvaluator ev, int** outRows, int** outCols, double** outVals, int* outNnz, int* outNrows, int* outNcols);
PYLGC_API int lgcEvaluatorGetBMatrix(LGCEvaluator ev, int** outRows, int** outCols, double** outVals, int* outNnz, int* outNrows, int* outNcols);
PYLGC_API int lgcEvaluatorGetInvBMatrix(LGCEvaluator ev, int** outRows, int** outCols, double** outVals, int* outNnz, int* outNrows, int* outNcols);
PYLGC_API int lgcEvaluatorGetA2Matrix(LGCEvaluator ev, int** outRows, int** outCols, double** outVals, int* outNnz, int* outNrows, int* outNcols);
PYLGC_API int lgcEvaluatorGetPMatrix(LGCEvaluator ev, int** outRows, int** outCols, double** outVals, int* outNnz, int* outNrows, int* outNcols);

/* Dense matrix (row-major flat array, size = nrows * ncols) */
PYLGC_API int lgcEvaluatorGetFiniteDifferenceA(LGCEvaluator ev, double epsilon, double** outData, int* outNrows, int* outNcols);

/* Solve — returns 0 on success (-1 on error); outOk is 1 if solve converged. */
PYLGC_API int lgcEvaluatorTrySolve(LGCEvaluator ev, int* outOk, double** outSolution, int* outLen);

/* Obs-index-to-line-number mapping (parallel key/value arrays) */
PYLGC_API int lgcEvaluatorGetObsIndexToLineNumber(LGCEvaluator ev, int** outKeys, int** outValues, int* outLen);

/* Point access — returns NULL on error (check lgcGetLastError).
   Returned handle is borrowed (valid as long as the evaluator lives). */
PYLGC_API LGCPoint  lgcEvaluatorGetPoint(LGCEvaluator ev, const char* name);
PYLGC_API const char* lgcPointGetName(LGCPoint pt);
PYLGC_API int         lgcPointGetFirstUidx(LGCPoint pt);
PYLGC_API int         lgcPointGetRelativeUnknIndices(LGCPoint pt, int** outData, int* outLen);
PYLGC_API int         lgcPointGetEstVector(LGCPoint pt, double** outData, int* outLen);

/* Frame access — same lifetime semantics as points. */
PYLGC_API LGCFrame  lgcEvaluatorGetFrame(LGCEvaluator ev, const char* name);
PYLGC_API const char* lgcFrameGetName(LGCFrame fr);
PYLGC_API int         lgcFrameGetFirstUidx(LGCFrame fr);
PYLGC_API int         lgcFrameGetRelativeUnknIndices(LGCFrame fr, int** outData, int* outLen);
PYLGC_API int         lgcFrameGetEstVector(LGCFrame fr, double** outData, int* outLen);

#ifdef __cplusplus
}
#endif

#endif /* PYLGC_C_H */
