#ifndef TADJUSTABLE_PLANE
#define TADJUSTABLE_PLANE

#include <bitset>
#include "AdjustableObjects\TAdjustablePoint.h"


class TAdjustablePlane : public TVAdjustableObject{
public:
	/*!@name Constructors, initialization */
	//@{

		/*!
			\brief Constructs an TAdjustablePlane based on existing reference point, distance of the reference point from the plane and on the plane's normal vector.
			
			\param[in] referencePoint Fixed point, used as a reference point for the plane's definition.
			\param[in] refPointDistance Distance of the reference point from the plane. Is variable at any case, implicitly set to be a variable. 
			\param[in] planeNormalVect Plane's normal vector. 
			\param[in] planeNormalVectLock Lock state of the plane.
			\param[in] name Name of the adjustable plane.

			\note Reference point distance (refPointDistance) is always adjusted and is implicitly set to TRUE, therefore this object introduces up to 4 unknowns (up to 3 for 'planeNormalVect' + 1 RP distance).
				  Reference point itself is adjusted separately, in TAdjustablePoint object independently on the plane.
		*/
		TAdjustablePlane(const TAdjustablePoint* referencePoint, const TScalar& refPointDistance, const TFreeVector& planeNormalVect, const std::bitset<3>& planeNormalVectLock, const std::string& name);

		/// Create an unitialized plane. 
		static TAdjustablePlane createUninitialized(const std::string& name);

		void initialize(const TAdjustablePoint* referencePoint, const TScalar& refPointDistance, const TFreeVector& planeNormalVect, const std::bitset<3>& planeNormalVectLock);
		//@}

	/*!@name Access methods*/
	//@{
		/*! Returns the provisional value of the plane's normal vector*/
		inline TFreeVector		getNormalVectorProvisionalValue() const {return fProvValPlaneNormalVector;}

		/*! Returns the correction value of the plane's normal vector*/
		inline TFreeVector		getNormalVectorCorrection() const {	return fCorrectionPlaneNormalVector;}

		/*! Returns the estimated value of the plane's normal vector */
		inline TFreeVector		getNormalVectorEstimatedValue() const {	return fEstValPlaneNormalVector;}

		/*! Returns the estimated precision of the plane's normal vector */
		inline TFreeVector		getNormalVectorEstimatedPrecision() const {	return fEstPrecisionPlaneNormalVector;}

		/*! Returns the covariance element of the plane's normal vector */
		inline TFreeVector		getNormalVectorEstimatedCovariance() const {return fCovariancePlaneNormalVector;}


		/*! Returns the provisional value of the reference point distance. */
		inline TScalar 	getRefPtDistProvisionalValue() const {return fProvValRefPtDist;}

		/*! Returns the correction value of the reference point distance.. */
		inline TScalar		getRefPtDistCorrection() const {return fCorrectionRefPtDist;}

		/*! Returns the estimated value of the reference point distance. */
		inline TScalar		getRefPtDistEstimatedValue() const {	return fEstValRefPointDist;}
			
		/*! Returns the estimated precision of the reference point distance. */
		inline TScalar		getRefPDistEstimatedPrecision() const {	return fEstPrecisionRefPtDist;}

		/*! Returns the reference point position. */
		const TAdjustablePoint*		getReferencePoint() const {return fReferencePoint;}


		/*!
			Calculates the number of unknowns that are added to the adjustment by this plane.
			This number varies from zero to 4 unknowns (3 for normal vector of the plane + 1 for the reference point distance).

			\returns The number of unknowns for this plane.
		*/
		inline virtual int getNumUkn() const {
			return (int)!fixedStatePlaneVector[0] + (int)!fixedStatePlaneVector[1] + (int)!fixedStatePlaneVector[2] +
				  (int)!fixedStateRpDistance ;
		}

		/*! See \ref TVAdjustableObject::isFixed
			Reference point distance is never fixed, so the plane either.
		*/
		inline virtual bool isFixed() const { return false;}

		/*!
			Tells if at least one component is unfixed (variable). Reference point distance is eveytime variable. Returning TRUE.
		*/
		inline bool hasVariable() const { return true;}

		/*! 
			See \ref TVAdjustableObject::getFirstUidx
		*/
		inline virtual int getFirstUidx() const {	
			for (int i = 0; i < 3; i++)
				if (!fixedStatePlaneVector[i])
					return uidx_planeVector[i];		
				return uidx_rpDistance;		
		}

		/*!
			Tells if element of a normal vector of the plane is fixed
		*/
		inline bool isPlaneVectorFixed(int d){
			assert3D(d);
			return fixedStatePlaneVector[d]; 
		}

		/*!
			Returns index of an unknown index of the normal vector of the plane in LS matrices.

			\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
		*/
		inline int getPlaneVectorUnknIndex(int d) const { 
			assert3D(d);
			if(!fixedStatePlaneVector[d])
				return uidx_planeVector[d];
			throw std::logic_error("Trying to get unknown index from fixed coordinate.");
		}

		/*!
			Returns index of an unknown index of the reference point for the distance in LS matrices.
		*/
		inline int getRefPtDistUnknIndex() const { 
			return uidx_rpDistance;
		}


		/*! 
			See \ref TVAdjustableObject::getLastUidx
		*/
		inline virtual int getLastUidx() const {return uidx_rpDistance;}

		inline int getRefPtUidx() const {return uidx_rpDistance;}

		/*!
			\returns Name of the plane.
		*/
		inline virtual const std::string& getName() const { return fName;}
		/*!
			See \ref TVAdjustableObject::isInitialized
		*/
		inline virtual bool isInitialized() const{ return fInitialized;}

	//@}



	/*!@name Setting.*/
	//@{

	/*! 
		See \ref TVAdjustableObject::setFirstUidx

		\throws Throws a logic_error if no component of the plane is variable.
	*/
		virtual void setFirstUidx(int idx);


		/*! 
			See \ref TVAdjustableObject::setCorrection
		*/
		virtual void setCorrection(int idx, TReal value);

		/*! Sets the estimated precision after calculation */
		void	setEstimatedPrecision(int idx, TReal value);

		/*! Sets the XY covariance after calculation 	
			\param[in] value Value to be set.
		*/
		void	setXYNormalVectorCovariance(TReal value);

		/*! Sets the YZ covariance after calculation 	
			\param[in] value Value to be set.
		*/
		void	setYZNormalVectorCovariance(TReal value);

	   /*! Sets the XZ covariance after calculation 	
			\param[in] value Value to be set.
		*/
		void	setXZNormalVectorCovariance(TReal value);

	//@}

private:
	
	const TAdjustablePoint*		fReferencePoint; /*!< Reference point*/

	//Reference point distance definition
	TScalar					fProvValRefPtDist; /*!< Provisional value of the reference point distance. */
	TScalar					fCorrectionRefPtDist; /*!< Correction of the reference point distance.  */
	TScalar					fEstValRefPointDist; /*!< Estimated value of the reference point distance. */
	TScalar					fEstPrecisionRefPtDist;  /*!<Estimated precision of the reference point distance. */

	//Definition of the plane's normal vector
	TFreeVector				fProvValPlaneNormalVector; /*!< Provisional value of the normal vector of the plane. */
	TFreeVector				fCorrectionPlaneNormalVector; /*!< Correction of the normal vector of the plane. */
	TFreeVector				fEstValPlaneNormalVector; /*!< Estimated value of the normal vector of the plane. */
	TFreeVector				fEstPrecisionPlaneNormalVector;  /*!<Estimated precision of the normal vector of the plane. */
	TFreeVector				fCovariancePlaneNormalVector;  /*!<Covariance of the normal vector of the plane. */

	std::bitset<3> fixedStatePlaneVector;	/*!< Tells which element of the plane's normal vector is fixed(TRUE).*/
	
	bool fixedStateRpDistance;	/*!< Tells if reference point distance is fixed or not. So far it is in any case variable.*/

	int uidx_planeVector[3]; /*!< Position of the plane normal vector elements in the LS input matrix (columns).*/
	int uidx_rpDistance;	/*!< Position of the reference point distance in the LS input matrix (column).*/

	std::string fName; /*!< Name of the adjustable plane */

	bool fInitialized;
};

#endif //TADJUSTABLE_PLANE
