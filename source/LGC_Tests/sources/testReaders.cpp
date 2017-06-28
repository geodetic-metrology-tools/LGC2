#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCData.h>
#include <TReader.h>
#include <OptionReaders.h>
#include <AdjObjectsReader.h>
#include <InstrumentReaders.h>
#include <MeasurementReaders.h> 
#include <TLGCApp.h>
#include "TLGCCalculation.h"

#include <StringManager.h>
#include <TXYH2CCS.h>

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
		
		std::shared_ptr<TLGCData> projShared (new TLGCData);
		
		string title("My title");
		
		try {
			TReader r(projShared);

			istringstream mockfile("*TITR\n" + title + "\n" + "*OLOC");

			r.read(mockfile,mockfile);
		} catch (exception& e) {
			ensure("Unexpected execption while testing 'read title': " + string(e.what()) + "\n", 0);
		}
		ensure("Input title must match extracted title.", projShared->getConfig().title == title);
	}

	TLGCData proj;
	auto& cfg(proj.getConfig());

	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing options");
				
		std::vector<std::string> empty(0);
		
		try {		
			// referentiels
			//
			TKeyOLOC r1(proj);

			r1.parse(empty, -1);
			ensure("Refsystem must be OLOC", cfg.referential == TRefSystemFactory::ERefFrame::kLocalRefFrame);
			TKeyRS2K r2(proj);
			cfg.referential = TRefSystemFactory::ERefFrame::kNotInGraph;
			r2.parse(empty, -1);
			ensure("Refsystem must be RS2K", cfg.referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine);
			TKeySPHE r3(proj);
			cfg.referential = TRefSystemFactory::ERefFrame::kNotInGraph;
			r3.parse(empty, -1);
			ensure("Refsystem must be SPHE", cfg.referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS);
			TKeyLEP r4(proj);
			cfg.referential = TRefSystemFactory::ERefFrame::kNotInGraph;
			r4.parse(empty, -1);
			ensure("Refsystem must be LEP", cfg.referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine);


			// calc options
			//
			ensure("All options must be OFF ion init", 
				!cfg.allfixed.isActive() && !cfg.nodup.isActive() && 
				!cfg.libre.isActive()    && !cfg.pdor.isActive() && !cfg.sim.isActive());
			//
			TKeyALLFIXED c1(proj);
			c1.parse(empty, -1);
			ensure("ALLFIXED must be ON now", cfg.allfixed.isActive());

			TPositionVector position2(1.0, 2.0, 3.0, TCoordSysFactory::ECoordSys::k2DPlusH);
			TDataTreeIterator iter;
			LGCAdjustablePoint pH(position2, false, false, true, "pointH1", TRefSystemFactory::ERefFrame::kCernXYHg00Machine, iter);
			// point should be fixed
			ensure_equals("0 unknowns introduced", pH.getNumUnkn(), 0);
			ensure_equals("Point is  fixed", pH.isFixed(), true);
			ensure_equals("Name should match", pH.getName(), "pointH1");

			//Need to re initialise allfixed parameter
            cfg.allfixed = TLGCConfig::TBinaryOption(false);
			LGCAdjustablePoint::setAllFixedParam(False);

			//
			TKeyLIBR c3(proj);
			c3.parse(empty, -1);
			ensure("LIBRE must be ON now", cfg.libre.isActive());
			//
			TKeyPDOR c4(proj);
			EXPECT_FAIL(c4.parse(empty, -1))
			EXPECT_FAIL(c4.parse(tokenizefileString("*PDOR"), -1))
			EXPECT_FAIL(c4.parse(tokenizefileString("*PDOR P3 0.efg"), -1))
			c4.parse(tokenizefileString("P1 0.31"), -1);
			ensure("PDOR must be ON now", cfg.pdor.isActive());
			ensure("PDOR Point name must match input", cfg.pdor.fptname == "P1");
			ensure_equals("PDOR gisement must match input", cfg.pdor.fgis.getGonsValue(), 0.31, 1e-6);
			
     

			// output options (not testing pure boolean options)
			//
			TKeyEREL o1(proj);
			EXPECT_FAIL(o1.parse(tokenizefileString("*EREL P1 P2"), -1))
			o1.parse(tokenizefileString("P1 P2"), -1);
			o1.parse(tokenizefileString("P3 P4"), -1);
			ensure("2 point pairs must be there for relative errors now", cfg.erelPairs.size() == 2);
			ensure("First point pair for EREL must be P1 P2",  
				cfg.erelPairs.at(0).first == "P1" && cfg.erelPairs.at(0).second == "P2");
			ensure("Second point pair for EREL must be P3 P4", 
				cfg.erelPairs.at(1).first == "P3" && cfg.erelPairs.at(1).second == "P4");
			//
			TKeyFMTO o2(proj);
			o2.parse(tokenizefileString("*FMTO COL"), -1);
			ensure("Custom separator must not be active using *FMTO COL", !cfg.CustomOutputSeparator.isActive());
			EXPECT_FAIL(o2.parse(tokenizefileString("*FMTO STR"), -1));
			o2.parse(tokenizefileString("*FMTO SEP \"   ;\""), -1);
			ensure_equals("Custom separator must be \"   ;\"", cfg.CustomOutputSeparator.separator, "   ;");
			//
			TKeyPREC o3(proj);
			o3.parse(tokenizefileString("*PREC 7"), -1);
			ensure_equals("Precision must be 7 digits now", cfg.outPrecision.digits, 7);
			ensure_equals("Convergence criteria must be 5e-8", cfg.outPrecision.convCrit, 5e-8);
			TKeyPREC o4(proj);
			EXPECT_FAIL(o4.parse(tokenizefileString("*PREC 8"), -1));
			ensure_equals("Precision must be at least 7 digits, we expect an error", cfg.outPrecision.digits, 7);
			//
			TKeyPRES o5(proj);
			o5.parse(tokenizefileString("*PRES"), -1);
			ensure("Ellips error, must match input", cfg.errorEllipses.isActive());


			// additional output files
			//
			TKeyFAUT f1(proj);
			f1.parse(tokenizefileString("*FAUT"), -1);
			ensure_equals("Fault detection quantiles must match default values", 
				cfg.faut.alpha, 0.01);
			ensure_equals("Fault detection quantiles must match default values", 
				cfg.faut.beta, 0.1);
			f1.parse(tokenizefileString("*FAUT 0.1 0.3"), -1);
			ensure_equals("Fault detection quantiles must match input", 
				cfg.faut.alpha, 0.1);
			ensure_equals("Fault detection quantiles must match input", 
				cfg.faut.beta, 0.3);
			//
			TKeyPUNC f2(proj);
			EXPECT_FAIL(f2.parse(tokenizefileString("*PUNC NOPE"), -1))
			EXPECT_FAIL(f2.parse(tokenizefileString("*PUNC EE NOPE"), -1))
			f2.parse(tokenizefileString("*PUNC"), -1);
			ensure_equals("Using plain PUNC file format", 
				cfg.writePunch.fmode, TLGCConfig::TCoordOut::kPLAIN);
			f2.parse(tokenizefileString("*PUNC T"), -1);
			ensure_equals("Using T PUNC file format", 
				cfg.writePunch.fmode, TLGCConfig::TCoordOut::kT);
			//
			TKeySOBS f3(proj);
			f3.parse(tokenizefileString("*SOBS"), -1);
			ensure("New LGC file must be on with measurement values disabled", cfg.sim.writeLGCFile);
			
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
			cfg.referential = TRefSystemFactory::ERefFrame::kCernXYHg85Machine;

			TKeyCALA pr1(proj);
			
			pr1.parse(tokenizefileString("P0 1 2 3"), -1);
			ensure("Point data must match input", proj.getPoints().doesObjectExist("P0"));
			const LGCAdjustablePoint& pt0 = proj.getPoints().getObject("P0");

			
			ensure_equals("Point coords must match input", pt0.getProvisionalValue().getX().getMetresValue(), 1.0);
			ensure_equals("Point coords must match input", pt0.getProvisionalValue().getY().getMetresValue(), 2.0);
			ensure_equals("Point coords must match input", pt0.getProvisionalValue().getH().getMetresValue(), 3.0);
			ensure("Lock state must match", pt0.isFixed());
			ensure_equals("Must be in the right frame", pt0.getFrameTreePosition()->get()->frame.getName(), "ROOT");

			TKeyVXY prxy(proj);
			TKeyVXZ prxz(proj);
			TKeyVYZ pryz(proj);
			TKeyVZ  prz(proj);
			EXPECT_FAIL(prxy.parse(tokenizefileString("P0 1 2 3"), -1));
			prxy.parse(tokenizefileString("P1 1 2 3 $This is a DB comment"), -1);
			
			prxz.parse(tokenizefileString("%Pc1 1 2 3"), -1);
			prxz.parse(tokenizefileString("%Pc2 1 2 3"), -1);
			prxz.parse(tokenizefileString("P2 1 2 3"), -1);
			pryz.parse(tokenizefileString("P3 1 2 3"), -1);
			prz.parse(tokenizefileString( "P4 1 2 3"), -1);
			
			const LGCAdjustablePoint& pt1 = proj.getPoints().getObject("P1");

			ensure_equals(pt1.eolcomment, "$This is a DB comment");
			ensure_equals(pt1.getName(), "P1");
			ensure("Lock state must match", pt1.isCoordinateFixed(2));

			const LGCAdjustablePoint& pt2 = proj.getPoints().getObject("P2");
			// Fails, but the values seem correct anyway
			//ensure_equals(pt2.hdrcomment, "%Pc1 1 2 3\n%Pc2 1 2 3");
			ensure("Lock state must match", pt2.isCoordinateFixed(1));
			ensure_equals(proj.getPoints().numObjects(), (size_t)5);

			//Testing POIN point definition
			TKeyPOIN poin1(proj);
			//Incorrect definition, all 3 standard deviations should be assigned in POIN used. Just warning is produced, point is defined, but sigmas are not assigned.
			poin1.parse(tokenizefileString("POIN1 1 2 3 SX 0.1 SY 0.2"), -1);
			ensure("Point should be defined", proj.getPoints().doesObjectExist("POIN1"));
			ensure_equals("Standard deviations should not be assigned.", proj.getPoints().getObject("POIN1").hasStandDeviations(), false);

			//Correct definition
			poin1.parse(tokenizefileString("POIN2 1 2 3 SX 0.1 SY 0.2 SZ 0.01"), -1);
			ensure_equals("Standard deviations should be assigned", proj.getPoints().getObject("POIN2").hasStandDeviations(), true);

			ensure_equals("POIN standard deviations should match", proj.getPoints().getObject("POIN2").getStandDev(0), 0.1 * MM2M); //Values are stored in metres
			ensure_equals("POIN standard deviations should match", proj.getPoints().getObject("POIN2").getStandDev(1), 0.2 * MM2M ); //Values are stored in metres
			ensure_equals("POIN standard deviations should match", proj.getPoints().getObject("POIN2").getStandDev(2), 0.01 * MM2M); //Values are stored in metres
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
			TInstrumentData& instr(proj.getInstruments());
			// TSTN station with targets
			TKeyPOLAR m1(proj);
			m1.parse(tokenizefileString( "*POLAR TS1 PT1 1 2 3 4"), -1);
			m1.parse(tokenizefileString( "PT1 1 2 3 4 0 5 6 7 8 9"), -1);
			m1.parse(tokenizefileString( "PT9 2 3 3 4 0 5 6 7 8 9"), -1);
			m1.parse(tokenizefileString( "PT8 3 1 3 4 0 5 6 7 8 9"), -1);
			m1.parse(tokenizefileString( "PT7 3 1 3 4 0 5 6 7 8 9"), -1);
			// Already exists, must fail
			EXPECT_FAIL(m1.parse(tokenizefileString( "PT1 1 2 3 4 0 5 6 7 8 9"), -1));
			EXPECT_FAIL(m1.parse(tokenizefileString( "*POLAR TS1 PT1 1 2 3 4"), -1));
			//Uncomplete definition must fail
			EXPECT_FAIL(m1.parse(tokenizefileString( "*POLAR TS2 PT1 1 2 3 "), -1));
			m1.parse(tokenizefileString( "PT2 11 12 13 14 1 15 16 17 18 19"), -1);
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
			ensure_equals(pt1.distCorrectionAdjustable->getProvisionalValue(), 5);

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
			ensure_equals(pt2.distCorrectionAdjustable->getProvisionalValue(), 15);

			//
			// EDM with targets
			TKeyEDM m2(proj);
			m2.parse(tokenizefileString( "*EDM DM1 ET1 1 2 3"), -1);
			//targetID   sigmaDSpt   ppmDSpt   distCorrectionKnown    distCorrectionValue    sigmaDCorr    sigmaTargetCentering   targetHt    sigmaTargetHt    
			m2.parse(tokenizefileString( "ET1 1 2 0 3 4 5 6 7"), -1);
			m2.parse(tokenizefileString( "ET2 11 12 1 13 14 15 16 17"), -1);
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
			ensure_equals(dt1.distCorrectionAdjustable->getProvisionalValue(), 3);

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
			ensure_equals(dt2.distCorrectionAdjustable->getProvisionalValue(), 13);

			//
			// Leveling with staffs
			TKeyLEVEL m3(proj);
			m3.parse(tokenizefileString( "*LEVEL LI1 ST1 0 100"), -1);
			//staffID   sigmaD   ppmD   distCorrectionValue    sigmaDCorr    staffHt    sigmaStaffHt    
			m3.parse(tokenizefileString( "ST1 1 2 3 4 5 6"), -1);
			m3.parse(tokenizefileString( "ST2 1 2 3 4 5 6"), -1);
			const TInstrumentData::TLEVEL& ls1(instr.getDevice(instr.fLEVEL, "LI1"));			
			ensure_equals(ls1.ID, "LI1");
			ensure_equals(ls1.defStaffID, "ST1");
			ensure_equals(ls1.collAngleUnknown, false);
			ensure_equals(ls1.collAngleAdjustable->isFixed(), true);
			ensure_equals(ls1.collAngleValue, 100 * GON2RAD); //in CC
			ensure_equals(ls1.collAngleAdjustable->getEstimatedValue().getDegreesValue(), 90);
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
			m4.parse(tokenizefileString( "*SCALE SC1 1 2 3 4 5"), -1);
			const TInstrumentData::TSCALE& sc1(instr.getDevice(instr.fSCALE, "SC1"));		
			ensure_equals(sc1.ID, "SC1");		
			ensure_equals(sc1.sigmaD, 1 * MM2M);		
			ensure_equals(sc1.ppmD, 2 * MM2M);		
			ensure_equals(sc1.distCorrectionValue, 3);		
			ensure_equals(sc1.sigmaDCorr, 4 * MM2M);		
			ensure_equals(sc1.sigmaInstrCentering, 5 * MM2M);	

			m4.parse(tokenizefileString( "*SCALE SC2 1 2 3 4 5"), -1);

			
		} catch (exception& e) {
			ensure("Unexpected execption while testing reader: " + string(e.what()) + "\n", 0);
		}
	}

	template<>
	template<>
	void object::test<5>()
		{
			set_test_name("Testing measurement input");
			using namespace LGC;
			TInstrumentData& instr(proj.getInstruments());

			// add a total station
			TKeyTSTN tstn(proj);
			EXPECT_FAIL(tstn.parse(tokenizefileString( "*TSTN PT1 POLAR1"), -1));
			tstn.parse(tokenizefileString( "*TSTN P1 TS1 ROT3D TRGT PT2 ICSE 33"), -1);
			const auto& ts1(proj.getCurrentNode().measurements.fTSTN.back());
			ensure_equals(ts1->rot3D, true);
			ensure_equals(ts1->instrument.defTarget, "PT2");
			ensure_equals(ts1->instrument.sigmaInstrCentering, 33 * MM2M);
			ensure_equals(ts1->instrumentPos->getName(), "P1");
			// add a V0 to this station
			TKeyV0 v0(proj);
			v0.parse(tokenizefileString("*V0 TRGT PT9 ACST 66"), -1);
			ensure_equals("Default target in thisTSTN should not be affected", ts1->instrument.defTarget, "PT2");
			ensure_equals("In this ROM, default target should be updated", ts1->roms.back()->defaultTarget->ID, "PT9");
			ensure_equals("Adjustable distance correction should be valid and unchanged", ts1->roms.back()->defaultTarget->distCorrectionAdjustable->getProvisionalValue(), 5.0);

			ensure_distance(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->acst.getGonsValue(), 66.0, 1e-8);

			// Add one of each POLAR measurement to this ROM
			//
			// PLR3D
			TKeyPLR3D plr(proj);
			plr.parse(tokenizefileString( "*PLR3D TRGT PT8"), -1);
			plr.parse(tokenizefileString( "P2 1 2 3 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1->roms.back()->defaultTarget->ID, "PT9");
			ensure_equals("Target of this measurement taken implicitly from *PLR3D", ts1->roms.back()->measPLR3D.back().target.ID, "PT8");

			const auto& plrmeas(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measPLR3D.back());
			ensure_distance(plrmeas.getAngle(kANGL).getGonsValue(), 1.0, 1e-8);
			ensure_distance(plrmeas.getAngle(kZEND).getGonsValue(), 2.0, 1e-8);
			ensure_distance(plrmeas.getDistance().getMetresValue(), 3.0, 1e-8);
			ensure_equals(plrmeas.target.ID, "PT8");
			ensure_equals(plrmeas.target.targetHt, 11);
			ensure_equals(plrmeas.target.sigmaTargetHt, 12* MM2M);
			ensure_equals(plrmeas.target.sigmaTargetCentering, 13* MM2M);
			ensure_equals(plrmeas.target.sigmaAngl.getRadiansValue(), 14 * CC2RAD);
			ensure_equals(plrmeas.target.sigmaZenD.getRadiansValue(), 15 * CC2RAD);
			ensure_equals(plrmeas.target.sigmaDist, 16* MM2M);
			ensure_equals(plrmeas.target.ppmDist, 17* MM2M);

			plr.parse(tokenizefileString( "P3 1 2 3 TRGT PT2 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1->roms.back()->defaultTarget->ID, "PT9");
			ensure_equals("Target of this measurement should be updated", ts1->roms.back()->measPLR3D.back().target.ID, "PT2");

			plr.parse(tokenizefileString( "P4 1 2 3 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Target of this measurement taken from previous measurement (currValue)", ts1->roms.back()->measPLR3D.back().target.ID, "PT2");

			plr.parse(tokenizefileString( "P5 1 2 3 TRGT PT7 TH 11 THSE 12 TCSE 13 ASE 14 ZSE 15 DSE 16 PPM 17"), -1);
			ensure_equals("Target of this measurement should be updated", ts1->roms.back()->measPLR3D.back().target.ID, "PT7");
			ensure_equals("Default target in this ROM should not be affected",  ts1->roms.back()->defaultTarget->ID, "PT9");
			//
			// ANGL
			TKeyANGL ang(proj);
			ang.parse(tokenizefileString( "*ANGL"), -1);
			ang.parse(tokenizefileString( "P2 88 OBSE 21 TCSE 22"), -1);
			ensure_equals("ANGL deafault target should be taken from ROM", ts1->roms.back()->measANGL.back().target.ID, "PT9");

			ang.parse(tokenizefileString( "P3 88 TRGT PT7 OBSE 21 TCSE 22"), -1);
			ensure_equals("Default target in this ROM should not be affected",  ts1->roms.back()->defaultTarget->ID, "PT9");
			ensure_equals("ANGL target of this measurement updated", ts1->roms.back()->measANGL.back().target.ID, "PT7");

			const auto& angmeas(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measANGL.back());
			ensure_distance(angmeas.getAngle().getGonsValue(), 88.0, 1e-8);
			ensure_equals(angmeas.target.sigmaAngl.getRadiansValue(), 21 * CC2RAD);
			ensure_equals(angmeas.target.sigmaTargetCentering, 22 * MM2M);
			//
			// ZEND
			TKeyZEND zend(proj);
			zend.parse(tokenizefileString( "*ZEND TRGT PT8"), -1);
			zend.parse(tokenizefileString( "P2 22 OBSE 31 TH 32 THSE 33 TCSE 34"), -1);
			ensure_equals("Target of this measurement taken from *ZEND",  ts1->roms.back()->measZEND.back().target.ID, "PT8");
			const auto& zendmeas(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measZEND.back());
			ensure_equals(zendmeas.targetPos->getName(), "P2");
			ensure_equals(zendmeas.target.sigmaZenD.getRadiansValue(), 31 * CC2RAD);
			ensure_equals(zendmeas.target.targetHt, 32);
			ensure_equals(zendmeas.target.sigmaTargetHt, 33 * MM2M);
			ensure_equals(zendmeas.target.sigmaTargetCentering, 34 * MM2M);

			zend.parse(tokenizefileString( "P3 22 TRGT PT7 OBSE 31 TH 32 THSE 33 TCSE 34"), -1);
			ensure_equals("Target of this measurement updated ZEND",  ts1->roms.back()->measZEND.back().target.ID, "PT7");
			//
			// DIST
			//New v0 occured i.e. without any parametr, default value from TSTN taken
			TKeyV0 v1(proj);
			v1.parse(tokenizefileString("*V0"), -1);
			TKeyDIST dist(proj);
			dist.parse(tokenizefileString( "*DIST"), -1);
			dist.parse(tokenizefileString( "P2 40 OBSE 41 PPM 42 TH 43 THSE 44 TCSE 45"), -1);
			ensure_equals("Default target taken from new ROM, which takes default target implicitly from TSTN", ts1->roms.back()->measDIST.back().target.ID, "PT2");

			const auto& distmeas(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measDIST.back());
			ensure_equals(distmeas.target.sigmaDist, 41 * MM2M);
			ensure_equals(distmeas.target.ppmDist, 42 * MM2M);
			ensure_equals(distmeas.target.targetHt, 43);
			ensure_equals(distmeas.target.sigmaTargetHt, 44 * MM2M);
			ensure_equals(distmeas.target.sigmaTargetCentering, 45 * MM2M);
			//
			// ECTH
			TKeyECTH ecth(proj);
			ecth.parse(tokenizefileString( "*ECTH 1 SC1"), -1);
			ecth.parse(tokenizefileString("P2 1.1 OBSE 0.01 PPM 0.1 ICSE 0.5"),-1);
			const auto& ecthmeas(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measECTH.back());
			ensure_equals(ecthmeas.targetPos->getName(), "P2");
			ensure_equals(ecthmeas.obsHorAngle.getGonsValue(), 1);
			ensure_equals(ecthmeas.target.ID, "SC1");
			ensure_equals(ecthmeas.target.sigmaD, 0.01 * MM2M);
			ensure_equals(ecthmeas.target.ppmD, 0.1 * MM2M);
			ensure_equals(ecthmeas.target.sigmaInstrCentering, 0.5 * MM2M);
			ensure_equals(ecthmeas.getDistance(), 1.1);
			ensure_equals("Default values in instrument data not affected", proj.getInstruments().getDevice(proj.getInstruments().fSCALE,"SC1").sigmaInstrCentering, 5 * MM2M);

			ecth.parse(tokenizefileString("P2 0.9 SCALE SC2 OBSE 0.01 PPM 0.1 ICSE 0.5"),-1);
			const auto& ecthmeas2(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measECTH.back());
			ensure_equals(ecthmeas2.target.ID, "SC2");
			ensure_equals(ecthmeas2.getDistance(), 0.9);

			ecth.parse(tokenizefileString("P2 0.9"),-1);
			const auto& ecthmeas3(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measECTH.back());
			ensure_equals("This takes current instrument from previous", ecthmeas3.target.ID, "SC2");
			ensure_equals("Has default values", ecthmeas3.target.sigmaInstrCentering, 1e-8, 5 * MM2M);
			
			// DHOR
			TKeyDHOR dhor(proj);
			dhor.parse(tokenizefileString( "*DHOR"), -1);
			dhor.parse(tokenizefileString( "P2 50 TRGT PT7 OBSE 51 PPM 52 TCSE 53"), -1);
			ensure_equals("Target of the measurement should match", ts1->roms.back()->measDHOR.back().target.ID, "PT7");
			dhor.parse(tokenizefileString( "P2 50 OBSE 51 PPM 52 TCSE 53"), -1);
			ensure_equals("Target implicitly taken from previous measurement", ts1->roms.back()->measDHOR.back().target.ID, "PT7");

			const auto& dhormeas(proj.getCurrentNode().measurements.fTSTN.back()->roms.back()->measDHOR.back());
			ensure_equals(dhormeas.target.sigmaDist, 51 * MM2M);
			ensure_equals(dhormeas.target.ppmDist, 52 * MM2M);
			ensure_equals(dhormeas.target.sigmaTargetCentering, 53 * MM2M);
			
			// non-TSTN measurements

			// DSPT
			TKeyDSPT dspt(proj);
			dspt.parse(tokenizefileString( "*DSPT P1 DM1 IH 60 IHSE 61 ICSE 62"), -1);
			dspt.parse(tokenizefileString( "P2 63 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("Instrument height updated DSPT",proj.getCurrentNode().measurements.fEDM.back().instrument.instrHeight , 60.0);
			ensure_equals("Instrument height updated DSPT", proj.getCurrentNode().measurements.fEDM.back().instrumentPos->getProvisionalValue().getX().getMetresValue(), 1.0);

			ensure_equals("DPST default target from instrument section",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET1");
			dspt.parse(tokenizefileString( "P2 63 TRGT ET2 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("DPST default target updated",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET2");
			dspt.parse(tokenizefileString( "P3 63 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("DPST default target implicitly taken from preceding",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET2");
			dspt.parse(tokenizefileString( "P3 63 TRGT ET1 OBSE 64 PPM 65 TH 66 THSE 67 TCSE 68"), -1);
			ensure_equals("DPST default target updated",proj.getCurrentNode().measurements.fEDM.back().instrument.defTarget , "ET1");

			const auto& dspPOLAR(proj.getCurrentNode().measurements.fEDM.back().instrument);
			ensure_equals(dspPOLAR.ID, "DM1");
			ensure_equals(dspPOLAR.instrHeight, 60);
			ensure_equals(dspPOLAR.sigmaInstrHeight, 61 * MM2M);
			ensure_equals(dspPOLAR.sigmaInstrCentering, 62 * MM2M);
			const auto& dsptmeas(proj.getCurrentNode().measurements.fEDM.back().measDSPT.back());
			ensure_equals(dsptmeas.getDistance(), 63);
			ensure_equals(dsptmeas.target.sigmaDSpt, 64 * MM2M);
			ensure_equals(dsptmeas.target.ppmDSpt, 65 * MM2M);
			ensure_equals(dsptmeas.target.targetHt, 66);
			ensure_equals(dsptmeas.target.sigmaTargetHt, 67 * MM2M);
			ensure_equals(dsptmeas.target.sigmaTargetCentering, 68 * MM2M);
			ensure_equals("Adjustable distance correction should be valid and unchanged", dsptmeas.target.distCorrectionAdjustable->getProvisionalValue(), 3.0);
			//
			// DVER
			TKeyDVER dver(proj);
			dver.parse(tokenizefileString( "*DVER"), -1);
			dver.parse(tokenizefileString( "P2 P3 2.0 OBSE 0.01"), -1);
			const auto& dverM(proj.getCurrentNode().measurements.fDVER.back());
			ensure_equals("Point coordinates should match", dverM.station->getProvisionalValue().getX().getMetresValue(), 1);
			ensure_equals("Point coordinates should match", dverM.targetPos->getProvisionalValue().getX().getMetresValue(),1);
			ensure_equals("Point coordinates should match", dverM.getObservedStDev(), 0.01 * MM2M);

			//
			// DLEV
			TKeyDLEV dlev(proj);

			dlev.parse(tokenizefileString( "*DLEV LI1 RefPt P2"), -1);
			dlev.parse(tokenizefileString( "P1 5 DHOR 1.0 TRGT ST2 OBSE 0.1 PPM 0.01 TH 1.0 THSE 0.1 DSE 0.1"), -1);
			dlev.parse(tokenizefileString( "P3 6"), -1);

			const auto& levelRound(proj.getCurrentNode().measurements.fLEVEL.back());
			ensure_equals("Instrument ID should match", levelRound.instrument.ID,"LI1");
			ensure_equals("Default staff ID should match", levelRound.instrument.defStaffID,"ST2");
			ensure_equals("Reference point given, plane should be initialized", levelRound.fMeasuredPlane->isInitialized(),true);
			ensure_equals("Distance of the reference point should be zero", levelRound.fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 0.0);

			auto& firstDLEVMeasurement(levelRound.measDLEV.front());
			ensure_equals("Name of the target position should match", firstDLEVMeasurement.targetPos->getName(),"P1");
			ensure_equals("Measured vertical distance should match", firstDLEVMeasurement.getDistance(),5);
			ensure_equals("Target should be overidden", firstDLEVMeasurement.target.ID,"ST2");
			ensure_equals("Target's ppm value should be overidden", firstDLEVMeasurement.target.ppmD, 0.01 * MM2M);

			auto& firstDLEVMeasurement2(*(std::next(levelRound.measDLEV.begin(), 1)));
			ensure_equals("Name of the target position should match", firstDLEVMeasurement2.targetPos->getName(),"P3");
			ensure_equals("Measured vertical distance should match", firstDLEVMeasurement2.getDistance(),6);
			ensure_equals("Target should be overidden", firstDLEVMeasurement2.target.ID,"ST2");
			ensure_equals("Target's ppm value should be default", firstDLEVMeasurement2.target.ppmD, 1e-8, 2.0 * MM2M);

			dlev.parse(tokenizefileString( "*DLEV LI1"), -1);
			const auto& levelRound2(proj.getCurrentNode().measurements.fLEVEL.back());
			ensure_equals("Reference point given, plane should not be initialized", levelRound2.fMeasuredPlane->isInitialized(),false);

			//
			// ECVE
			TKeyECVE ecve(proj);
			ecve.parse(tokenizefileString("*ECVE SC1 PtLine P2"), -1);
			ecve.parse(tokenizefileString("P2 1.1 OBSE 0.01 PPM 0.1 ICSE 0.5"), -1);
			const auto& ecvemeas(proj.getCurrentNode().measurements.fECVE.back().measECVE.back());
			ensure_equals(ecvemeas.targetPos->getName(), "P2");
			ensure_equals(ecvemeas.target.ID, "SC1");
			ensure_equals(ecvemeas.target.sigmaD, 0.01 * MM2M);
			ensure_equals(ecvemeas.target.ppmD, 0.1 * MM2M);
			ensure_equals(ecvemeas.target.sigmaInstrCentering, 0.5 * MM2M);
			ensure_equals(ecvemeas.getDistance(), 1.1);
			ensure_equals("Default values in instrument data not affected", proj.getInstruments().getDevice(proj.getInstruments().fSCALE, "SC1").sigmaInstrCentering, 5 * MM2M);

			ecve.parse(tokenizefileString("P2 0.9 SCALE SC2 OBSE 0.01 PPM 0.1 ICSE 0.5"), -1);
			const auto& ecvemeas2(proj.getCurrentNode().measurements.fECVE.back().measECVE.back());
			ensure_equals(ecvemeas2.target.ID, "SC2");
			ensure_equals(ecvemeas2.getDistance(), 0.9);

			ecve.parse(tokenizefileString("P2 0.9"), -1);
			const auto& ecvemeas3(proj.getCurrentNode().measurements.fECVE.back().measECVE.back());
			ensure_equals("This takes current instrument from previous", ecvemeas3.target.ID, "SC2");
			ensure_equals("Has default values", ecvemeas3.target.sigmaInstrCentering, 1e-8, 5 * MM2M);
			//
			// ECSP
			TKeyCALA pr1(proj);
			pr1.parse(tokenizefileString("A 1 2 3"), -1);
			pr1.parse(tokenizefileString("B 0 2 3"), -1);
			TKeyECSP ecsp(proj);
			ecsp.parse(tokenizefileString("*ECSP A B SC1 "), -1);
			ecsp.parse(tokenizefileString("P2 1.1 OBSE 0.01 PPM 0.1 ICSE 0.5"), -1);
			const auto& ecspmeas(proj.getCurrentNode().measurements.fECSP.back().measECSP.back());
			ensure_equals(ecspmeas.targetPos->getName(), "P2");
			ensure_equals(ecspmeas.target.ID, "SC1");
			ensure_equals(ecspmeas.target.sigmaD, 0.01 * MM2M);
			ensure_equals(ecspmeas.target.ppmD, 0.1 * MM2M);
			ensure_equals(ecspmeas.target.sigmaInstrCentering, 0.5 * MM2M);
			ensure_equals(ecspmeas.getDistance(), 1.1);
			ensure_equals("Default values in instrument data not affected", proj.getInstruments().getDevice(proj.getInstruments().fSCALE, "SC1").sigmaInstrCentering, 5 * MM2M);

			ecsp.parse(tokenizefileString("P2 0.9 SCALE SC2 OBSE 0.01 PPM 0.1 ICSE 0.5"), -1);
			const auto& ecspmeas2(proj.getCurrentNode().measurements.fECSP.back().measECSP.back());
			ensure_equals(ecspmeas2.target.ID, "SC2");
			ensure_equals(ecspmeas2.getDistance(), 0.9);

			ecsp.parse(tokenizefileString("P2 0.9"), -1);
			const auto& ecspmeas3(proj.getCurrentNode().measurements.fECSP.back().measECSP.back());
			ensure_equals("This takes current instrument from previous", ecspmeas3.target.ID, "SC2");
			ensure_equals("Has default values", ecspmeas3.target.sigmaInstrCentering, 1e-8, 5 * MM2M);
			//
			// RADI
			TKeyRADI radi(proj);
			radi.parse(tokenizefileString("*RADI"), -1);
			radi.parse(tokenizefileString("P1 100.0 SIGMA 0.01"), -1);
			const auto& radiM(proj.getCurrentNode().measurements.fRADI.back());
			ensure_equals("Point name should match", radiM.station->getName(), "P1");
			ensure_equals("bearing should match", radiM.getAngleCnstr().getGonsValue(), 100.0, 1e-7);
			ensure_equals("sigma should match", radiM.getObservedStDev().getMMetresValue(), 0.01);

			radi.parse(tokenizefileString("P2 150.0"), -1);
			const auto& radiM2(proj.getCurrentNode().measurements.fRADI.back());
			ensure_equals("Point name should match", radiM2.station->getName(), "P2");
			ensure_equals("bearing should match", radiM2.getAngleCnstr().getGonsValue(), 150.0, 1e-7);
			ensure_equals("sigma should match", radiM2.getObservedStDev().getMMetresValue(), 1.0);

			TKeyRADI radi2(proj);
			radi2.parse(tokenizefileString("*RADI 2"), -1);
			radi2.parse(tokenizefileString("P3 100.0"), -1);
			const auto& radi2M(proj.getCurrentNode().measurements.fRADI.back());
			ensure_equals("Point name should match", radi2M.station->getName(), "P3");
			ensure_equals("bearing should match", radi2M.getAngleCnstr().getGonsValue(), 100.0, 1e-7);
			ensure_equals("sigma should match", radi2M.getObservedStDev().getMMetresValue(), 2.0);


			//OBSXYZ
			TKeyOBSXYZ cobsXYZ(proj);
			cobsXYZ.parse(tokenizefileString("*OBSXYZ"), -1);
			cobsXYZ.parse(tokenizefileString("P2 1 2 3 0.1 0.5 0.8"), -1);
			const auto& obsXYZM(proj.getCurrentNode().measurements.fOBSXYZ.back());
			ensure_equals("Point name should match", obsXYZM.station->getName(), "P2");
			ensure_equals("obs_X should match", obsXYZM.initialValue.getX(), 1);
			ensure_equals("obs_Y should match", obsXYZM.initialValue.getY(), 2);
			ensure_equals("obs_Z should match", obsXYZM.initialValue.getZ(), 3);
			ensure_equals("SX should match", obsXYZM.getXObservedStDev().getMMetresValue(), 0.1);
			ensure_equals("SY should match", obsXYZM.getYObservedStDev().getMMetresValue(), 0.5);
			ensure_equals("SZ should match", obsXYZM.getZObservedStDev().getMMetresValue(), 0.8);

			////////////////////////////////////
			//Testing FRAME measurements
			////////////////////////////////////
			TLGCData measProj;
			const std::string line = "*FRAME fff1  1 2 3 4 5 6 1 STX 0.1";
			const std::string lineClose = "*ENDFRAME";
			TKeyFRAME fr(measProj);
			TKeyENDFRAME endFr(measProj);
			fr.parse(tokenizefileString(line),1);
			endFr.parse(tokenizefileString(lineClose),2);
			TDataTreeIterator pos = measProj.getCurrentPosition()++;
			TDataTreeIterator currentNodeIter = measProj.getTree().begin();
			++currentNodeIter;

			ensure_equals("One FRAME measurement", currentNodeIter->get()->frame.getTranslationStandDev(0).getMMetresValue(), 0.1);
			EXPECT_FAIL(currentNodeIter->get()->frame.getTranslationStandDev(1));


			EXPECT_FAIL(ensure_equals("Scale standard deviation not assigned",currentNodeIter->get()->frame.getScaleStandDev()*M2MM, 5));
		}

}
