#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testMasking.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
	struct test_Masking 
	{
		test_Masking() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
		std::shared_ptr<TLGCData> projTest;
		TReader reader;
	};
	typedef test_group<test_Masking> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test Masking (deactivation) of observations");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{

		set_test_name("Testing simple Masking use");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/Masking.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(Masking::Masking_test_1);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		const TLGCData& dataset = calcul.getData();
		// check position of point2, should be zero when both observations are active
		TPositionVector result = dataset.getPoints().getObject("point2").getEstimatedValue();
		TVector resultVec(3);
		resultVec << result.getX(), result.getY(), result.getZ();
		TVector expectedResult(3);
		expectedResult.setZero();
		ensure("Position of point2 has to be estimated at the center ", resultVec.isApprox(expectedResult));

		// now deactivate one of the measurements
		TDataTreeIterator root = projTest.get()->getTree().begin();
		for (auto &obsxyz : root.node->data.get()->measurements.fOBSXYZ)
		{
			if (obsxyz.obsID == "firstObs")
			{
				//deactivate this observation
				obsxyz.maskStatus = true;
			}
		}
		// do new calculation
		TLGCCalculation calcul2(projTest);
		succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		const TLGCData& dataset2 = calcul.getData();
		// check position of point2, should be -1,-2,-3 after first observation is deactivated
		TPositionVector result2 = dataset2.getPoints().getObject("point2").getEstimatedValue();
		TVector resultVec2(3);
		resultVec2 << result2.getX(), result2.getY(), result2.getZ();
		TVector expectedResult2(3);
		expectedResult2 << -1, -2, -3;
		ensure("After deactivation of firstObs, the Position of point2 has to be estimated at the -1,-2,-3 ", resultVec2.isApprox(expectedResult2));


	}

};

