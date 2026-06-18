/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LGCADJUSTABLE_SAG
#define LGCADJUSTABLE_SAG

// STL
#include <bitset>
// SURVEYLIB
#include <TAngle.h>
#include <TLength.h>
#include <TAdjustablePoint.h>
#include <TAdjustableLength.h>
#include <TVAdjustableObject.h>
#include <TRefSystemFactory.h>
#include <TAdjustableHelmertTransformation.h>

#if USE_SERIALIZER
#	include <Serializer.hpp>
#endif // USE_SERIALIZER

class LGCAdjustablePoint;

/*!
	\brief Adds adjustable information to a sag deformation.

	The sag offset is evaluated in the base frame coordinates:
	- dy = y coordinate of the reference point in the base frame
	- z offset = ZS + ZC * dy^2
	- x offset = XS + XC * dy^2
*/
class LGCAdjustableSag : public TVAdjustableObject
{
public:
	// number of sag parameters: ZS, ZC, XS, XC
	static constexpr int kNumSagParams = 4;

private:
	std::string fName; /*!< Name of the adjustable sag element*/
	std::string fBaseFrame; /*!< Name of the base frame*/
	// Number of the line in the input file where the sag element is introduced
	int line = -1;

	// 4 parameters: sag+curvature for z and x
	TAdjustableLength fZSag;
	TAdjustableLength fZCurv;
	TAdjustableLength fXSag;
	TAdjustableLength fXCurv;
	Eigen::Matrix<double, kNumSagParams, kNumSagParams> fCovar;
	std::bitset<kNumSagParams> fIsFixed;
	int uidx[kNumSagParams];
	bool fInitialized;

public:
	/*!@name Constructors/Initialization */
	//@{
	LGCAdjustableSag(
		const std::string &name,
		const std::string &baseFrame,
		const TLength &zSag,
		const TLength &zCurv,
		const TLength &xSag,
		const TLength &xCurv,
		const std::bitset<kNumSagParams> &isFixed) :
		fName(name),
		fBaseFrame(baseFrame),
		fZSag(TAdjustableLength(zSag, isFixed[0], name + "_ZS")),
		fZCurv(TAdjustableLength(zCurv, isFixed[1], name + "_ZC")),
		fXSag(TAdjustableLength(xSag, isFixed[2], name + "_XS")),
		fXCurv(TAdjustableLength(xCurv, isFixed[3], name + "_XC")),
		fIsFixed(isFixed),
		uidx{-1, -1, -1, -1},
		fInitialized(true)
	{
		fCovar.setConstant(NO_VALf);
	};

	//@}

	static LGCAdjustableSag createUninitialized(const std::string &name);
	/*!@name Access methods*/
	//@{

	/// getters
	const TAdjustableLength &getZSag() const { return fZSag; }
	TAdjustableLength &getZSag() { return fZSag; }
	const TAdjustableLength &getZCurv() const { return fZCurv; }
	TAdjustableLength &getZCurv() { return fZCurv; }

	const TAdjustableLength &getXSag() const { return fXSag; }
	TAdjustableLength &getXSag() { return fXSag; }
	const TAdjustableLength &getXCurv() const { return fXCurv; }
	TAdjustableLength &getXCurv() { return fXCurv; }

	// getting the estimated vector with all variables
	virtual Eigen::VectorXd getEstVector() const override;
	// get value corresponding to unknown index
	virtual TReal getValue(int idx) const override;
	// set the value corresponding to an index
	virtual void setValue(int idx, TReal value) override;

	/*!
		\brief Returns the number of unknowns for this sag element.
	*/
	virtual int getNumUnkn() const
	{
		return fIsFixed.size() - fIsFixed.count();
	}

	///	See \ref TVAdjustableObject::getFirstUidx
	virtual int getFirstUidx() const;
	///	See \ref TVAdjustableObject::getLastUidx
	virtual int getLastUidx() const;
	const std::vector<int> getRelativeUnknIndices() const override;
	bool isParameterFixed(int d) const
	{
		return (fIsFixed[d]);
	}
	// zs,zc,xs,xc=0,1,2,3
	// get the unknown index, returns -1 if variable is fixed
	int getUnknIndex(int d) const;

	/// Returns Name of the sag element.
	virtual const std::string &getName() const { return fName; }

	// returns the name of the base frame
	const std::string &getBaseFrame() const { return fBaseFrame; }

	///	See \ref TVAdjustableObject::isInitialized
	virtual bool isInitialized() const { return fInitialized; }

	/// Returns Number of line where the sag element was defined.
	int getLine() const { return line; }
	/// Sets the number of the line where the sag element was defined.
	void setLine(int lineNumber) { line = lineNumber; }

	///	See \ref TVAdjustableObject::isFixed
	virtual bool isFixed() const { return fIsFixed.all(); }


	/*!@name Setting.*/
	//@{

	/*!
		\brief See \ref TVAdjustableObject::setFirstUidx

		\throws Throws a logic_error if no component of the plane is variable.
	*/
	virtual void setFirstUidx(int idx);

	///	See \ref TVAdjustableObject::setCorrection
	//virtual void setCorrection(int idx, TReal value);
	void setCovar(Eigen::Matrix<double, kNumSagParams, kNumSagParams> covarMat) { fCovar = covarMat; }
	const Eigen::Matrix<double, kNumSagParams, kNumSagParams> &getCovar() const { return fCovar; }

	/// Reset the adjustment results (estimated values, corrections, precisions, covariance) for SIMU re-runs.
	void reInitialise();

	//@}
#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

};
#endif // LGCADJUSTABLE_SAG
