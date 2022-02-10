#include "Serializer_yaml.hpp"
#include <yaml-cpp/stlemitter.h>

std::string yamlSerializerObject::getStringRepresentation()
{
	if (doc.good())
		return doc.c_str();
	return "ERROR";
}

void yamlSerializerObject::startObject(const std::string& name)
{
	doc << YAML::Key << name << YAML::BeginMap;
}

void yamlSerializerObject::startObject()
{
	doc << YAML::BeginMap;
}

void yamlSerializerObject::endObject()
{
	doc << YAML::EndMap;
}

void yamlSerializerObject::startArray(const std::string& name)
{
	doc << YAML::Key << name << YAML::BeginSeq;
}

void yamlSerializerObject::startArray()
{
	doc << YAML::BeginSeq;
}

void yamlSerializerObject::endArray()
{
	doc << YAML::EndSeq;
}

void yamlSerializerObject::startPrimitive(const std::string& name)
{
	doc << YAML::Key << name << YAML::Value;
}
