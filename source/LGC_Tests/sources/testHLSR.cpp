#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>
#include "testHLSR.h"
#include "TLGCCalculation.h"
#include <Behavior.h>
#include "TXYH2CCS.h"

namespace tut
{
	struct test_HLSR
	{
		test_HLSR() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

		std::shared_ptr<TLGCData> projTest;
		TReader r;
	};
	typedef test_group<test_HLSR> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("HLSR tests");
}

namespace tut
{
	
	//------------------------------------------ ECWS -------------------------------------------------//

	template<>
	template<>
	void object::test<1>()
	{
		set_test_name("Testing HLSR, 4 arguments (3 declared) in INSTR section");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_PARSE_2.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_PARSE_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Too little HLSR arguments not detected", succesReading, false);
	}

	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing HLSR, 4 arguments (5 declared)) in INSTR section");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_PARSE_3.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_PARSE_3);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Too many HLSR arguments not detected", succesReading, false);
	}

	template<>
	template<>
	void object::test<3>()
	{
		set_test_name("Testing ECWS, Water Surface name duplicate");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_PARSE_4.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_PARSE_4);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Same water surface name not detected", succesReading, false);
	}

	template<>
	template<>
	void object::test<4>()
	{
		set_test_name("Testing ECWS,OLOC Case with perfect observations");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_OLOC_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_OLOC_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Same water surface name not detected", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TDataTree tree = projTest->getTree();

		//Check the Z coordinate of a point in the root
		TPositionVector PT1 = dataset.getPoints().getObject("ROOTPT1").getEstimatedValue();
		ensure_equals("ROOTPT1 Z coordinate should match", PT1.getZ().getMetresValue(), 0.5, 1e-7);

		//Check the Z coordinate of a point with a 0gon subframe rotation
		TLOR2LOR PT2locToROOT(tree, "Q3_A_HLS_LOTH", "ROOT", "LOC2TOROOT");
		TPositionVector PT2 = dataset.getPoints().getObject("Q3_A_HLS_LO_RefPtet").getEstimatedValue();
		PT2locToROOT.transform(PT2);
		ensure_equals("Q3_A_HLS_LO_RefPtet Z coordinate should match", PT2.getZ().getMetresValue(), 0.3, 1e-7);


		//Check the Z coordinate of a point with a 200gon subframe rotation
		TLOR2LOR PT3locToROOT(tree, "Q3_A_HLS_LO", "ROOT", "LOC3TOROOT");
		TPositionVector PT3 = dataset.getPoints().getObject("Q3_A_HLS_LO_RefPt").getEstimatedValue();
		PT3locToROOT.transform(PT3);
		ensure_equals("Q3_A_HLS_LO_RefPt Z coordinate should match", PT3.getZ().getMetresValue(), 0.7, 1e-7);

		TDataTreeIterator frameIt = tree.begin();
		auto romIt = frameIt.node->data->measurements.fECWS.begin();
		
		ensure_equals("LHC.L2.HLS_LO water height should match", romIt->fMeasuredWSHeight->getEstimatedValue().getMetresValue(), 0.5, 1e-7);
		ensure_equals("Water Surface name should match", romIt->romName, "LHC.L2.HLS_LO");

		romIt++;
		ensure_equals("LHC.L2.HLS_HI water height should match", romIt->fMeasuredWSHeight->getEstimatedValue().getMetresValue(), 0.6, 1e-7);
		ensure_equals("Water Surface name should match", romIt->romName, "LHC.L2.HLS_HI");

		frameIt++;
		ensure_equals("Q3_A_HLS_LO Z Translation should match", frameIt.node->data->frame.getEstTranslation(2).getMetresValue(), 1, 1e-7);
		frameIt++;
		ensure_equals("Q3_A_HLS_LOTH Z Translation should match", frameIt.node->data->frame.getEstTranslation(2).getMetresValue(), 0.2, 1e-7);
	}

	template<>
	template<>
	void object::test<5>()
	{
		set_test_name("Testing ECWS,OLOC Case");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_OLOC_2.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_OLOC_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Same water surface name not detected", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	}


	template<>
	template<>
	void object::test<6>()
	{
		set_test_name("Testing ECWS,Geodetic Case with perfect observations");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_GEO_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_GEO_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Same water surface name not detected", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TDataTree tree = projTest->getTree();

		//Check the Z coordinate of a point in the root
		TPositionVector PT1 = dataset.getPoints().getObject("LHC.WATER3001.MQXA.3L8.E.").getEstimatedValue();
		TXYH2CCS::CCS2XYHg1985Machine(PT1);
		ensure_equals("LHC.WATER3001.MQXA.3L8.E. H coordinate should match", PT1.getH().getMetresValue(), 330, 1e-7);

		//Check the Z coordinate of a point with a 0gon subframe rotation
		TLOR2LOR PT2locToROOT(tree, "Reel", "ROOT", "LOCTOROOT");
		TPositionVector PT2 = dataset.getPoints().getObject("LHC.WATER3001.MQXB.B2L8.T.").getEstimatedValue();
		PT2locToROOT.transform(PT2);
		TXYH2CCS::CCS2XYHg1985Machine(PT2);
		ensure_equals("LHC.WATER3001.MQXB.B2L8.T. H coordinate should match", PT2.getH().getMetresValue(), 329.5, 1e-7);


		//Check the Z coordinate of a point with a 200gon subframe rotation
		TLOR2LOR PT3locToROOT(tree, "Reel2", "ROOT", "LOC3TOROOT");
		TPositionVector PT3 = dataset.getPoints().getObject("LHC.WATER3001.MQXB.B2L8.Z.").getEstimatedValue();
		PT3locToROOT.transform(PT3);
		TXYH2CCS::CCS2XYHg1985Machine(PT3);
		ensure_equals("LHC.WATER3001.MQXB.B2L8.Z. H coordinate should match", PT3.getH().getMetresValue(), 330.5, 1e-7);

		//testing all the options changes
		TDataTreeIterator frameIt = tree.begin();
		auto romIt = frameIt.node->data->measurements.fECWS.begin();
		ensure_equals("WATER3001 water height should match", romIt->fMeasuredWSHeight->getEstimatedValue().getMetresValue(), 330, 1e-7);
		ensure_equals("Instrument name ROM should match", romIt->instrument.ID, "HL_SNSR_1");
		
		auto measIt = romIt->measECWS.begin();
		ensure_equals("Firt measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_2");
		ensure_equals("Firt measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.05 / 1000, 1e-7);
		ensure_equals("Firt measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.02 / 1000, 1e-7);
		ensure_equals("Firt measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 1.0 / 1000, 1e-7);
		ensure_equals("Firt measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
		measIt++;
		ensure_equals("Second measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_1");
		ensure_equals("Second measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.03 / 1000, 1e-7);
		ensure_equals("Second measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.0, 1e-7);
		ensure_equals("Second measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 0.0, 1e-7);
		ensure_equals("Second measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
		measIt++;
		measIt++;
		ensure_equals("Fourth measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_1");
		ensure_equals("Fourth measurement: Sigma Dist should match", measIt->target.sigmaDist, 1.0 / 1000, 1e-7);
		ensure_equals("Fourth measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.0, 1e-7);
		ensure_equals("Fourth measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 0.0, 1e-7);
		ensure_equals("Fourth measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
		measIt++;
		ensure_equals("Fifth measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_1");
		ensure_equals("Fifth measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.03 / 1000, 1e-7);
		ensure_equals("Fifth measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 2.0 / 1000, 1e-7);
		ensure_equals("Fifth measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 0.0, 1e-7);
		ensure_equals("Fifth measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
		measIt++;
		ensure_equals("Sixth measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_1");
		ensure_equals("Sixth measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.03 / 1000, 1e-7);
		ensure_equals("Sixth measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.0, 1e-7);
		ensure_equals("Sixth measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 3.0 / 1000, 1e-7);
		ensure_equals("Sixth measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
		measIt++;
		ensure_equals("Seventh measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_1");
		ensure_equals("Seventh measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.03 / 1000, 1e-7);
		ensure_equals("Seventh measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.0, 1e-7);
		ensure_equals("Seventh measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 0.0, 1e-7);
		ensure_equals("Seventh measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 4.0 / 1000, 1e-7);
		measIt++;
		measIt++;
		ensure_equals("Nineth measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_2");
		ensure_equals("Nineth measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.05 / 1000, 1e-7);
		ensure_equals("Nineth measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.02 / 1000, 1e-7);
		ensure_equals("Nineth measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 1.0 / 1000, 1e-7);
		ensure_equals("Nineth measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
		measIt++;
		ensure_equals("Tenth measurement: Instrument name should match", measIt->target.ID, "HL_SNSR_1");
		ensure_equals("Tenth measurement: Sigma Dist should match", measIt->target.sigmaDist, 0.03 / 1000, 1e-7);
		ensure_equals("Tenth measurement: Sigma Instr Height should match", measIt->target.sigmaInstrHeight, 0.0, 1e-7);
		ensure_equals("Tenth measurement: Sigma Instr Centering should match", measIt->target.sigmaInstrCentering, 0.0, 1e-7);
		ensure_equals("Tenth measurement: Sigma Water Surface should match", measIt->target.sigmaWS, 0.02 / 1000, 1e-7);
	}


	template<>
	template<>
	void object::test<7>()
	{
		set_test_name("Testing ECWS,Geodetic Case from a real IP8 low beta measurement");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_GEO_2.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_GEO_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : Same water surface name not detected", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	}

		
	template<>
	template<>
	void object::test<8>()
	{
		set_test_name("Testing ECWS, 1 STATION in sub-frame");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_PARSE_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_PARSE_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : No allowed ECWS in sub-frame", succesReading, false);
	}

	template<>
	template<>
	void object::test<9>()
	{
		set_test_name("Testing empty ECWS Rom");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_PARSE_5.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_PARSE_5);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull : empty ECWS read", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TDataTree tree = projTest->getTree();

		//Check the Z coordinate of a point in the root
		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("PT2 Z coordinate should match", PT2.getZ().getMetresValue(), -0.2, 1e-7);
		ensure_equals("Computation should have 3 unknows", dataset.fUEOIndices.UIndex, 3);
		ensure_equals("Computation should have 3 equations", dataset.fUEOIndices.EIndex, 3);
		ensure_equals("Computation should have 3 observations", dataset.fUEOIndices.OIndex, 3);
	}

	template<>
	template<>
	void object::test<10>()
	{
		set_test_name("Testing reading observation ID");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/HLSR_ID.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestHLSR::ECWS_ID);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull ", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData &dataset = calcul.getData();

		TDataTree tree = projTest->getTree();

		// Check the observation ID
		ensure_equals("The observation ID is HL_SNSR_1_PT1", tree.begin()->get()->measurements.fECWS.begin()->measECWS.begin()->obsID, "HL_SNSR_1_PT1"); 
		ensure_equals("The length of the observation ID is 13", dataset.getConfig().obsIDwidth, (int)tree.begin()->get()->measurements.fECWS.begin()->measECWS.begin()->obsID.size());
	}
}
