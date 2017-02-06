#ifndef LGCADJUSTABLE_OBJECT_COLLECTION
#define LGCADJUSTABLE_OBJECT_COLLECTION

//Copyright 2017 CERN EN/ACE/SU.  All rights reserved.

//LGC
#include <LGCAdjustableLine.h>
#include <LGCAdjustablePlane.h>
#include <LGCAdjustablePoint.h>

class TAdjustableScalar;
class TAdjustableHelmertTransformation;
class TAdjustableAngle;
class TAdjustableLength;

/*! 
	\ingroup AdjustableObjects
	\brief A collection of adjustable objects, the only requirement is that the objects implement getName():string and getNumUnkn():int.

	This class is basically a composite of an stl list with more comfortable retrieving functions and counting of the number of unknowns.
*/
template<typename T>
class LGCAdjustableObjectCollection {
	public:

		/// Default constructor creating an empty collection
		LGCAdjustableObjectCollection() :
			fUnknowns(0) {}

		/*!
			\brief Returns A reference to the adjustable object.

			Get an adjustable object by its name.

			\param name Adjustable object is identified by its name.			

			\throw Throws a runtime_error if the requested object is not found. 
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


		/// Returns the reference to a constant adjustable object by its name
		const T& getObject(const std::string& name) const {
			return const_cast<LGCAdjustableObjectCollection<T>& >( *this ).getObject(name);  
		}

		/// Returns the total number of unknowns for all objects in the collection
		inline size_t numUnknowns() const {
			return fUnknowns;
		}

		/// Checks if an adjustable object with a given name is in the collection
		bool doesObjectExist(const std::string& objectName) const {
			auto p(const_cast<LGCAdjustableObjectCollection<T>& >( *this ).findObject(objectName));
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
			\brief Adds an adjustable to the collection. 

			This also updates the counter for the total number of unknowns.

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
				fUnknowns += obj.getNumUnkn();
				return *o;
			}
			else {
				fUnknowns += obj.getNumUnkn();
				objects.emplace_back(obj);
				return objects.back();
			}
		}

		/// Returns The reference to the last adjustable object
		T& back() {
			return objects.back();
		}

		/// Returns The total number of objects
		size_t numObjects() const {
			return objects.size();
		}

		/// Returns the begin iterator
		typename std::list<T>::iterator begin() {
			return objects.begin();
		}

		/// Returns the begin iterator
		typename std::list<T>::const_iterator begin() const {
			return objects.cbegin();
		}

		/// Returns the begin const iterator
		typename std::list<T>::const_iterator cbegin() const {
			return objects.cbegin();
		}

		/// Returns the end iterator
		typename std::list<T>::iterator end() {
			return objects.end();
		}
		
		/// Returns the end iterator
		typename std::list<T>::const_iterator end() const {
			return objects.end();
		}

		/// Returns the end const iterator
		typename std::list<T>::const_iterator cend() const {
			return objects.cend();
		}

	protected:

		size_t fUnknowns;

		/// Returns The  object position in the collection by its name
		typename std::list<T>::iterator findObject(const std::string& objName) {
         for(auto it = objects.begin() ; it != objects.end() ;it++)
				if( (*it).getName() == objName)
               return it;
            return objects.end();
		}

	private : 

		/// The container of objects
		typename std::list<T> objects;

};

typedef LGCAdjustableObjectCollection<TAdjustableHelmertTransformation> TAdjustableTransformationCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustablePoint> LGCAdjustablePointCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustableLine> LGCAdjustableLineCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustablePlane> LGCAdjustablePlaneCollection;
typedef LGCAdjustableObjectCollection<TAdjustableAngle> TAdjustableAngleCollection;
typedef LGCAdjustableObjectCollection<TAdjustableLength> TAdjustableLengthCollection;
typedef LGCAdjustableObjectCollection<TAdjustableScalar> TAdjustableScalarCollection;

typedef std::list<LGCAdjustablePoint>::const_iterator AdjPointIter;



#endif