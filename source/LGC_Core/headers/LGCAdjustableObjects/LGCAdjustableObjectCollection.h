/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef LGCADJUSTABLE_OBJECT_COLLECTION
#define LGCADJUSTABLE_OBJECT_COLLECTION

// LGC
#include <LGCAdjustableLine.h>
#include <LGCAdjustablePlane.h>
#include <LGCAdjustablePoint.h>
#include <LGCAdjustableSag.h>

class TAdjustableScalar;
class TAdjustableHelmertTransformation;
class TAdjustableAngle;
class TAdjustableLength;

/*!
	\ingroup AdjustableObjects
	\brief A collection of adjustable objects (T) that implement the following functions:
	- createUninitialized(const std::string &) : static T
	- getName() : string
	- getNumUnkn() : int

	This class is basically a composite of an stl list with more comfortable retrieving functions and counting of the number of unknowns.
*/
template<typename T>
class LGCAdjustableObjectCollection
{
public:
	typedef T value_type;

	/// Default constructor creating an empty collection
	LGCAdjustableObjectCollection() {}

	/*!
		\brief Returns a reference to the adjustable object.

		Get an adjustable object by its name. Creates a new uninitialised object
		and returns a reference to it if the specified object did not exist in
		the collection before.

		\param name Adjustable object is identified by its name.

	*/
	T &getObject(const std::string &name)
	{
		auto obj(findObject(name));

		if (obj == objects.cend())
		{
			// Forward-declare the object by name
			addObject(T::createUninitialized(name));
			return objects.back();
		}
		else
		{
			return *obj;
		}
	}

	/// Returns the reference to a constant adjustable object by its name
	const T &getObject(const std::string &name) const { return const_cast<LGCAdjustableObjectCollection<T> &>(*this).getObject(name); }

	/// Returns the total number of unknowns for all objects in the collection
	inline size_t numUnknowns() const
	{
		size_t n = 0;
		for (const auto &obj : objects)
			n += obj.getNumUnkn();
		return n;
	}

	/// Checks if an adjustable object with the given name is in the collection and is initialised
	bool doesObjectExist(const std::string &objectName) const
	{
		auto p(const_cast<LGCAdjustableObjectCollection<T> &>(*this).findObject(objectName));
		if (p != objects.cend())
		{
			// an object with that name is already there, check if it was just forward-declared
			return p->isInitialized();
		}
		else
		{
			// Object with that name not in collection
			return false;
		}
	}

	/*!
		\brief Adds an adjustable to the collection or replaces the old object with the same name

		\note If an object with the given name already exists in the container when calling
		this function, the old object will be replaced with the given object (content set
		with assignment operator).

		\note The container takes a copy of the reference for the object that is passed here.
	*/
	T &addObject(const T &obj)
	{
		auto it = objectMap.find(obj.getName());
		if (it != objectMap.end())
		{
			// Object was forward-declared, initialize it. The name->location in list map does not have to be updated
			*(it->second) = obj;
			return *(it->second);
		}
		else
		{
			objects.emplace_back(obj);
			auto listIt = --objects.end();
			// update the name->location in list map
			objectMap[obj.getName()] = listIt;
			return *listIt;
		}
	}

	/// Remove the given object
	void removeObject(const T &obj)
	{
		auto it = objectMap.find(obj.getName());
		if (it != objectMap.end())
		{
			objects.erase(it->second);
			objectMap.erase(it);
		}
	}

	template<class UnaryPredicate>
	void removeObjectIf(UnaryPredicate p)
	{
		for (auto it = objects.begin(); it != objects.end(); /* no increment here to keep iterator valid in case object has been removed */)
		{
			if (p(*it))
			{
				objectMap.erase(it->getName());
				it = objects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	/// Returns The reference to the last adjustable object
	T &back() { return objects.back(); }

	/// Returns The total number of objects
	size_t numObjects() const { return objects.size(); }

	/// Returns the begin iterator
	typename std::list<T>::iterator begin() { return objects.begin(); }

	/// Returns the begin iterator
	typename std::list<T>::const_iterator begin() const { return objects.cbegin(); }

	/// Returns the begin const iterator
	typename std::list<T>::const_iterator cbegin() const { return objects.cbegin(); }

	/// Returns the end iterator
	typename std::list<T>::iterator end() { return objects.end(); }

	/// Returns the end iterator
	typename std::list<T>::const_iterator end() const { return objects.end(); }

	/// Returns the end const iterator
	typename std::list<T>::const_iterator cend() const { return objects.cend(); }

	// check consistency of map and list
	bool checkMapConsistency() const
	{
		if (objects.size() != objectMap.size())
		{
			return false;
		}
		// the loop will pass if all objects in the list can be found via the map
		for (auto object : objects)
		{
			std::string objectName = object.getName();
			auto it = objectMap.find(objectName);
			if (it == objectMap.end())
			{
				return false;
			}
		}
		return true;
	}

protected:
	/// Returns The  object position in the collection by its name
	typename std::list<T>::iterator findObject(const std::string &objName)
	{
		auto it = objectMap.find(objName);
		if (it != objectMap.end())
		{
			return it->second;
		}
		return objects.end();
	}

private:
	/// The container of objects
	typename std::list<T> objects;
	// map names -> object locations, used to efficiently searching for objects
	std::unordered_map<std::string, typename std::list<T>::iterator> objectMap;
};

typedef LGCAdjustableObjectCollection<TAdjustableHelmertTransformation> TAdjustableTransformationCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustablePoint> LGCAdjustablePointCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustableLine> LGCAdjustableLineCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustablePlane> LGCAdjustablePlaneCollection;
typedef LGCAdjustableObjectCollection<LGCAdjustableSag> LGCAdjustableSagCollection;
typedef LGCAdjustableObjectCollection<TAdjustableAngle> TAdjustableAngleCollection;
typedef LGCAdjustableObjectCollection<TAdjustableLength> TAdjustableLengthCollection;
typedef LGCAdjustableObjectCollection<TAdjustableScalar> TAdjustableScalarCollection;

typedef std::list<LGCAdjustablePoint>::const_iterator AdjPointIter;

#endif
