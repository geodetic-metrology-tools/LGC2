/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef LGCADJUSTABLE_POINT
#define LGCADJUSTABLE_POINT

// SURVEYLIB
#include <TAdjustablePoint.h>
// LGC
#include <Global.h>
#include <LGCAdjustableObjectCollection.h>

class TLGCData;

#if USE_SERIALIZER
struct pointSigmaData : public Serializable
#else
struct pointSigmaData
#endif // USE_SERIALIZER
{
	// allowing to associate a weight to the point
	// the weight is either given via a rotated system with bearing slope roll and half deflection or directly with a covariance matrix
	// in case a matrix is supplied directly the standard deviaton values are kept as NAN
	// for the meaning of the angles, see https://edms.cern.ch/document/1476360/2
	std::array<TAngle, 4> fAngles;
	std::array<std::string, 4> fAngleNames;
	Eigen::Matrix3d fRotMat;
	bool fHasAngle, fHasApriCovMat;

	Eigen::Vector3d fSigmas;
	Eigen::Matrix3d fApriCovMat;
	int firstObsIdx, firstCIdx;
	// the rotated offset
	Eigen::Vector3d fRotRes;
	Eigen::Vector3d fRotResNormalized;
	// the a-posteriori covar matrix (in the rotated coordinate system)
	Eigen::Matrix3d fRotCovar;
	// constructor
	pointSigmaData() :
		fAngles({TAngle(0), TAngle(0), TAngle(0), TAngle(0)}),
		fAngleNames({"BEAR", "SLOPE", "ROLL", "HDEFL"}),
		fRotMat(Eigen::Matrix3d::Identity(3, 3)),
		fHasAngle(false),
		fApriCovMat(Eigen::Matrix3d::Constant(NAN)),
		fHasApriCovMat(false),
		fSigmas(Eigen::Vector3d::Constant(NAN)),
		firstObsIdx(-1),
		firstCIdx(-1),
		fRotRes(Eigen::Vector3d::Constant(NAN)),
		fRotResNormalized(Eigen::Vector3d::Constant(NAN)),
		fRotCovar(Eigen::Matrix3d::Constant(NAN)){};
	// xyz rotation order
	// for the meaning of the angles, see https://edms.cern.ch/document/1476360/2
	// see also LGC user guide
	// rotation matrix describes the transformation from local coordinates (usually CCS) to rotated system ("RST" coordinates)
	void calcAndSetRotMat()
	{
		TRotationMatrix RSTI(TRotationMatrix::ERotationType::kRxyz, 0, 0, fAngles[0]);
		TRotationMatrix RST(TRotationMatrix::ERotationType::kRxyz, -fAngles[1], fAngles[2], -fAngles[3]);
		TRotationMatrix rot2Loc = RSTI * RST;
		TRotationMatrix loc2Rot = rot2Loc.inverse();
		fRotMat = loc2Rot.getMat();
	}
	Eigen::Vector3d calcRotOffset(const LGCAdjustablePoint &pt, const TLGCData *fData) const;

#if USE_SERIALIZER
	// Inherited via Serializable
	void serialize(ObjectSerializer &obj) const
	{
		obj.addProperty("fAngles", fAngles);
		obj.addProperty("fAngleNames", fAngleNames);
		obj.addProperty("fHasAngle", fHasAngle);
		obj.addProperty("fApriCovMat", fApriCovMat);
		obj.addProperty("fHasApriCovMat", fHasApriCovMat);
		obj.addProperty("fSigmas", fSigmas);
		obj.addProperty("firstObsIdx", firstObsIdx);
		obj.addProperty("firstCIdx", firstCIdx);
		obj.addProperty("fRotRes", fRotRes);
		obj.addProperty("fRotResNormalized", fRotResNormalized);
		obj.addProperty("fRotCovar", fRotCovar);
		obj.addProperty("fSigmasPost", fRotCovar.diagonal().array().sqrt());
	};
#endif
};

/*!
	\ingroup AdjustableObjects
	\brief Adds adjustable information to a point represented by a TPositionVector class.
*/
class LGCAdjustablePoint : public TAdjustablePoint
{
public:
	/*!@name Constructors */
	//@{

	/*!
		\brief Constructs an AdjustmentPoint based on a position vector defined in one of the local object reference frames.

		Provisional value is represented in the coordinate system passed to the constructor, but estimated value is kept in k3DCartesian coordinate system, used in the adjustment process.

		\param[in] pos A reference to a position vector that will be adjusted. The point will be copied
					   so the poited-to object is no longer needed after construction.
		\param[in] isXfixed The X component of the point, fixed (not adjusted) if set to TRUE.
		\param[in] isYfixed The Y component of the point, fixed (not adjusted) if set to TRUE.
		\param[in] isZHfixed The Z component of the point, fixed (not adjusted) if set to TRUE.
		\param[in] name Name of the adjustable point.
		\param[in] referential Reference frame used (OLOC, RS2K, LEP, SPHE).
		\param[in] positionInTree iterator on the local object reference frame in which the point is defined.
	*/
	LGCAdjustablePoint(const TPositionVector &pos, bool isXfixed, bool isYfixed, bool isZHfixed, const std::string &name, TRefSystemFactory::ERefFrame referential, TDataTreeIterator positionInTree);

	/// Create an unitialized point.
	static LGCAdjustablePoint createUninitialized(const std::string &name);
	//@}

	/*!@name Access methods*/
	//@{

	/*!
		\brief Calculates and \returns the number of unknowns that are added to the adjustment by this point.
		This number varies from zero to three unknowns.
	*/
	virtual int getNumUnkn() const;

	/// Tells if at least one coordinate is unfixed (variable).
	virtual bool hasVariable() const;

	/// See \ref TVAdjustableObject::isFixed
	inline virtual bool isFixed() const { return ((fixedState[0] && fixedState[1] && fixedState[2]) | allfixedParam); }

	/*!
		\brief See \ref TVAdjustableObject::getFirstUidx

		\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
	*/
	virtual int getFirstUidx() const;

	/*!
		\brief See \ref TVAdjustableObject::getLastUidx

		\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
	*/
	virtual int getLastUidx() const;

	/// Returns iterator to the position in the tree.
	TDataTreeIterator getFrameTreePosition() const { return fFramePosition; };

	/*!
		\brief Returns The boolean result of the query.

		Checks if a component of the point is excluded from the adjustment.

		\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
	*/
	inline virtual bool isCoordinateFixed(int d) const
	{
		assert3D(d);
		return (fixedState[d] | allfixedParam);
	}

	/*!
		\brief Returns index of an unknown point coordinate in a LS matrices.

		\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
	*/
	int virtual getCoordinateUnknIndex(int d) const;

#if USE_SERIALIZER
	// Inherited via Serializable
	virtual void serialize(ObjectSerializer &obj) const override;
#endif

	//@}

	/*!@name Settings */
	//@{

	/// Rename the adjustable point
	virtual void setName(const std::string name) { fName = name; }

	/// Set the frame position to *pos*
	void setFrameTreePosition(const TDataTreeIterator &pos) { fFramePosition = pos; };

	/// Reset the provisional position vector
	virtual void setProvisionalValue(const TReal &x, const TReal &y, const TReal &z);

	/// Transform and Store the covariance matrix in ROOT
	void setCovarianceMatrixInRoot(const TLGCData *fData);

	/// Transform the provisional coordinates (X,Y,Z) in ROOT and compute the provisional height in ROOT
	void transformProvisionalCoordinates(const TLGCData *fData);

	/// Transform the estimated coordinates (X,Y,Z) in ROOT and compute the estimated height in ROOT
	void transformEstimatedCoordinates(const TLGCData *fData);

	/// Temporary method to change provisional values after computation without breaking the LGC other logic (Method to be deleted in the future)
	void changeProvValueToCCS(const TLGCData *fData);

	/// Returns a constant reference on the estimated value of the point in ROOT
	inline const TPositionVector &getEstimatedValueInRoot() const { return fEstimatedValueInRoot; }

	/// Returns a constant reference on the provisional value of the point in ROOT
	inline const TPositionVector &getProvisionalValueInRoot() const { return fProvisionalValueInRoot; }

	/// Returns a constant reference on the estimated height of the point in ROOT
	inline const TLength &getEstimatedHeightInRoot() const { return fEstimatedHeightInRoot; }

	/// Returns a constant reference on the provisional height of the point in ROOT
	inline const TLength &getProvisionalHeightInRoot() const { return fProvisionalHeightInRoot; }

	/// Returns a constant reference on the covariance matrix of the point in ROOT
	inline const Eigen::Matrix3d &getCovarianceMatrixInRoot() const { return fCovarianceMatrixInRoot; }

	/*!
		\brief See \ref TVAdjustableObject::setCorrection

		Sets correction for the current estimatedValue and updates it, if point has fixed H value transformation is made.

		\throws Throws a logic_error if the adjusted point does not contain required index.
	*/
	virtual void setCorrection(int idx, TReal value);

	/*!
		\brief See \ref TVAdjustableObject::setFirstUidx

		\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
	*/
	virtual void setFirstUidx(int idx);

	/// Update the adjustment information of a point, used to set point coordinates fixed if ALLFIXED used
	virtual void updateFixedState(bool lx, bool ly, bool lz);

	/// Set the boolean reference allfixedParam (to the TLGCConfig binary option ALLFIXED)
	static void setAllFixedParam(const bool &param) { allfixedParam = param; };
	//@}

	/// Transform sigma a posteriori (= estimated precision) in root
	static TFreeVector transformSigmaInRoot(const LGCAdjustablePoint &pv, const TLGCData *fData);
	/// Transform sigma a posteriori (= estimated precision) to arbitrary subframe
	static TFreeVector transformSigma(const LGCAdjustablePoint &pv, const TLGCData *fData, const TDataTreeIterator toFrame);
	static TFreeVector transformSigma(const LGCAdjustablePoint &pv, const TLGCData *fData, const std::string toFrame);

	static Eigen::Matrix3d transformCovar(const LGCAdjustablePoint &pv, const TLGCData *fData, const TDataTreeIterator toFrame);

	/// Returns true if this point is defined in the ROOT frame
	bool isInRootFrame();

	// returns a reference of the point weight data
	inline pointSigmaData &getPointSigmaData() { return fPointSigma; }
	// const version, needed for result writer
	inline const pointSigmaData &getPointSigmaData() const { return fPointSigma; }
	inline bool hasPointSigma() const { return fHasPointSigma; }
	inline void activatePointSigma() { fHasPointSigma = true; }

private:
	TDataTreeIterator fFramePosition; /*!< Iterator on the position in the tree. */

	static bool allfixedParam; /*!< Reference to the boolean which indicate if ALLFIXED option is used. By default, the value is false.*/

	TPositionVector fProvisionalValueInRoot = getProvisionalValue(); /*!< initialization of point's provisional value in ROOT*/
	TPositionVector fEstimatedValueInRoot = getEstimatedValue(); /*!< initialization of point's estimated value in ROOT*/
	Eigen::Matrix3d fCovarianceMatrixInRoot = Eigen::Matrix3d::Zero(); /*!< initialization of point's covariance matrix in ROOT*/

	TLength fProvisionalHeightInRoot = TLength(0); /*!< point's provisional height value in ROOT*/
	TLength fEstimatedHeightInRoot = TLength(0); /*!< point's estimated height value in ROOT*/

	// indicating whether the point has associated weights
	bool fHasPointSigma{false};
	pointSigmaData fPointSigma;
	/*!Private constructor for creating uninitialized object	*/
	LGCAdjustablePoint(const std::string &name);
};

#endif // TADJUSTABLE_POINT
