#include <list>
#include <map>
#include <string>
#include <vector>

#include <tut/tut.hpp>

#include "Serializer_yaml.hpp"

namespace tut
{
	struct shared_data
	{
		yamlSerializerObject ser;
		SerializerObject::SerializationHelper serobj = ser.getSerializationHelper("Test");
		const std::string test_header = "---\nTest\n---\n";

		shared_data() {}
		~shared_data() {}
	};

	typedef test_group<shared_data> tg;
	tg yamlSerializerObject_test_group("Test Serializer_yaml class.");
	typedef tg::object testobject;
} // namespace tut

namespace tut
{
	/* *********************** */
	/*     YAML SERIALIZER     */
	/* *********************** */

	template<>
	template<>
	void testobject::test<1>()
	{
		set_test_name("Serializer_yaml: Primitives");

		serobj.addProperty("header", std::string("I am the header!"));
		serobj.addProperty("vec", std::vector<int>{1, 2, 3, 4});
		serobj.addProperty("int", 1);
		serobj.addProperty("str", "cstyle");
		serobj.addProperty("strstd", std::string("std"));
		serobj.addProperty("list", std::list<double>{1, 2, 3});
		serobj.addProperty("map", std::map<std::string, int>{ {"foo", 42}, { "bar", 3 }});
		serobj.addProperty("vecpair", std::vector<std::pair<std::string, bool>>{ {"yes", true}, { "no", false }});
		serobj.addProperty("pair", std::pair<std::string, std::string>{"yes", "no"});
		serobj.addProperty("pairvecpair", std::pair<std::string, std::vector<std::pair<std::string, bool>>>{"first", { {"yes", true}, {"no", false} }});
		// obj.addProperty("listvecpair", std::list<std::vector<std::pair<std::string, bool>>>{{{"yes", true}}, {{"no", false}}, {{"yes", true}, {"no", false}}});
		ensure_equals(test_header + R"""(header: I am the header!
vec:
  - 1
  - 2
  - 3
  - 4
int: 1
str: cstyle
strstd: std
list:
  - 1
  - 2
  - 3
map:
  bar: 3
  foo: 42
vecpair:
  yes: true
  no: false
pair:
  yes: no
pairvecpair:
  first:
    yes: true
    no: false)""",
			ser.getStringRepresentation());
	}

	template<>
	template<>
	void testobject::test<2>()
	{
		set_test_name("Serializer_yaml: Custom Classes");

		class PrecisionData : public Serializable
		{
		public:
			PrecisionData(int prec) : precision(prec) {}

			virtual void serialize(SerializerObject::SerializationHelper& obj) const override { obj.addProperty("precision", precision); }

		private:
			int precision;
		};

		class GridData : public Serializable
		{
		public:
			virtual void serialize(SerializerObject::SerializationHelper& obj) const override
			{
				obj.addProperty("grid", grid);
				obj.addProperty("grid_height", grid_height);
			}

		private:
			std::list<int> grid = { 10, 20, 30, 40, 50 };
			std::vector<double> grid_height = { 1.2, 1.25, 1.22, 1.3, 1.5 };
		};

		class Data : public Serializable
		{
		public:
			virtual void serialize(SerializerObject::SerializationHelper& obj) const override
			{
				obj.addProperty("input_name", input_name);
				obj.addProperty("output_name", output_name);
				obj.addProperty("origin", origin);
				obj.addProperty("measures", measures);
				obj.addProperty("precision_scale", *precision_scale);
				obj.addProperty("precision_digit", *precision_digit);
				obj.addProperty("gridData", gridData);
			}

		private:
			std::string input_name = "input.lgc";
			std::string output_name = "output.lgc";
			std::string origin = "MLA";
			std::vector<int> measures{ 1, 2, 3, 4 };
			std::unique_ptr<PrecisionData> precision_scale = std::make_unique<PrecisionData>(5);
			std::unique_ptr<PrecisionData> precision_digit = std::make_unique<PrecisionData>(3);
			GridData gridData;
		};

		serobj.addProperty("Data", Data());
		ensure_equals(test_header + R"""(Data:
  input_name: input.lgc
  output_name: output.lgc
  origin: MLA
  measures:
    - 1
    - 2
    - 3
    - 4
  precision_scale:
    precision: 5
  precision_digit:
    precision: 3
  gridData:
    grid:
      - 10
      - 20
      - 30
      - 40
      - 50
    grid_height:
      - 1.2
      - 1.25
      - 1.22
      - 1.3
      - 1.5)""",
			ser.getStringRepresentation());
	}

} // namespace tut
