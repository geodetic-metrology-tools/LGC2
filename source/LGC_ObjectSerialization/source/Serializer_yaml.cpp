#include "Serializer_yaml.hpp"

#include "yaml-cpp/emitter.h"

class yamlSerializerObject::_yamlSerializerObject_pimpl
{
public:
	YAML::Emitter doc;
};

yamlSerializerObject::yamlSerializerObject() : _pimpl(std::make_unique<_yamlSerializerObject_pimpl>())
{
	_pimpl->doc << YAML::BeginDoc;
}

yamlSerializerObject::~yamlSerializerObject()
{
	_pimpl->doc << YAML::EndDoc;
}

std::string yamlSerializerObject::getStringRepresentation()
{
	if (_pimpl->doc.good())
		return _pimpl->doc.c_str();
	return "SERIALIZATION_ERROR";
}

void yamlSerializerObject::startObject(const std::string &name)
{
	_pimpl->doc << YAML::Key << name << YAML::BeginMap;
}

void yamlSerializerObject::startObject()
{
	_pimpl->doc << YAML::BeginMap;
}

void yamlSerializerObject::endObject()
{
	_pimpl->doc << YAML::EndMap;
}

void yamlSerializerObject::startArray(const std::string &name)
{
	_pimpl->doc << YAML::Key << name << YAML::BeginSeq;
}

void yamlSerializerObject::startArray()
{
	_pimpl->doc << YAML::BeginSeq;
}

void yamlSerializerObject::endArray()
{
	_pimpl->doc << YAML::EndSeq;
}

void yamlSerializerObject::startPrimitive(const std::string &name)
{
	_pimpl->doc << YAML::Key << name << YAML::Value;
}

void yamlSerializerObject::addValue(int value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(double value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(float value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(char value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(wchar_t value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(bool value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(const std::string &value)
{
	_pimpl->doc << value;
}

void yamlSerializerObject::addValue(char const *value)
{
	_pimpl->doc << value;
}
