// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PYMONITORING_C_H
#define PYMONITORING_C_H

#ifdef _WIN32
#	ifdef PYMONITORING_C_EXPORTS
#		define PYMONITORING_API __declspec(dllexport)
#	else
#		define PYMONITORING_API __declspec(dllimport)
#	endif
#else
#	define PYMONITORING_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	typedef void *MonitoringHandle;

	/* Error handling — returns message from last failed call (thread-local). */
	PYMONITORING_API const char *moniGetLastError(void);

	/* Memory management — caller must free arrays returned by the API. */
	PYMONITORING_API void moniFreeDoubleArray(double *ptr);

	/* Lifecycle. moniCreate returns NULL on error (check moniGetLastError).
	   The remaining calls return 0 on success and -1 on error, unless noted. */
	PYMONITORING_API MonitoringHandle moniCreate(const char *inputFilePath);
	PYMONITORING_API void moniDestroy(MonitoringHandle h);
	PYMONITORING_API int moniReset(MonitoringHandle h);

	/* Execution. moniAdjust / moniGetStatus return 1=true, 0=false, -1=error. */
	PYMONITORING_API int moniAdjust(MonitoringHandle h);
	PYMONITORING_API int moniGetStatus(MonitoringHandle h);
	PYMONITORING_API int moniGetSigma0(MonitoringHandle h, double *out);
	PYMONITORING_API int moniWriteLGCInputFile(MonitoringHandle h);

	/* Observations. moniGetActivationStatus returns 1=true, 0=false, -1=error. */
	PYMONITORING_API int moniSetActivationStatus(MonitoringHandle h, const char *id, int status);
	PYMONITORING_API int moniGetActivationStatus(MonitoringHandle h, const char *id);
	PYMONITORING_API int moniUpdateMeas(MonitoringHandle h, const char *id, const double *data, int len);
	PYMONITORING_API int moniSetObsSigma(MonitoringHandle h, const char *id, const double *data, int len);
	PYMONITORING_API int moniGetMeas(MonitoringHandle h, const char *id, double **outData, int *outLen);
	PYMONITORING_API int moniGetEstimateResidual(MonitoringHandle h, const char *obsName, double **outData, int *outLen);
	PYMONITORING_API int moniGetCalcMeas(MonitoringHandle h, const char *obsName, double **outData, int *outLen);
	PYMONITORING_API int moniGetObsSigma(MonitoringHandle h, const char *obsName, double **outData, int *outLen);

	/* Parameter control (idx is the component index; see MonitoringImpl for conventions). */
	PYMONITORING_API int moniSetFixedPointParameter(MonitoringHandle h, const char *name, int idx, double val);
	PYMONITORING_API int moniSetFixedFrameParameter(MonitoringHandle h, const char *name, int idx, double val);
	PYMONITORING_API int moniSetFixedSagParameter(MonitoringHandle h, const char *name, int idx, double val);
	PYMONITORING_API int moniFreezePointParameter(MonitoringHandle h, const char *name, int idx, double val);
	PYMONITORING_API int moniFreezeFrameParameter(MonitoringHandle h, const char *name, int idx, double val);
	PYMONITORING_API int moniFreezeSagParameter(MonitoringHandle h, const char *name, int idx, double val);
	PYMONITORING_API int moniUnfreezePointParameter(MonitoringHandle h, const char *name, int idx);
	PYMONITORING_API int moniUnfreezeFrameParameter(MonitoringHandle h, const char *name, int idx);
	PYMONITORING_API int moniUnfreezeSagParameter(MonitoringHandle h, const char *name, int idx);

	/* Estimation results (heap-allocated vectors — free with moniFreeDoubleArray). */
	PYMONITORING_API int moniGetPointEstimate(MonitoringHandle h, const char *name, double **outData, int *outLen);
	PYMONITORING_API int moniGetPointEstimateInFrame(MonitoringHandle h, const char *name, const char *frame, double **outData, int *outLen);
	PYMONITORING_API int moniGetFrameEstimate(MonitoringHandle h, const char *name, double **outData, int *outLen);
	PYMONITORING_API int moniGetSagEstimate(MonitoringHandle h, const char *name, double **outData, int *outLen);
	PYMONITORING_API int moniGetPointEstimatePrec(MonitoringHandle h, const char *name, double **outData, int *outLen);
	PYMONITORING_API int moniGetPointEstimatePrecInFrame(MonitoringHandle h, const char *name, const char *frame, double **outData, int *outLen);
	PYMONITORING_API int moniGetFrameEstimatePrec(MonitoringHandle h, const char *name, double **outData, int *outLen);
	PYMONITORING_API int moniGetSagEstimatePrec(MonitoringHandle h, const char *name, double **outData, int *outLen);

	/* Transformations — in3 and out3 are caller-allocated arrays of length 3. */
	PYMONITORING_API int moniTransformCoordinates(MonitoringHandle h, const double *in3, const char *from, const char *to, double *out3);
	PYMONITORING_API int moniTransformDirection(MonitoringHandle h, const double *in3, const char *from, const char *to, double *out3);

	/* Round-of-measurement results.
	   ECWS (water): two scalar out-params.
	   ECWI (wire): two heap-allocated vectors (Dx,Dz,bearing,slope,sag) — free with moniFreeDoubleArray. */
	PYMONITORING_API int moniGetECWSData(MonitoringHandle h, const char *romName, double *outEstimate, double *outPrec);
	PYMONITORING_API int moniGetECWIData(MonitoringHandle h, const char *romName, double **outEstimate, int *outEstLen, double **outPrec, int *outPrecLen);

#ifdef __cplusplus
}
#endif

#endif /* PYMONITORING_C_H */
