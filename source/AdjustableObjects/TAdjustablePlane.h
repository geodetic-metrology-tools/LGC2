#ifndef TADJUSTABLE_PLANE
#define TADJUSTABLE_PLANE

#include "TVAdjustableObject.h"
#include "TLGCAngle.h"
#include "TScalar.h"
#include <bitset>

class TAdjustablePoint;

/*! 
	\brief Adds adjustable information to a plane.

	Plane is defined throught a Reference Point (TAdjustablePoint), distance of this Reference point (TScalar) from the plane and two angles (LGC::TAngle) defining the normal vector of the plane.
*/
class TAdjustablePlane : public TVAdjustableObject{
public:
	/*!@name Constructors/Initialization */
	//@{

		/*!
			\brief Constructs an TAdjustablePlane based on existing reference point, distance of the reference point from the plane and two angles used to represent the normal vector of the plane.
			
			\param[in] referencePoint Fixed point, used as a reference point for the plane definition.
			\param[in] refPointDistance Distance of the reference point from the plane. Is variable at any case, implicitly set to be a variable. 
			\param[in] theta Theta angle (horizontal direction). 
			\param[in] phi Phi angle (zenith distance). 
			\param[in] thetaFixed Boolean value telling if the horizontal direction Theta angle is fixed(TRUE) or variable(FALSE).
			\param[in] phiFixed Boolean value telling if the zenith distance Phi angle is fixed(TRUE) or variable(FALSE).
			\param[in] name Name of the adjustable plane.

			\note Reference point distance (refPointDistance) is always variable, this object therefore introduces from 1 up to 3 unknowns (up to 2 for the two angle THETA and PHI + 1 RP distance). Reference point is adjusted separately from the TAdjustablePoint object independently on the plane.
		*/
      TAdjustablePlane(const TAdjustablePoint* referencePoint, const TLength& refPointDistance, const LGC::TAngle& theta, const LGC::TAngle& phi, bool thetaFixed, bool phiFixed, const std::string& name);

		/// Create an unitialized plane. 
		static TAdjustablePlane createUninitialized(const std::string& name);

		/// See the constructor definition.
      void initialize(const TAdjustablePoint* referencePoint, const TLength& refPointDistance, const LGC::TAngle& theta, const LGC::TAngle& phi, bool thetaFixed, bool phiFixed);
		//@}

	/*!@name Access methods*/
	//@{
		/// Returns a constant reference on Theta angle provisional value.
		inline const LGC::TAngle&		getThetaProvisionalValue() const {return fProvValTheta;}

		/// Returns a constant reference on the correction of the Theta angle.
		inline const LGC::TAngle&		getThetaCorrection() const {	return fCorrectionTheta;}

		/// Returns a constant reference on the estimated value of the Theta angle.
		inline const LGC::TAngle&		getThetaEstimatedValue() const {	return fEstValTheta;}

		/// Returns a constant reference on the estimated precision of the Theta angle. 
		inline const LGC::TAngle&		getThetaEstimatedPrecision() const {	return fEstPrecisionTheta;}


		/// Returns a constant reference on Phi angle provisional value.
		inline const LGC::TAngle&		getPhiProvisionalValue() const {return fProvValPhi;}

		/// Returns a constant reference on the correction value of the Phi angle.
		inline const LGC::TAngle&		getPhiCorrection() const {	return fCorrectionPhi;}

		/// Returns a constant reference on the estimated value of the Phi angle.
		inline const LGC::TAngle&		getPhiEstimatedValue() const {	return fEstValPhi;}

		/// Returns a constant reference on the estimated precision of the Phi angle. 
		inline const LGC::TAngle&		getPhiEstimatedPrecision() const {	return fEstPrecisionPhi;}


		/// Returns a constant reference on the provisional value of the reference point distance. 
      inline const TLength& 		getRefPtDistProvisionalValue() const { return fProvValRefPtDist; }

		/// Returns a constant reference on the correction value of the reference point distance. 
      inline const TLength&		getRefPtDistCorrection() const { return fCorrectionRefPtDist; }

		/// Returns a constant reference on the estimated value of the reference point distance. 
      inline const TLength&		getRefPtDistEstimatedValue() const { return fEstValRefPointDist; }
			
		/// Returns a constant reference on the estimated precision of the reference point distance. 
      inline const TLength&		getRefPDistEstimatedPrecision() const { return fEstPrecisionRefPtDist; }

		/// Returns pointer to the Reference point position. 
		const TAdjustablePoint*		getReferencePoint() const {return fReferencePoint;}


		/*!
			\brief Returns the number of unknowns for this plane.

			Calculates the number of unknowns that are added to the adjustment by this plane.
			This number varies from 1 to 3 unknowns (2 angles + 1 for the reference point distance (always)). Reference point itself is adjusted separately outside the plane object.
		*/
		inline virtual int getNumUnkn() const {
			return (int)!fThetaFixed + (int)!fPhiFixed + (int)!fRefPtDistFixed;
		}

		/*! 
			\brief See \ref TVAdjustableObject::isFixed

			Reference point distance is never fixed, so the plane either.	
		*/
		inline virtual bool isFixed() const { return false;}

		///	Tells if at least one component is unfixed (variable). Reference point distance is always variable. True returned.
		inline bool hasVariable() const { return true;}
 
		///	See \ref TVAdjustableObject::getFirstUidx
		inline virtual int getFirstUidx() const {	
				if (!fThetaFixed)
					return uidx_Theta;		
				else if (!fPhiFixed)
					return uidx_Phi;
				else
					return uidx_rpDistance;		
		}


		///	Tells if the Theta angle is fixed or unknown.
		inline bool isThetaFixed(){
			return fThetaFixed; 
		}


		///	Tells if the Phi angle is fixed or unknown.
		inline bool isPhiFixed(){
			return fPhiFixed; 
		}


		/// Returns the index of an unknown index in LS matrices of the Theta angle (horizontal direction) of the plane.
		inline int getThetaUnknIndex() const { 
			if(!fThetaFixed)
				return uidx_Theta;
			throw std::logic_error("Trying to get unknown index from fixed Theta angle of the plane.");
		}

		/// Returns index of an unknown index in LS matrices of the Phi angle (zenith distance) of the plane.
		inline int getPhiUnknIndex() const { 
			if(!fThetaFixed)
				return uidx_Phi;
			throw std::logic_error("Trying to get unknown index from fixed Theta angle of the plane.");
		}

		/// Returns index of an unknown index of the reference point for the distance in LS matrices.
		inline int getRefPtDistUnknIndex() const { 
			return uidx_rpDistance;
		}



		///	See \ref TVAdjustableObject::getLastUidx
		inline virtual int getLastUidx() const {return uidx_rpDistance;}

		inline int getRefPtDistUidx() const {return uidx_rpDistance;}

		/// Returns Name of the plane.
		inline virtual const std::string& getName() const { return fName;}

		///	See \ref TVAdjustableObject::isInitialized
		inline virtual bool isInitialized() const{ return fInitialized;}

		/*! 
			\brief Re-initialise the object

			Sets the estimated values to be the provisional values and for the precisions, correections and covariances zeros.
		*/
		void reInitialise();
	//@}



	/*!@name Setting.*/
	//@{

		/*! 
			\brief See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if no component of the plane is variable.
		*/
		virtual void setFirstUidx(int idx);


		///	See \ref TVAdjustableObject::setCorrection
		virtual void setCorrection(int idx, TReal value);


		/// Sets the estimated precision after calculation 
		void	setEstimatedPrecision(int idx, TReal value);

	//@}

private:
	std::string fName; /*!< Name of the adjustable plane */

	const TAdjustablePoint*		fReferencePoint; /*!< Reference point*/

	//Reference point distance definition
   TLength					fProvValRefPtDist; /*!< Provisional value of the reference point distance. */
   TLength					fCorrectionRefPtDist; /*!< Correction of the reference point distance.  */
   TLength					fEstValRefPointDist; /*!< Estimated value of the reference point distance. */
   TLength					fEstPrecisionRefPtDist;  /*!<Estimated precision of the reference point distance. */

	//Definition of the plane normal vector angles
	LGC::TAngle			fProvValTheta; /*!< Provisional value of the Theta angle. */
	LGC::TAngle			fCorrectionTheta; /*!< Correction of the normal vector of the Theta angle. */
	LGC::TAngle			fEstValTheta; /*!< Estimated value of the Theta angle. */
	LGC::TAngle			fEstPrecisionTheta;  /*!<Estimated precision of the Theta angle. */


	LGC::TAngle			fProvValPhi; /*!< Provisional value of the Phi angle. */
	LGC::TAngle			fCorrectionPhi; /*!< Correction of the Phi angle. */
	LGC::TAngle			fEstValPhi; /*!< Estimated value of the Phi angle. */
	LGC::TAngle			fEstPrecisionPhi;  /*!<Estimated precision of the Phi angle. */

	bool  fThetaFixed;	/*!< Tells if Theta is fixed or not (TRUE means fixed).*/
	bool  fPhiFixed;	/*!< Tells if Phi is fixed or not (TRUE means fixed).*/
	bool  fRefPtDistFixed;	/*!< Tells if reference point distance is fixed or not. So far it is in any case variable.*/

	int uidx_Theta; /*!< Index of the Theta unknown angle in the LS input matrix (unknowns). If angle fixed, -1 stored.*/
	int uidx_Phi; /*!< Index of the Phi unknown angle in the LS input matrix (unknowns). If angle fixed, -1 stored.*/
	int uidx_rpDistance;	/*!< Index of the Reference point unknown distance in the LS input matrix (unknowns). If distance fixed, -1 stored.*/

	bool fInitialized;
};
#endif //TADJUSTABLE_PLANE
