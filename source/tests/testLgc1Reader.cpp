#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCData.h>
#include <TReader.h>
#include "testLgc1Reader.h"
#include "TLGCCalculation.h"


namespace tut
{
	struct test_lgc1Reader{};
	typedef test_group<test_lgc1Reader> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test lgc v1 file");
}

namespace tut
{
	//----------------------------- ANGL_ZENH --------------------------------//
	template<>
	template<>
	void object::test<1>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZENH measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/lgc1_ANGL_ZENH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLgc1::ANGL_ZENH);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		std::vector<shared_ptr<TTSTN>> meas = projTest->getCurrentNode().measurements.fTSTN;
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		
		const TLGCData& dataset = calcul.getData();
		
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("PT x coordinate should match", P2.getX().getMetresValue(), 99.7916953, 1e-7);
		ensure_equals("PT y coordinate should match", P2.getY().getMetresValue(), 99.9915105, 1e-7);
		ensure_equals("PT z coordinate should match", P2.getZ().getMetresValue(), 99.8916736, 1e-7);
		
		
		//sigma value
		ensure_equals("sigma should match ", meas.at(0)->roms.back()->measANGL.at(1).target.sigmaAngl.getSignedCCValue(), 2.0, 1e-1);
		ensure_equals("1rst tgt heigth should match ", meas.at(0)->roms.back()->measZEND.at(0).target.targetHt, 0.5, 1e-1);
		//1rst instrument heigth
		ensure_equals("1rst intrument heigth should match ", meas.at(0)->instrumentHeightAdjustable->getEstimatedValue().getMetresValue(), 0.001, 1e-3 );
	}

	//----------------------------- ANGL_ZENI --------------------------------//
	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZENI measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/lgc1_ANGL_ZENI.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLgc1::ANGL_ZENI);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		std::vector<shared_ptr<TTSTN>> meas = projTest->getCurrentNode().measurements.fTSTN;
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		
		const TLGCData& dataset = calcul.getData();
		
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("PT x coordinate should match", P2.getX().getMetresValue(), 99.7768768, 1e-7);
		ensure_equals("PT y coordinate should match", P2.getY().getMetresValue(), 99.9766624, 1e-7);
		ensure_equals("PT z coordinate should match", P2.getZ().getMetresValue(), 99.9213383, 1e-7);
		
		//instrument heigth is adk=justable
		ensure_equals("1rst instrument heigth free ", meas.at(0)->instrumentHeightAdjustable->isFixed(), false);
		ensure_equals("1rst instrument heigth  ", meas.at(0)->instrumentHeightAdjustable->getEstimatedValue(), 0.2193, 1e-3);
		ensure_equals("2nd nstrument heigth free ", meas.at(1)->instrumentHeightAdjustable->isFixed(), false);
		ensure_equals("1rst instrument heigth  ", meas.at(1)->instrumentHeightAdjustable->getEstimatedValue(), 0.0167, 1e-3);
		
		//sigma value
		ensure_equals("sigma should match ", meas.at(0)->roms.back()->measANGL.at(1).target.sigmaAngl.getSignedCCValue(), 2.0, 1e-1);
		ensure_equals("1rst tgt heigth should match ", meas.at(0)->roms.back()->measZEND.at(0).target.targetHt, 0.5, 1e-1);

	}

	//----------------------------- TSTN_DVER --------------------------------//
	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("DVER + TSTN in RS2K");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLgc1::RS2K_TSTN_DVER);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		
		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z
		PT     -934.3289435  10514.1302755   2413.7946543   0.6305   1.0423   0.0704
		PT2    -895.6502606  10421.8383549   2413.0328245   0.7538   1.4660   0.0706
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), -934.3289435, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 10514.1302755, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2413.7946543, 1e-7);
		//sigma are store in m in lgc2
		ensure_equals("Pt sx should match", dataset.getPoints().getObject("PT").getXEstPrecision(), 0.001*0.6305, 1e-5);
		ensure_equals("Pt sy should match", dataset.getPoints().getObject("PT").getYEstPrecision(), 0.001*1.0423, 1e-5);
		ensure_equals("Pt sz should match", dataset.getPoints().getObject("PT").getZEstPrecision(), 0.001*0.0704, 1e-5);
		
		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT2.getX().getMetresValue(), -895.6502606, 1e-7);
		ensure_equals("Pt y coordinate should match", PT2.getY().getMetresValue(), 10421.8383549, 1e-7);
		ensure_equals("Pt z coordinate should match", PT2.getZ().getMetresValue(), 2413.0328245, 1e-7);
		//sigma are store in m in lgc2
		ensure_equals("PT2 sx should match", dataset.getPoints().getObject("PT2").getXEstPrecision(), 0.001*0.7538, 1e-5);
		ensure_equals("PT2 sy should match", dataset.getPoints().getObject("PT2").getYEstPrecision(), 0.001*1.4660, 1e-5);
		ensure_equals("PT2 sz should match", dataset.getPoints().getObject("PT2").getZEstPrecision(), 0.001*0.0706, 1e-5);

		std::vector<TDVER> meas = projTest->getCurrentNode().measurements.fDVER;
		//constante
		ensure_equals("constante correction should match", meas.back().getDistanceCorrection().getMetresValue(), 1.0, 1e-5);

	}

	//----------------------------- DLEV --------------------------------//
	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("DLEV");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDLEV.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLgc1::OLOC_DLEV);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z
		PT              0.00000      0.00000     50.00000
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 0.0, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 50.0, 1e-7);

		std::vector<TLEVEL> meas = projTest->getCurrentNode().measurements.fLEVEL;
		//constante
		ensure_equals("constante correction should match", meas.back().measDLEV.at(2).target.distCorrectionValue, 1.0, 1e-1);
		ensure_equals("constante correction should match", meas.back().measDLEV.at(3).target.distCorrectionValue, 0.0, 1e-1);
		//sigma
		ensure_equals("constante correction should match", meas.back().measDLEV.at(2).target.sigmaD.getMMetresValue(), 0.2, 1e-2);
		ensure_equals("constante correction should match", meas.back().measDLEV.at(3).target.sigmaD.getMMetresValue(), 0.8, 1e-2);

	}

	//----------------------------- RADI --------------------------------//
	template<>
	template<>
	void object::test<5>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("RADI");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outRADI.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLgc1::LEP_RADI);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z
		B___________1   2100.0120782   3999.9957879   2400.7144736		
		C___________1   2100.0076157   4099.9943840   2399.6836689		
		D___________1   2000.0054888   4099.9974433   2401.6845825

		*/
		TPositionVector B___________1 = dataset.getPoints().getObject("B___________1").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", B___________1.getX().getMetresValue(), 2100.0120782, 1e-7);
		ensure_equals("Pt y coordinate should match", B___________1.getY().getMetresValue(), 3999.9957879, 1e-7);
		ensure_equals("Pt z coordinate should match", B___________1.getZ().getMetresValue(), 2400.7144736, 1e-7);

		TPositionVector C___________1 = dataset.getPoints().getObject("C___________1").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", C___________1.getX().getMetresValue(), 2100.0076157, 1e-7);
		ensure_equals("Pt y coordinate should match", C___________1.getY().getMetresValue(), 4099.9943840, 1e-7);
		ensure_equals("Pt z coordinate should match", C___________1.getZ().getMetresValue(), 2399.6836689, 1e-7);

		TPositionVector D___________1 = dataset.getPoints().getObject("D___________1").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", D___________1.getX().getMetresValue(), 2000.0054888, 1e-7);
		ensure_equals("Pt y coordinate should match", D___________1.getY().getMetresValue(), 4099.9974433, 1e-7);
		ensure_equals("Pt z coordinate should match", D___________1.getZ().getMetresValue(), 2401.6845825, 1e-7);

		std::vector<TRADI> meas = projTest->getCurrentNode().measurements.fRADI;
		//sigma
		ensure_equals("constante correction should match", meas.at(0).getObservedStDev().getMMetresValue(), 0.2, 1e-2);
		ensure_equals("constante correction should match", meas.at(1).getObservedStDev().getMMetresValue(), 0.1, 1e-2);

	}
}