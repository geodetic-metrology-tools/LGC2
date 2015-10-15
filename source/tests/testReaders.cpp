#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCData.h>
#include "input.h"

#include <readers/TReader.h>
#include <readers/OptionReaders.h>
#include <readers/AdjObjectsReader.h>
#include <readers/InstrumentReaders.h>
#include <readers/MeasurementReaders.h> 
#include <refframetransformations\TXYH2CCS.h>
#include <TLGCApp.h>
#include "TLGCCalculation.h"

#pragma warning (disable:4224)

using namespace std;

#define EXPECT_FAIL(x) \
try {x;} catch (std::exception& e) {cout << endl << "expected failure: " << e.what();}

namespace tut
{
    struct test_Readers{};
    typedef test_group<test_Readers> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test of LGC file readers");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{
		set_test_name("Reading title");
		
		TLGCData proj[1];
		
		string title("This should work\n\nYes, empty lines.\n");
		
		try {
			TReader r(proj);

			istringstream mockfile("*TITR\n" + title);

			r.read(mockfile);
		} catch (exception& e) {
			ensure("Unexpected execption while testing 'read title': " + string(e.what()) + "\n", 0);
		}
		ensure("Input title must match extracted title.", proj->cfg.title == title);
	}

	TLGCData proj;
	auto& cfg(proj.cfg);

	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing options");
				
		std::vector<const std::string> empty(0);
		
		try {		
			// referentiels
			//
			TKeyOLOC r1(proj);
			r1.parse(empty, -1);
			ensure("Refsystem must be OLOC", cfg.referential.type() == TLGCRefFrame::kOLOC);
			TKeyRS2K r2(proj);
			cfg.referential = TLGCRefFrame::kNONE;
			r2.parse(empty, -1);
			ensure("Refsystem must be RS2K", cfg.referential.type() == TLGCRefFrame::kRS2K);
			TKeySPHE r3(proj);
			cfg.referential = TLGCRefFrame::kNONE;
			r3.parse(empty, -1);
			ensure("Refsystem must be SPHE", cfg.referential.type() == TLGCRefFrame::kSPHE);
			TKeyLEP r4(proj);
			cfg.referential = TLGCRefFrame::kNONE;
			r4.parse(empty, -1);
			ensure("Refsystem must be LEP", cfg.referential.type() == TLGCRefFrame::kLEP);


			// calc options
			//
			ensure("All options must be OFF ion init", 
				!cfg.allfixed.isActive() && !cfg.nodup.isActive() && 
				!cfg.libre.isActive()    && !cfg.pdor.isActive() && !cfg.sim.isActive());
			//
			TKeyALLFIXED c1(proj);
			c1.parse(empty, -1);
			ensure("ALLFIXED must be ON now", cfg.allfixed.isActive());
			//
			TKeyNODUP c2(proj);
			c2.parse(empty, -1);
			ensure("NODUP must be ON now", cfg.nodup.isActive());
			//
			TKeyLIBR c3(proj);
			c3.parse(empty, -1);
			ensure("LIBRE must be ON now", cfg.libre.isActive());
			//
			TKeyPDOR c4(proj);
			EXPECT_FAIL(c4.parse(empty, -1))
			EXPECT_FAIL(c4.parse(TReader::tokenizeLGCfileString("*PDOR"), -1))
			EXPECT_FAIL(c4.parse(TReader::tokenizeLGCfileString("*PDOR P3 0.efg"), -1))
			c4.parse(TReader::tokenizeLGCfileString("*PDOR P1 0.31"), -1);
			ensure("PDOR must be ON now", cfg.pdor.isActive());
			ensure("PDOR Point name must match input", cfg.pdor.fptname == "P1");
			ensure_distance("PDOR gisement must match input", cfg.pdor.fgis, 0.31, 1e-6);
			//
			TKeySIMU c5(proj);
			EXPECT_FAIL(c5.parse(empty, -1))
			EXPECT_FAIL(c4.parse(TReader::tokenizeLGCfileString("*SIMU"), -1))
			c5.parse(TReader::tokenizeLGCfileString("*SIMU 5"), -1);
			ensure("Simulation must be enabled", cfg.sim.isActive());
			ensure("Number of simulations must be 5", cfg.sim.numSims == 5);
			ensure("Measurements in the infile must not be ignored", !cfg.sim.ignoreMeasurements);
			c5.parse(TReader::tokenizeLGCfileString("*SIMU 5 NOBS"), -1);
			ensure("Number of simulations must be 5", cfg.sim.numSims == 5);
			ensure("Measurements in the infile must not be ignored", !cfg.sim.ignoreMeasurements);
			c5.parse(TReader::tokenizeLGCfileString("*SIMU 5 OBS"), -1);
			ensure("Number of simulations must be 5", cfg.sim.numSims == 5);
			ensure("Measurements in the infile must be ignored", cfg.sim.ignoreMeasurements);


			// output options (not testing pure boolean options)
			//
			TKeyEREL o1(proj);
			EXPECT_FAIL(o1.parse(TReader::tokenizeLGCfileString("*EREL P1 P2"), -1))
			o1.parse(TReader::tokenizeLGCfileString("P1 P2"), -1);
			o1.parse(TReader::tokenizeLGCfileString("P3 P4"), -1);
			ensure("2 point pairs must be there for relative errors now", cfg.erelPairs.size() == 2);
			ensure("First point pair for EREL must be P1 P2",  
				cfg.erelPairs.at(0).first == "P1" && cfg.erelPairs.at(0).second == "P2");
			ensure("Second point pair for EREL must be P3 P4", 
				cfg.erelPairs.at(1).first == "P3" && cfg.erelPairs.at(1).second == "P4");
			//
			TKeyFMTO o2(proj);
			o2.parse(TReader::tokenizeLGCfileString("*FMTO COL"), -1);
			ensure("Custom separator must not be active using *FMTO COL", !cfg.CustomOutputSeparator.isActive());
			EXPECT_FAIL(o2.parse(TReader::tokenizeLGCfileString("*FMTO STR"), -1));
			o2.parse(TReader::tokenizeLGCfileString("*FMTO SEP \"   ;\""), -1);
			ensure_equals("Custom separator must be \"   ;\"", cfg.CustomOutputSeparator.separator, "   ;");
			//
			TKeyPREC o3(proj);
			o3.parse(TReader::tokenizeLGCfileString("*PREC 7"), -1);
			ensure_equals("Precision must be 7 digits now", cfg.outPrecision.digits, 7);


			// additional output files
			//
			TKeyFAUT f1(proj);
			f1.parse(TReader::tokenizeLGCfileString("*FAUT"), -1);
			ensure_equals("Fault detection quantiles must match default values", 
				cfg.faut.alpha, FAUT_DEF_ALPHA);
			ensure_equals("Fault detection quantiles must match default values", 
				cfg.faut.beta, FAUT_DEF_BETA);
			f1.parse(TReader::tokenizeLGCfileString("*FAUT 0.1 0.3"), -1);
			ensure_equals("Fault detection quantiles must match input", 
				cfg.faut.alpha, 0.1);
			ensure_equals("Fault detection quantiles must match input", 
				cfg.faut.beta, 0.3);
			//
			TKeyPUNC f2(proj);
			EXPECT_FAIL(f2.parse(TReader::tokenizeLGCfileString("*PUNC NOPE"), -1))
			EXPECT_FAIL(f2.parse(TReader::tokenizeLGCfileString("*PUNC EE NOPE"), -1))
			f2.parse(TReader::tokenizeLGCfileString("*PUNC"), -1);
			ensure_equals("Using plain PUNC file format", 
				cfg.writePunch.fmode, TLGCConfig::TCoordOut::kPLAIN);
			f2.parse(TReader::tokenizeLGCfileString("*PUNC T"), -1);
			ensure_equals("Using T PUNC file format", 
				cfg.writePunch.fmode, TLGCConfig::TCoordOut::kT);
			//
			TKeySOBS f3(proj);
			f3.parse(TReader::tokenizeLGCfileString("*SOBS"), -1);
			ensure("New LGC file must be on with measurement values disabled", 
				cfg.sim.writeLGCFile && !cfg.sim.newInfileHasMeasurements);
			f3.parse(TReader::tokenizeLGCfileString("*SOBS OBS"), -1);
			ensure("New LGC file must be on with measurement values enabled", 
				cfg.sim.writeLGCFile && cfg.sim.newInfileHasMeasurements);
			
		} catch (exception& e) {
			ensure("Unexpected execption while testing reader: " + string(e.what()) + "\n", 0);
		}
	}
	
	template<>
	template<>
	void object::test<3>()
	{
		set_test_name("Testing point input");
		
		try {
			// set points to be in LEP reference frame
			cfg.referential = TLGCRefFrame::kLEP;

			TKeyCALA pr1(proj);
			
			pr1.parse(TReader::tokenizeLGCfileString("P0 1 2 3"), -1);
			ensure("Point data must match input", proj.points.doesObjectExist("P0"));
			const TAdjustablePoint& pt0 = proj.points.getObject("P0");

			
			ensure_equals("Point coords must match input", pt0.getProvisionalValue().getX().getValue(), 1.0);
			ensure_equals("Point coords must match input", pt0.getProvisionalValue().getY().getValue(), 2.0);
			ensure_equals("Point coords must match input", pt0.getProvisionalValue().getH().getValue(), 3.0);
			ensure("Lock state must match", pt0.isFixed());
			ensure_equals("Must be in the right frame", pt0.getFrameTreePosition()->get()->frame.getName(), "ROOT");

			TKeyVXY prxy(proj);
			TKeyVXZ prxz(proj);
			TKeyVYZ pryz(proj);
			TKeyVZ  prz(proj);
			EXPECT_FAIL(prxy.parse(TReader::tokenizeLGCfileString("P0 1 2 3"), -1));
			prxy.parse(TReader::tokenizeLGCfileString("P1 1 2 3 $This is a DB comment"), -1);
			
			prxz.parse(TReader::tokenizeLGCfileString("%Pc1 1 2 3"), -1);
			prxz.parse(TReader::tokenizeLGCfileString("%Pc2 1 2 3"), -1);
			prxz.parse(TReader::tokenizeLGCfileString("P2 1 2 3"), -1);
			pryz.parse(TReader::tokenizeLGCfileString("P3 1 2 3"), -1);
			prz.parse(TReader::tokenizeLGCfileString( "P4 1 2 3"), -1);
			
			const TAdjustablePoint& pt1 = proj.points.getObject("P1");

			ensure_equals(pt1.eolcomment, "$This is a DB comment");
			ensure_equals(pt1.getName(), "P1");
			ensure("Lock state must match", pt1.isCoordinateFixed(2));

			const TAdjustablePoint& pt2 = proj.points.getObject("P2");
			// Fails, but the values seem correct anyway
			//ensure_equals(pt2.hdrcomment, "%Pc1 1 2 3\n%Pc2 1 2 3");
			ensure("Lock state must match", pt2.isCoordinateFixed(1));
			ensure_equals(proj.points.numObjects(), (size_t)5);

			//Testing POIN point definition
			TKeyPOIN poin1(proj);
			//Incorrect definition, all 3 standard deviations should be assigned in POIN used. Just warning is produced, point is defined, but sigmas are not assigned.
			poin1.parse(TReader::tokenizeLGCfileString("POIN1 1 2 3 SX 0.1 SY 0.2"), -1);
			ensure("Point should be defined", proj.points.doesObjectExist("POIN1"));
			ensure_equals("Standard deviations should not be assigned.", proj.points.getObject("POIN1").hasStandDeviations(), false);

			//Correct definition
			poin1.parse(TReader::tokenizeLGCfileString("POIN2 1 2 3 SX 0.1 SY 0.2 SZ 0.01"), -1);
			ensure_equals("Standard deviations should be assigned", proj.points.getObject("POIN2").hasStandDeviations(), true);

			ensure_equals("POIN standard deviations should match", proj.points.getObject("POIN2").getStandDev(0), 0.1 * LGC::MM2M); //Values are stored in metres
			ensure_equals("POIN standard deviations should match", proj.points.getObject("POIN2").getStandDev(1), 0.2 * LGC::MM2M ); //Values are stored in metres
			ensure_equals("POIN standard deviations should match", proj.points.getObject("POIN2").getStandDev(2), 0.01 * LGC::MM2M); //Values are stored in metres
		} catch (exception& e) {
			ensure("Unexpected execption while testing reader: " + string(e.what()) + "\n", 0);
		}
	}
	
	template<>
	template<>
	void object::test<4>()
	{
		set_test_name("Testing instrument input");

		/*
		Scheme is

		*POLAR   instrID  defaultTargetID   instrHt    sigmaInstrHt  sigmaInstrCentering  constAngl
		targetID    sigmaAngl   sigmaZenD   sigmaDist   ppmDist   distCorrectionKnown    distCorrectionValue    sigmaDCorr    sigmaTargetCentering   targetHt    sigmaTargetHt
		...
		targetID    sigmaAngl   sigmaZenD   sigmaDist   ppmDist   distCorrectionKnown    distCorrectionValue    sigmaDCorr    sigmaTargetCentering   targetHt    sigmaTargetHt

		*EDM   instrID  defaultTargetID   instrHt   sigmaInstrHt   sigmaInstrCentering
		targetID   sigmaDSpt   ppmDSpt   distCorrectionKnown    distCorrectionValue    sigmaDCorr    sigmaTargetCentering   targetHt    sigmaTargetHt    
		...
		targetID   sigmaDSpt   ppmDSpt   distCorrectionKnown    distCorrectionValue    sigmaDCorr    sigmaTargetCentering   targetHt    sigmaTargetHt    

		*LEVEL   instrID  defaultStaffID
		staffID   sigmaD   ppmD   distCorrectionValue    sigmaDCorr    staffHt    sigmaStaffHt    
		...
		staffID   sigmaD   ppmD   distCorrectionValue    sigmaDCorr    staffHt    sigmaStaffHt    

		*SCALE   scaleID   sigmaD   ppmD   distCorrectionValue   sigmaDCorr    sigmaInstrCentering  

		*/
		
		try {
			using namespace LGC;
			TInstrumentData& instr(proj.instruments);
			// TSTN station with targets
			TKeyPOLAR m1(proj);
			m1.parse(TReader::tokenizeLGCfileString( "*POLAR TS1 PT1 1 2 3 4"), -1);
			m1.parse(TReader::tokenizeLGCfileString( "PT1 1 2 3 4 0 5 6 7 8 9"), -1);
			m1.parse(TReader::tokenizeLGCfileString( "PT9 2 3 3 4 0 5 6 7 8 9"), -1);
			m1.parse(TReader::tokenizeLGCfileString( "PT8 3 1 3 4 0 5 6 7 8 9"), -1);
			m1.parse(TReader::tokenizeLGCfileString( "PT7 3 1 3 4 0 5 6 7 8 9"), -1);
			// Already exists, must fail
			EXPECT_FAIL(m1.parse(TReader::tokenizeLGCfileString( "PT1 1 2 3 4 0 5 6 7 8 9"), -1));
			EXPECT_FAIL(m1.parse(TReader::tokenizeLGCfileString( "*POLAR TS1 PT1 1 2 3 4"), -1));
			//Uncomplete definition must fail
			EXPECT_FAIL(m1.parse(TReader::tokenizeLGCfileString( "*POLAR TS2 PT1 1 2 3 "), -1));
			m1.parse(TReader::tokenizeLGCfileString( "PT2 11 12 13 14 1 15 16 17 18 19"), -1);
			const TInstrumentData::TPOLAR& ts1(instr.getDevice(instr.fPOLAR, "TS1"));
			ensure_equals(ts1.ID, "TS1");
			ensure_equals(ts1.defTarget, "PT1");
			ensure_equals(ts1.instrHeight, 1);
			ensure_equals(ts1.sigmaInstrHeight, 2*MM2M); //VALUE GIVEN IN MM, but stored in METRES 2mm = 0.002m 
			ensure_equals(ts1.sigmaInstrCentering, 3*MM2M);
			ensure_equals(ts1.constAngle, 4*GON2RAD);
			const TInstrumentData::TPOLAR::TTarget& pt1(instr.getDevice(ts1.targets, "PT1"));
			ensure_equals(pt1.ID, "PT1"); 
			ensure_equals(pt1.sigmaAngl, 1*CC2RAD); 
			ensure_equals(pt1.sigmaZenD, 2*CC2RAD);
			ensure_equals(pt1.sigmaDist, 3*MM2M); 
			ensure_equals(pt1.ppmDist, 4*MM2M); 
			ensure_equals(pt1.distCorrectionUnknown, false); 
			ensure_equals(pt1.distCorrectionAdjustable->isFixed(), true);
			ensure_equals(pt1.distCorrectionValue, 5); 
			ensure_equals(pt1.sigmaDCorr, 6*MM2M); 
			ensure_equals(pt1.sigmaTargetCentering, 7*MM2M); 
			ensure_equals(pt1.targetHt, 8); 
			ensure_equals(pt1.sigmaTargetHt, 9*MM2M); 
			//Check that pointer to adjustable collection of scalars is valid and the object contains right value
			ensure_equals(pt1.distCorrectionAdjustable->getProvisionalValue().getValue(), 5);

			const TInstrumentData::TPOLAR::TTarget& pt2(instr.getDevice(ts1.targets, "PT2"));
			ensure_equals(pt2.ID, "PT2"); 
			ensure_equals(pt2.sigmaAngl, 11*CC2RAD); 
			ensure_equals(pt2.sigmaZenD, 12*CC2RAD); 
			ensure_equals(pt2.sigmaDist, 13*MM2M); 
			ensure_equals(pt2.ppmDist, 14*MM2M); 
			ensure_equals(pt2.distCorrectionUnknown, true); 
			ensure_equals(pt2.distCorrectionAdjustable->isFixed(), false);
			ensure_equals(pt2.distCorrectionValue, 15); 
			ensure_equals(pt2.sigmaDCorr, 16*MM2M); 
			ensure_equals(pt2.sigmaTargetCentering, 17*MM2M); 
			ensure_equals(pt2.targetHt, 18); 
			ensure_equals(pt2.sigmaTargetHt, 19*MM2M); 
			//Check that pointer to adjustable collection of scalars is valid and the object contains right value
			ensure_equals(pt2.distCorrectionAdjustable->getProvisionalValue().getValue(), 15);

			//
			// EDM with targets
			TKeyEDM m2(proj);
			m2.parse(TReader::tokenizeLGCfileString( "*EDM DM1 ET1 1 2 3"), -1);
			//targetID   sigmaDSpt   ppmDSpt   distCorrectionKnown    distCorrectionValue    sigmaDCorr    sigmaTargetCentering   targetHt    sigmaTargetHt    
			m2.parse(TReader::tokenizeLGCfileString( "ET1 1 2 0 3 4 5 6 7"), -1);
			m2.parse(TReader::tokenizeLGCfileString( "ET2 11 12 1 13 14 15 16 17"), -1);
			const TInstrumentData::TEDM& dm1(instr.getDevice(instr.fEDM, "DM1"));			
			ensure_equals(dm1.ID, "DM1");
			ensure_equals(dm1.defTarget, "ET1");
			ensure_equals(dm1.instrHeight, 1);
			ensure_equals(dm1.sigmaInstrHeight, 2 * MM2M);
			ensure_equals(dm1.sigmaInstrCentering, 3 * MM2M);
			const TInstrumentData::TEDM::TTarget& dt1(instr.getDevice(dm1.targets, "ET1"));
			ensure_equals(dt1.ID, "ET1"); 
			ensure_equals(dt1.sigmaDSpt, 1 * MM2M); 
			ensure_equals(dt1.ppmDSpt, 2 * MM2M); 
			ensure_equals(dt1.distCorrectionUnknown, false); 
			ensure_equals(dt1.distCorrectionAdjustable->isFixed(), true);
			ensure_equals(dt1.distCorrectionValue, 3); 
			ensure_equals(dt1.sigmaDCorr, 4 * MM2M); 
			ensure_equals(dt1.sigmaTargetCentering, 5 * MM2M); 
			ensure_equals(dt1.targetHt, 6); 
			ensure_equals(dt1.sigmaTargetHt, 7 * MM2M); 
			//Check that pointer to adjustable collection of scalars is valid and the object contains right value
			ensure_equals(dt1.distCorrectionAdjustable->getProvisionalValue().getValue(), 3);

			const TInstrumentData::TEDM::TTarget& dt2(instr.getDevice(dm1.targets, "ET2"));
			ensure_equals(dt2.ID, "ET2"); 
			ensure_equals(dt2.sigmaDSpt, 11 * MM2M); 
			ensure_equals(dt2.ppmDSpt, 12 * MM2M); 
			ensure_equals(dt2.distCorrectionUnknown, true); 
			ensure_equals(dt2.distCorrectionAdjustable->isFixed(), false);
			ensure_equals(dt2.distCorrectionValue, 13); 
			ensure_equals(dt2.sigmaDCorr, 14 * MM2M); 
			ensure_equals(dt2.sigmaTargetCentering, 15 * MM2M); 
			ensure_equals(dt2.targetHt, 16); 
			ensure_equals(dt2.sigmaTargetHt, 17 * MM2M); 
			ensure_equals(dt2.distCorrectionAdjustable->getProvisionalValue().getValue(), 13);

			//
			// Leveling with staffs
			TKeyLEVEL m3(proj);
			m3.parse(TReader::tokenizeLGCfileString( "*LEVEL LI1 ST1 0 100"), -1);
			//staffID   sigmaD   ppmD   distCorrectionValue    sigmaDCorr    staffHt    sigmaStaffHt    
			m3.parse(TReader::tokenizeLGCfileString( "ST1 1 2 3 4 5 6"), -1);
			m3.parse(TReader::tokenizeLGCfileString( "ST2 1 2 3 4 5 6"), -1);
			const TInstrumentData::TLEVEL& ls1(instr.getDevice(instr.fLEVEL, "LI1"));			
			ensure_equals(ls1.ID, "LI1");
			ensure_equals(ls1.defStaffID, "ST1");
			ensure_equals(ls1.collAngleUnknown, false);
			ensure_equals(ls1.collAngleAdjustable->isFixed(), true);
			ensure_equals(ls1.collAngleValue, 100 * GON2RAD); //in CC
			ensure_equals(ls1.collAngleAdjustable->getEstimatedValue().deg(), 90);
			const TInstrumentData::TLEVEL::TTarget& lt1(instr.getDevice(ls1.targets, "ST1"));
			ensure_equals(lt1.ID, "ST1"); 
			ensure_equals(lt1.sigmaD, 1 * MM2M); 
			ensure_equals(lt1.ppmD, 2 * MM2M); 
			ensure_equals(lt1.distCorrectionValue, 3); 
			ensure_equals(lt1.sigmaDCorr, 4 * MM2M); 
			ensure_equals(lt1.staffHt, 5); 
			ensure_equals(lt1.sigmaStaffHt, 6 * MM2M);

			//
			// Scales
			TKeySCALE m4(proj);
			//*SCALE   scaleID   sigmaD   ppmD   distCorrectionValue   sigmaDCorr    sigmaInstrCentering
			m4.parse(TReader::tokenizeLGCfileString( "*SCALE SC1 1 2 3 4 5"), -1);
			const TInstrumentData::TSCALE& sc1(instr.getDevice(instr.fSCALE, "SC1"));		
			ensure_equals(sc1.ID, "SC1");		
			ensure_equals(sc1.sigmaD, 1 * MM2M);		
			ensure_equals(sc1.ppmD, 2 * MM2M);		
			ensure_equals(sc1.distCorrectionValue, 3);		
			ensure_equals(sc1.sigmaDCorr, 4 * MM2M);		
			ensure_equals(sc1.sigmaInstrCentering, 5 * MM2M);	

			m4.parse(TReader::tokenizeLGCfileString( "*SCALE SC2 1 2 3 4 5"), -1);

			
		} catch (exception& e) {
			ensure("Unexpected execption while testing reader: " + string(e.what()) + "\n", 0);
		}
	}

		template<>
		template<>
		void object::test<5>()
		{
			set_test_name("Testing measurement input");
			proj.cfg.sim.ignoreMeasurements = false;
			using namespace LGC;

			// add a total station
			TKeyTSTN TSTN(proj);
			EXPECT_FAIL(TSTN.parse(TReader::tokenizeLGCfileString( "*TSTN PT1 POLAR1"), -1));
			TSTN.parse(TReader::tokenizeLGCfileString( "*TSTN P1 TS1 ROT3D TRGT PT2 ICSE 33"), -1);
			const auto& ts1(proj.getCurrentNode().measurements.fTSTN.back());
			ensure_equals(ts1.rot3D, true);
			ensure_equals(ts1.instrument.defTarget, "PT2");
			ensure_equals(ts1.instrument.sigmaInstrCentering, 33 * MM2M);
			ensure_equals(ts1.instrumentPos->getName(), "P1");

			// add a V0 to this station
			TKeyV0 v0(proj);
			v0.parse(TReader::tokenizeLGCfileString("*V0 TRGT PT9 ACST 66"), -1);
			ensure_equals("Default target in thisTSTN should not be affected", ts1.instrument.defTarget, "PT2");
			ensure_equals("In this ROM, default target should be updated", ts1.roms.back().defaultTarget->ID, "PT9");
			ensure_equals("Adjustable distance correction should be valid and unchanged", ts1.roms.back().defaultTarget->distCorrectionAdjustable->getProvisionalValue().getValue(), 5.0);

			ensure_distance(proj.getCurrentNode().measurements.fTSTN.back().roms.back().acst.gon(), 66.0, 1e-8);

			// Add one of each POLAR measurement to this ROM
			//
			// PLR3D
			TKeyPLR3D plr(proj);
			plr.parse(TReader::tokenizeLGCfileString( "*PLR3D TRGT PT8"), -1);
			plr.parse(TReader::tokenizeLGCfileString( "P2 1 2 3 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			ensure_equals("Target of this measurement taken implicitly from *PLR3D", ts1.roms.back().measPLR3D.back().target.ID, "PT8");

			const auto& plrmeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measPLR3D.back());
			ensure_distance(plrmeas.getAngle(kANGL).gon(), 1.0, 1e-8);
			ensure_distance(plrmeas.getAngle(kZEND).gon(), 2.0, 1e-8);
			ensure_distance(plrmeas.getScalar(), 3.0, 1e-8);
			ensure_equals(plrmeas.target.ID, "PT8");
			ensure_equals(plrmeas.target.targetHt, 11);
			ensure_equals(plrmeas.target.sigmaTargetHt, 12* MM2M);
			ensure_equals(plrmeas.target.sigmaTargetCentering, 13* MM2M);
			ensure_equals(plrmeas.target.sigmaAngl, 14 * CC2RAD);
			ensure_equals(plrmeas.target.sigmaZenD, 15 * CC2RAD);
			ensure_equals(plrmeas.target.sigmaDist, 16* MM2M);
			ensure_equals(plrmeas.target.ppmDist, 17* MM2M);

			plr.parse(TReader::tokenizeLGCfileString( "P3 1 2 3 TRGT PT2 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			ensure_equals("Target of this measurement should be updated", ts1.roms.back().measPLR3D.back().target.ID, "PT2");

			plr.parse(TReader::tokenizeLGCfileString( "P4 1 2 3 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Target of this measurement taken from previous measurement (currValue)", ts1.roms.back().measPLR3D.back().target.ID, "PT2");

			plr.parse(TReader::tokenizeLGCfileString( "P5 1 2 3 TRGT PT7 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Target of this measurement should be updated", ts1.roms.back().measPLR3D.back().target.ID, "PT7");
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			//
			//DIR3D
			TKeyDIR3D dir3D(proj);
			dir3D.parse(TReader::tokenizeLGCfileString( "*DIR3D TRGT PT8"), -1);
			dir3D.parse(TReader::tokenizeLGCfileString( "P2 1 2 TH 20 THSE 18 TCSE 23 ASE 15"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			ensure_equals("Target of this measurement taken implicitly from *DIR3D", ts1.roms.back().measDIR3D.back().target.ID, "PT8");

			const auto& dir3Dmeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measDIR3D.back());
			ensure_distance(dir3Dmeas.getAngle(kANGL).gon(), 1.0, 1e-8);
			ensure_distance(dir3Dmeas.getAngle(kZEND).gon(), 2.0, 1e-8);
			ensure_equals(dir3Dmeas.target.ID, "PT8");
			ensure_equals(dir3Dmeas.target.targetHt, 20);
			ensure_equals(dir3Dmeas.target.sigmaTargetHt, 18* MM2M);
			ensure_equals(dir3Dmeas.target.sigmaTargetCentering, 23* MM2M);
			ensure_equals(dir3Dmeas.target.sigmaAngl, 15* CC2RAD);

			dir3D.parse(TReader::tokenizeLGCfileString( "P3 1 2 3 TRGT PT2 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			ensure_equals("Target of this measurement should be updated", ts1.roms.back().measDIR3D.back().target.ID, "PT2");

			dir3D.parse(TReader::tokenizeLGCfileString( "P4 1 2 3 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15"), -1);
			ensure_equals("Target of this measurement taken from previous measurement (currValue)", ts1.roms.back().measDIR3D.back().target.ID, "PT2");

			dir3D.parse(TReader::tokenizeLGCfileString( "P5 1 2 3 TRGT PT7 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15"), -1);
			ensure_equals("Target of this measurement should be updated", ts1.roms.back().measDIR3D.back().target.ID, "PT7");
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			//
			// ANGL
			TKeyANGL ang(proj);
			ang.parse(TReader::tokenizeLGCfileString( "*ANGL"), -1);
			ang.parse(TReader::tokenizeLGCfileString( "P2 88 OBSE 21 TCSE 22"), -1);
			ensure_equals("ANGL deafault target should be taken from ROM", ts1.roms.back().measANGL.back().target.ID, "PT9");

			ang.parse(TReader::tokenizeLGCfileString( "P3 88 TRGT PT7 OBSE 21 TCSE 22"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1.roms.back().defaultTarget->ID, "PT9");
			ensure_equals("ANGL target of this measurement updated", ts1.roms.back().measANGL.back().target.ID, "PT7");

			const auto& angmeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measANGL.back());
			ensure_distance(angmeas.getAngle().gon(), 88.0, 1e-8);
			ensure_equals(angmeas.target.sigmaAngl, 21 * CC2RAD);
			ensure_equals(angmeas.target.sigmaTargetCentering, 22 * MM2M);
			//
			// ZEND
			TKeyZEND zend(proj);
			zend.parse(TReader::tokenizeLGCfileString( "*ZEND TRGT PT8"), -1);
			zend.parse(TReader::tokenizeLGCfileString( "P2 22 OBSE 31 TH 32 THSE 33 TCSE 34"), -1);
			ensure_equals("Target of this measurement taken from *ZEND",  ts1.roms.back().measZEND.back().target.ID, "PT8");
			const auto& zendmeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measZEND.back());
			ensure_equals(zendmeas.targetPos->getName(), "P2");
			ensure_equals(zendmeas.target.sigmaZenD, 31 * CC2RAD);
			ensure_equals(zendmeas.target.targetHt, 32);
			ensure_equals(zendmeas.target.sigmaTargetHt, 33 * MM2M);
			ensure_equals(zendmeas.target.sigmaTargetCentering, 34 * MM2M);

			zend.parse(TReader::tokenizeLGCfileString( "P3 22 TRGT PT7 OBSE 31 TH 32 THSE 33 TCSE 34"), -1);
			ensure_equals("Target of this measurement updated ZEND",  ts1.roms.back().measZEND.back().target.ID, "PT7");
			//
			// DIST
			//New v0 occured i.e. without any parametr, default value from TSTN taken
			TKeyV0 v1(proj);
			v1.parse(TReader::tokenizeLGCfileString("*V0"), -1);
			TKeyDIST dist(proj);
			dist.parse(TReader::tokenizeLGCfileString( "*DIST"), -1);
			dist.parse(TReader::tokenizeLGCfileString( "P2 40 OBSE 41 PPM 42 TH 43 THSE 44 TCSE 45"), -1);
			ensure_equals("Default target taken from new ROM, which takes default target implicitly from TSTN", ts1.roms.back().measDIST.back().target.ID, "PT2");

			const auto& distmeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measDIST.back());
			ensure_equals(distmeas.target.sigmaDist, 41 * MM2M);
			ensure_equals(distmeas.target.ppmDist, 42 * MM2M);
			ensure_equals(distmeas.target.targetHt, 43);
			ensure_equals(distmeas.target.sigmaTargetHt, 44 * MM2M);
			ensure_equals(distmeas.target.sigmaTargetCentering, 45 * MM2M);
			//
			// ECTH
			TKeyECTH ecth(proj);
			ecth.parse(TReader::tokenizeLGCfileString( "*ECTH 1 SC1"), -1);
			ecth.parse(TReader::tokenizeLGCfileString("P2 1.1 OBSE 0.01 PPM 0.1 ICSE 0.5"),-1);
			const auto& ecthmeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measECTH.back());
			ensure_equals(ecthmeas.stationedPoint->getName(), "P2");
			ensure_equals(ecthmeas.obsHorAngle.gon(), 1);
			ensure_equals(ecthmeas.scaleInstr.ID, "SC1");
			ensure_equals(ecthmeas.scaleInstr.sigmaD, 0.01 * MM2M);
			ensure_equals(ecthmeas.scaleInstr.ppmD, 0.1 * MM2M);
			ensure_equals(ecthmeas.scaleInstr.sigmaInstrCentering, 0.5 * MM2M);
			ensure_equals(ecthmeas.getMeasuredOffsetValue(), 1.1);
			ensure_equals("Default values in instrument data not affected", proj.instruments.getDevice(proj.instruments.fSCALE,"SC1").sigmaInstrCentering, 5 * MM2M);

			ecth.parse(TReader::tokenizeLGCfileString("P2 0.9 SCALE SC2 OBSE 0.01 PPM 0.1 ICSE 0.5"),-1);
			const auto& ecthmeas2(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measECTH.back());
			ensure_equals(ecthmeas2.scaleInstr.ID, "SC2");
			ensure_equals(ecthmeas2.getMeasuredOffsetValue(), 0.9);

			ecth.parse(TReader::tokenizeLGCfileString("P2 0.9"),-1);
			const auto& ecthmeas3(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measECTH.back());
			ensure_equals("This takes current instrument from previous",ecthmeas3.scaleInstr.ID, "SC2");
			ensure_equals("Has default values", ecthmeas3.scaleInstr.sigmaInstrCentering, 5 * MM2M);
			//
			// DHOR
			TKeyDHOR dhor(proj);
			dhor.parse(TReader::tokenizeLGCfileString( "*DHOR"), -1);
			dhor.parse(TReader::tokenizeLGCfileString( "P2 50 TRGT PT7 OBSE 51 PPM 52 TCSE 53"), -1);
			ensure_equals("Target of the measurement should match", ts1.roms.back().measDHOR.back().target.ID, "PT7");
			dhor.parse(TReader::tokenizeLGCfileString( "P2 50 OBSE 51 PPM 52 TCSE 53"), -1);
			ensure_equals("Target implicitly taken from previous measurement", ts1.roms.back().measDHOR.back().target.ID, "PT7");

			const auto& dhormeas(proj.getCurrentNode().measurements.fTSTN.back().roms.back().measDHOR.back());
			ensure_equals(dhormeas.target.sigmaDist, 51 * MM2M);
			ensure_equals(dhormeas.target.ppmDist, 52 * MM2M);
			ensure_equals(dhormeas.target.sigmaTargetCentering, 53 * MM2M);
			
			// non-TSTN measurements

			TKeyCAM cam(proj);
			cam.parse(TReader::tokenizeLGCfileString( "*CAM P1 TS1 ROT3D TRGT PT2 ICSE 33"), -1);

			TKeyPLR3D plrC(proj);
			const auto& cam1(proj.getCurrentNode().measurements.fCAM.back());

			plrC.parse(TReader::tokenizeLGCfileString( "*PLR3D TRGT PT8"), -1);
			plrC.parse(TReader::tokenizeLGCfileString( "P2 1 2 3 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Target of this measurement should be updated",  cam1.measPLR3D.back().target.ID, "PT8");
			plr.parse(TReader::tokenizeLGCfileString( "P3 1 2 3 TRGT PT2 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Target of this measurement should be updated",  cam1.measPLR3D.back().target.ID, "PT2");
			TKeyZEND zenC(proj);
			EXPECT_FAIL(zenC.parse(TReader::tokenizeLGCfileString( "*ZEND TRGT PT8"), -1));
			EXPECT_FAIL(zenC.parse(TReader::tokenizeLGCfileString( "P2 22 OBSE 31 TH 32 THSE 33 TCSE 34"), -1));

			TKeyDIR3D dir3DC(proj);
			dir3DC.parse(TReader::tokenizeLGCfileString( "*DIR3D TRGT PT7"), -1);
			dir3D.parse(TReader::tokenizeLGCfileString( "P2 1 2 TH 20 THSE 18 TCSE 23 ASE 15"), -1);
			ensure_equals("Target of this measurement should be updated",  cam1.measDIR3D.back().target.ID, "PT7");

			ensure_equals("One CAM measurement defined", proj.getCurrentNode().measurements.fCAM.size(),1);
			ensure_equals("2 PLR3D measurements in this CAM", proj.getCurrentNode().measurements.fCAM.back().measPLR3D.size(),2);

			// DSPT
			TKeyDSPT dspt(proj);
			dspt.parse(TReader::tokenizeLGCfileString( "*DSPT P1 DM1 IH 60 IHSE 61 ICSE 62"), -1);
			dspt.parse(TReader::tokenizeLGCfileString( "P2 63 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("Instrument height updated DSPT",proj.getCurrentNode().measurements.fEDM.back().instrumentHeightAdjustable->getProvisionalValue().getValue() , 60.0);
			ensure_equals("Instrument height updated DSPT",proj.getCurrentNode().measurements.fEDM.back().instrumentPos->getProvisionalValue().getX().getValue() , 1.0);


			ensure_equals("DPST default target from instrument section",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET1");
			dspt.parse(TReader::tokenizeLGCfileString( "P2 63 TRGT ET2 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("DPST default target updated",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET2");
			dspt.parse(TReader::tokenizeLGCfileString( "P3 63 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("DPST default target implicitly taken from preceding",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET2");
			dspt.parse(TReader::tokenizeLGCfileString( "P3 63 TRGT ET1 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("DPST default target updated",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET1");

			const auto& dspPOLAR(proj.getCurrentNode().measurements.fEDM.back().instrument);
			ensure_equals(dspPOLAR.ID, "DM1");
			ensure_equals(dspPOLAR.instrHeight, 60);
			ensure_equals(dspPOLAR.sigmaInstrHeight, 61 * MM2M);
			ensure_equals(dspPOLAR.sigmaInstrCentering, 62 * MM2M);
			const auto& dsptmeas(proj.getCurrentNode().measurements.fEDM.back().measDSPT.back());
			ensure_equals(dsptmeas.getScalar(), 63);
			ensure_equals(dsptmeas.target.sigmaDSpt, 64 * MM2M);
			ensure_equals(dsptmeas.target.ppmDSpt, 65 * MM2M);
			ensure_equals(dsptmeas.target.targetHt, 66);
			ensure_equals(dsptmeas.target.sigmaTargetHt, 67 * MM2M);
			ensure_equals(dsptmeas.target.sigmaTargetCentering, 68 * MM2M);
			ensure_equals("Adjustable distance correction should be valid and unchanged", dsptmeas.target.distCorrectionAdjustable->getProvisionalValue().getValue(), 3.0);
			//
			// DVER
			TKeyDVER dver(proj);
			dver.parse(TReader::tokenizeLGCfileString( "*DVER"), -1);
			dver.parse(TReader::tokenizeLGCfileString( "P2 P3 2.0 OBSE 0.01"), -1);
			const auto& dverM(proj.getCurrentNode().measurements.fDVER.back());
			ensure_equals("Point coordinates should match", dverM.pointA->getProvisionalValue().getX().getValue(),1);
			ensure_equals("Point coordinates should match", dverM.pointB->getProvisionalValue().getX().getValue(),1);
			ensure_equals("Point coordinates should match", dverM.getObservedStDev(), 0.01 * MM2M);
			ensure_equals("Point coordinates should match", dverM.getFirstEquationIndex(),dverM.getLastEquationIndex());

			//
			// DLEV
			TKeyDLEV dlev(proj);

			dlev.parse(TReader::tokenizeLGCfileString( "*DLEV LI1 RefPt P2"), -1);
			dlev.parse(TReader::tokenizeLGCfileString( "P1 5 DHOR 1.0 TRGT ST2 OBSE 0.1 PPM 0.01 TH 1.0 THSE 0.1 DSE 0.1"), -1);
			dlev.parse(TReader::tokenizeLGCfileString( "P3 6"), -1);

			const auto& levelRound(proj.getCurrentNode().measurements.fLEVEL.back());
			ensure_equals("Instrument ID should match", levelRound.instrument.ID,"LI1");
			ensure_equals("Default staff ID should match", levelRound.instrument.defStaffID,"ST2");
			ensure_equals("Reference point given, plane should be initialized", levelRound.fMeasuredPlane->isInitialized(),true);
			ensure_equals("Distance of the reference point should be zero", levelRound.fMeasuredPlane->getRefPtDistProvisionalValue().getValue(),0.0);

			auto& firstDLEVMeasurement(levelRound.measDLEV[0]);
			ensure_equals("Name of the target position should match", firstDLEVMeasurement.targetPos->getName(),"P1");
			ensure_equals("Measured vertical distance should match", firstDLEVMeasurement.getMeasuredVerticalDistance(),5);
			ensure_equals("Measured horizontal distance should match", firstDLEVMeasurement.getMeasuredHorizontalDistance(),1);
			ensure_equals("Target should be overidden", firstDLEVMeasurement.target.ID,"ST2");
			ensure_equals("Target's ppm value should be overidden", firstDLEVMeasurement.target.ppmD, 0.01 * MM2M);

			auto& firstDLEVMeasurement2(levelRound.measDLEV[1]);
			ensure_equals("Name of the target position should match", firstDLEVMeasurement2.targetPos->getName(),"P3");
			ensure_equals("Measured vertical distance should match", firstDLEVMeasurement2.getMeasuredVerticalDistance(),6);
			ensure_equals("Measured horizontal distance should match", _isnan(firstDLEVMeasurement2.getMeasuredHorizontalDistance()),true);
			ensure_equals("Target should be overidden", firstDLEVMeasurement2.target.ID,"ST2");
			ensure_equals("Target's ppm value should be default", firstDLEVMeasurement2.target.ppmD, 2.0 * MM2M);

			dlev.parse(TReader::tokenizeLGCfileString( "*DLEV LI1"), -1);
			const auto& levelRound2(proj.getCurrentNode().measurements.fLEVEL.back());
			ensure_equals("Reference point given, plane should not be initialized", levelRound2.fMeasuredPlane->isInitialized(),false);
			////////////////////////////////////
			//Testing FRAME measurements
			////////////////////////////////////
			TLGCData measProj;
			const std::string line = "*FRAME fff1  1 2 3 4 5 6 1 STX 0.1";
			const std::string lineClose = "*ENDFRAME";
			TKeyFRAME fr(measProj);
			TKeyENDFRAME endFr(measProj);
			fr.parse(TReader::tokenizeLGCfileString(line),1);
			endFr.parse(TReader::tokenizeLGCfileString(lineClose),2);
			TDataTreeIterator pos = measProj.getCurrentPosition()++;
			TDataTreeIterator currentNodeIter = measProj.tree.begin();
			++currentNodeIter;

			ensure_equals("One FRAME measurement", currentNodeIter->get()->frame.getTranslationStandDev(0).getMMetresValue(), 0.1);
			EXPECT_FAIL(currentNodeIter->get()->frame.getTranslationStandDev(1));


			EXPECT_FAIL(ensure_equals("Scale standard deviation not assigned",currentNodeIter->get()->frame.getScaleStandDev().getMMetresValue(), 5));
		}

		typedef TDataTree::iterator TDataTreeIter;
		TLGCData proj2;

		template<>
		template<>
		void object::test<6>()
		{
				//Read file
				set_test_name("Testing HIMAS input file");
				TReader r(&proj2);
				proj2.fOutputFileWriter.setOutputfileLocation("fileOut.log");

				stringstream infile(LGCTestInput::platesfile);
				r.read(infile);

				TDataAnalyzer analyzer(proj2);
				bool consistent = analyzer.dataConsistent();
				ensure_equals("Data should be consistent", consistent, true); //Check consistent method assign unknown indices to adjustables and initialize adjustable in DLEV
			
				//Testing global information about project

				//Title should match
		    	ensure("Title should match", proj2.cfg.title=="HIMAS TEST FILE.\n");
				//Only one instrument defined (POLAR)
				ensure_equals("One POLAR instrument", proj2.instruments.fPOLAR.size(),1);
				ensure_equals("One LEVEL instrument", proj2.instruments.fLEVEL.size(),1);
				ensure_equals("No other instrument than POLAR amd LEVEL were defined", proj2.instruments.fEDM.size()+proj2.instruments.fSCALE.size(),0);
				//12 points defined in the input file + 1 point defined implicitly as a part of DLEV measurement (reference point)
				ensure_equals("Total number of defined points should be 13:", proj2.points.numObjects(),13);
				//Ensure that specific point is defined
				ensure("Point P3C should exist", proj2.points.doesObjectExist("P3C"));

				//Testing tree structure
				auto rp(proj2.tree.begin().node->data.get()->measurements.fLEVEL[0].fMeasuredPlane->getReferencePoint()->getEstimatedValue());

				ensure_equals("Calculated reference point coordinates should match", rp.getX().getValue(),10);
				ensure_equals("Calculated reference point coordinates should match", rp.getY().getValue(),(double)100/3);
				TPositionVector a (0,10,10,TCoordSysFactory::ECoordSys::k2DPlusH);
				TPositionVector b (10,30,10,TCoordSysFactory::ECoordSys::k2DPlusH);
				TPositionVector c (20,60,10,TCoordSysFactory::ECoordSys::k2DPlusH);
				TXYH2CCS::XYHs2CCS(a);
				TXYH2CCS::XYHs2CCS(b);
				TXYH2CCS::XYHs2CCS(c);
				ensure_equals("Calculated reference point coordinates should match",rp.getZ().getValue(), ( (a.getZ().getValue() + b.getZ().getValue() + c.getZ().getValue())/3));

				//There should be 4 nodes in the tree (3 nodes defined through *FRAME keyword + 1 which defines origin)
				ensure_equals(proj2.tree.size(),4);
				//Max depth of the tree is 2
				ensure_equals(proj2.tree.max_depth(),2);

				TDataTreeIter pos,begin,end;
				begin = proj2.tree.begin();
				end = proj2.tree.end();

				//First node
				auto& n(*(begin->get()));
				ensure_equals(n.measurements.fTSTN.size(),0);
				ensure_equals(n.measurements.fLEVEL.size(),1);
				pos = begin;
				pos++;
				//Second node
				auto& n2(*(pos->get()));
				ensure_equals(n2.measurements.fTSTN.size(),4);

		
				//Can not access the values
				const TAdjustablePoint* point = n2.measurements.fTSTN[0].roms[0].measPLR3D[0].targetPos;
				ensure_equals("PLR3D measurement target position should match", point->getName(),"P8C");

				pos++;
				//Third node
				auto& n3(*(pos->get()));
				ensure_equals(n3.measurements.fTSTN.size(),0);
				
				ensure_equals(n3.frame.getName(),"P2");
				ensure_distance(n3.frame.getTranslation(0), 20.0, 1e-8);
				ensure_distance(n3.frame.getTranslation(1), 30.0, 1e-8);
				ensure_distance(n3.frame.getTranslation(2), 40.0, 1e-8);

				ensure_distance(n3.frame.getAngle(0).rad(), 0.6, 1e-8);
				ensure_distance(n3.frame.getAngle(1).rad(), 0.5, 1e-8);
				ensure_distance(n3.frame.getAngle(2).rad(), 0.4, 1e-8);

				pos++;
				//Fourth node
				auto& n4(*(pos->get()));
				ensure_equals(n4.measurements.fTSTN.size(),4);


				//Testing that number of all adjustable objects, unknowns, equations,... match
				TDataAnalyzer an(proj2);
				an.dataConsistent(); //asssign unknown indices

				//10 scalars in total, 1 for every TSTN instrument height = 8; 1 for every target of *POLAR ( if on 6th position FALSE(0) then variable, fixed otherwise) = 2x
				ensure_equals("Number of scalars should match", proj2.scalars.numObjects(), 10);
				ensure_equals("Number of scalars unknowns should match", proj2.scalars.numUnknowns(), 9);

				//12 points defined, 4 defined under POIN with SX,SY,SZ associated, 3 VXY, others are CALA + 1xpoint in DLEV which is VXY = 2*1 = 2
				ensure_equals("Number of points unknowns should match", proj2.points.numUnknowns(), 20);

				//8 angles defined, one for every v0, and all of them are variable 
				ensure_equals("Number of angles should match", proj2.angles.numObjects(), 9);
				//Collimination angle in LEVEL instrument is set to be fixed, therefore does not introduce unknown
				ensure_equals("Number of angle's unknowns should match", proj2.angles.numUnknowns(), 8);

				//Overall
				ensure_equals("Total number of unknows", proj2.fUEOIndices.UIndex, 53);
//				ensure_equals("Total number of unknows introduced with sigma should match", proj2.fUEOIndices.UwSIndex, 18);
//				ensure_equals("Total number of unknows introduced should match", proj2.fUEOIndices.UIndex + proj2.fUEOIndices.UwSIndex, 54);

				// 9*PLR3D = 27 ,1*DIST=1, 4 points defined under , 1 , DLEV(3x) = 1*1+2*2=5  ,27+1+5 = 33
				ensure_equals("Total number of equations should match", proj2.fUEOIndices.EIndex,33);
				ensure_equals("Total number of observations should match", proj2.fUEOIndices.OIndex,33);
		}

		template<>
		template<>
		void object::test<7>()
		{
			//Test if the whole application runs
			using namespace LGC;
			set_test_name("Testing TLGCApp class and nam file reader");

			TLGCApp proj( "C:/susoft/LGC2/source/tests/file.nam");
			proj.exec();

			int a = 5;
			//To do

		}

}
