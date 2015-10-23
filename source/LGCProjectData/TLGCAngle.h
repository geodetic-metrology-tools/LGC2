#ifndef _LGCG_ANGLE_H_
#define _LGCG_ANGLE_H_

#include <memory>
#include "TAngle.h"

namespace LGC {

	/*! 
		Wraps angle values to treat them as radians in [-PI,PI) internally and
		provide access in common units. The cmath functions also use this range.

		\note Should be moved to SurveyLib!
	*/
   /*
	class TAngle {
		public:
			/// Avaliable units to represent angles
			enum EUnits { 
				kRadians, ///< Radians in the range [0,2*PI) 
				kDegrees, ///< Common degree value in the range [0,360)
				kGons,    ///< Gon angle value in the range [0,400)
				kCC,      ///< Deci-Milligons, i.e. 1/1000 GON.
			}; 
			
			/// Returns a const reference to the SurveyLib angle around which it wraps
			::TAngle const& getSLAngle()const;

			TAngle(const TAngle& rhs);
			TAngle& operator=(const TAngle& rhs);

			/// Default constructor, sets the  angle to 0 Gon.
			TAngle();
			//Destructor
			~TAngle();

			/// Constructs an angle object in any  valid unit.
			TAngle(EUnits unit, TReal v);

			/// compares two angles for equivalence
			bool operator==(const TAngle& a) const;
			/// compares two angles for non-equivalence
			bool operator!=(const TAngle& a) const;
			/// lesser-or-equal operator for two agles
			bool operator<=(const TAngle& a) const;
			/// greater-or-equal operator for two agles
			bool operator>=(const TAngle& a) const;
			/// lesser comparison operator for two agles
			bool operator<(const TAngle& a)  const;
			/// greater comparison operator for two agles
			bool operator>(const TAngle& a)  const;
			
			/// Adds two angles and returns a new object containing the result
			TAngle operator+(const TAngle& a) const;
			/// Substracts two angles and returns a new object containing the result
			TAngle operator-(const TAngle& a) const;
			/// Multiplies two angles and returns a new object containing the result
			TAngle operator*(const TAngle& a) const;
			/// Multiplies a constant to this angle and returns a new object containing the result
			TAngle operator*(TReal v);
			/// Divides two angles and returns a new object containing the result
			TAngle operator/(const TAngle& a) const;
			/// Adds an angle to this angle
			TAngle& operator+=(const TAngle& a);
			/// Substracts an angle from this angle
			TAngle& operator-=(const TAngle& a);
			/// Multiplies an angle to this angle
			TAngle& operator*=(const TAngle& a);
			/// Multiplies a real constant to this angle
			TAngle& operator*=(const TReal v);
			/// Divides this angle by another angle
			TAngle& operator/=(const TAngle& a);
			/// Divides this angle by a real constant
			TAngle& operator/=(const TReal v);



			
			/// Returns the sine value of the angle 
			TReal sin() const;
			/// Returns the cosine value of the angle 
			TReal cos() const;
			/// Returns the tangent value of the angle
			TReal tan() const;
			
			/// Creates an angle object based on the arcus sinus of v where v must be in [-1,1]
			static TAngle asin(TReal v);
			/// Creates an angle object based on the arcus cosinus of v where v must be in [-1,1]
			static TAngle acos(TReal v);
			/// Creates an angle object based on the arcus tangent (a/b) of v 
			static TAngle atan(TReal v);
			/// Creates an angle object based on the arcus tangent (a/b) of v 
			static TAngle atan2(TReal a, TReal b);
			
			///   Returns the angle value in CC
			TReal  cc() const;
			///   Returns the angle value in GON
			TReal gon() const;
			///   Returns the angle value in degrees
			TReal deg() const;
			///   Returns the angle value in radians
			TReal rad() const;

			void set(EUnits unit, TReal v);

			/// only for debugging/testing prurposes
			TReal raw();

		private:
			struct D;
			std::unique_ptr<D> d;
	};
*/
}

#endif