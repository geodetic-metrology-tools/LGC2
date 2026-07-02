// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Monitor_C.h"

#include <algorithm>
#include <string>

#include <Eigen/Dense>

#include "MonitorCore.h"

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

#define M(handle) (*static_cast<Monitor *>(handle))

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
	const char *moniGetLastError(void)
	{
		return lastError.c_str();
	}

	void moniFreeDoubleArray(double *ptr)
	{
		delete[] ptr;
	}

	void moniFreeString(char *ptr)
	{
		delete[] ptr;
	}

	// --- Lifecycle -------------------------------------------------------------

	MonitorHandle moniCreate(const char *inputFilePath)
	{
		try
		{
			return new Monitor(std::string(inputFilePath));
		}
		CATCH_NULL
	}

	void moniDestroy(MonitorHandle h)
	{
		delete static_cast<Monitor *>(h);
	}

	int moniReset(MonitorHandle h)
	{
		try
		{
			M(h).reset();
			return 0;
		}
		CATCH_ERR
	}

	// --- Execution -------------------------------------------------------------

	int moniAdjust(MonitorHandle h)
	{
		try
		{
			return M(h).adjust() ? 1 : 0;
		}
		CATCH_ERR
	}

	int moniGetStatus(MonitorHandle h)
	{
		try
		{
			return M(h).getStatus() ? 1 : 0;
		}
		CATCH_ERR
	}

	int moniGetSigma0(MonitorHandle h, double *out)
	{
		try
		{
			*out = M(h).getSigma0();
			return 0;
		}
		CATCH_ERR
	}

	int moniWriteLGCInputFile(MonitorHandle h)
	{
		try
		{
			M(h).writeLGCInputFile();
			return 0;
		}
		CATCH_ERR
	}

	// Always exported so the ABI does not depend on build flags; without the
	// serializer it reports an error instead of dropping the symbol.
	int moniGetResultsJson(MonitorHandle h, char **outJson)
	{
#if USE_SERIALIZER
		try
		{
			const std::string json = M(h).getResultsJson();
			*outJson = new char[json.size() + 1];
			std::copy(json.begin(), json.end(), *outJson);
			(*outJson)[json.size()] = '\0';
			return 0;
		}
		CATCH_ERR
#else
		(void)h;
		(void)outJson;
		lastError = "LGC_API was built without serializer support (USE_SERIALIZER=0).";
		return -1;
#endif
	}

	// --- Observations ----------------------------------------------------------

	int moniSetActivationStatus(MonitorHandle h, const char *id, int status)
	{
		try
		{
			M(h).setActivationStatus(std::string(id), status != 0);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetActivationStatus(MonitorHandle h, const char *id)
	{
		try
		{
			return M(h).getActivationStatus(std::string(id)) ? 1 : 0;
		}
		CATCH_ERR
	}

	int moniUpdateMeas(MonitorHandle h, const char *id, const double *data, int len)
	{
		try
		{
			Eigen::VectorXd meas = Eigen::Map<const Eigen::VectorXd>(data, len);
			M(h).updateMeas(std::string(id), meas);
			return 0;
		}
		CATCH_ERR
	}

	int moniSetObsSigma(MonitorHandle h, const char *id, const double *data, int len)
	{
		try
		{
			Eigen::VectorXd sigma = Eigen::Map<const Eigen::VectorXd>(data, len);
			M(h).setObsSigma(std::string(id), sigma);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetMeas(MonitorHandle h, const char *id, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getMeas(std::string(id)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetEstimateResidual(MonitorHandle h, const char *obsName, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getEstimateResidual(std::string(obsName)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetCalcMeas(MonitorHandle h, const char *obsName, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getCalcMeas(std::string(obsName)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetObsSigma(MonitorHandle h, const char *obsName, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getObsSigma(std::string(obsName)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Parameter control -----------------------------------------------------

	int moniSetFixedPointParameter(MonitorHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).setFixedPointParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniSetFixedFrameParameter(MonitorHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).setFixedFrameParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniSetFixedSagParameter(MonitorHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).setFixedSagParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniFreezePointParameter(MonitorHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).freezePointParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniFreezeFrameParameter(MonitorHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).freezeFrameParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniFreezeSagParameter(MonitorHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).freezeSagParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniUnfreezePointParameter(MonitorHandle h, const char *name, int idx)
	{
		try
		{
			M(h).unfreezePointParameter(std::string(name), idx);
			return 0;
		}
		CATCH_ERR
	}

	int moniUnfreezeFrameParameter(MonitorHandle h, const char *name, int idx)
	{
		try
		{
			M(h).unfreezeFrameParameter(std::string(name), idx);
			return 0;
		}
		CATCH_ERR
	}

	int moniUnfreezeSagParameter(MonitorHandle h, const char *name, int idx)
	{
		try
		{
			M(h).unfreezeSagParameter(std::string(name), idx);
			return 0;
		}
		CATCH_ERR
	}

	// --- Estimation results ----------------------------------------------------

	int moniGetPointEstimate(MonitorHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimate(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetPointEstimateInFrame(MonitorHandle h, const char *name, const char *frame, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimate(std::string(name), std::string(frame)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetFrameEstimate(MonitorHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getFrameEstimate(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetSagEstimate(MonitorHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getSagEstimate(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetPointEstimatePrec(MonitorHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimatePrec(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetPointEstimatePrecInFrame(MonitorHandle h, const char *name, const char *frame, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimatePrec(std::string(name), std::string(frame)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetFrameEstimatePrec(MonitorHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getFrameEstimatePrec(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetSagEstimatePrec(MonitorHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getSagEstimatePrec(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Transformations -------------------------------------------------------

	int moniTransformCoordinates(MonitorHandle h, const double *in3, const char *from, const char *to, double *out3)
	{
		try
		{
			Eigen::Vector3d in(in3[0], in3[1], in3[2]);
			Eigen::Vector3d out = M(h).transformCoordinates(in, std::string(from), std::string(to));
			out3[0] = out[0];
			out3[1] = out[1];
			out3[2] = out[2];
			return 0;
		}
		CATCH_ERR
	}

	int moniTransformDirection(MonitorHandle h, const double *in3, const char *from, const char *to, double *out3)
	{
		try
		{
			Eigen::Vector3d in(in3[0], in3[1], in3[2]);
			Eigen::Vector3d out = M(h).transformDirection(in, std::string(from), std::string(to));
			out3[0] = out[0];
			out3[1] = out[1];
			out3[2] = out[2];
			return 0;
		}
		CATCH_ERR
	}

	// --- Round-of-measurement results ------------------------------------------

	int moniGetECWSData(MonitorHandle h, const char *romName, double *outEstimate, double *outPrec)
	{
		try
		{
			waterRom rom = M(h).getECWSData(std::string(romName));
			*outEstimate = rom.estimate;
			*outPrec = rom.prec;
			return 0;
		}
		CATCH_ERR
	}

	int moniGetECWIData(MonitorHandle h, const char *romName, double **outEstimate, int *outEstLen, double **outPrec, int *outPrecLen)
	{
		try
		{
			wireRom rom = M(h).getECWIData(std::string(romName));
			eigenVecToArray(rom.estimate, outEstimate, outEstLen);
			eigenVecToArray(rom.prec, outPrec, outPrecLen);
			return 0;
		}
		CATCH_ERR
	}

} // extern "C"
