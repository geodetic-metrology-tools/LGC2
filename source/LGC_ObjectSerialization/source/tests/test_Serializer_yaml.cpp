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
	SerializerObject::SerializationHelper serobj = ser.getSerializationHelper();
	const std::string test_header = "---\n";

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
	set_test_name("Serializer_yaml: Primitives and containers serialization test");

	serobj.addProperty("t1_header", std::string("I am the header!"));
	serobj.addProperty("t2_vec", std::vector<int>{1, 2, 3, 4});
	serobj.addProperty("t3_int", 1);
	serobj.addProperty("t4_str", "cstyle");
	serobj.addProperty("t5_strstd", std::string("std"));
	serobj.addProperty("t6_list", std::list<double>{1, 2, 3});
	serobj.addProperty("t7_map", std::map<std::string, int>{{"foo", 42}, {"bar", 3}});
	serobj.addProperty("t8_mapvec", std::map<std::string, std::vector<int>>{{"foo", {42, 1, 2}}, {"bar", {3, 5}}});
	serobj.addProperty("t9_pair", std::pair<std::string, std::string>{"pairkey", "pairvalue"});
	serobj.addProperty("t10_vecpair", std::vector<std::pair<std::string, bool>>{{"pair1", true}, {"pair2", false}});
	serobj.addProperty("t11_pairvecpair", std::pair<std::string, std::vector<std::pair<std::string, bool>>>{"pairTop", {{"pairsub1", true}, {"pairsub2", false}}});
	serobj.addProperty("t12_listvecpair", std::list<std::vector<std::pair<std::string, bool>>>{{{"pair1", true}}, {{"pair2", false}}, {{"pair31", true}, {"pair32", false}}});
	serobj.addProperty("t13_pairvecmaplistpair",
		std::pair<std::string, std::vector<std::map<std::string, std::list<std::pair<std::string, int>>>>>{"toppair",
			{
				{{"map1key1", {{"pair1", 1}, {"pair2", 2}, {"pair3", 3}}}, {"map2key2", {{"pair1", 1}}}},
				{{"map2key1", {{"pair1", 1}, {"pair2", 2}, {"pair3", 3}, {"pair4", 4}, {"pair5", 5}}}},
				{{"map3key1", {{"pair1", 1}}}, {"map3key2", {{"pair1", 1}}}, {"map3key3", {{"pair1", 1}}}, {"map3key4", {{"pair1", 1}}}},
			}});

	ensure_equals(test_header + R"""(t1_header: I am the header!
t2_vec:
  - 1
  - 2
  - 3
  - 4
t3_int: 1
t4_str: cstyle
t5_strstd: std
t6_list:
  - 1
  - 2
  - 3
t7_map:
  bar: 3
  foo: 42
t8_mapvec:
  bar:
    - 3
    - 5
  foo:
    - 42
    - 1
    - 2
t9_pair:
  pairkey: pairvalue
t10_vecpair:
  pair1: true
  pair2: false
t11_pairvecpair:
  pairTop:
    pairsub1: true
    pairsub2: false
t12_listvecpair:
  - pair1: true
  - pair2: false
  - pair31: true
    pair32: false
t13_pairvecmaplistpair:
  toppair:
    - map1key1:
        pair1: 1
        pair2: 2
        pair3: 3
      map2key2:
        pair1: 1
    - map2key1:
        pair1: 1
        pair2: 2
        pair3: 3
        pair4: 4
        pair5: 5
    - map3key1:
        pair1: 1
      map3key2:
        pair1: 1
      map3key3:
        pair1: 1
      map3key4:
        pair1: 1)""",
		ser.getStringRepresentation());
}

template<>
template<>
void testobject::test<2>()
{
	set_test_name("Serializer_yaml: Serializable classes `serialize` method test");

	class PrecisionData : public Serializable
	{
	public:
		PrecisionData(int prec) : precision(prec) {}

		virtual void serialize(SerializerObject::SerializationHelper &obj) const override { obj.addProperty("precision", precision); }

	private:
		int precision;
	};

	class GridData : public Serializable
	{
	public:
		virtual void serialize(SerializerObject::SerializationHelper &obj) const override
		{
			obj.addProperty("grid", grid);
			obj.addProperty("grid_height", grid_height);
		}

	private:
		std::list<int> grid = {10, 20, 30, 40, 50};
		std::vector<double> grid_height = {1.2, 1.25, 1.22, 1.3, 1.5};
	};

	class Data : public Serializable
	{
	public:
		virtual void serialize(SerializerObject::SerializationHelper &obj) const override
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
		std::vector<int> measures{1, 2, 3, 4};
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

template<>
template<>
void testobject::test<3>()
{
	set_test_name("Serializer_yaml: container of Serializable classes, pointer and reference test");

	class Point : public Serializable
	{
	public:
		Point(int precision, int x, int y, std::string name, std::vector<double> coefficients) : precision(precision), x(x), y(y), name(name), coefficients(coefficients)
		{
		}

		virtual void serialize(SerializerObject::SerializationHelper &obj) const override
		{
			obj.addProperty("precision", precision);
			obj.addProperty("x", x);
			obj.addProperty("y", y);
			obj.addProperty("name", name);
			obj.addProperty("coefficients", coefficients);
		}

	private:
		int precision;
		int x;
		int y;
		std::string name;
		std::vector<double> coefficients;
	};

	std::vector<std::pair<std::string, Point>> points{
		{"p1", {1, 2, 3, "point1", {1, 2}}},
		{"p2", {1, 2, 3, "point2", {1, 2, 3}}},
		{"p3", {1, 2, 3, "point3", {}}},
		{"p4", {1, 2, 3, "point4", {1, 2, 3, 4, 5, 6}}},
	};
	std::vector<std::pair<std::string, Point *>> points_pointer{
		{"p1", &points[0].second},
		{"p2", &points[1].second},
		{"p3", &points[2].second},
		{"p4", &points[3].second},
	};

	serobj.addProperty("reference_vs_pointer", points);

	yamlSerializerObject ser2;
	SerializerObject::SerializationHelper serobj2 = ser2.getSerializationHelper();
	serobj2.addProperty("reference_vs_pointer", points_pointer);

	ensure_equals(ser.getStringRepresentation(), ser2.getStringRepresentation());
	ensure_equals(test_header + R"""(reference_vs_pointer:
  p1:
    precision: 1
    x: 2
    y: 3
    name: point1
    coefficients:
      - 1
      - 2
  p2:
    precision: 1
    x: 2
    y: 3
    name: point2
    coefficients:
      - 1
      - 2
      - 3
  p3:
    precision: 1
    x: 2
    y: 3
    name: point3
    coefficients:
      []
  p4:
    precision: 1
    x: 2
    y: 3
    name: point4
    coefficients:
      - 1
      - 2
      - 3
      - 4
      - 5
      - 6)""",
		ser.getStringRepresentation());
}

template<>
template<>
void testobject::test<4>()
{
	set_test_name("Serializer_yaml: Primitive pointer");

	// smart as well
	double *pointer_double = new double(6);

	serobj.addProperty("pointer_double", pointer_double);
	ensure_equals(test_header + R"""(pointer_double: 6)""", ser.getStringRepresentation());

	// cleanup
	delete pointer_double;
}

} // namespace tut
