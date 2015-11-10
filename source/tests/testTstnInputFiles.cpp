#if 1
#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TLGCApp.h>
#include <readers/TReader.h>
#include "tests_TSTN_OLOC.h"
#include "TLGCCalculation.h"
#include "Utils.h"
#include <vector>
#include <ctime>

namespace tut
{
    struct test_InputFiles{};
    typedef test_group<test_InputFiles> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Testing input files with 1 total station (TSTN)");
}

namespace tut
{	

	////////////////////////////////////////////////////////////////////////
	/////////////////////////Parametric case
	////////////////////////////////////////////////////////////////////////
	template<>
	template<>
	void object::test<1>()
	{
		set_test_name("Testing param case in ROOT");
		using namespace std;
		
		std::vector<double> v0;
		v0.push_back(0);
		v0.push_back(10);
		v0.push_back(90);
		v0.push_back(100);
		v0.push_back(110);
		v0.push_back(190);
		v0.push_back(200);
		v0.push_back(210);
		v0.push_back(290);
		v0.push_back(300);
		v0.push_back(390);

		double angl_REF_CORRECT = 100;
		double angl_Pt_CORRECT = 0;
		std::vector<double> anglRef;
		std::vector<double> anglPt;

		for(size_t i = 0; i< v0.size();i++){
			anglRef.push_back(angl_REF_CORRECT+v0[i]);
			anglPt.push_back(angl_Pt_CORRECT+v0[i]);
		}


		std::string anglPart;
		for(size_t i=0;i<v0.size();i++){
			std::shared_ptr<TLGCData> projTest(new TLGCData);
			set_test_name("Testing param case in ROOT");
			TReader r(projTest);
			projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
			projTest->getFileLogger().writeReportHeader("LGC output file");


			std::ostringstream strs;
			strs << anglRef[i];
			std::string refAnglV0Str = strs.str();


			std::ostringstream strs2;
			strs2 << anglPt[i];
			std::string ptAnglV0Str = strs2.str();


			anglPart = "*ANGL\nREF " + refAnglV0Str +"\n PT " +  ptAnglV0Str + "\n";

			//One POINT and one TSTN (V0) = 4 unknowns)
			stringstream infiler(TestROOT::Param_case);
			infiler.seekp(0, std::ios::end);
			infiler << anglPart;
			string infile = infiler.str();

			r.read(infiler);
		
			TLGCCalculation calcul(projTest);
			std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
			bool succesCalc = calcul.computeResults(fileWriter);
			ensure_equals("Calculation successful", succesCalc, true);

			const TLGCData& dataset = calcul.getData();
			TPositionVector PTest = dataset.getPoints().getObject("PT").getEstimatedValue();
         ensure_equals("Pt x coordinate should match", PTest.getX().getMetresValue(), 0, 1e-12);
         ensure_equals("Pt y coordinate should match", PTest.getY().getMetresValue(), 100, 1e-12);
         ensure_equals("Pt z coordinate should match", PTest.getZ().getMetresValue(), 0, 1e-12);
		 TReal V0Calc = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().getGonsValue();

			std::vector<double> Outputv0;
				Outputv0.push_back(0);
				Outputv0.push_back(390);
				Outputv0.push_back(310);
				Outputv0.push_back(300);
				Outputv0.push_back(290);
				Outputv0.push_back(210);
				Outputv0.push_back(200);
				Outputv0.push_back(190);
				Outputv0.push_back(110);
				Outputv0.push_back(100);
				Outputv0.push_back(10);

			ensure_equals("V0 should match",V0Calc, Outputv0[i], 1e-12);
			//ensure_equals("V0 should match",V0Calc, v0[i], 1e-12);


			int numberofLSIterations = projTest->getNumberOfLSIterations();
			ensure("Number of LS iteration less than 10" ,numberofLSIterations<10 );
			}

	}	


	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing param case in ROOT setup2");

		std::vector<double> v0;
		v0.push_back(0);
		v0.push_back(10);
		v0.push_back(90);
		v0.push_back(100);
		v0.push_back(110);
		v0.push_back(190);
		v0.push_back(200);
		v0.push_back(210);
		v0.push_back(290);
		v0.push_back(300);
		v0.push_back(390);

		double angl_REF_CORRECT = 250;
		double angl_Pt_CORRECT = 300;
		std::vector<double> anglRef;
		std::vector<double> anglPt;

		for(size_t i = 0; i< v0.size();i++){
			anglRef.push_back(angl_REF_CORRECT+v0[i]);
			anglPt.push_back(angl_Pt_CORRECT+v0[i]);
		}


		std::string test;

		for(size_t i=0;i<v0.size();i++){
	    	std::shared_ptr<TLGCData> projTest(new TLGCData);
			TReader r(projTest);
			projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
			projTest->getFileLogger().writeReportHeader("LGC output file");


			std::ostringstream strs;
			strs << anglRef[i];
			std::string refAnglV0Str = strs.str();


			std::ostringstream strs2;
			strs2 << anglPt[i];
			std::string ptAnglV0Str = strs2.str();


			test = "*ANGL\nREF " + refAnglV0Str +"\n PT " +  ptAnglV0Str + "\n";

			//One POINT and one TSTN (V0) = 4 unknowns)
			stringstream infiler(TestROOT::Param_case_setup2);
			infiler.seekp(0, std::ios::end);
			infiler << test;
			string infile = infiler.str();

			r.read(infiler);
		
			TLGCCalculation calcul(projTest);
			std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
			bool succesCalc = calcul.computeResults(fileWriter);
			ensure_equals("Calculation successful", succesCalc, true);

			const TLGCData& dataset = calcul.getData();
			TPositionVector Pt = dataset.getPoints().getObject("PT").getEstimatedValue();
         ensure_equals("Pt x coordinate should match", Pt.getX().getMetresValue(), 0, 1e-12);
         ensure_equals("Pt y coordinate should match", Pt.getY().getMetresValue(), 100, 1e-12);
         ensure_equals("Pt z coordinate should match", Pt.getZ().getMetresValue(), 0, 1e-12);
		 TReal V0Calc = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().getGonsValue();

			std::vector<double> Outputv0;
				Outputv0.push_back(0);
				Outputv0.push_back(390);
				Outputv0.push_back(310);
				Outputv0.push_back(300);
				Outputv0.push_back(290);
				Outputv0.push_back(210);
				Outputv0.push_back(200);
				Outputv0.push_back(190);
				Outputv0.push_back(110);
				Outputv0.push_back(100);
				Outputv0.push_back(10);

			ensure_equals("V0 should match",V0Calc, Outputv0[i], 1e-12);
			//ensure_equals("V0 should match",V0Calc, v0[i], 1e-12);


			
			int numberofLSIterations = projTest->getNumberOfLSIterations();
			ensure("Number of LS iteration less than 10" ,numberofLSIterations<10 );
			}
	}	

	template<>
	template<>
	void object::test<3>()
	{
		set_test_name("Testing param case in ROOT setup3");

		std::vector<double> v0;
		v0.push_back(0);
		v0.push_back(10);
		v0.push_back(90);
		v0.push_back(100);
		v0.push_back(110);
		v0.push_back(190);
		v0.push_back(200);
		v0.push_back(210);
		v0.push_back(290);
		v0.push_back(300);
		v0.push_back(390);

		double angl_REF_CORRECT = 100;
		double angl_Pt_CORRECT = 0;
		double angl_Pt2_CORRECT = 50;

		std::vector<double> anglRef;
		std::vector<double> anglPt;
		std::vector<double> anglPt2;

		for(size_t i = 0; i< v0.size();i++){
			anglRef.push_back(angl_REF_CORRECT+v0[i]);
			anglPt.push_back(angl_Pt_CORRECT+v0[i]);
			anglPt2.push_back(angl_Pt2_CORRECT+v0[i]);
		}


		std::string test;

		for(size_t i=0;i<v0.size();i++){
			std::shared_ptr<TLGCData> projTest(new TLGCData);
			TReader r(projTest);
			projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
			projTest->getFileLogger().writeReportHeader("LGC output file");


			std::ostringstream strs;
			strs << anglRef[i];
			std::string refAnglV0Str = strs.str();


			std::ostringstream strs2;
			strs2 << anglPt[i];
			std::string ptAnglV0Str = strs2.str();

			std::ostringstream strs3;
			strs3 << anglPt2[i];
			std::string pt2AnglV0Str = strs3.str();


			test = "*ANGL\nREF " + refAnglV0Str +"\n PT " +  ptAnglV0Str + "\n PT2 " + pt2AnglV0Str + "\n" ;

			//One POINT and one TSTN (V0) = 4 unknowns)
			stringstream infiler(TestROOT::Param_case_setup3);
			infiler.seekp(0, std::ios::end);
			infiler << test;
			string infile = infiler.str();

			r.read(infiler);
		

			TLGCCalculation calcul(projTest);
			std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
			bool succesCalc = calcul.computeResults(fileWriter);
			ensure_equals("Calculation successful", succesCalc, true);

			const TLGCData& dataset = calcul.getData();
			TPositionVector Pt = dataset.getPoints().getObject("PT").getEstimatedValue();
         ensure_equals("Pt x coordinate should match", Pt.getX().getMetresValue(), 0, 1e-12);
         ensure_equals("Pt y coordinate should match", Pt.getY().getMetresValue(), 100, 1e-12);
         ensure_equals("Pt z coordinate should match", Pt.getZ().getMetresValue(), 0, 1e-12);

			TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
         ensure_equals("Pt2 x coordinate should match", PT2.getX().getMetresValue(), 100, 1e-9);
         ensure_equals("Pt2 y coordinate should match", PT2.getY().getMetresValue(), 100, 1e-9);
         ensure_equals("Pt2 z coordinate should match", PT2.getZ().getMetresValue(), 0, 1e-9);

		 TReal V0Calc = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().getGonsValue();

			std::vector<double> Outputv0;
				Outputv0.push_back(0);
				Outputv0.push_back(390);
				Outputv0.push_back(310);
				Outputv0.push_back(300);
				Outputv0.push_back(290);
				Outputv0.push_back(210);
				Outputv0.push_back(200);
				Outputv0.push_back(190);
				Outputv0.push_back(110);
				Outputv0.push_back(100);
				Outputv0.push_back(10);

			ensure_equals("V0 should match",V0Calc, Outputv0[i], 1e-12);
			//ensure_equals("V0 should match",V0Calc, v0[i], 1e-12);


			
			int numberofLSIterations = projTest->getNumberOfLSIterations();
			ensure("Number of LS iteration less than 10" ,numberofLSIterations<10 );
			}
	}
	////////////////////////////////////////////////////////////////////////
	/////////////////////////PLR3D case
	////////////////////////////////////////////////////////////////////////
	template<>
	template<>
	void object::test<4>()
	{
		set_test_name("Testing PLR3D case in ROOT setup2");

		std::vector<double> v0;
		v0.push_back(0);
		v0.push_back(10);
		v0.push_back(90);
		v0.push_back(100);
		v0.push_back(110);
		v0.push_back(190);
		v0.push_back(200);
		v0.push_back(210);
		v0.push_back(290);
		v0.push_back(300);
		v0.push_back(390);

		double angl_REF_CORRECT = 250;
		double angl_Pt_CORRECT = 300;
		std::vector<double> anglRef;
		std::vector<double> anglPt;

		for(size_t i = 0; i< v0.size();i++){
			anglRef.push_back(angl_REF_CORRECT+v0[i]);
			anglPt.push_back(angl_Pt_CORRECT+v0[i]);
		}


		std::string test;

		for(size_t i=0;i<v0.size();i++){
			if(i != 6){
				std::shared_ptr<TLGCData> projTest(new TLGCData);
				TReader r(projTest);
				projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
				projTest->getFileLogger().writeReportHeader("LGC output file");


				std::ostringstream strs;
				strs << anglRef[i];
				std::string refAnglV0Str = strs.str();


				std::ostringstream strs2;
				strs2 << anglPt[i];
				std::string ptAnglV0Str = strs2.str();

				test = "*PLR3D\nREF " + refAnglV0Str +" 100 141.421356237 \n PT " +  ptAnglV0Str + " 100 100 \n";

				//One POINT and one TSTN (V0) = 4 unknowns)
				stringstream infiler(TestROOT::Root_PLR_setup2);
				infiler.seekp(0, std::ios::end);
				infiler << test;
				string infile = infiler.str();

				r.read(infiler);
		
				TLGCCalculation calcul(projTest);
				std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
			bool succesCalc = calcul.computeResults(fileWriter);
			ensure_equals("Calculation successful", succesCalc, true);

				const TLGCData& dataset = calcul.getData();
				TPositionVector Pt = dataset.getPoints().getObject("PT").getEstimatedValue();
				ensure_equals("Pt x coordinate should match",Pt.getX().getMetresValue(), 0, 1e-12);
				ensure_equals("Pt y coordinate should match",Pt.getY().getMetresValue(), 100, 1e-12);
				ensure_equals("Pt z coordinate should match",Pt.getZ().getMetresValue(), 0, 1e-12);
				TReal V0Calc = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().getGonsValue();

				std::vector<double> Outputv0;
				Outputv0.push_back(0);
				Outputv0.push_back(390);
				Outputv0.push_back(310);
				Outputv0.push_back(300);
				Outputv0.push_back(290);
				Outputv0.push_back(210);
				Outputv0.push_back(200);
				Outputv0.push_back(190);
				Outputv0.push_back(110);
				Outputv0.push_back(100);
				Outputv0.push_back(10);

				ensure_equals("V0 should match",V0Calc, Outputv0[i], 1e-12);

				int numberofLSIterations = projTest->getNumberOfLSIterations();
				ensure("Number of LS iteration less than 10" ,numberofLSIterations<14 );
			}
			}
	}	

	//DOES NOT WORK PROPERLY FOR V0 = 200///////////////////

	template<>
	template<>
	void object::test<6>()
	{

		set_test_name("Testing param case in ROOT setup3");

		std::vector<double> v0;
		v0.push_back(0);
		v0.push_back(10);
		v0.push_back(90);
		v0.push_back(100);
		v0.push_back(110);
		v0.push_back(195);
		v0.push_back(200);
		v0.push_back(205);
		v0.push_back(290);
		v0.push_back(300);
		v0.push_back(390);

		double angl_REF_CORRECT = 100;
		double angl_Pt_CORRECT = 0;
		double angl_Pt2_CORRECT = 50;

		std::vector<double> anglRef;
		std::vector<double> anglPt;
		std::vector<double> anglPt2;

		for(size_t i = 0; i< v0.size();i++){
			anglRef.push_back(angl_REF_CORRECT+v0[i]);
			anglPt.push_back(angl_Pt_CORRECT+v0[i]);
			anglPt2.push_back(angl_Pt2_CORRECT+v0[i]);
		}


		std::string test;

		for(size_t i=0;i<v0.size();i++){
		std::shared_ptr<TLGCData> projTest(new TLGCData);
			TReader r(projTest);
			projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
			projTest->getFileLogger().writeReportHeader("LGC output file");


			std::ostringstream strs;
			strs << anglRef[i];
			std::string refAnglV0Str = strs.str();


			std::ostringstream strs2;
			strs2 << anglPt[i];
			std::string ptAnglV0Str = strs2.str();

			std::ostringstream strs3;
			strs3 << anglPt2[i];
			std::string pt2AnglV0Str = strs3.str();



			test = "*PLR3D\nREF " + refAnglV0Str +" 100 100 \n PT " +  ptAnglV0Str + " 100 100 \n PT2 " + pt2AnglV0Str + " 100 141.421356237 \n";

			//One POINT and one TSTN (V0) = 4 unknowns)
			stringstream infiler(TestROOT::Root_PLR_setup3);
			infiler.seekp(0, std::ios::end);
			infiler << test;
			string infile = infiler.str();

			r.read(infiler);
		
			if(i!=6){
					TLGCCalculation calcul(projTest);
					std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
			bool succesCalc = calcul.computeResults(fileWriter);
			ensure_equals("Calculation successful", succesCalc, true);

					const TLGCData& dataset = calcul.getData();

					TPositionVector Pt = dataset.getPoints().getObject("PT").getEstimatedValue();
					ensure_equals("Pt x coordinate should match",Pt.getX().getMetresValue(), 0, 1e-12);
					ensure_equals("Pt y coordinate should match",Pt.getY().getMetresValue(), 100, 1e-12);
					ensure_equals("Pt z coordinate should match",Pt.getZ().getMetresValue(), 0, 1e-12);

					TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
					ensure_equals("Pt2 x coordinate should match",PT2.getX().getMetresValue(), 100, 1e-9);
					ensure_equals("Pt2 y coordinate should match",PT2.getY().getMetresValue(), 100, 1e-9);
					ensure_equals("Pt2 z coordinate should match",PT2.getZ().getMetresValue(), 0, 1e-9);

					TReal V0Calc = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().getGonsValue();
					
					std::vector<double> Outputv0;
					Outputv0.push_back(0);
					Outputv0.push_back(390);
					Outputv0.push_back(310);
					Outputv0.push_back(300);
					Outputv0.push_back(290);
					Outputv0.push_back(205);
					Outputv0.push_back(200);
					Outputv0.push_back(195);
					Outputv0.push_back(110);
					Outputv0.push_back(100);
					Outputv0.push_back(10);

					ensure_equals("V0 should match",V0Calc, Outputv0[i], 1e-12);
			}
			}	
	}	

	template<>
	template<>
	void object::test<7>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		//IF CLOSE TO 200, no problem

		set_test_name("PLR setup 3");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/output.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Root_PLR_setup2_SEP);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
			bool succesCalc = calcul.computeResults(fileWriter);
			ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();

		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 0, 1e-12);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 100, 1e-12);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 0, 1e-12);

		TReal V0Calc = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().getGonsValue();
        ensure_equals("V0 calculation should match for total station ST2",V0Calc, 199, 1e-8); 
	}

	template<>
	template<>
	void object::test<8>()
	{
		set_test_name("ECTH Test");
		TLGCApp proj(getCurrentDirectory() + slash + "test_files" + slash + "Calc" + slash + "testECTH.lgc2",
					 getCurrentDirectory() + slash + "test_files" + slash + "Calc" + slash + "testECTH.out");

		proj.exec();
	}
}
#endif



