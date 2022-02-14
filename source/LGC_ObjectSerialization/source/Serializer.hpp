/*
© Copyright CERN 2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _SERIALIZER
#define _SERIALIZER

#include "CustomTypeTraits.hpp"

class Serializable;

class SerializerObject
{
public:
	class SerializationHelper
	{
	public:
		SerializationHelper(SerializerObject& ser, const std::string& name) : ser(ser), isSubObject(true) { ser.startObject(name); }
		SerializationHelper(SerializerObject& ser) : ser(ser) { ser.startObject(); }
		~SerializationHelper() { ser.endObject(); }

		template<typename T>
		void addProperty(const std::string& name, const T& value)
		{
			ser.addProperty(name, value);
		}

	protected:
		SerializerObject& ser;
		bool isSubObject = false;
	};

public:
	SerializationHelper getSerializationHelper(const std::string& objectName) { return SerializationHelper(*this, objectName); }
	SerializationHelper getSerializationHelper() { return SerializationHelper(*this); }
	virtual std::string getStringRepresentation() = 0;

protected:
	// Virtual functions
	// structure related
	virtual void startObject(const std::string& name) = 0;
	virtual void startObject() = 0;
	virtual void endObject() = 0;
	virtual void startArray(const std::string& name) = 0;
	virtual void startArray() = 0;
	virtual void endArray() = 0;
	virtual void startPrimitive(const std::string& name) = 0;
	virtual void endPrimitive() = 0;

	// value related
	virtual void addValue(int value) = 0;
	virtual void addValue(double value) = 0;
	virtual void addValue(float value) = 0;
	virtual void addValue(char value) = 0;
	virtual void addValue(wchar_t value) = 0;
	virtual void addValue(bool value) = 0;
	virtual void addValue(const std::string& value) = 0;
	virtual void addValue(const char* value) = 0;


	// ADD PROPERTY
	// Primitive
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value
		&& !is_pair_t<T>::value
		&& ((is_iterable_container<T>::value&& is_string<T>::value)
			|| !is_iterable_container<T>::value)>::type
		addProperty(const std::string& name, const T& value)
	{
		startPrimitive(name);
		addValue(value);
		endPrimitive();
	}
	// Serializable class
	template<typename T>
	typename std::enable_if<is_Serializable<T>::value>::type addProperty(const std::string& name, const T& o)
	{
		SerializerObject::SerializationHelper serHelper = getSerializationHelper(name); // implicit startObject
		to_ptr(o)->serialize(serHelper); // implicit endObject in destructor
	}
	// Serializable class
	template<typename T>
	typename std::enable_if<is_Serializable<T>::value>::type addProperty(const T& o)
	{
		SerializerObject::SerializationHelper serHelper = getSerializationHelper(); // implicit startObject
		to_ptr(o)->serialize(serHelper); // implicit endObject in destructor
	}
	// Map, and container of maps/pairs
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value
		&& !is_string<T>::value
		&& is_iterable_container<T>::value
		&& (has_mapped_type<T>::value
			|| is_pair_t<typename T::value_type>::value)>::type
		addProperty(const std::string& name, const T& value)
	{
		startObject(name);
		addValue(value);
		endObject();
	}
	// Map, and container of maps/pairs (overload to dynamically adjust the type for nested structures)
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value
		&& !is_string<T>::value
		&& is_iterable_container<T>::value
		&& (has_mapped_type<T>::value
			|| is_pair_t<typename T::value_type>::value)>::type
		addProperty(const T& value)
	{
		startObject();
		addValue(value);
		endObject();
	}
	// Pair
	template<typename T>
	typename std::enable_if<is_pair_t<T>::value>::type
		addProperty(const std::string& name, const T& value)
	{
		startObject(name);
		addValue(value);
		endObject();
	}
	// Container (no maps, no pairs in subiterable)
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value
		&& !is_string<T>::value&& is_iterable_container<T>::value
		&& (!has_mapped_type<T>::value
			&& !is_pair_t<T>::value
			&& !is_pair_t<typename T::value_type>::value)>::type
		addProperty(const std::string& name, const T& value)
	{
		startArray(name);
		addValue(value);
		endArray();
	}


	// ADD VALUE
	// 	   Helpers to dispatch the types based on the sub-element (pair or container)
	// Pair - make an object of it
	template<typename K, typename V>
	void addValue(const std::pair<K, V>& p)
	{
		addProperty(p.first, p.second);
	}
	// If Serializable elements or elements are also containers (adapt the type  (i.e. Object/Array) depending on the container type)
	template<typename T>
	typename std::enable_if<is_iterable_container<T>::value
		&& (is_iterable_container<typename T::value_type>::value
			|| is_Serializable<typename T::value_type>::value)>::type addValue(const T& container)
	{
		for (const auto& t : container)
			addProperty(t);
	}
	// If basic element - primitive or pair - just addValue
	template<typename T>
	typename std::enable_if<is_iterable_container<T>::value
		&& !is_iterable_container<typename T::value_type>::value
		&& !is_Serializable<typename T::value_type>::value>::type addValue(const T& container)
	{
		for (const auto& t : container)
			addValue(t);
	}
};

class Serializable
{
public:
	virtual void serialize(SerializerObject::SerializationHelper& obj) const = 0;
};

#endif
