#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCApp.h>
#include <TLGCData.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "Utils.h"
#include "testMixingObservation.h"
#include "testSimulation.h"

namespace tut
{
struct test_simulation
{
	test_simulation() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
};
typedef test_group<test_simulation> factory;
typedef factory::object object;

} // namespace tut

namespace
{
tut::factory tf("Testing SIMU option");
}

namespace tut
{

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing OBSXYZ Simulation");

	std::stringstream infiler(Simu::OBSXYZ_1);
	projTest->getFileLogger().writeReportHeader("LGC output file");
	bool succesReading = r.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	std::string infileLocation = "dummy_string";
	std::string outfileLocation = "C:/Temp/SIMU_OBSXYZ_test.txt";

	TLGCApp testApp(infileLocation, outfileLocation, 80);

	std::shared_ptr<TAStreamFormatter> streamFormatter;
	// Initialize the writer into the output file.
	testApp.initializeStream(projTest, outfileLocation, streamFormatter);

	TSimulationOutputFileWriter fileWriter(streamFormatter.get(), projTest.get());
	std::shared_ptr<TSimulationOutputFileWriter> writerPointer = std::make_shared<TSimulationOutputFileWriter>(fileWriter);
	TLGCCalculation calcul(projTest);
	Behavior success = calcul.computeResults(writerPointer);

	TDataTreeIterator frameIterator = projTest->getTree().begin();
	// go to the first subframe
	frameIterator++;
	std::list<TOBSXYZ> obsxyzMeasurements = frameIterator.node->data->measurements.fOBSXYZ;
	// test if the simulated observations are set consistent with the provisional parameter values
	auto it = obsxyzMeasurements.begin();
	ensure_equals("Observed x coordinate shoule be approximately 0", it->obsValue.getX(), 0.0, 1e-3);
	ensure_equals("Observed y coordinate shoule be approximately -3", it->obsValue.getY(), -3.0, 1e-3);
	ensure_equals("Observed z coordinate shoule be approximately -7", it->obsValue.getZ(), -7.0, 1e-3);
	it++;
	ensure_equals("Observed x coordinate shoule be approximately -1", it->obsValue.getX(), -1.0, 1e-3);
	ensure_equals("Observed y coordinate shoule be approximately -2", it->obsValue.getY(), -2.0, 1e-3);
	ensure_equals("Observed z coordinate shoule be approximately -7", it->obsValue.getZ(), -7.0, 1e-3);
	it++;
	ensure_equals("Observed x coordinate shoule be approximately -1", it->obsValue.getX(), -1.0, 1e-3);
	ensure_equals("Observed y coordinate shoule be approximately -3", it->obsValue.getY(), -3.0, 1e-3);
	ensure_equals("Observed z coordinate shoule be approximately -6", it->obsValue.getZ(), -6.0, 1e-3);

	// Check the estimated Helmert rotation Parameters, they should be roughly 1,3,7,
	// independant of the observed values in the input file
	TAdjustableHelmertTransformation frameTrafo(frameIterator.node->data.get()->frame);
	TransformParameters trafoPar = frameTrafo.getEstParam();

	ensure_equals("Estimated TX should be around 1 m", trafoPar.tX.getMetresValue(), 1.0, 1e-4);
	ensure_equals("Estimated TY should be around 2 m", trafoPar.tY.getMetresValue(), 3.0, 1e-4);
	ensure_equals("Estimated TZ should be around 3 m", trafoPar.tZ.getMetresValue(), 7.0, 1e-4);
}

} // namespace tut
