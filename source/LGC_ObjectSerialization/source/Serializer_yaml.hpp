/*
© Copyright CERN 2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _SERIALIZER_YAML
#define _SERIALIZER_YAML

#include "Serializer.hpp"
#include <yaml-cpp/emitter.h>

class yamlSerializerObject : public SerializerObject
{
public:
	yamlSerializerObject() { doc << YAML::BeginDoc; }
	~yamlSerializerObject() { doc << YAML::EndDoc; }

	virtual std::string getStringRepresentation() override;

protected:
	virtual void startObject(const std::string& name) override;
	virtual void endObject() override;
	virtual void startArray(const std::string& name) override;
	virtual void endArray() override;
	virtual void startPrimitive(const std::string& name) override;
	virtual void endPrimitive() override {}

	virtual void addValue(int value) override { doc << value; }
	virtual void addValue(double value) override { doc << value; }
	virtual void addValue(float value) override { doc << value; }
	virtual void addValue(char value) override { doc << value; }
	virtual void addValue(wchar_t value) override { doc << value; }
	virtual void addValue(bool value) override { doc << value; }
	virtual void addValue(const std::string& value) override { doc << value; }
	virtual void addValue(char const* value) override { doc << value; }

private:
	template<class T>
	static void addPrimitive(YAML::Emitter& doc, const std::string& name, const T value)
	{
		doc << YAML::Key << name << YAML::Value << value;
	}

	template<class T>
	static void addContainer(YAML::Emitter& doc, const std::string& name, const T& value)
	{
		doc << YAML::Key << name << YAML::BeginSeq << YAML::Flow << value << YAML::EndSeq;
	}

private:
	YAML::Emitter doc;
};
#endif
