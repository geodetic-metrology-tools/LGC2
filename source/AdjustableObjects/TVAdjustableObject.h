#ifndef TVADJUSTABLE_OBJECT
#define TVADJUSTABLE_OBJECT

#include <string>
#include "Defaults.h"

#ifndef isnotanumber
#define isnotanumber(x) ((x)!=(x))
#endif

/*! 
	\ingroup AdjustableObjects
	\brief A pure virtual class which is an interface for all the adjustable objects.
*/
class TVAdjustableObject {
	public:

		/// This virtual base class destructor does nothing since this is an interface.
		~TVAdjustableObject() {}

		/// Tells if this object is totally fixed, i.e. introduces no unknowns.
		virtual bool isFixed() const = 0;	

		/// Some adjustables can be forward declared, they are uninitialized in this case until they are overwritten by the definition.
		virtual bool isInitialized() const = 0;

		/*!
			\brief Returns The number of unknowns of this object.

			Calculates the number of unknowns that are added to the adjustment by this object.
			This number varies from zero to N unknowns, e.g. three for points.
			
		*/
		virtual int   getNumUnkn() const = 0;		

		/*! 
			This is part 1/2 to assign unknown indices for the matrix entries to  this object.
			A starting index is given and incremented for each variable component.

			Example: If point a point p0 is variable in Y and Z and this function is called with the integer '5',
			the Y component is adjusted in the 5th and the Z component in the 6th matrix row.

			\see getLastUidx is part 2/2.

			\throws Throws a logic_error if no component of the object is variable, i.e. it is fixed.
		*/
		virtual void  setFirstUidx(int idx) = 0;

		/*! 
			This is part 2/2 to assign unknown indices for the matrix entries to unknowns.
			It \returns the last unknown index that is assigned to this object. According to the example
			in \ref setFirstUidx this method \returns '6'.

			An application of the method is to iterate over a vector of adjustable objects and assign matrix indices using
			calls like currentPoint->setFirstUidx(previousPoint->getLastUidx()+1).

			\note This function only \returns useful results after a preceding call to \ref setFirstUidx .

			\see setFirstUidx is part 1/2.

			\throws Throws a logic_error if no component of the object is variable, i.e. it is fixed point.
		*/
		virtual int   getLastUidx() const = 0;
		
		/*!
			Provides read acces to the value that was set in \ref setFirstUidx.
		*/
		virtual int   getFirstUidx() const = 0;

		/// Sets/updates the value (oldValue+correction), a parameter of this object that is accesed by the unknown index specified by \ref setFirstUidx.
		virtual void setCorrection(int idx, TReal correction) = 0;


		/// Returns the name (which serves as an ID in most cases) of the adjustable object. This can be a name of: point, line, plane, scalar, angle or HelmertTransformation.
		virtual const std::string& getName() const = 0;

};
#endif //TVADJUSTABLE_OBJECT
