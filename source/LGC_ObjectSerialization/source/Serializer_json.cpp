#include "Serializer_json.hpp"

#include <deque>
#include <iostream>

#include <math.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

struct JSonObject
{
	JSonObject(const std::string name, rapidjson::Type type) : name(name), value(rapidjson::Value(type)) {}

	const std::string name;
	rapidjson::Value value;
};

class jsonSerializerObject::_jsonSerializerObject_pimpl
{
public:
	rapidjson::Value &getLastValue() { return stack.back().value; }
	JSonObject &getLastObject() { return stack.back(); }

public:
	std::deque<JSonObject> stack;
	rapidjson::Document doc;
	/**
	 * If defined it will be the first node containing all other members. If not then all the members will be directly
	 * defined without an outer object.
	 */
	std::string rootName;
};

template<typename T>
typename std::enable_if<!std::is_floating_point<T>::value>::type addToValue(rapidjson::Value &value, T newValue, rapidjson::Document::AllocatorType &allocator)
{
	if (value.IsArray())
		value.PushBack(newValue, allocator);
	else
		value = newValue;
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type addToValue(rapidjson::Value &value, T newValue, rapidjson::Document::AllocatorType &allocator)
{
	if (std::isnan(newValue))
	{
		if (value.IsArray())
			value.PushBack(rapidjson::Value(), allocator);
		else
			value = rapidjson::Value();
	}
	else
	{
		if (value.IsArray())
			value.PushBack(newValue, allocator);
		else
			value = newValue;
	}
}

jsonSerializerObject::jsonSerializerObject() : _pimpl(std::make_unique<_jsonSerializerObject_pimpl>())
{
}

jsonSerializerObject::~jsonSerializerObject()
{
}

std::string jsonSerializerObject::getStringRepresentation()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer writer(buffer);
	_pimpl->doc.Accept(writer);
	return buffer.GetString();
}

void jsonSerializerObject::startObject(const std::string &name)
{
	if (_pimpl->doc.IsObject())
	{
		_pimpl->stack.push_back(JSonObject(name, rapidjson::kObjectType));
		return;
	}

	// Initialization of the root node and tree
	_pimpl->rootName = name;
	_pimpl->doc.SetObject();
	if (!_pimpl->rootName.empty())
		_pimpl->doc.AddMember(rapidjson::Value(name, _pimpl->doc.GetAllocator()), rapidjson::Value(rapidjson::kObjectType), _pimpl->doc.GetAllocator());
	_pimpl->doc.GetObject();
}

void jsonSerializerObject::startObject()
{
	const std::string noname = "";
	startObject(noname);
}

void jsonSerializerObject::endObject()
{
	if (_pimpl->stack.empty())
		return;

	JSonObject &lastElement = _pimpl->getLastObject();

	if (_pimpl->stack.size() == 1)
	{
		rapidjson::Value &root = _pimpl->rootName.empty() ? _pimpl->doc : _pimpl->doc[_pimpl->rootName];
		root.AddMember(rapidjson::Value(lastElement.name, _pimpl->doc.GetAllocator()), lastElement.value, _pimpl->doc.GetAllocator());
	}
	else
	{
		JSonObject &parentElement = *(_pimpl->stack.end() - 2);

		if (parentElement.value.IsArray())
			parentElement.value.PushBack(lastElement.value, _pimpl->doc.GetAllocator());
		else
			parentElement.value.AddMember(rapidjson::Value(lastElement.name, _pimpl->doc.GetAllocator()), lastElement.value, _pimpl->doc.GetAllocator());
	}

	_pimpl->stack.pop_back();
}

void jsonSerializerObject::startArray(const std::string &name)
{
	_pimpl->stack.push_back(JSonObject(name, rapidjson::kArrayType));
}

void jsonSerializerObject::startArray()
{
	_pimpl->stack.push_back(JSonObject("", rapidjson::kArrayType));
}

void jsonSerializerObject::endArray()
{
	endObject();
}

void jsonSerializerObject::startPrimitive(const std::string &name)
{
	startObject(name);
}

void jsonSerializerObject::endPrimitive()
{
	endObject();
}

void jsonSerializerObject::addValue(int value)
{
	addToValue(_pimpl->getLastValue(), value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(double value)
{
	addToValue(_pimpl->getLastValue(), value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(float value)
{
	addToValue(_pimpl->getLastValue(), value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(char value)
{
	addToValue(_pimpl->getLastValue(), value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(wchar_t value)
{
	addToValue(_pimpl->getLastValue(), value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(bool value)
{
	addToValue(_pimpl->getLastValue(), value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(const std::string &value)
{
	_pimpl->getLastValue().SetString(value, _pimpl->doc.GetAllocator());
}

void jsonSerializerObject::addValue(char const *value)
{
	_pimpl->getLastValue().SetString(value, _pimpl->doc.GetAllocator());
}
