#include "THelmertTransformation.h"
#include "TReflection.h"



class TAffineTransformFactory
 {
 public:
	
		static TReflection
		createReflection(const std::string& name, const TReflection::EType& type);
	 	 /*!
			Creates a pure scaling transformation. 

			\param name transformation name
			\param scale Scale factor
		*/
		static THelmertTransformation
		createScaling(const std::string& name, const TReal& scale);

	 	/*!
			Creates a pure rotation transformation. 

			\param name transformation name
			\param rx Rotation angle around the x-axis (omega)
			\param ry Rotation angle around the y-axis (phi)
			\param rz Rotation angle around the z-axis (kappa)
		*/
		static THelmertTransformation 
		createRotation(const std::string& name, const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz);
		
		/*!
			Creates a pure translation transformation.

			\param name transformation name
			\param tx movement along the x-axis
			\param ty movement along the y-axis
			\param tz movement along the z-axis
			whether the translation should be adjusted or not
		*/
		static THelmertTransformation
		createTranslation(const std::string& name, const TReal tx, const TReal ty, const TReal tz);
		
		/*!
			Creates a six parameter transformation (3 rotations and 3 translations).
			The scale factor set to 1.0.

			\param name transformation name
			\param tx movement along the x-axis
			\param ty movement along the y-axis
			\param tz movement along the z-axis
			\param rx Rotation angle around the x-axis (omega)
			\param ry Rotation angle around the y-axis (phi)
			\param rz Rotation angle around the z-axis (kappa)

		*/
		static THelmertTransformation
		create6Parameter(const std::string& name, const TReal tx, const TReal ty, const TReal tz, 
		                 const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz);

		/*!
			Creates a seven parameter transformation (3 rotations, 3 translations and 1 scale factor).

			\param name transformation name
			\param tx movement along the x-axis
			\param ty movement along the y-axis
			\param tz movement along the z-axis
			\param rx Rotation angle around the x-axis (omega)
			\param ry Rotation angle around the y-axis (phi)
			\param rz Rotation angle around the z-axis (kappa)
			\param scale scale factor
			*/
		static THelmertTransformation
		create7Parameter(const std::string& name, const TReal tx, const TReal ty, const TReal tz, 
		                 const LGC::TAngle& rx, const LGC::TAngle& ry, const LGC::TAngle& rz, TReal scale);
 };