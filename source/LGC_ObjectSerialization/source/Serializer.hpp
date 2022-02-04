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
		SerializationHelper(SerializerObject& ser, const std::string& name) : ser(ser) { ser.startObject(name); }
		~SerializationHelper() { ser.endObject(); }

		template<typename T>
		void addProperty(const std::string& name, const T& value)
		{
			ser.addProperty(name, value);
		}

	protected:
		SerializerObject& ser;
	};

public:
	SerializationHelper getSerializationHelper(const std::string& objectName) { return SerializationHelper(*this, objectName); }
	virtual std::string getStringRepresentation() = 0;

protected:
	virtual void startObject(const std::string& name) = 0;
	virtual void endObject() = 0;
	virtual void startArray(const std::string& name) = 0;
	virtual void endArray() = 0;
	virtual void startPrimitive(const std::string& name) = 0;
	virtual void endPrimitive() = 0;

	// Primitive
	template<typename T>
	typename std::enable_if<!std::is_base_of<Serializable, T>::value && !is_pair_d<T>::value && ((is_iterable_container<T>::value&& is_string<T>::value) || !is_iterable_container<T>::value)>::type
		addProperty(const std::string& name, const T& value)
	{
		startPrimitive(name);
		addValue(value);
		endPrimitive();
	}

	// Map
	template<typename T>
	typename std::enable_if<!std::is_base_of<Serializable, T>::value && !is_string<T>::value&& is_iterable_container<T>::value&& has_mapped_type<T>::value>::type
		addProperty(const std::string& name, const T& value)
	{
		startObject(name);
		addValue(value);
		endObject();
	}

	// Pair
	template<typename T>
	typename std::enable_if<!std::is_base_of<Serializable, T>::value && !has_mapped_type<T>::value && !is_string<T>::value && !is_iterable_container<T>::value&& is_pair_d<T>::value>::type
		addProperty(const std::string& name, const T& value)
	{
		startObject(name);
		addValue(value);
		endObject();
	}

	// Container of Pairs
	template<typename T>
	typename std::enable_if<!std::is_base_of<Serializable, T>::value && !has_mapped_type<T>::value && !is_string<T>::value&& is_iterable_container<T>::value&& is_pair<typename T::value_type>::value>::type
		addProperty(const std::string& name, const T& value)
	{
		startObject(name);
		addValue(value);
		endObject();
	}

	// Container
	template<typename T>
	typename std::enable_if<!std::is_base_of<Serializable, T>::value && !is_string<T>::value&& is_iterable_container<T>::value && !has_mapped_type<T>::value && !is_pair<typename T::value_type>::value>::type
		addProperty(const std::string& name, const T& value)
	{
		startArray(name);
		addValue(value);
		endArray();
	}

	// Serializable class
	template<typename T>
	typename std::enable_if<std::is_base_of<Serializable, T>::value>::type addProperty(const std::string& name, const T& o)
	{
		SerializerObject::SerializationHelper serHelper = getSerializationHelper(name); // implicit startObject
		o.serialize(serHelper); // implicit endObject in destructor
	}

	virtual void addValue(int value) = 0;
	virtual void addValue(double value) = 0;
	virtual void addValue(float value) = 0;
	virtual void addValue(char value) = 0;
	virtual void addValue(wchar_t value) = 0;
	virtual void addValue(bool value) = 0;
	virtual void addValue(const std::string& value) = 0;
	virtual void addValue(const char* value) = 0;

	// pairs (map entries)
	template<typename K, typename V>
	void addValue(const std::pair<K, V>& p)
	{
		addProperty(p.first, p.second);
	}

	// iterables
	template<typename T>
	typename std::enable_if<is_iterable_container<T>::value>::type addValue(const T& container)
	{
		for (const auto& t : container)
		{
			addValue(t);
		}
	}
};

class Serializable
{
public:
	virtual void serialize(SerializerObject::SerializationHelper& obj) const = 0;
};

#endif
