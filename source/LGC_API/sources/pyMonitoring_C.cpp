// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pyMonitoring_C.h"

#include <algorithm>
#include <string>

#include <Eigen/Dense>

#include "Monitoring.h"

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

#define M(handle) (*static_cast<Monitoring *>(handle))

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

	// --- Lifecycle -------------------------------------------------------------

	MonitoringHandle moniCreate(const char *inputFilePath)
	{
		try
		{
			return new Monitoring(std::string(inputFilePath));
		}
		CATCH_NULL
	}

	void moniDestroy(MonitoringHandle h)
	{
		delete static_cast<Monitoring *>(h);
	}

	int moniReset(MonitoringHandle h)
	{
		try
		{
			M(h).reset();
			return 0;
		}
		CATCH_ERR
	}

	// --- Execution -------------------------------------------------------------

	int moniAdjust(MonitoringHandle h)
	{
		try
		{
			return M(h).adjust() ? 1 : 0;
		}
		CATCH_ERR
	}

	int moniGetStatus(MonitoringHandle h)
	{
		try
		{
			return M(h).getStatus() ? 1 : 0;
		}
		CATCH_ERR
	}

	int moniGetSigma0(MonitoringHandle h, double *out)
	{
		try
		{
			*out = M(h).getSigma0();
			return 0;
		}
		CATCH_ERR
	}

	int moniWriteLGCInputFile(MonitoringHandle h)
	{
		try
		{
			M(h).writeLGCInputFile();
			return 0;
		}
		CATCH_ERR
	}

	// --- Observations ----------------------------------------------------------

	int moniSetActivationStatus(MonitoringHandle h, const char *id, int status)
	{
		try
		{
			M(h).setActivationStatus(std::string(id), status != 0);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetActivationStatus(MonitoringHandle h, const char *id)
	{
		try
		{
			return M(h).getActivationStatus(std::string(id)) ? 1 : 0;
		}
		CATCH_ERR
	}

	int moniUpdateMeas(MonitoringHandle h, const char *id, const double *data, int len)
	{
		try
		{
			Eigen::VectorXd meas = Eigen::Map<const Eigen::VectorXd>(data, len);
			M(h).updateMeas(std::string(id), meas);
			return 0;
		}
		CATCH_ERR
	}

	int moniSetObsSigma(MonitoringHandle h, const char *id, const double *data, int len)
	{
		try
		{
			Eigen::VectorXd sigma = Eigen::Map<const Eigen::VectorXd>(data, len);
			M(h).setObsSigma(std::string(id), sigma);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetMeas(MonitoringHandle h, const char *id, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getMeas(std::string(id)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetEstimateResidual(MonitoringHandle h, const char *obsName, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getEstimateResidual(std::string(obsName)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetCalcMeas(MonitoringHandle h, const char *obsName, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getCalcMeas(std::string(obsName)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetObsSigma(MonitoringHandle h, const char *obsName, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getObsSigma(std::string(obsName)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Parameter control -----------------------------------------------------

	int moniSetFixedPointParameter(MonitoringHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).setFixedPointParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniSetFixedFrameParameter(MonitoringHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).setFixedFrameParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniSetFixedSagParameter(MonitoringHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).setFixedSagParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniFreezePointParameter(MonitoringHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).freezePointParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniFreezeFrameParameter(MonitoringHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).freezeFrameParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniFreezeSagParameter(MonitoringHandle h, const char *name, int idx, double val)
	{
		try
		{
			M(h).freezeSagParameter(std::string(name), idx, val);
			return 0;
		}
		CATCH_ERR
	}

	int moniUnfreezePointParameter(MonitoringHandle h, const char *name, int idx)
	{
		try
		{
			M(h).unfreezePointParameter(std::string(name), idx);
			return 0;
		}
		CATCH_ERR
	}

	int moniUnfreezeFrameParameter(MonitoringHandle h, const char *name, int idx)
	{
		try
		{
			M(h).unfreezeFrameParameter(std::string(name), idx);
			return 0;
		}
		CATCH_ERR
	}

	int moniUnfreezeSagParameter(MonitoringHandle h, const char *name, int idx)
	{
		try
		{
			M(h).unfreezeSagParameter(std::string(name), idx);
			return 0;
		}
		CATCH_ERR
	}

	// --- Estimation results ----------------------------------------------------

	int moniGetPointEstimate(MonitoringHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimate(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetPointEstimateInFrame(MonitoringHandle h, const char *name, const char *frame, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimate(std::string(name), std::string(frame)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetFrameEstimate(MonitoringHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getFrameEstimate(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetSagEstimate(MonitoringHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getSagEstimate(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetPointEstimatePrec(MonitoringHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimatePrec(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetPointEstimatePrecInFrame(MonitoringHandle h, const char *name, const char *frame, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getPointEstimatePrec(std::string(name), std::string(frame)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetFrameEstimatePrec(MonitoringHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getFrameEstimatePrec(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	int moniGetSagEstimatePrec(MonitoringHandle h, const char *name, double **outData, int *outLen)
	{
		try
		{
			eigenVecToArray(M(h).getSagEstimatePrec(std::string(name)), outData, outLen);
			return 0;
		}
		CATCH_ERR
	}

	// --- Transformations -------------------------------------------------------

	int moniTransformCoordinates(MonitoringHandle h, const double *in3, const char *from, const char *to, double *out3)
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

	int moniTransformDirection(MonitoringHandle h, const double *in3, const char *from, const char *to, double *out3)
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

	int moniGetECWSData(MonitoringHandle h, const char *romName, double *outEstimate, double *outPrec)
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

	int moniGetECWIData(MonitoringHandle h, const char *romName, double **outEstimate, int *outEstLen, double **outPrec, int *outPrecLen)
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
