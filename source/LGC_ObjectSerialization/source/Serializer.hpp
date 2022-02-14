/*
© Copyright CERN 2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _SERIALIZER
#define _SERIALIZER

#include "CustomTypeTraits.hpp"

class Serializable;

/**
 * Abstract class featuring Serialization of data structures.
 *
 * To use this class, you should override all the virtual methods.
 *
 * The  principle of operation of this class can be explained with two concepts:
 * - using std::enable_if and (custom) type traits to allow proper object type resolution during the compile time
 * - using double dispatch thanks to inner @SerializerObject::SerializationHelper class that dispatches the calls to the
 *		relevant @SerializerObject::addProperty methods, some of which are virtual. This enables to use dynamic (inheritance)
 *		and static (templates) polymorphism at the same time. Then @addProperty methods may dispatch it back to @addProperty
 *		methods or to @addValue methods
 *
 * @addProperty methods are used to resolve the types properly and allow to create nested objects. These and @addValue methods
 *		are heavily templated in order to resolve all possible types correctly. This makes the API extremly easy to use by only calling
 *		@SerializerObject::SerializationHelper::addProperty method and entire complicated logic is hidden inside of @SerializerObject.
 * @addValue methods are used to pass the value to be serialized or to dispatch it back to @addProperty if it cannot be resolved yet
 * @startObject @endObject @startArray @endArray @startPrimitive @endPrimitive are used to create proper serialized structure
 *		that should be adapted to different serialization standards
 *
 * For type traits and helpers please refer to `CustomTypeTraits.hpp`
 */
class SerializerObject
{
public:
	/**
	 * Helper class that is used to benefit from both dynamic and static polymorphism, it allows to call virtual @addProperty methods
	 * of @SerializerObject with a templated argument using @SerializerObject::SerializationHelper::addProperty.
	 */
	class SerializationHelper
	{
	public:
		/**
		 * Constructor, calls @SerializerObject::startObject
		 *
		 * @param name of the object, if empty - a new object without name is assumed that is most likely an element of some container
		 * @param ser SerializerObject used for serialization
		 */
		SerializationHelper(SerializerObject &ser, const std::string &name = std::string()) : ser(ser) { name.empty() ? ser.startObject() : ser.startObject(name); }
		/**
		 * Calls @SerializerObject::endObject
		 */
		~SerializationHelper() { ser.endObject(); }

		/**
		 * Calls @addProperty methods of @SerializerObject with a templated argument
		 *
		 * @param name of the object
		 * @param ser SerializerObject used for serialization
		 */
		template<typename T>
		void addProperty(const std::string &name, const T &value)
		{
			ser.addProperty(value, name);
		}

	protected:
		SerializerObject &ser;
	};

public:
	/**
	 * Creates a new @SerializerObject::SerializationHelper object
	 *
	 * @param objectName of the new object/structure, if empty - a new object without name is assumed that is most likely an element of some container
	 */
	SerializationHelper getSerializationHelper(const std::string &objectName = std::string()) { return SerializationHelper(*this, objectName); }
	virtual std::string getStringRepresentation() = 0;

protected:
	// Virtual functions
	// structure related
	virtual void startObject(const std::string &name) = 0;
	virtual void startObject() = 0;
	virtual void endObject() = 0;
	virtual void startArray(const std::string &name) = 0;
	virtual void startArray() = 0;
	virtual void endArray() = 0;
	virtual void startPrimitive(const std::string &name) = 0;
	virtual void endPrimitive() = 0;

	// value related
	virtual void addValue(int value) = 0;
	virtual void addValue(double value) = 0;
	virtual void addValue(float value) = 0;
	virtual void addValue(char value) = 0;
	virtual void addValue(wchar_t value) = 0;
	virtual void addValue(bool value) = 0;
	virtual void addValue(const std::string &value) = 0;
	virtual void addValue(const char *value) = 0;

	/* *************** */
	/*   ADD_PROPERTY  */
	/* *************** */
	//		Add some new property, can be primitive, Serializable, map/container of maps, pair, container

	// clang-format off
	// Primitive - !Serializable && !pair && !pointer && ((container && is_string) || !container))
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value 
		&& !is_pair_t<T>::value 
		&& !std::is_pointer<T>::value
		&& ((is_iterable_container<T>::value 
			&& is_string<T>::value) 
			|| !is_iterable_container<T>::value)>::type
		// clang-format on
		addProperty(const T &value, const std::string &name = std::string())
	{
		startPrimitive(name);
		addValue(value);
		endPrimitive();
	}
	// Primitive pointer - !Serializable && is_pointer
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value && std::is_pointer<T>::value>::type
		// clang-format on
		addProperty(const T &value, const std::string &name = std::string())
	{
		startPrimitive(name);
		addValue(*value);
		endPrimitive();
	}
	// Serializable class
	template<typename T>
	typename std::enable_if<is_Serializable<T>::value>::type addProperty(const T &o, const std::string &name = std::string())
	{
		SerializerObject::SerializationHelper serHelper = getSerializationHelper(name); // implicit startObject
		to_ptr(o)->serialize(serHelper); // implicit endObject in destructor
	}
	// Map, and container of maps/pairs - !Serializable && !is_string && container && (is_map || element_is_pair)
	// clang-format off
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value 
		&& !is_string<T>::value 
		&& is_iterable_container<T>::value 
		&& (has_mapped_type<T>::value 
			|| is_pair_t<typename T::value_type>::value)>::type
		// clang-format on
		addProperty(const T &value, const std::string &name = std::string())
	{
		name.empty() ? startObject() : startObject(name);
		addValue(value);
		endObject();
	}
	// Pair
	template<typename T>
	typename std::enable_if<is_pair_t<T>::value>::type addProperty(const T &value, const std::string &name = std::string())
	{
		name.empty() ? startObject() : startObject(name);
		addValue(value);
		endObject();
	}
	// Container (no maps, no pairs in subiterable) - !Serializable && !is_string && container && !is_map && !is_pair && !element_is_pair
	// clang-format off
	template<typename T>
	typename std::enable_if<!is_Serializable<T>::value 
		&& !is_string<T>::value 
		&& is_iterable_container<T>::value
		&& !has_mapped_type<T>::value 
		&& !is_pair_t<T>::value 
		&& !is_pair_t<typename T::value_type>::value>::type
		// clang-format on
		addProperty(const T &value, const std::string &name = std::string())
	{
		name.empty() ? startObject() : startArray(name);
		addValue(value);
		endArray();
	}

	/* ************** */
	/*    ADD_VALUE   */
	/* ************** */
	// 	   Helpers to dispatch the types based on the sub-element (pair or container)

	// Pair - make an object of it
	template<typename K, typename V>
	void addValue(const std::pair<K, V> &p)
	{
		addProperty(p.second, p.first);
	}
	// If Serializable elements or elements are also containers (adapt the type  (i.e. Object/Array) depending on the container type)
	// clang-format off
	template<typename T>
	typename std::enable_if<is_iterable_container<T>::value 
		&& (is_iterable_container<typename T::value_type>::value 
			|| is_Serializable<typename T::value_type>::value)>::type
		// clang-format on
		addValue(const T &container)
	{
		for (const auto &t : container)
			addProperty(t);
	}
	// If basic element - primitive or pair - just addValue
	// clang-format off
	template<typename T>
	typename std::enable_if<is_iterable_container<T>::value 
		&& !is_iterable_container<typename T::value_type>::value 
		&& !is_Serializable<typename T::value_type>::value>::type
		// clang-format on
		addValue(const T &container)
	{
		for (const auto &t : container)
			addValue(t);
	}
};

class Serializable
{
public:
	virtual void serialize(SerializerObject::SerializationHelper &obj) const = 0;
};

#endif
