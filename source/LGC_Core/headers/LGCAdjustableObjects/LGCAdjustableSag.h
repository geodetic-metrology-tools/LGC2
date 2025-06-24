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
#include <TAdjustableAngle.h>
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
*/
class LGCAdjustableSag : public TVAdjustableObject
{
private:
	std::string fName; /*!< Name of the adjustable sag element*/
	std::string fBaseFrame;
	 /*!< Name of the adjustable sag element*/
	// Number of the line in the input file where the sag element is introduced
	int line = -1; 


	// total 5 parameters: bearing and sag+curvature for vertical and radial. Bearing however is constrained to match the y axis of the subframe to which the sag element is associated
	TAdjustableAngle fBearing; // bearing angle of the sag, will be constrained to match the y axis of the frame in which the sag element is defined, this corresponds to a sag that gets bigger in y direction
	TAdjustableLength fVertCurv; 
	TAdjustableLength fVertSag; 
	TAdjustableLength fRadCurv; 
	TAdjustableLength fRadSag; 
	Eigen::Matrix<double, 5, 5> fCovar;
	std::bitset<5> fIsFixed;
	int fUidx[5];
	int firstCIndex{-1};
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
		const std::bitset<5> &isFixed) :
		fName(name),
		fBaseFrame(baseFrame),
		fIsFixed(isFixed),
		fUidx{-1, -1, -1, -1, -1},
		// bearing always free
		fBearing(TAdjustableAngle(TAngle(0), false, name + "_bearing")),
		fVertSag(TAdjustableLength(vertSag, isFixed[1], name + "_VS")),
		fVertCurv(TAdjustableLength(vertCurv, isFixed[2], name + "_VC")),
		fRadSag(TAdjustableLength(radSag, isFixed[3], name + "_RS")),
		fRadCurv(TAdjustableLength(radCurv, isFixed[4], name + "_RC")),
		fInitialized(true)
	{
		fCovar.setConstant(NO_VALf);
	};

	//@}

	static LGCAdjustableSag createUninitialized(const std::string name);
	/*!@name Access methods*/
	//@{

	/// Returns a constant reference of the bearing angle
	inline const TAdjustableAngle &getBearing() const { return fBearing; }
	inline TAdjustableAngle &getBearing() { return fBearing; }

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

	
	// returning the constraint index of the bearing constraint
	inline int getFirstCidx() const { return firstCIndex; }
	inline int getLastCidx() const { return firstCIndex; }

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
	// this adjustable element also contains a constraint (orthogonality between bearing and low point)
	void setFirstCidx(int idx) { firstCIndex = idx; };


	///	See \ref TVAdjustableObject::setCorrection
	//virtual void setCorrection(int idx, TReal value);
	void setCovar(Eigen::Matrix<double, 5, 5> covarMat) { fCovar = covarMat; }
	const Eigen::Matrix<double, 5, 5> &getCovar() const { return fCovar; }

	///// Sets the estimated precision after calculation
	// void setEstimatedPrecision(int idx, TReal value);

	//@}
#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

};
#endif // TADJUSTABLE_SAG
