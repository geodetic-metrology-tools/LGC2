// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Header-only C++ facade over the flat C ABI (Monitor_C.h). Every method is
// implemented inline in the CONSUMER's translation unit and only ever passes
// C types (const char*, double*, int) across the library boundary, so no
// std::string / Eigen layout has to match the library's toolchain. This is the
// C++ counterpart of pyMonitoring.py, and the shipped public header — the
// internal implementation lives in MonitorCore.h (not exported).
#ifndef MONITOR
#define MONITOR

#include <stdexcept>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Monitor_C.h"

// results of a water / wire round of measurements
struct waterRom
{
	std::string romName;
	double estimate = 0;
	double prec = 0;
	waterRom(const std::string &name, double level, double sigma) : romName(name), estimate(level), prec(sigma) {}
};

struct wireRom
{
	std::string romName;
	// Dx, Dz, bearing, slope, sag (and their sigmas)
	Eigen::VectorXd estimate = Eigen::VectorXd::Zero(5);
	Eigen::VectorXd prec = Eigen::VectorXd::Zero(5);
	wireRom(const std::string &name, const Eigen::VectorXd &vals, const Eigen::VectorXd &sigmas) : romName(name), estimate(vals), prec(sigmas) {}
};

class Monitor
{
public:
	explicit Monitor(const std::string &inputFilePath)
	{
		h_ = moniCreate(inputFilePath.c_str());
		if (!h_)
			throw std::runtime_error(moniGetLastError());
	}
	~Monitor()
	{
		if (h_)
			moniDestroy(h_);
	}
	Monitor(const Monitor &) = delete;
	Monitor &operator=(const Monitor &) = delete;
	Monitor(Monitor &&o) noexcept : h_(o.h_) { o.h_ = nullptr; }
	Monitor &operator=(Monitor &&o) noexcept
	{
		if (this != &o) { if (h_) moniDestroy(h_); h_ = o.h_; o.h_ = nullptr; }
		return *this;
	}

	void reset() { chk(moniReset(h_)); }
	void writeLGCInputFile() { chk(moniWriteLGCInputFile(h_)); }

	void setActivationStatus(const std::string &id, bool status) { chk(moniSetActivationStatus(h_, id.c_str(), status ? 1 : 0)); }
	bool getActivationStatus(const std::string &id) { return chkBool(moniGetActivationStatus(h_, id.c_str())) != 0; }
	void updateMeas(const std::string &id, const Eigen::VectorXd &v) { chk(moniUpdateMeas(h_, id.c_str(), v.data(), static_cast<int>(v.size()))); }
	void setObsSigma(const std::string &id, const Eigen::VectorXd &s) { chk(moniSetObsSigma(h_, id.c_str(), s.data(), static_cast<int>(s.size()))); }

	void setFixedFrameParameter(const std::string &n, int idx, double val) { chk(moniSetFixedFrameParameter(h_, n.c_str(), idx, val)); }
	void setFixedPointParameter(const std::string &n, int idx, double val) { chk(moniSetFixedPointParameter(h_, n.c_str(), idx, val)); }
	void setFixedSagParameter(const std::string &n, int idx, double val) { chk(moniSetFixedSagParameter(h_, n.c_str(), idx, val)); }
	void freezeFrameParameter(const std::string &n, int idx, double val) { chk(moniFreezeFrameParameter(h_, n.c_str(), idx, val)); }
	void freezePointParameter(const std::string &n, int idx, double val) { chk(moniFreezePointParameter(h_, n.c_str(), idx, val)); }
	void freezeSagParameter(const std::string &n, int idx, double val) { chk(moniFreezeSagParameter(h_, n.c_str(), idx, val)); }
	void unfreezeFrameParameter(const std::string &n, int idx) { chk(moniUnfreezeFrameParameter(h_, n.c_str(), idx)); }
	void unfreezePointParameter(const std::string &n, int idx) { chk(moniUnfreezePointParameter(h_, n.c_str(), idx)); }
	void unfreezeSagParameter(const std::string &n, int idx) { chk(moniUnfreezeSagParameter(h_, n.c_str(), idx)); }

	bool adjust() { return chkBool(moniAdjust(h_)) != 0; }
	bool getStatus() { return chkBool(moniGetStatus(h_)) != 0; }
	double getSigma0()
	{
		double out = 0;
		chk(moniGetSigma0(h_, &out));
		return out;
	}

	Eigen::VectorXd getPointEstimate(const std::string &n) { return vec(moniGetPointEstimate, n); }
	Eigen::VectorXd getPointEstimate(const std::string &n, const std::string &frame) { return vec2(moniGetPointEstimateInFrame, n, frame); }
	Eigen::VectorXd getFrameEstimate(const std::string &n) { return vec(moniGetFrameEstimate, n); }
	Eigen::VectorXd getSagEstimate(const std::string &n) { return vec(moniGetSagEstimate, n); }
	Eigen::VectorXd getPointEstimatePrec(const std::string &n) { return vec(moniGetPointEstimatePrec, n); }
	Eigen::VectorXd getPointEstimatePrec(const std::string &n, const std::string &frame) { return vec2(moniGetPointEstimatePrecInFrame, n, frame); }
	Eigen::VectorXd getFrameEstimatePrec(const std::string &n) { return vec(moniGetFrameEstimatePrec, n); }
	Eigen::VectorXd getSagEstimatePrec(const std::string &n) { return vec(moniGetSagEstimatePrec, n); }
	Eigen::VectorXd getMeas(const std::string &id) { return vec(moniGetMeas, id); }
	Eigen::VectorXd getEstimateResidual(const std::string &obsName) { return vec(moniGetEstimateResidual, obsName); }
	Eigen::VectorXd getCalcMeas(const std::string &obsName) { return vec(moniGetCalcMeas, obsName); }
	Eigen::VectorXd getObsSigma(const std::string &obsName) { return vec(moniGetObsSigma, obsName); }

	Eigen::Vector3d transformCoordinates(const Eigen::Vector3d &coord, const std::string &from, const std::string &to)
	{
		Eigen::Vector3d out;
		chk(moniTransformCoordinates(h_, coord.data(), from.c_str(), to.c_str(), out.data()));
		return out;
	}
	Eigen::Vector3d transformDirection(const Eigen::Vector3d &dir, const std::string &from, const std::string &to)
	{
		Eigen::Vector3d out;
		chk(moniTransformDirection(h_, dir.data(), from.c_str(), to.c_str(), out.data()));
		return out;
	}
	waterRom getECWSData(const std::string &romName)
	{
		double est = 0, prec = 0;
		chk(moniGetECWSData(h_, romName.c_str(), &est, &prec));
		return waterRom(romName, est, prec);
	}
	wireRom getECWIData(const std::string &romName)
	{
		double *e = nullptr, *p = nullptr;
		int en = 0, pn = 0;
		chk(moniGetECWIData(h_, romName.c_str(), &e, &en, &p, &pn));
		return wireRom(romName, toVec(e, en), toVec(p, pn));
	}

private:
	MonitorHandle h_ = nullptr;

	static void chk(int rc)
	{
		if (rc < 0)
			throw std::runtime_error(moniGetLastError());
	}
	static int chkBool(int rc)
	{
		if (rc < 0)
			throw std::runtime_error(moniGetLastError());
		return rc;
	}
	static Eigen::VectorXd toVec(double *p, int n)
	{
		Eigen::VectorXd v = (n > 0) ? Eigen::VectorXd(Eigen::Map<Eigen::VectorXd>(p, n)) : Eigen::VectorXd();
		moniFreeDoubleArray(p);
		return v;
	}
	template <class Fn>
	Eigen::VectorXd vec(Fn fn, const std::string &n)
	{
		double *p = nullptr;
		int len = 0;
		chk(fn(h_, n.c_str(), &p, &len));
		return toVec(p, len);
	}
	template <class Fn>
	Eigen::VectorXd vec2(Fn fn, const std::string &n, const std::string &frame)
	{
		double *p = nullptr;
		int len = 0;
		chk(fn(h_, n.c_str(), frame.c_str(), &p, &len));
		return toVec(p, len);
	}
};

#endif
