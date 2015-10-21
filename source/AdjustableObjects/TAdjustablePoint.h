#ifndef TADJUSTABLE_POINT
#define TADJUSTABLE_POINT

#include "TVAdjustableObject.h"
#include "TLGCRefFrame.h"
#include "TPositionVector.h"
#include "TFreeVector.h"
#include "TSpatialStatus.h"

#ifndef isnan
#define isnan(x) ((x)!=(x))
#endif

/*! 
	\ingroup AdjustableObjects
	\brief Adds adjustable information to a point represented by a TPositionVector class.
*/
class TAdjustablePoint : public TVAdjustableObject
{
public:

		/// comments (multi-line) above the point definition including line breaks
		std::string hdrcomment;
		/// DB comment after the point definition
		std::string eolcomment;
		/// the line of the point definition in the input file
		int line;

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
		TAdjustablePoint(const TPositionVector& pos, bool isXfixed, bool isYfixed, bool isZHfixed, const std::string& name, TLGCRefFrame::ERefs referential, TDataTreeIterator positionInTree);

		/// Create an unitialized point. 
		static TAdjustablePoint createUninitialized(const std::string& name);
		
	//@}
			
	/*!@name Access methods*/
	//@{
		/// Returns a constant reference on the provisional value of the position vector
		inline const TPositionVector&	getProvisionalValue() const {return fProvisionalValue;}

		/// Returns a constant reference on the correction value of the point
		inline const TFreeVector&		getCorrection() const {	return fCorrection;}

		/// Returns a constant reference on the estimated value of the point
		inline const TPositionVector&	getEstimatedValue() const {	return fEstimatedValue;}

		/*!
			\brief Calculates and \returns the number of unknowns that are added to the adjustment by this point.
			This number varies from zero to three unknowns.
		*/
		inline virtual int getNumUnkn() const { return !(int)fixedState[0] + !(int)fixedState[1] + !(int)fixedState[2];}


		/// Tells if at least one coordinate is unfixed (variable).
		inline bool hasVariable() const { return !fixedState[0] || !fixedState[1] || !fixedState[2];}
		

		/// See \ref TVAdjustableObject::isFixed
		inline virtual bool isFixed() const { return (fixedState[0] && fixedState[1] && fixedState[2]);}

		/*! 
			\brief See \ref TVAdjustableObject::getFirstUidx

			\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
		*/
		inline virtual int getFirstUidx() const {
			for (int i = 0; i < 3; i++)
				if (!fixedState[i])
					return uidx[i];
			throw std::logic_error("Trying to get unknown index from fixed point.");
		}

		/*! 
			\brief See \ref TVAdjustableObject::getLastUidx

			\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
		*/
		inline virtual int getLastUidx() const {
			for (int i = 2; i >= 0; i--)
				if (!fixedState[i])
					return uidx[i];
			throw std::logic_error("Trying to get unknown index from fixed point.");
		}

		/// Returns name of the point */
		inline virtual const std::string& getName() const { return fName; }

		/// Returns the semi major axis of the error ellipsoid  
		TLength				getErrorEllMajorAxis() const;
		/// Returns the semi minor axis of the error ellipsoid 
		TLength				getErrorEllMinorAxis() const;
		//Not used
		/// Returns the gisement of the error ellipsoid 
		//TAngle				getErrorEllGis() const;

		/// Returns the three axes of the error ellipsoid
		struct ErrorEllipsoid {
			TReal vx[3]; // Direction (unit)vector of the x-semiaxis
			TReal vy[3]; // Direction (unit)vector of the y-semiaxis
			TReal vz[3]; // Direction (unit)vector of the z-semiaxis
			TReal lx; // Length of the x-semiaxis
			TReal ly; // Length of the y-semiaxis
			TReal lz; // Length of the z-semiaxis
		};
		ErrorEllipsoid getErrorEllipsoid() const;

		/*!
			\brief Returns the estimated coordinate of TAdjustablePoint.

			\param[in] idxCoordinate Allowed values are 0(X), 1(Y) and 2(Z) of the coordinate we want to access.
		*/
		inline TReal	getEstValue(int idxCoordinate) const {
			assert3D(idxCoordinate);
			if(idxCoordinate == 0)
				return fEstimatedValue.getX().getMetresValue();
			else if(idxCoordinate == 1)
            return fEstimatedValue.getY().getMetresValue();
			else
            return fEstimatedValue.getZ().getMetresValue();
		}
// If these methods are needed, must be rewritten, because Estimated value and Provisional value can be in different reference systems (2DH and 3DCartesian)/

		/// Returns the estimated DX of TAdjustablePoint
      inline TReal getDXValue() const { return (fEstimatedValue.getX().getMetresValue() - fProvisionalValue.getX().getMetresValue()); }

		/// Returns the estimated DY of TAdjustablePoint
      inline TReal getDYValue() const { return (fEstimatedValue.getY().getMetresValue() - fProvisionalValue.getY().getMetresValue()); }

		/// Returns the estimated DZ of TAdjustablePoint
      inline TReal getDZValue() const { return (fEstimatedValue.getZ().getMetresValue() - fProvisionalValue.getZ().getMetresValue()); }

//This necessary when the provisional value stored is defined in XYH.
#if 0
		/*!Compute and return the estimated H coordinate of TAdjustablePoint (this object is always in CCS)*/
		TScalar				getHEstValue(const ERefFrame	refFrame) const;

		/*!Compute and return the estimated H coordinate of TAdjustablePoint (this object is always in CCS)*/
		TScalar				getHProvValue(const ERefFrame	refFrame) const;

		/*!Compute and return the estimated H coordinate of TAdjustablePoint (this object is always in CCS)*/
		TScalar				getDHValue(const ERefFrame	refFrame) const {return getHEstValue(refFrame) - getHProvValue(refFrame);}

		/*!Compute and return the estimated N */
		TLength				getNEstValue(const EGeoid	geo) const;
#endif

		/// Returns the estimated XY covariance
      inline TReal getXYCovar() const { return fCovariance.getX().getMetresValue(); }

		/// Returns the estimated YZ covariance
      inline TReal getYZCovar() const { return fCovariance.getY().getMetresValue(); }

		/// Returns the estimated XZ covariance
      inline TReal getXZCovar() const { return fCovariance.getZ().getMetresValue(); }


		/// Returns the estimated X estimated precision
      inline TReal getXEstPrecision() const { return fEstimatedPrecision.getX().getMetresValue(); }

		/// Returns the estimated Y estimated precision
      inline TReal getYEstPrecision() const { return fEstimatedPrecision.getY().getMetresValue(); }

		/// Returns the estimated Z estimated precision
      inline TReal getZEstPrecision() const { return fEstimatedPrecision.getZ().getMetresValue(); }

		/// Returns iterator to the position in the tree.
		TDataTreeIterator	getFrameTreePosition() const{return fFramePosition;};

		/*!
			\brief Returns The boolean result of the query.

			Checks if a component of the point is excluded from the adjustment.

			\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
		*/
		inline bool isCoordinateFixed(int d) const { 
			assert3D(d);
			return fixedState[d]; 
		}

		/*!
			\brief Returns index of an unknown point coordinate in a LS matrices.

			\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
		*/
		inline int getCoordinateUnknIndex(int d) const { 
			assert3D(d);
			if(!fixedState[d])
				return uidx[d];
			throw std::logic_error("Trying to get unknown index from fixed coordinate.");
		}

		/*!
			
			\brief Returns A standard deviations of the point's component.
			
			\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the point's component.

			\throws Throws a runtime_error if the standard deviation not assigned.
		*/
		TReal getStandDev(int d) const;

		/// Returns reference frame of the point 
		inline TLGCRefFrame::ERefs getReferenceFrame() const {return fReferential;}

		/// \see TAdjustableObject::isInitialized
		inline virtual bool isInitialized() const { return !isnan(fProvisionalValue.getX().getMetresValue());}

		/// Returns Boolean value telling if the standard deviations were assigned to the point.
		inline bool hasStandDeviations() const{return !isnan( fStandardDeviations[0]);} 

		/// Returns the height estimated value
		TReal getHEstValue ()const;

		/// Returns the spatial status
		inline TSpatialStatus::ESpatialStatus getSpatialStatus() const{return fSpatialStatus;}
	//@}

	/*!@name Settings */
	//@{
		/*! 
			\brief See \ref TVAdjustableObject::setCorrection

			Sets correction for the current estimatedValue and updates it, if point has fixed H value transformation is made.

			\throws Throws a logic_error if the adjusted point does not contain required index.
		*/
      virtual void setCorrection(int idx, TReal value);

		/// Sets the estimated precision after calculation
      virtual void setEstimatedPrecision(int idx, TReal value);

		/// Sets the XY covariance after calculation
		void	setXYEstimatedCovariance(TReal value);

		/// Sets the YZ covariance after calculation 
		void	setYZEstimatedCovariance(TReal value);

	   /// Sets the XZ covariance after calculation 
		void	setXZEstimatedCovariance(TReal value);

		/*! 
		    \brief See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
		*/
		virtual void setFirstUidx(int idx);

		/// Update the adjustment information of a point, used to set point coordinates fixed if ALLFIXED used
		void updateFixedState(bool lx, bool ly, bool lz);

		/// Assign standard deviations to this point
		void setStandardDeviations(TReal stDevX, TReal stDevY, TReal stDevZ);

		/*! 
			\brief Re-initialise the object 

			Sets the estimated values to be the provisional values and for the precisions, corrections and covariances zeros.
		*/
		void reInitialise();
	//@}

private:

	TPositionVector			fProvisionalValue; /*!< point's provisional value */
	TFreeVector				fCorrection; /*!< point's correction after calculation  */
	TPositionVector			fEstimatedValue; /*!< point's estimated value after calculation */
	TFreeVector				fEstimatedPrecision; /*!< point's estimated precision after calculation */
	TFreeVector				fCovariance; /*!< point's covariance */	

	TLGCRefFrame::ERefs fReferential; /*!< Reference frame of the point */

	std::string fName; /*!< Name of the adjustable point. */

	TDataTreeIterator fFramePosition; /*!< Iterator on the position in the tree. */

	TReal fStandardDeviations[3]; /*!< Standard deviations of the point.*/

	bool fixedState[3]; /*!< Tells which element of the point is FIXED or VARIABLE (TRUE means that point element is fixed).*/
	int  uidx[3];	 /*!< Indices of the point elements in LS input matrices (unknowns).*/

	void setDefaults(bool lx, bool ly, bool lz);  /*!< Set default adjustment related values.*/

	bool fHfixed; /*!< TRUE if provisional value given in XYH coordinates and H set to be TRUE */

	//This two variables are needed for determinig whether all variable components of a point were set in particular LS calculation run. 
	//We need to control it, because in a case that point was defined in 2D+H coordinates and H  with the height value fixed, we need to make transformation X1Y1Z1 -> X1Y1H0 --> X1Y1Z0new,
	//but only after all variable components of a point were updated in this LS run.
	bool fXValueSet; /*!< Tells whether X value was set in one particular LS calculation run, if X component is variable. */	
	bool fYValueSet; /*!< Tells whether Y value was set in one particular LS calculation run, if Y component is variable. */

	//This method set an original H0 value to fEstimatedValue, transform it into XYZ coordinates
	void transformEstimatedValue();

	/*!
		Private constructor for creating uninitialized object
	*/
	TAdjustablePoint(const std::string& name);

	TSpatialStatus::ESpatialStatus fSpatialStatus;
};
#endif //TADJUSTABLE_POINT