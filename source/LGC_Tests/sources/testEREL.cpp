// SPDX-FileCopyrightText: 2025 CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testEREL.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
	struct test_EREL 
	{
		test_EREL() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
		std::shared_ptr<TLGCData> projTest;
		TReader reader;
	};
	typedef test_group<test_EREL> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test EREL and ERELFRAME (relative Error) computations");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{

		set_test_name("Testing standard EREL use");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/EREL.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(Erel::EREL_test);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		const TLGCData& dataset = calcul.getData();
		std::vector<TLSCalcRelativeErrorPoint> lsErrors = dataset.getRelError().points;
		auto relErrorIt = lsErrors.begin();
		
		TLSCalcRelativeErrorPoint relError1 = *relErrorIt;
		// test if precisions from polar measurement target can be recovered by erel (all in root)
		ensure_equals("Zend sigma has to match.", relError1.getSigmaV().getSignedCCValue(), 13, 1e-4);
		ensure_equals("Orientation sigma has to to match.", relError1.getSigmaG().getSignedCCValue(), 12, 1e-4);
		ensure_equals("Distance sigma has to match.", relError1.getSigmaL().getMMetresValue(), 14, 1e-4);

		// test if precisions from polar measurement target can be recovered by erel (point in root observed from uncertain frame) in uncertain frame coordinates (should be independent of frame uncertainty)
		relErrorIt++;
		TLSCalcRelativeErrorPoint relError2 = *relErrorIt;
		ensure_equals("Zend sigma has to match.", relError2.getSigmaV().getSignedCCValue(), 13, 1e-4);
		ensure_equals("Orientation sigma has to to match.", relError2.getSigmaG().getSignedCCValue(), 12, 1e-4);
		ensure_equals("Distance sigma has to match.", relError2.getSigmaL().getMMetresValue(), 14, 1e-4);
		
		// test if the relative error between a fixed point in a frame and a point in a different frame measured only by one obsxyz from that frame is equal to the relative error of an equivalent setup in root
		relErrorIt++;
		TLSCalcRelativeErrorPoint relErrorReferenceSetup = *relErrorIt;
		relErrorIt++;
		TLSCalcRelativeErrorPoint relErrorComplicatedTrSetup = *relErrorIt;
		ensure_equals("L relative Error has to match.", relErrorReferenceSetup.getSigmaL(), relErrorComplicatedTrSetup.getSigmaL(), 1e-4);
		ensure_equals("G relative Error has to match.", relErrorReferenceSetup.getSigmaG(), relErrorComplicatedTrSetup.getSigmaG(), 1e-4);
		ensure_equals("R relative Error has to match.", relErrorReferenceSetup.getSigmaR(), relErrorComplicatedTrSetup.getSigmaR(), 1e-4);
		ensure_equals("Z relative Error has to match.", relErrorReferenceSetup.getSigmaZ(), relErrorComplicatedTrSetup.getSigmaZ(), 1e-4);
		ensure_equals("V relative Error has to match.", relErrorReferenceSetup.getSigmaV(), relErrorComplicatedTrSetup.getSigmaV(), 1e-4);
		relErrorIt++;
		TLSCalcRelativeErrorPoint relErrorComplicatedRotSetup = *relErrorIt;	
		ensure_equals("L relative Error has to match.", relErrorReferenceSetup.getSigmaL(), relErrorComplicatedRotSetup.getSigmaL(), 1e-4);
		ensure_equals("G relative Error has to match.", relErrorReferenceSetup.getSigmaG(), relErrorComplicatedRotSetup.getSigmaG(), 1e-4);
		ensure_equals("R relative Error has to match.", relErrorReferenceSetup.getSigmaR(), relErrorComplicatedRotSetup.getSigmaR(), 1e-4);
		ensure_equals("Z relative Error has to match.", relErrorReferenceSetup.getSigmaZ(), relErrorComplicatedRotSetup.getSigmaZ(), 1e-4);
		ensure_equals("V relative Error has to match.", relErrorReferenceSetup.getSigmaV(), relErrorComplicatedRotSetup.getSigmaV(), 1e-4);


	}
	template<>
	template<>
	void object::test<2>()
	{

		set_test_name("Testing *ERELFRAME use");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/EREL.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(Erel::ERELFRAME_test_1);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	}

	template<>
	template<>
	void object::test<3>()
	{
		set_test_name("Testing *ERELFRAME computation");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/EREL.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(Erel::ERELFRAME_test_2);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		const TLGCData &dataset = calcul.getData();

		std::vector<TLSCalcRelativeErrorFrame> lsErrors = dataset.getRelError().frames;
		// get the first frame erel
		auto relErrorIt = lsErrors.begin();
		TLSCalcRelativeErrorFrame frameRelError = *relErrorIt;
		TAdjustableHelmertTransformation relativeFrame = frameRelError.getResult();

		// get the F1 frame, which should be equal
		TAdjustableHelmertTransformation F1 = projTest.get()->locateNode("F1").node->data.get()->frame;

		// compare them, they should be equal per construction of the test
		// compare the frame parameters
		TransformParameters F1par = F1.getEstParam();
		TransformParameters Relpar = relativeFrame.getEstParam();
		double tol = 1e-12;
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.tX), double(Relpar.tX), tol);
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.tY), double(Relpar.tY), tol);
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.tZ), double(Relpar.tZ), tol);
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.omega), double(Relpar.omega), tol);
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.phi), double(Relpar.phi), tol);
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.kappa), double(Relpar.kappa), tol);
		ensure_equals("Helmert parameters need to be equivalent", double(F1par.scale), double(Relpar.scale), tol);

		// compare the covars
		ensure_equals("Covariance needs to be equivalent", (F1.getCovar() - relativeFrame.getCovar()).norm(), 0, tol);

	}

};
