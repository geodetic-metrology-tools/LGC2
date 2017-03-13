#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testAllfixed.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
	struct test_ALLFIXED{};
	typedef test_group<test_ALLFIXED> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Tests ALLFIXED");
}

namespace tut
{
	// test ZEND (Hi), DIST (Hi + Cs) and ANGL (V0) in OLOC
	template<>
	template<>
	void object::test<1>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing AZDist measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/AZDist.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestAF::AZDist);
		stringstream cp_infiler(TestAF::AZDist);

		bool succesReading = r.read(infiler, cp_infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& data = calcul.getData();

		// lgc1 results
		TAngle V0_lgc1[3];
		V0_lgc1[0] = TAngle(399.99981, TAngle::EUnits::kGons);  //pt2
		V0_lgc1[1] = TAngle(399.99988, TAngle::EUnits::kGons);  //ref
		V0_lgc1[2] = TAngle(0.00055, TAngle::EUnits::kGons);    //pt

		TLength Hi_lgc1[3];
		Hi_lgc1[0] = TLength(0.00102);
		Hi_lgc1[1] = TLength(0.00032);
		Hi_lgc1[2] = TLength(-0.00157);

		TLength Hi2_lgc1[3];
		Hi2_lgc1[0] = TLength(-0.19165);
		Hi2_lgc1[1] = TLength(-0.05138);
		Hi2_lgc1[2] = TLength(NO_VALf );

		TLength Cs_lgc1(-0.08, TLength::EUnits::kMillimetres);

		//comparison
		for (TDataTreeIterator itTree = data.getTree().begin(); itTree != data.getTree().end(); itTree++)
			for (auto& itTSTN  : itTree.node->data->measurements.fTSTN)
				for (auto& itROM : itTSTN->roms)
				{
					int i = 0;
					int j = 0;
					int k = 0;
					for (auto& itANGL : itROM->measANGL)
					{
						ensure_equals("V0 should match", itANGL.fAllFixedV0, V0_lgc1[i], 1e-5);
						i++;
					}
					for (auto& itZEND : itROM->measZEND)
					{
						ensure_equals("Hi should match", itZEND.fAllFixedHi, Hi_lgc1[j], 1e-5);
						j++;
					}
					for (auto& itDIST : itROM->measDIST)
					{
						if (!isnotanumber(Hi2_lgc1[k]))
							ensure_equals("Hi should match", itDIST.fAllFixedHi, Hi2_lgc1[k], 1e-5);
						
						//ensure_equals("Cs should match", itDIST.fAllFixedCs, Cs_lgc1, 1e-5);
						k++;
					}
				}

		// re initialise static object allfixed param to false
		LGCAdjustablePoint::setAllFixedParam(false);

	}

	// test ZEND (Hi), DIST (Hi + Cs) and ANGL (V0) in RS2K
	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing AZDist measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/AZDist.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestAF::AZDist_RS2K);
		stringstream cp_infiler(TestAF::AZDist_RS2K);

		bool succesReading = r.read(infiler, cp_infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& data = calcul.getData();

		// lgc1 results
		TAngle V0_lgc1[3];
		V0_lgc1[0] = TAngle(399.9998113, TAngle::EUnits::kGons);  //pt2
		V0_lgc1[1] = TAngle(399.9998797, TAngle::EUnits::kGons);  //ref
		V0_lgc1[2] = TAngle(0.0005556, TAngle::EUnits::kGons);    //pt

		TLength Hi_lgc1[3];
		Hi_lgc1[0] = TLength(-0.0005942);
		Hi_lgc1[1] = TLength(-0.0005413);
		Hi_lgc1[2] = TLength(-0.0023228);

		TLength Hi2_lgc1[3];
		Hi2_lgc1[0] = TLength(0.1796493);
		Hi2_lgc1[1] = TLength(-0.0425556);
		Hi2_lgc1[2] = TLength(NO_VALf);

		TLength Cs_lgc1(-0.08, TLength::EUnits::kMillimetres);

		//comparison
		for (TDataTreeIterator itTree = data.getTree().begin(); itTree != data.getTree().end(); itTree++)
			for (auto& itTSTN : itTree.node->data->measurements.fTSTN)
				for (auto& itROM : itTSTN->roms)
				{
					int i = 0;
					int j = 0;
					int k = 0;
					for (auto& itANGL : itROM->measANGL)
					{
						ensure_equals("V0 should match", itANGL.fAllFixedV0, V0_lgc1[i], 1e-5);
						i++;
					}
					for (auto& itZEND : itROM->measZEND)
					{
						ensure_equals("Hi should match", itZEND.fAllFixedHi, Hi_lgc1[j], 1e-5);
						j++;
					}
					for (auto& itDIST : itROM->measDIST)
					{
						if (!isnotanumber(Hi2_lgc1[k]))
							ensure_equals("Hi should match", itDIST.fAllFixedHi, Hi2_lgc1[k], 1e-5);
						k++;
					}
				}

		// re initialise static object allfixed param to false
		LGCAdjustablePoint::setAllFixedParam(false);

	}

	// test ECTH (V0) in OLOC
	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
	
		set_test_name("Testing ECTH measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECTH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
	
		stringstream infiler(TestAF::ECTH);
		stringstream cp_infiler(TestAF::ECTH);
	
		bool succesReading = r.read(infiler, cp_infiler);
		ensure_equals("Reading file successful", succesReading, true);
	
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	
		const TLGCData& data = calcul.getData();
	
		TAngle V0_lgc1[18];
		V0_lgc1[0]=TAngle(	200.0000000,TAngle::EUnits::kGons);
		V0_lgc1[1]=TAngle(	200.0000255,TAngle::EUnits::kGons);
		V0_lgc1[2]=TAngle(	200.0000000,TAngle::EUnits::kGons);
		V0_lgc1[3]=TAngle(	198.4721859,TAngle::EUnits::kGons);
		V0_lgc1[4]=TAngle(	198.9814174,TAngle::EUnits::kGons);
		V0_lgc1[5]=TAngle(	199.6180293,TAngle::EUnits::kGons);
		V0_lgc1[6]=TAngle(	200.0000002,TAngle::EUnits::kGons);
		V0_lgc1[7]=TAngle(	200.0000764,TAngle::EUnits::kGons);
		V0_lgc1[8]=TAngle(	199.9999999,TAngle::EUnits::kGons);
		V0_lgc1[9]=TAngle(	198.4721097,TAngle::EUnits::kGons);
		V0_lgc1[10]=TAngle(	198.9814302,TAngle::EUnits::kGons);
		V0_lgc1[11]=TAngle(	199.6180292,TAngle::EUnits::kGons);
		V0_lgc1[12]=TAngle(	179.7453834,TAngle::EUnits::kGons);
		V0_lgc1[13]=TAngle(	179.8727032,TAngle::EUnits::kGons);
		V0_lgc1[14]=TAngle(	179.9363447,TAngle::EUnits::kGons);
		V0_lgc1[15]=TAngle(	178.4721863,TAngle::EUnits::kGons);
		V0_lgc1[16]=TAngle(	178.9814053,TAngle::EUnits::kGons);
		V0_lgc1[17]=TAngle(	179.6180295,TAngle::EUnits::kGons);

		//comparison
		int i = 0;
		for (TDataTreeIterator itTree = data.getTree().begin(); itTree != data.getTree().end(); itTree++)
			for (auto& itTSTN : itTree.node->data->measurements.fTSTN)
				for (auto& itROM : itTSTN->roms)
					for (auto& itECTH : itROM->measECTH)
					{
						ensure_equals("V0 should match", itECTH.fAllFixedV0, V0_lgc1[i], 1e-7);
						i++;
					}
		// re initialise static object allfixed param to false
		LGCAdjustablePoint::setAllFixedParam(false);
	}

	// test ECTH (V0) in LEP
	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECTH measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECTHLEP.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestAF::ECTH_LEP);
		stringstream cp_infiler(TestAF::ECTH_LEP);

		bool succesReading = r.read(infiler,cp_infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& data = calcul.getData();
	
		TAngle V0_lgc1[18];
		V0_lgc1[0] = TAngle(200.0003647, TAngle::EUnits::kGons);
		V0_lgc1[1] = TAngle(200.0003902, TAngle::EUnits::kGons);
		V0_lgc1[2] = TAngle(200.0002737, TAngle::EUnits::kGons);
		V0_lgc1[3] = TAngle(198.4725410, TAngle::EUnits::kGons);
		V0_lgc1[4] = TAngle(198.9817756, TAngle::EUnits::kGons);
		V0_lgc1[5] = TAngle(199.6183010, TAngle::EUnits::kGons);
		V0_lgc1[6] = TAngle(200.0000991, TAngle::EUnits::kGons);
		V0_lgc1[7] = TAngle(200.0001756, TAngle::EUnits::kGons);
		V0_lgc1[8] = TAngle(200.0000747, TAngle::EUnits::kGons);
		V0_lgc1[9] = TAngle(198.4720004, TAngle::EUnits::kGons);
		V0_lgc1[10] = TAngle(198.9812717, TAngle::EUnits::kGons);
		V0_lgc1[11] = TAngle(199.6181062, TAngle::EUnits::kGons);
		V0_lgc1[12] = TAngle(179.7448577, TAngle::EUnits::kGons);
		V0_lgc1[13] = TAngle(179.8721774, TAngle::EUnits::kGons);
		V0_lgc1[14] = TAngle(179.9359503, TAngle::EUnits::kGons);
		V0_lgc1[15] = TAngle(178.4727151, TAngle::EUnits::kGons);
		V0_lgc1[16] = TAngle(178.9819340, TAngle::EUnits::kGons);
		V0_lgc1[17] = TAngle(179.6184261, TAngle::EUnits::kGons);

		//comparison
		int i = 0;
		for (TDataTreeIterator itTree = data.getTree().begin(); itTree != data.getTree().end(); itTree++)
			for (auto& itTSTN : itTree.node->data->measurements.fTSTN)
				for (auto& itROM : itTSTN->roms)
					for (auto& itECTH : itROM->measECTH)
					{
						ensure_equals("V0 should match", itECTH.fAllFixedV0, V0_lgc1[i], 1e-7);
						i++;
					}

		// re initialise static object allfixed param to false
		LGCAdjustablePoint::setAllFixedParam(false);
	}
}
