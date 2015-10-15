#ifndef TADJUSTABLE_HELMERT_TRANSFORMATION
#define TADJUSTABLE_HELMERT_TRANSFORMATION

#include "TVAdjustableObject.h"
#include "TTransformParameters.h"
#include <bitset>
#include "TLength.h"
#include "TScalar.h"

/*! 
	\ingroup AdjustableObjects
	\brief Adds adjustable information to a TransformParameters class. 
	Impementation detail: first translations, then rotations and scale as last for the unknown indices assignment.
*/
class TAdjustableHelmertTransformation : public TVAdjustableObject { 
	public:

		/*!@name Construction */
		//@{	
			/*!
				\brief Creates an adjustable transfromation based on the TransformParameters structure. 

				\param[in] transfParam A reference to the transformation parameters that will be adjusted. The object will be copied so the poited-to object is no longer needed after construction.
				\param[in] fixedTranslations Bitset telling if the particular translation is fixed or variable.
				\param[in] fixedRotations    Bitset telling if the particular rotation is fixed or variable.
				\param[in] fixedScale		 Bitset telling if scale factor is fixed or variable.
				\param[in] name			     Name of the adjustable transformation.
			*/
			TAdjustableHelmertTransformation(const TransformParameters& transfParam, const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale, const string& name);


			/*!
				\brief Creates an adjustable identity transfromation. setParam() method should be use to set-up provisional values after the construction.

				\param[in] fixedTranslations Bitset telling if the particular translation is fixed or variable.
				\param[in] fixedRotations    Bitset telling if the particular rotation is fixed or variable.
				\param[in] fixedScale		 Bitset telling if scale factor is fixed or variable.
				\param[in] name				 Name of the adjustable object.
			*/
			TAdjustableHelmertTransformation(const std::bitset<3>& fixedTranslations, const std::bitset<3>& fixedRotations, const std::bitset<1>& fixedScale, const string& name);

			///	Creates an uninitialized TAdjustableHelmertTransformation object.
			static TAdjustableHelmertTransformation createUninitialized(const std::string& name);
		//@}

		/*!@name Access methods*/
		//@{

			/*!
				\brief Returns The number of unknowns of this transformation.

				Calculates the number of unknowns that are added to the adjustment by this transformation.
				This number varies from zero to seven unknowns
			*/
			virtual int getNumUnkn() const;
		

			/// See \ref TVAdjustableObject::isFixed
			virtual bool isFixed() const {
				return (fixedRotations.all() && fixedTranslations.all() && fixedScale.all());
			}


			///	Tells if at least one component of the transformation is not fixed (variable).
			inline bool hasVariable() const { 
				return !fixedRotations[0] || !fixedRotations[1] || !fixedRotations[2] || !fixedTranslations[0] || !fixedTranslations[1] || !fixedTranslations[2] || !fixedScale[0];
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


			/// Returns the name of the transformation.
			const std::string& getName() const { return name;}

			///	Sets the name of the transformation.
			void setName(const std::string& _name) { name = _name;}

			///	Transformation is everytime initialized, if needed in the future to have uninitialized transformation, change this method.
			virtual bool isInitialized() const { return true;}
			 
			/// Returns Number of line where the transfromation was defined.
			int& getLine() {return line;}

			/*!
				\brief Returns Translation unknown index on a 'd' position.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's element.

				\throws Throws a logic_error if the translation's element is fixed.
			*/ 
			int getTranslationUnknIndex(int d) const;

			/*!
				\brief Returns Rotation unknown index on a 'd' position.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's element.

				\throws Throws a logic_error if the rotation element is fixed.
			*/ 
			int getRotationUnknIndex(int d) const;

			/*!
				\brief Returns Scale's unknown index.

				\throws Throws a logic_error if the scale is fixed.
			*/ 
			int getScaleUnknIndex() const;

			/*!
				\brief Checks if a component of the translation is fixed.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation component.

				\returns The boolean result of the query. TRUE if fixed.
			*/ 
			bool isTranslationFixed(int d) const;

			/*!
				\brief Checks if a component of the rotation is fixed.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation component.

				\returns The boolean result of the query. TRUE if fixed.
			*/ 
			bool isRotationFixed(int d) const;

			/*!
				\brief Checks if scale is fixed.

				\returns The boolean result of the query. TRUE if fixed.
			*/ 
			inline bool isScaleFixed() const { 
				return fixedScale[0];
			}

			/*!
				
				\brief Returns the standard deviations of the translation.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's component.
			*/
			const TLength& getTranslationStandDev(int d) const;

			/*!
				\brief Returns the standard deviations of the rotation.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's component.
			*/
			const LGC::TAngle& getRotationStandDev(int d)const;

			/// Returns the standard deviations of the scale factor.
			const TScalar& getScaleStandDev()const;

			/// Returns TRUE if at least one component of the transformation has standard deviation, FLASE otherwise.
			bool hasStandDev(); 

			
			/// Returns TRUE if the translation part has assigned standard deviation, FALSE otherwise. Allowed value are 0(X transl.), 1(Y transl.), 2(Z transl)
			bool hasTranslStandDev(int d)const;

			
			/// Returns TRUE if the rotation part has assigned standard deviation, FALSE otherwise. Allowed value are 0(X rot.), 1(Y rot.), 2(Z rot.)
			bool hasRotationStandDev(int d)const;

			
			/// Returns TRUE if the scale has assigned standard deviation, FALSE otherwise.
			bool hasScaleStandDev()const;

			/// Returns the estimated precision of the dth rotation
			const LGC::TAngle& getEstimatedPrecisionRot(int d)const;

			/// Returns the estimated precision of the dth translation
			const TLength& getEstimatedPrecisionTransl(int d)const;

			/// Returns the estimated precision of the scale factor
			const TScalar& getEstimatedPrecisionScale()const;


			/// Returns XY translation covariance
			const TLength&				getXYCovarTransl() const;

			/// Returns YZ translation covariance
			const TLength&				getYZCovarTransl() const;

			/// Returns XZ translation covariance
			const TLength&				getXZCovarTransl() const;


			/// Returns XY rotation covariance
			const LGC::TAngle&			getXYCovarRot() const;

			/// Returnss YZ rotation covariance
			const LGC::TAngle&			getYZCovarRot() const;

			/// Returns XZ rotation covariance
			const LGC::TAngle&			getXZCovarRot() const;
	
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
				\brief Sets a standard deviation of a translation's component.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the translation's component.
				\param[in] stDev Standard deviation.
			*/ 
			void setTranslationStandDev(int d, TReal stDev);

			/*!
				\brief Sets a standard deviation of a rotation component.

				\param[in] d Allowed values are 0(X), 1(Y) and 2(Z) of the rotation's component.
				\param[in] stDev Standard deviation.
			*/ 
			void setRotationStandDev(int d, LGC::TAngle stDev);


		    /*!
				\brief Sets a standard deviation of a scale.

				\param[in] stDev Standard deviation.
			*/ 
			void setScaleStandDev(TReal stDev);

			/*! 
				\brief Sets the XY covariance after calculation 	
				\param[in] value Value to be set.
			*/
			void	setXYTranslationCovariance(TReal value);

			/*! 
				\brief Sets the YZ covariance after calculation 	
				\param[in] value Value to be set.
			*/
			void	setYZTranslationCovariance(TReal value);

		   /*! 
				\brief Sets the XZ covariance after calculation 	
				\param[in] value Value to be set.
			*/
			void	setXZTranslationCovariance(TReal value);


			/*! 
				\brief Sets the XY covariance after calculation 	
				\param[in] value Value to be set in radians [RAD].
			*/
			void	setXYRotationCovariance(TReal value);

			/*! 
				\brief Sets the YZ covariance after calculation 	
				\param[in] value Value to be set in radians [RAD].
			*/
			void	setYZRotationCovariance(TReal value);

		   /*! 
				\brief Sets the XZ covariance after calculation.	
				\param[in] value Value to be set in radians [RAD].
			*/
			void	setXZRotationCovariance(TReal value);


			/// Sets the estimated precision after calculation 
			void setEstimatedPrecision(int idx, TReal value);

			/*! 
				\brief Re-initialise the object
				Sets the estimated values to be the provisional values and for the precisions, correections and covariances zeros. Used for SIMULATION.
			*/
			void reInitialise();
		//@}

		/// Sets the rotation parameters
		void setParam(const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz);
		/// Sets the translation parameters
		void setParam(const TReal tx, const TReal ty, const TReal tz);
		/// Sets the scale parameters
		void setParam(const TReal scl);

		/*! 
			\brief Sets the parameters of the transformation

			tx, ty,tz -- translation about the x, y and z axes respectively

			rx, ry, tz -- rotations about the x, y and z axes respectively

			scl -- scale factor
		*/
		void setParam(const TReal tx, const TReal ty, const TReal tz, const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, const TReal scl);
		
		/// Sets the translation correction
		void setTranslationCorrection (int idx, TReal value);
		/// Sets the rotation correction
		void setRotationCorrection (int idx, const LGC::TAngle& value);
		/// Sets the scale correction
		void setScaleCorrection (TReal value);
		
		/// Returns Estimated parameters of the Helmert transformation
		const TransformParameters& getEstParam() const {return fEstParameter;}
		/// Returns Provisional parameters of the Helmert transformation
		const TransformParameters& getProvParam() const {return fProvParameter;}

		/// Returns the called estimated scale
		inline TReal getEstScale () const {return fEstParameter.scale;}
		/// Returns the called provisional scale
		inline TReal getProvScale () const {return fProvParameter.scale;}
		/// Returns the called estimated translation i (X [0], Y [1], Z [3])
		TReal getEstTranslation(int axis) const;
		/// Returns the called provisional translation i (X [0], Y [1], Z [3])
		TReal getProvTranslation(int axis) const;
		/// Returns the called estimated rotation i (X [0], Y [1], Z [3])
		const LGC::TAngle& getEstRotation(int axis) const;
		/// Returns the called provisional rotation i (X [0], Y [1], Z [3])
		const LGC::TAngle& getProvRotation(int axis) const;
		
	private:

		//Number of the line in the input file where the tranformation was introduced
		int line;

		//Name of the transformation
		std::string name;

		TransformParameters fProvParameter; //Provisional paremeters of this adjustable transformation
		TransformParameters fEstParameter;  //Estimated paremeters of this adjustable transformation

		std::bitset<3> fixedTranslations; // Translation fixed state
		std::bitset<3> fixedRotations; // Rotation fixed state
		std::bitset<1> fixedScale; // Scale fixed state

		TLength		    fTransStandDev[3]; //Standard deviations of translations
		LGC::TAngle		fRotStandDev[3];  //Standard deviations of rotations 
		TScalar			fScaleStandDev; //Standard deviations of a scale

		int uidx_rot[3];  // Unknown indices of rotation
		int uidx_trans[3]; // Unknown indices of translation
		int uidx_scale; // Unknown indices of scale

		void setDefaults();
		void setDefaultsParams();

		//Estimated precisons of transformation parametres
		LGC::TAngle	 fEstPrecisionRotation[3];  /*!<Estimated precision of the rotation. */
		TLength		 fEstPrecisionTranslation[3];  /*!<Estimated precision of the translation. */
		TScalar	     fEstPrecisionScale;  /*!<Estimated precision of the scale factor. */

		LGC::TAngle	 fCovarianceRotation[3];  /*!<Covariance of the rotation  XY, YZ, XZ */
     	TLength		 fCovarianceTranslation[3];  /*!<Covariance of the translation XY, YZ, XZ  */
};

#endif //TADJUSTABLE_HELMERT_TRANSFORMATION