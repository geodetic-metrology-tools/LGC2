/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef LGCADJUSTABLE_POINT
#define LGCADJUSTABLE_POINT


//SURVEYLIB
#include <TAdjustablePoint.h>
//LGC
#include <LGCAdjustableObjectCollection.h>
#include <Global.h>

class TLGCData;

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
	LGCAdjustablePoint(const TPositionVector& pos, bool isXfixed, bool isYfixed, bool isZHfixed, const std::string& name, TRefSystemFactory::ERefFrame referential, TDataTreeIterator positionInTree);

	/// Create an unitialized point. 
	static LGCAdjustablePoint createUninitialized(const std::string& name);
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
		inline virtual bool isFixed() const { return ((fixedState[0] && fixedState[1] && fixedState[2])|allfixedParam);}

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
		TDataTreeIterator	getFrameTreePosition() const{ return fFramePosition; };


		/*!
			\brief Returns The boolean result of the query.

			Checks if a component of the point is excluded from the adjustment.

			\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
		*/
		inline virtual bool isCoordinateFixed(int d) const { 
			assert3D(d);
			return (fixedState[d]|allfixedParam); 
		}

		/*!
			\brief Returns index of an unknown point coordinate in a LS matrices.

			\param[in] d Allowed values are 0(X), 1(Y) and 2(Z).
		*/
		int virtual getCoordinateUnknIndex(int d) const;

#if USE_SERIALIZER
		// Inherited via Serializable
		virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
#endif

	//@}

	/*!@name Settings */
	//@{

        /// Rename the adjustable point
        virtual void setName(const std::string name) { fName = name; }

        /// Set the frame position to *pos*
        void setFrameTreePosition(const TDataTreeIterator& pos) { fFramePosition = pos; };

        /// Reset the provisional position vector
        virtual void setProvisionalValue(const TReal& x, const TReal& y, const TReal& z);

		/*! 
			\brief See \ref TVAdjustableObject::setCorrection

			Sets correction for the current estimatedValue and updates it, if point has fixed H value transformation is made.

			\throws Throws a logic_error if the adjusted point does not contain required index.
		*/
      virtual void setCorrection(int idx, TReal value);

		/// Sets the XY covariance after calculation
		virtual void	setXYEstimatedCovariance(TReal value);

		/// Sets the YZ covariance after calculation 
		virtual void	setYZEstimatedCovariance(TReal value);

	   /// Sets the XZ covariance after calculation 
		virtual void	setXZEstimatedCovariance(TReal value);

		/*! 
		    \brief See \ref TVAdjustableObject::setFirstUidx

			\throws Throws a logic_error if no component of the point is variable, i.e. a fixed point.
		*/
		virtual void setFirstUidx(int idx);

		/// Update the adjustment information of a point, used to set point coordinates fixed if ALLFIXED used
		virtual void updateFixedState(bool lx, bool ly, bool lz);


		///Set the boolean reference allfixedParam (to the TLGCConfig binary option ALLFIXED)
		static void setAllFixedParam(const bool& param){ allfixedParam = param; };
	//@}

		///Transform sigma a posteriori (= estimated precision) in root
		static TFreeVector transformSigmaInRoot(const LGCAdjustablePoint& pv, const TLGCData* fData);
		///Transform sigma a posteriori (= estimated precision) to arbitrary subframe
		static TFreeVector transformSigma(const LGCAdjustablePoint& pv, const TLGCData* fData, const TDataTreeIterator toFrame);
		static TFreeVector transformSigma(const LGCAdjustablePoint& pv, const TLGCData* fData, const std::string toFrame);

		/// Returns true if this point is defined in the ROOT frame
		bool isInRootFrame();

private:

	TDataTreeIterator fFramePosition; /*!< Iterator on the position in the tree. */

	static bool allfixedParam;/*!< Reference to the boolean which indicate if ALLFIXED option is used. By default, the value is false.*/

	/*!Private constructor for creating uninitialized object	*/
	LGCAdjustablePoint(const std::string& name);
};

#endif //TADJUSTABLE_POINT
