// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MONITOR_C_H
#define MONITOR_C_H

#ifdef _WIN32
#	ifdef MONITOR_C_EXPORTS
#		define MONITOR_C_API __declspec(dllexport)
#	else
#		define MONITOR_C_API __declspec(dllimport)
#	endif
#else
#	define MONITOR_C_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	typedef void *MonitorHandle;

	/* Error handling — returns message from last failed call (thread-local). */
	MONITOR_C_API const char *moniGetLastError(void);

	/* Memory management — caller must free arrays returned by the API. */
	MONITOR_C_API void moniFreeDoubleArray(double *ptr);

	/* Lifecycle. moniCreate returns NULL on error (check moniGetLastError).
	   The remaining calls return 0 on success and -1 on error, unless noted. */
	MONITOR_C_API MonitorHandle moniCreate(const char *inputFilePath);
	MONITOR_C_API void moniDestroy(MonitorHandle h);
	MONITOR_C_API int moniReset(MonitorHandle h);

	/* Execution. moniAdjust / moniGetStatus return 1=true, 0=false, -1=error. */
	MONITOR_C_API int moniAdjust(MonitorHandle h);
	MONITOR_C_API int moniGetStatus(MonitorHandle h);
	MONITOR_C_API int moniGetSigma0(MonitorHandle h, double *out);
	MONITOR_C_API int moniWriteLGCInputFile(MonitorHandle h);

	/* Observations. moniGetActivationStatus returns 1=true, 0=false, -1=error. */
	MONITOR_C_API int moniSetActivationStatus(MonitorHandle h, const char *id, int status);
	MONITOR_C_API int moniGetActivationStatus(MonitorHandle h, const char *id);
	MONITOR_C_API int moniUpdateMeas(MonitorHandle h, const char *id, const double *data, int len);
	MONITOR_C_API int moniSetObsSigma(MonitorHandle h, const char *id, const double *data, int len);
	MONITOR_C_API int moniGetMeas(MonitorHandle h, const char *id, double **outData, int *outLen);
	MONITOR_C_API int moniGetEstimateResidual(MonitorHandle h, const char *obsName, double **outData, int *outLen);
	MONITOR_C_API int moniGetCalcMeas(MonitorHandle h, const char *obsName, double **outData, int *outLen);
	MONITOR_C_API int moniGetObsSigma(MonitorHandle h, const char *obsName, double **outData, int *outLen);

	/* Parameter control (idx is the component index; see MonitorImpl for conventions). */
	MONITOR_C_API int moniSetFixedPointParameter(MonitorHandle h, const char *name, int idx, double val);
	MONITOR_C_API int moniSetFixedFrameParameter(MonitorHandle h, const char *name, int idx, double val);
	MONITOR_C_API int moniSetFixedSagParameter(MonitorHandle h, const char *name, int idx, double val);
	MONITOR_C_API int moniFreezePointParameter(MonitorHandle h, const char *name, int idx, double val);
	MONITOR_C_API int moniFreezeFrameParameter(MonitorHandle h, const char *name, int idx, double val);
	MONITOR_C_API int moniFreezeSagParameter(MonitorHandle h, const char *name, int idx, double val);
	MONITOR_C_API int moniUnfreezePointParameter(MonitorHandle h, const char *name, int idx);
	MONITOR_C_API int moniUnfreezeFrameParameter(MonitorHandle h, const char *name, int idx);
	MONITOR_C_API int moniUnfreezeSagParameter(MonitorHandle h, const char *name, int idx);

	/* Estimation results (heap-allocated vectors — free with moniFreeDoubleArray). */
	MONITOR_C_API int moniGetPointEstimate(MonitorHandle h, const char *name, double **outData, int *outLen);
	MONITOR_C_API int moniGetPointEstimateInFrame(MonitorHandle h, const char *name, const char *frame, double **outData, int *outLen);
	MONITOR_C_API int moniGetFrameEstimate(MonitorHandle h, const char *name, double **outData, int *outLen);
	MONITOR_C_API int moniGetSagEstimate(MonitorHandle h, const char *name, double **outData, int *outLen);
	MONITOR_C_API int moniGetPointEstimatePrec(MonitorHandle h, const char *name, double **outData, int *outLen);
	MONITOR_C_API int moniGetPointEstimatePrecInFrame(MonitorHandle h, const char *name, const char *frame, double **outData, int *outLen);
	MONITOR_C_API int moniGetFrameEstimatePrec(MonitorHandle h, const char *name, double **outData, int *outLen);
	MONITOR_C_API int moniGetSagEstimatePrec(MonitorHandle h, const char *name, double **outData, int *outLen);

	/* Transformations — in3 and out3 are caller-allocated arrays of length 3. */
	MONITOR_C_API int moniTransformCoordinates(MonitorHandle h, const double *in3, const char *from, const char *to, double *out3);
	MONITOR_C_API int moniTransformDirection(MonitorHandle h, const double *in3, const char *from, const char *to, double *out3);

	/* Round-of-measurement results.
	   ECWS (water): two scalar out-params.
	   ECWI (wire): two heap-allocated vectors (Dx,Dz,bearing,slope,sag) — free with moniFreeDoubleArray. */
	MONITOR_C_API int moniGetECWSData(MonitorHandle h, const char *romName, double *outEstimate, double *outPrec);
	MONITOR_C_API int moniGetECWIData(MonitorHandle h, const char *romName, double **outEstimate, int *outEstLen, double **outPrec, int *outPrecLen);

#ifdef __cplusplus
}
#endif

#endif /* MONITOR_C_H */
