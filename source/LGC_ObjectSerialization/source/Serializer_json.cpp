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
	_pimpl->doc.SetObject();
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
	_pimpl->stack.push_back(JSonObject(name, rapidjson::kObjectType));
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
	auto &allocator = _pimpl->doc.GetAllocator();

	if (_pimpl->stack.size() == 1)
	{
		_pimpl->doc.AddMember(rapidjson::Value(lastElement.name, allocator), lastElement.value, allocator);
	}
	else
	{
		JSonObject &parentElement = *(_pimpl->stack.end() - 2);

		if (parentElement.value.IsArray() && lastElement.name.empty())
			parentElement.value.PushBack(lastElement.value, allocator);
		else if (parentElement.value.IsArray() && !lastElement.name.empty())
		{
			rapidjson::Value obj(rapidjson::kObjectType);
			obj.AddMember(rapidjson::Value(lastElement.name, allocator), lastElement.value, allocator);
			parentElement.value.PushBack(obj, allocator);
		}
		else
			parentElement.value.AddMember(rapidjson::Value(lastElement.name, allocator), lastElement.value, allocator);
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
