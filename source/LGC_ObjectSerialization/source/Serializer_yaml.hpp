/*
© Copyright CERN 2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _SERIALIZER_YAML
#define _SERIALIZER_YAML

#include <yaml-cpp/emitter.h>

#include "Serializer.hpp"

class yamlSerializerObject : public SerializerObject
{
public:
	yamlSerializerObject() { doc << YAML::BeginDoc; }
	~yamlSerializerObject() { doc << YAML::EndDoc; }

	virtual std::string getStringRepresentation() override;

protected:
	virtual void startObject(const std::string &name) override;
	virtual void startObject() override;
	virtual void endObject() override;
	virtual void startArray(const std::string &name) override;
	virtual void startArray() override;
	virtual void endArray() override;
	virtual void startPrimitive(const std::string &name) override;
	virtual void endPrimitive() override {}

	virtual void addValue(int value) override { doc << value; }
	virtual void addValue(double value) override { doc << value; }
	virtual void addValue(float value) override { doc << value; }
	virtual void addValue(char value) override { doc << value; }
	virtual void addValue(wchar_t value) override { doc << value; }
	virtual void addValue(bool value) override { doc << value; }
	virtual void addValue(const std::string &value) override { doc << value; }
	virtual void addValue(char const *value) override { doc << value; }

private:
	YAML::Emitter doc;
};
#endif
