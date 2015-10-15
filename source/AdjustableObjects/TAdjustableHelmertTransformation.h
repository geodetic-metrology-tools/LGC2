#ifndef TADJUSTABLE_HELMERT_TRANSFORMATION
#define TADJUSTABLE_HELMERT_TRANSFORMATION

#include <bitset>

//#include "THelmertTransformation.h"
#include "TVAdjustableObject.h"
#include "TTransformParameters.h"
#include "TTransformation.h"
#include "TDirectTransformation.h"
#include "TInverseTransformation.h"

/*!
Adds adjustment information to a local transformation.
Impementation detail: first translations, then rotations as unknown index
*/
class TAdjustableHelmertTransformation : public TVAdjustableObject { 
	public:

		/*!@name Construction */
		//@{	
			/*!
				An adjusted transformation is created by passing a transformation which is kept as a copy.
				This allows the use of temporary objects for construction.

				\note There is no way to bind an existing transformation to an adjusted transformation at a later 
				stage. Instead the composite of trafo and adjTrafo must be created and the trafo inside adjtrafo must be used.
			*/
			//TAdjustableHelmertTransformation(TTransformation* t, const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale);
			TAdjustableHelmertTransformation(const string& name, const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale);

			/*!
				Creates an uninitialized TAdjustableHelmertTransformation object
			*/
			static TAdjustableHelmertTransformation createUninitialized(const std::string& name);
		//@}

		/*!@name Access methods*/
		//@{

			/*!
				Calculates the number of unknowns that are added to the adjustment by this transformation.
				This number varies from zero to six unknowns

				\returns The number of unknowns for this transformation
			*/
			virtual int getNumUkn() const;
		

			/// See \ref TVAdjustableObject::isFixed
			virtual bool isFixed() const {
				return (fixedRotations.all() && fixedTranslations.all() && fixedScale.all());
			}


			/*!
				Tells if at least one component if the transformation is unfixed (variable).
			*/
			inline bool hasVariable() const { 
				return !fixedRotations.at(0) || !fixedRotations.at(1) || !fixedRotations.at(2) || !fixedTranslations.at(0) || !fixedTranslations.at(1) || !fixedTranslations.at(2) || !fixedScale.at(0);
			}

			/*! 
				See \ref TVAdjustableObject::getFirstUidx

				\throws Throws a logic_error if no component of the transformation is variable, i.e. a fixed transformation.
			*/
			virtual int getFirstUidx() const;	
			/*! 
				See \ref TVAdjustableObject::getLastUidx

				\throws Throws a logic_error if no component of the transformation is variable, i.e. a fixed transformation.
			*/
			virtual int getLastUidx() const;


			/*!
				\returns Name of the transformation.
			*/
			const std::string& getName() const { return name;}

			void setName(const std::string& _name) { name = _name;}

			virtual bool isInitialized() const { return line != -1;}
			 
			int& getLine() {return line;}

			/*!
				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's element.

				\returns Translation's unknown index on a 'd' position.

				\throws Throws a logic_error if the translation's element is fixed.
			*/ 
			int getTranslationUnknIndex(int d) const;

			/*!
				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's element.

				\returns Rotation's unknown index on a 'd' position.

				\throws Throws a logic_error if the rotation's element is fixed.
			*/ 
			int getRotationUnknIndex(int d) const;

			/*!
				\returns Scale's unknown index.

				\throws Throws a logic_error if the scale is fixed.
			*/ 
			int getScaleUnknIndex() const;

			/*!
				Checks if a component of the translation is fixed.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's component.

				\returns The boolean result of the query. TRUE if fixed.
			*/ 
			inline bool isTranslationFixed(int d) const { 
				assert3D(d);
				return fixedTranslations[d]; 
			}

			/*!
				Checks if a component of the rotation is fixed.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's component.

				\returns The boolean result of the query. TRUE if fixed.
			*/ 
			inline bool isRotationFixed(int d) const { 
				assert3D(d);
				return fixedRotations[d]; 
			}

			/*!
				Checks if scale is fixed.
				\returns The boolean result of the query. TRUE if fixed.
			*/ 
			inline bool isScaleFixed() const { 
				return fixedScale[0];
			}

			/*!
				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's component.

				\returns A standard deviations of the translation.
			*/
			TLength getTranslationStandDev(int d) const;

			/*!
				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's component.

				\returns A standard deviations of the rotation.
			*/
			LGC::TAngle getRotationStandDev(int d)const;

			/*!
				\returns A standard deviations of the scale factor.
			*/
			TLength getScaleStandDev()const;

			/*!
				\returns True if at least one component of the transformation has standard deviation (i.e. was measured).
			*/
			bool hasStandDev(); 

			/*!
				\returns True if the translation part has assigned standard deviation. Allowed value are 0(X transl.), 1(Y transl.), 2(Z transl)
			*/
			bool hasTranslStandDev(int d)const;

			/*!
				\returns True if the rotation part has assigned standard deviation. Allowed value are 0(X rot.), 1(Y rot.), 2(Z rot.)
			*/
			bool hasRotationStandDev(int d)const;

			/*!
				\returns True if the scale has assigned standard deviation.
			*/
			bool hasScaleStandDev()const;

			LGC::TAngle getEstimatedPrecisionRot(int d)const;

			TLength getEstimatedPrecisionTransl(int d)const;

			TLength getEstimatedPrecisionScale()const;


			/*!Return the estimated XY covariance*/
			TLength				getXYCovarTransl() const;

			/*!Return the estimated YZ covariance*/
			TLength				getYZCovarTransl() const;

			/*!Return the estimated XZ covariance*/
			TLength				getXZCovarTransl() const;


			/*!Return the estimated XY covariance*/
			LGC::TAngle			getXYCovarRot() const;

			/*!Return the estimated YZ covariance*/
			LGC::TAngle			getYZCovarRot() const;

			/*!Return the estimated XZ covariance*/
			LGC::TAngle			getXZCovarRot() const;
	
		//@}

		/*!@name Settings */
		//@{

			/*! 
				See \ref TVAdjustableObject::setFirstUidx

				\throws Throws a logic_error if no component of the transformation is variable.
			*/
			virtual void setFirstUidx(int idx);

		
			/*! 
				See \ref TVAdjustableObject::setParameter
				\note Either in radians if setting rotation or scalar value if setting translation. 
			*/
			virtual void setCorrection(int idx, TReal value);

			/*!
				Sets a standard deviation of a translation's component.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's component.
				\param[in] stDev Standard deviation.
			*/ 
			void setTranslationStandDev(int d, TReal stDev);

			/*!
				Sets a standard deviation of a rotation's component.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's component.
				\param[in] stDev Standard deviation.
			*/ 
			void setRotationStandDev(int d, LGC::TAngle stDev);


		    /*!
				Sets a standard deviation of a scale.

				\param[in] stDev Standard deviation.
			*/ 
			void setScaleStandDev(TReal stDev);

			/*! Sets the XY covariance after calculation 	
				\param[in] value Value to be set.
			*/
			void	setXYTranslationCovariance(TReal value);

			/*! Sets the YZ covariance after calculation 	
				\param[in] value Value to be set.
			*/
			void	setYZTranslationCovariance(TReal value);

		   /*! Sets the XZ covariance after calculation 	
				\param[in] value Value to be set.
			*/
			void	setXZTranslationCovariance(TReal value);


			/*! Sets the XY covariance after calculation 	
				\param[in] value Value to be set in radians [RAD].
			*/
			void	setXYRotationCovariance(TReal value);

			/*! Sets the YZ covariance after calculation 	
				\param[in] value Value to be set in radians [RAD].
			*/
			void	setYZRotationCovariance(TReal value);

		   /*! Sets the XZ covariance after calculation.	
				\param[in] value Value to be set in radians [RAD].
			*/
			void	setXZRotationCovariance(TReal value);


			/*! Sets the estimated precision after calculation */
			void setEstimatedPrecision(int idx, TReal value);
		//@}

		void setParam(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz);
		void setParam(const TReal tx, const TReal ty, const TReal tz);
		void setParam(const TReal scl);
		void setParam(const TReal tx, const TReal ty, const TReal tz, const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, const TReal scl);
		
		void setTranslationCorrection (int idx, TReal value);
		void setRotationCorrection (int idx, LGC::TAngle value);
		void setScaleCorrection (TReal value);
		
		// returns the estimated Helmert parameters
		TransformParameters getEstParam() const {return fEstParameter;}
		// returns the provisional Helmert transformation
		TransformParameters getProvParam() const {return fProvParameter;}

		// returns the called estimated parameters
		inline TReal getScale () const {return fEstParameter.scale;}
		TReal getTranslation(int axis) const;
		LGC::TAngle getAngle(int axis) const;
		
	private:

		//Line in the input file where the tranformation was introduced
		int line;

		//Name of the transformation
		std::string name;

		/// Object composition: this transformation that is adjusted
		//TTransformation* transfo;

		TransformParameters fProvParameter; //Provisional paremeters for a Helmert transformation
		TransformParameters fEstParameter;  //Estimated paremeters for a Helmert transformation

		std::bitset<3> fixedTranslations; // Translation's fixed state
		std::bitset<3> fixedRotations; // Rotation's fixed state
		std::bitset<1> fixedScale; // Scale's fixed state

		TLength fTransStandDev[3]; //Standard deviations of translations
		LGC::TAngle fRotStandDev[3];  //Standard deviations of rotations 
		TLength fScaleStandDev; //Standard deviations of a scale

		int uidx_rot[3];  // Unknown indices of rotation
		int uidx_trans[3]; // Unknown indices of translation
		int uidx_scale; // Unknown indices of scale

		void setDefaults();
		void setDefaultsParams();

		//Estimated precisons of transformation parametres
		LGC::TAngle	 fEstPrecisionRotation[3];  /*!<Estimated precision of the rotation. */
		TLength		 fEstPrecisionTranslation[3];  /*!<Estimated precision of the translation. */
		TLength	     fEstPrecisionScale;  /*!<Estimated precision of the scale factor. */

		LGC::TAngle	 fCovarianceRotation[3];  /*!<Covariance of the rotation  XY, YZ, XZ */
     	TLength		 fCovarianceTranslation[3];  /*!<Covariance of the translation XY, YZ, XZ  */
};

#endif //TADJUSTABLE_HELMERT_TRANSFORMATION