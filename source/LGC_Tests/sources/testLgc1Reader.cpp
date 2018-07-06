#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCData.h>
#include <TReader.h>
#include "testLgc1Reader.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

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
	//----------------------------- ANGL_ZENH_DMES --------------------------------//
	template<>
	template<>
	void object::test<1>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZENH measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/lgc1_ANGL_ZENH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ANGL_ZENH);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		std::list<std::shared_ptr<TTSTN>> meas = projTest->getCurrentNode().measurements.fTSTN;
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		
		const TLGCData& dataset = calcul.getData();
		
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("PT x coordinate should match", P2.getX().getMetresValue(), 99.7916953, 1e-7);
		ensure_equals("PT y coordinate should match", P2.getY().getMetresValue(), 99.9915105, 1e-7);
		ensure_equals("PT z coordinate should match", P2.getZ().getMetresValue(), 99.8916736, 1e-7);
		
		
		//sigma value
		ensure_equals("sigma should match ", std::next(meas.front()->roms.back()->measANGL.begin(), 1)->target.sigmaAngl.getSignedCCValue(), 2.0, 1e-1);
		ensure_equals("1rst tgt heigth should match ", meas.front()->roms.back()->measZEND.front().target.targetHt, 0.5, 1e-1);
		//1rst instrument heigth
		ensure_equals("1rst intrument heigth should match ", meas.front()->instrumentHeightAdjustable->getEstimatedValue().getMetresValue(), 0.001, 1e-3 );
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

		std::stringstream infiler(TestLgc1::ANGL_ZENI);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		std::list<std::shared_ptr<TTSTN>> meas = projTest->getCurrentNode().measurements.fTSTN;
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		
		const TLGCData& dataset = calcul.getData();
		
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("PT x coordinate should match", P2.getX().getMetresValue(), 99.7768768, 1e-7);
		ensure_equals("PT y coordinate should match", P2.getY().getMetresValue(), 99.9766624, 1e-7);
		ensure_equals("PT z coordinate should match", P2.getZ().getMetresValue(), 99.9213383, 1e-7);
		
		//instrument heigth is adk=justable
		ensure_equals("1rst instrument heigth free ", meas.front()->instrumentHeightAdjustable->isFixed(), false);
		ensure_equals("1rst instrument heigth  ", meas.front()->instrumentHeightAdjustable->getEstimatedValue(), 0.2193, 1e-3);
		ensure_equals("2nd nstrument heigth free ", (*std::next(meas.begin(), 1))->instrumentHeightAdjustable->isFixed(), false);
        ensure_equals("1rst instrument heigth  ", (*std::next(meas.begin(), 1))->instrumentHeightAdjustable->getEstimatedValue(), 0.0167, 1e-3);
		
		//sigma value
        ensure_equals("sigma should match ", std::next(meas.front()->roms.back()->measANGL.begin(), 1)->target.sigmaAngl.getSignedCCValue(), 2.0, 1e-1);
		ensure_equals("1rst tgt heigth should match ", meas.front()->roms.back()->measZEND.front().target.targetHt, 0.5, 1e-1);

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

		std::stringstream infiler(TestLgc1::RS2K_TSTN_DVER);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		
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

		std::list<TDVER> meas = projTest->getCurrentNode().measurements.fDVER;
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

		std::stringstream infiler(TestLgc1::OLOC_DLEV);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z
		PT              0.00000      0.00000     50.00000
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 0.0, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 50.0, 1e-7);

		std::list<TLEVEL> meas = projTest->getCurrentNode().measurements.fLEVEL;
		//constante
		ensure_equals("constante correction should match", std::next(meas.back().measDLEV.begin(), 2)->target.distCorrectionValue, 1.0, 1e-1);
        ensure_equals("constante correction should match", std::next(meas.back().measDLEV.begin(), 3)->target.distCorrectionValue, 0.0, 1e-1);
		//sigma
		ensure_equals("constante correction should match", std::next(meas.back().measDLEV.begin(), 2)->target.sigmaD.getMMetresValue(), 0.2, 1e-2);
		ensure_equals("constante correction should match", std::next(meas.back().measDLEV.begin(), 3)->target.sigmaD.getMMetresValue(), 0.8, 1e-2);

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

		std::stringstream infiler(TestLgc1::LEP_RADI);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

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

		std::list<TRADI> meas = projTest->getCurrentNode().measurements.fRADI;
		//sigma
		ensure_equals("constante correction should match", meas.front().getObservedStDev().getMMetresValue(), 0.2, 1e-2);
		ensure_equals("constante correction should match", std::next(meas.begin())->getObservedStDev().getMMetresValue(), 0.1, 1e-2);

	}

	//----------------------------- DTHE --------------------------------//
	template<>
	template<>
	void object::test<6>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZENI_DTHE measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZENI_DTHE_DHOR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ANGL_ZENI_DTHE);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		std::list<std::shared_ptr<TTSTN>> meas = projTest->getCurrentNode().measurements.fTSTN;
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("PT x coordinate should match", P2.getX(), 99.7911671, 1e-7);
		ensure_equals("PT y coordinate should match", P2.getY(), 99.9909812, 1e-7);
		ensure_equals("PT z coordinate should match", P2.getZ(), 99.8908785, 1e-7);
		
		//sigma value
		ensure_equals("sigma should match ", std::next(meas.front()->roms.back()->measDIST.begin(), 1)->target.sigmaDist.getMMetresValue(), 0.5, 1e-1);
        ensure_equals("sigma should match ", (*std::next(meas.begin(), 1))->roms.back()->measDIST.front().target.sigmaDist.getMMetresValue(), 0.1, 1e-1);
        ensure_equals("ppm should match ", (*std::next(meas.begin(), 1))->roms.back()->measDIST.front().target.ppmDist.getMMetresValue(), 0.5, 1e-1);
        ensure_equals("tgt heigth should match ", std::next((*std::next(meas.begin(), 1))->roms.back()->measDIST.begin(), 1)->target.targetHt, 0.002, 1e-1);

	}

	//----------------------------- DHOR --------------------------------//
	template<>
	template<>
	void object::test<7>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZENI_DHOR measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZENI_DHOR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ANGL_ZENI_DHOR);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		std::list<std::shared_ptr<TTSTN>> meas = projTest->getCurrentNode().measurements.fTSTN;
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("PT x coordinate should match", P2.getX(), 99.8261720, 1e-7);
		ensure_equals("PT y coordinate should match", P2.getY(), 100.0260562, 1e-7);
		ensure_equals("PT z coordinate should match", P2.getZ(), 99.9247094, 1e-7);

		//sigma value
		ensure_equals("sigma should match ", std::next(meas.front()->roms.back()->measDHOR.begin(), 2)->target.sigmaDist.getMMetresValue(), 0.2, 1e-1);
		ensure_equals("sigma should match ", meas.front()->roms.back()->measDHOR.front().target.sigmaDist.getMMetresValue(), 0.5, 1e-1);
		ensure_equals("ppm should match ", meas.front()->roms.back()->measDHOR.front().target.ppmDist.getMMetresValue(), 0.5, 1e-1);
        ensure_equals("ppm should match ", std::next(meas.front()->roms.back()->measDHOR.begin(), 2)->target.ppmDist.getMMetresValue(), 0.2, 1e-1);
        ensure_equals("tgt heigth should match ", std::next((*std::next(meas.begin(), 1))->roms.back()->measDHOR.begin(), 2)->target.distCorrectionValue, 1.0, 1e-1);

	}

	//----------------------------- ORIE --------------------------------//
	template<>
	template<>
	void object::test<8>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ORIE measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ORIE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ORIE);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector I_1 = dataset.getPoints().getObject("I_1").getEstimatedValue();
		ensure_equals("I_1 x coordinate should match", I_1.getX(), 2236.9919372, 1e-7);
		ensure_equals("I_1 y coordinate should match", I_1.getY(), 2422.2046495, 1e-7);
		ensure_equals("I_1 z coordinate should match", I_1.getZ(), 2449.9999706, 1e-7);
		TPositionVector E_1 = dataset.getPoints().getObject("E_1").getEstimatedValue();
		ensure_equals("E_1 x coordinate should match", E_1.getX(), 2117.5543186, 1e-7);
		ensure_equals("E_1 y coordinate should match", E_1.getY(), 2261.8054678, 1e-7);
		ensure_equals("E_1 z coordinate should match", E_1.getZ(), 2449.9999921, 1e-7);
		TPositionVector B_1 = dataset.getPoints().getObject("B_1").getEstimatedValue();
		ensure_equals("B_1 x coordinate should match", B_1.getX(), 2029.3889489, 1e-7);
		ensure_equals("B_1 y coordinate should match", B_1.getY(), 2140.4511000, 1e-7);
		ensure_equals("B_1 z coordinate should match", B_1.getZ(), 2449.9999995, 1e-7);

		//sigma value
		std::list<TORIEROM> meas = projTest->getCurrentNode().measurements.fORIE;
		ensure_equals("sigma should match ", meas.front().measORIE.front().target.sigmaAngl.getSignedCCValue(), 5.0, 1e-1);
		ensure_equals("sigma should match ", std::next(meas.begin(), 3)->measORIE.front().target.sigmaAngl.getSignedCCValue(), 2.0, 1e-1);
		ensure_equals("contant angle should match ", meas.front().fConstantAngle, 0.0, 1e-1);
		ensure_equals("contant angle should match ", meas.back().fConstantAngle.getGonsValue(), 2.0, 1e-1);

	}

	//----------------------------- ECTH --------------------------------//
	template<>
	template<>
	void object::test<9>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECTH measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECTH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ECTH);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector CS1 = dataset.getPoints().getObject("CS1").getEstimatedValue();
		ensure_equals("CS1 x coordinate should match", CS1.getX(), 3100.0004758, 1e-7);
		ensure_equals("CS1 y coordinate should match", CS1.getY(), 3000.3004300, 1e-7);
		ensure_equals("CS1 z coordinate should match", CS1.getZ(), 2452.8445883, 1e-7);
		TPositionVector AS2 = dataset.getPoints().getObject("AS2").getEstimatedValue();
		ensure_equals("AS2 x coordinate should match", AS2.getX(), 2985.5482162, 1e-7);
		ensure_equals("AS2 y coordinate should match", AS2.getY(), 2979.5983695, 1e-7);
		ensure_equals("AS2 z coordinate should match", AS2.getZ(), 2450.8655837, 1e-7);
		TPositionVector CS3 = dataset.getPoints().getObject("CS3").getEstimatedValue();
		ensure_equals("CS3 x coordinate should match", CS3.getX(), 2968.9084481, 1e-7);
		ensure_equals("CS3 y coordinate should match", CS3.getY(), 3095.0445340, 1e-7);
		ensure_equals("CS3 z coordinate should match", CS3.getZ(), 2452.8508447, 1e-7);
		
		//sigma value
		std::list<TECTH> meas = projTest->getCurrentNode().measurements.fTSTN.back()->roms.back()->measECTH;
        ensure_equals("sigma should match ", std::next(meas.begin(), 16)->target.sigmaD.getMMetresValue(), 2.0, 1e-1);
		ensure_equals("distance correction should match ", std::next(meas.begin(), 12)->target.sigmaDCorr, 0.1, 1e-1);
	}

	//----------------------------- ECVE --------------------------------//
	template<>
	template<>
	void object::test<10>()
	{
		tut::skip("ECVE deactivated for the moment");
		set_test_name("Testing ECVE measurement");

		std::shared_ptr<TLGCData> projTest(new TLGCData);

		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECVE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ECVE);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector D1 = dataset.getPoints().getObject("D__________________1").getEstimatedValue();
		ensure_equals("D1 x coordinate should match", D1.getX(), 2088.1677910, 1e-7);
		ensure_equals("D1 y coordinate should match", D1.getY(), 2221.3525451, 1e-7);
		ensure_equals("D1 z coordinate should match", D1.getZ(), 2449.9999955, 1e-7);
		TPositionVector G1 = dataset.getPoints().getObject("G__________________1").getEstimatedValue();
		ensure_equals("G1 x coordinate should match", G1.getX(), 2176.3356346, 1e-7);
		ensure_equals("G1 y coordinate should match", G1.getY(), 2342.7051526, 1e-7);
		ensure_equals("G1 z coordinate should match", G1.getZ(), 2449.9999823, 1e-7);
		TPositionVector J1 = dataset.getPoints().getObject("J__________________1").getEstimatedValue();
		ensure_equals("J1 x coordinate should match", J1.getX(), 2264.5034547, 1e-7);
		ensure_equals("J1 y coordinate should match", J1.getY(), 2464.0577278, 1e-7);
		ensure_equals("J1 z coordinate should match", J1.getZ(), 2449.9999603, 1e-7);

		//sigma value
		std::list<TECVEROM> meas = projTest->getCurrentNode().measurements.fECVE;
		ensure_equals("sigma should match ", meas.front().measECVE.begin()->target.sigmaD.getMMetresValue(), 0.2, 1e-1);
		ensure_equals("sigma should match ", std::next(meas.begin(), 8)->measECVE.begin()->target.sigmaD.getMMetresValue(), 0.3, 1e-1);
        ensure_equals("distance correction should match ", std::next(meas.begin(), 8)->measECVE.begin()->target.distCorrectionValue.getMetresValue(), 1.0, 1e-1);
	}

	//----------------------------- ECHO --------------------------------//
	template<>
	template<>
	void object::test<11>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECHO measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECHO.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ECHO);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector BSx = dataset.getPoints().getObject("BSx").getEstimatedValue();
		ensure_equals("BSx x coordinate should match", BSx.getX(), 3050.0003283, 2e-7);
		ensure_equals("BSx y coordinate should match", BSx.getY(), 3000.4002934, 2e-7);
		ensure_equals("BSx z coordinate should match", BSx.getZ(), 2451.8503375, 2e-7);
		TPositionVector ES240 = dataset.getPoints().getObject("ES240").getEstimatedValue();
		ensure_equals("ES240 x coordinate should match", ES240.getX(), 2970.3160461, 2e-7);
		ensure_equals("ES240 y coordinate should match", ES240.getY(), 2959.8243428, 2e-7);
		ensure_equals("ES240 z coordinate should match", ES240.getZ(), 2446.8685899, 2e-7);
		TPositionVector CS360 = dataset.getPoints().getObject("CS360").getEstimatedValue();
		ensure_equals("CS360 x coordinate should match", CS360.getX(), 2940.2550709, 5e-7);
		ensure_equals("CS360 y coordinate should match", CS360.getY(), 3080.1913342, 5e-7);
		ensure_equals("CS360 z coordinate should match", CS360.getZ(), 2452.8556750, 5e-7);

		//sigma value
		std::list<TECHOROM> meas = projTest->getCurrentNode().measurements.fECHO;
		ensure_equals("sigma should match ", std::next(meas.front().measECHO.begin(), 2)->target.sigmaD.getMMetresValue(), 0.2, 1e-1);
		ensure_equals("sigma should match ", meas.back().measECHO.back().target.sigmaD.getMMetresValue(), 0.1, 1e-1);
		ensure_equals("distance correction should match ", std::next(std::next(meas.begin(), 2)->measECHO.begin(), 2)->target.distCorrectionValue.getMetresValue(), 1.0, 1e-1);
	}


	//----------------------------- ECSP --------------------------------//
	template<>
	template<>
	void object::test<12>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECSP measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECSP.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestLgc1::ECSP);

		bool succesReading = r.readLgc1File(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector BS1 = dataset.getPoints().getObject("BS1").getEstimatedValue();
		ensure_equals("BS1 x coordinate should match", BS1.getX(), 3050.0001715, 2e-7);
		ensure_equals("BS1 y coordinate should match", BS1.getY(), 3000.3990996, 2e-7);
		ensure_equals("BS1 z coordinate should match", BS1.getZ(), 2451.8527090, 2e-7);
		TPositionVector DS2 = dataset.getPoints().getObject("DS2").getEstimatedValue();
		ensure_equals("DS2 x coordinate should match", DS2.getX(), 2985.5482256, 2e-7);
		ensure_equals("DS2 y coordinate should match", DS2.getY(), 2979.5980837, 2e-7);
		ensure_equals("DS2 z coordinate should match", DS2.getZ(), 2448.8655836, 2e-7);
		TPositionVector Z100 = dataset.getPoints().getObject("Z100").getEstimatedValue();
		ensure_equals("Z100 x coordinate should match", Z100.getX(), 3099.9999987, 5e-7);
		ensure_equals("Z100 y coordinate should match", Z100.getY(), 3000.0004569, 5e-7);
		ensure_equals("Z100 z coordinate should match", Z100.getZ(), 2449.8446312, 5e-7);

		//sigma value
		std::list<TECSPROM> meas = projTest->getCurrentNode().measurements.fECSP;
		ensure_equals("sigma should match ", meas.front().measECSP.front().target.sigmaD.getMMetresValue(), 0.2, 1e-1);
		ensure_equals("sigma should match ", meas.front().measECSP.back().target.sigmaD.getMMetresValue(), 0.5, 1e-1);
		ensure_equals("distance correction should match ", std::next(meas.front().measECSP.begin())->target.distCorrectionValue.getMetresValue(), 1.0, 1e-1);
	}
}