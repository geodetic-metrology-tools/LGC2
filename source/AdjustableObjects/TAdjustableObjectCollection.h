#ifndef TADJUSTABLE_OBJECT_COLLECTION
#define TADJUSTABLE_OBJECT_COLLECTION


#include "TAdjustableLine.h"
#include "TAdjustableHelmertTransformation.h"
#include "TAdjustableAngle.h"
#include "TAdjustablePlane.h"
#include "TAdjustablePoint.h"
#include "TAdjustableScalar.h"

/*!
	\brief A collection of adjustable objects, the only requirement is that the objects implement 
	getName():string and getNumUkn():int.

	This class is basically a composite of an stl list with more comfortable retrieving functions
	and counting of the number of unknowns.
*/
template<typename T>
class TAdjustableObjectCollection {
	public:

		/// Default constructor creating an empty collection
		TAdjustableObjectCollection() :
			fUnknowns(0) {}

		/*!
			Get an adjustable object by its name.

			\param name Adjustable object is identified by its name

			\returns A reference to the adjustable object.

			\throw Throws a runtime_error if the desired object is not found. 
		*/
		T& getObject(const std::string& name) {
			auto obj(findObject(name));

			if (obj == objects.cend()) {
				// Forward-declare the object by name
				addObject(T::createUninitialized(name));
				return objects.back();
			} 
			else {
				return *obj;
			}
		}


		/// \returns The reference to a constant adjustable object by its name
		const T& getObject(const std::string& name) const {
			return const_cast<TAdjustableObjectCollection<T>& >( *this ).getObject(name);  
		}

		/// \returns The total number of unknowns for all objects in the collection
		inline size_t numUnknowns() const {
			return fUnknowns;
		}

		/// Checks if an adjustable object with a given name is in the collection
		bool doesObjectExist(const std::string& objectName) const {
			auto p(const_cast<TAdjustableObjectCollection<T>& >( *this ).findObject(objectName));
			if (p != objects.cend()) {
				// an object with that name is already there, check if it was just forward-declared
				return p->isInitialized();
			} 
			else {
				// Object with that name not in collection
				return false;
			}
		}

		/*!
			Adds an adjustable to the collection. This also updates the counter for the 
			total number of unknowns.

			\note Uninitialized objects may be in the collection by forward declaration. They will 
			be initialized (content set by the assignemnt operator) when this method is called with a valid object.

			\note The container takes a copy of the reference for the object that is passed here.
		*/
		T& addObject(const T& obj) {
			auto o = findObject(obj.getName());
			if (o != objects.cend()) {
				// Object was forward-declared, initialize it
				*o = obj;
				// add number of unknowns introduced, uninitialized object did not introduced any
				fUnknowns += obj.getNumUkn();
				return *o;
			}
			else {
				fUnknowns += obj.getNumUkn();
				objects.emplace_back(obj);
				return objects.back();
			}
		}

		/// \returns The reference to the last adjustable object
		T& back() {
			return objects.back();
		}

		/// \returns The total number of objects
		size_t numObjects() const {
			return objects.size();
		}

		/// The container of objects
		typename std::list<T> objects;

	protected:

		size_t fUnknowns;

		/// \returns The  object position in the collection by its name
		typename std::list<T>::iterator findObject(const std::string& objName) {
			return std::find_if(objects.begin(), objects.end(), [&](T& obj) {
				return obj.getName() == objName;
			});
		}
};

typedef TAdjustableObjectCollection<TAdjustableHelmertTransformation> TAdjustableTransformationCollection;
typedef TAdjustableObjectCollection<TAdjustablePoint> TAdjustablePointCollection;
typedef TAdjustableObjectCollection<TAdjustableLine> TAdjustableLineCollection;
typedef TAdjustableObjectCollection<TAdjustablePlane> TAdjustablePlaneCollection;
typedef TAdjustableObjectCollection<TAdjustableAngle> TAdjustableAngleCollection;
typedef TAdjustableObjectCollection<TAdjustableScalar> TAdjustableScalarCollection;


#endif