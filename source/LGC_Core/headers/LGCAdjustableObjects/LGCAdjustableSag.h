/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
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
	- vertical offset = (VS + VC * dy^2) along frame z-axis
	- radial offset = (RS + RC * dy^2) along frame x-axis
*/
class LGCAdjustableSag : public TVAdjustableObject
{
private:
	std::string fName; /*!< Name of the adjustable sag element*/
	std::string fBaseFrame; /*!< Name of the base frame*/
	// Number of the line in the input file where the sag element is introduced
	int line = -1;

	// 4 parameters: sag+curvature for vertical and radial
	TAdjustableLength fVertSag;
	TAdjustableLength fVertCurv;
	TAdjustableLength fRadSag;
	TAdjustableLength fRadCurv;
	Eigen::Matrix<double, 4, 4> fCovar;
	std::bitset<4> fIsFixed;
	int fUidx[4];
	bool fInitialized;

public:
	/*!@name Constructors/Initialization */
	//@{
	LGCAdjustableSag(
		const std::string &name,
		const std::string &baseFrame,
		const TLength &vertSag,
		const TLength &vertCurv,
		const TLength &radSag,
		const TLength &radCurv,
		const std::bitset<4> &isFixed) :
		fName(name),
		fBaseFrame(baseFrame),
		fIsFixed(isFixed),
		fUidx{-1, -1, -1, -1},
		fVertSag(TAdjustableLength(vertSag, isFixed[0], name + "_VS")),
		fVertCurv(TAdjustableLength(vertCurv, isFixed[1], name + "_VC")),
		fRadSag(TAdjustableLength(radSag, isFixed[2], name + "_RS")),
		fRadCurv(TAdjustableLength(radCurv, isFixed[3], name + "_RC")),
		fInitialized(true)
	{
		fCovar.setConstant(NO_VALf);
	};

	//@}

	static LGCAdjustableSag createUninitialized(const std::string name);
	/*!@name Access methods*/
	//@{

	/// getters
	inline const TAdjustableLength &getVertSag() const { return fVertSag; }
	inline TAdjustableLength &getVertSag() { return fVertSag; }
	inline const TAdjustableLength &getVertCurv() const { return fVertCurv; }
	inline TAdjustableLength &getVertCurv() { return fVertCurv; }

	inline const TAdjustableLength &getRadSag() const { return fRadSag; }
	inline TAdjustableLength &getRadSag() { return fRadSag; }
	inline const TAdjustableLength &getRadCurv() const { return fRadCurv; }
	inline TAdjustableLength &getRadCurv() { return fRadCurv; }

	// getting the estimated vector with all variables
	virtual Eigen::VectorXd getEstVector() const override;
	// get value corresponding to unknown index
	virtual TReal getValue(int idx) const override;
	// set the value corresponding to an index
	virtual void setValue(int idx, TReal value) override;

	void setEstValue(int idx, TReal value);

	/*!
		\brief Returns the number of unknowns for this sag element.
	*/
	inline virtual int getNumUnkn() const;

	///	See \ref TVAdjustableObject::getFirstUidx
	inline virtual int getFirstUidx() const;
	///	See \ref TVAdjustableObject::getLastUidx
	inline virtual int getLastUidx() const;
	const std::vector<int> getRelativeUnknIndices() const;
	inline virtual bool isCoordinateFixed(int d) const
	{
		return (fIsFixed[d]);
	}
	// vs,vc,rs,rc=0,1,2,3
	// get the unknown index, returns -1 if variable is fixed
	int getUnknIndex(int d) const;

	/// Returns Name of the sag element.
	inline virtual const std::string &getName() const { return fName; }

	// returns the name of the base frame
	const std::string &getBaseFrame() const { return fBaseFrame; }

	///	See \ref TVAdjustableObject::isInitialized
	inline virtual bool isInitialized() const { return fInitialized; }

	/// Returns Number of line where the sag element was defined.
	int &getLine() { return line; }

	///	See \ref TVAdjustableObject::isFixed
	inline virtual bool isFixed() const { return getNumUnkn() == 0; }


	/*!@name Setting.*/
	//@{

	/*!
		\brief See \ref TVAdjustableObject::setFirstUidx

		\throws Throws a logic_error if no component of the plane is variable.
	*/
	virtual void setFirstUidx(int idx);

	///	See \ref TVAdjustableObject::setCorrection
	//virtual void setCorrection(int idx, TReal value);
	void setCovar(Eigen::Matrix<double, 4, 4> covarMat) { fCovar = covarMat; }
	const Eigen::Matrix<double, 4, 4> &getCovar() const { return fCovar; }

	//@}
#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

};
#endif // TADJUSTABLE_SAG
