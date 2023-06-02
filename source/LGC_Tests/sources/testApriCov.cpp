#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testApriCov.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
	struct test_APRICOV 
	{
		test_APRICOV() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
		std::shared_ptr<TLGCData> projTest;
		TReader reader;
	};
	typedef test_group<test_APRICOV> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test functionality of a priori covariance weights");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{

		set_test_name("Testing if point covariance given by weight can be recovered");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/apricov.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(ApriCov::APRICOV_recovering_point_covariance);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		//const TLGCData& dataset = calcul.getData();
		// TODO repair copy constructor for the member that stores the apriori covariance matrix
		LGCAdjustablePoint& P1 = projTest->getPoints().getObject("P1");

		int first = P1.getFirstUidx();
		std::vector<int> P1_idx({first, first + 1, first + 2});
		TSparseMatrix fullCovar = *projTest.get()->getCovMatByConst();
		TDenseMatrix retrievedCovar = fullCovar.toDense()(P1_idx, P1_idx);
		TDenseMatrix suppliedCovar = P1.getApriCovar();

		ensure_equals("Retrieved Covariance does not match supplied covariance", retrievedCovar.isApprox(suppliedCovar), true);

	}
	template<>
	template<>
	void object::test<2>()
	{

		set_test_name("Testing if point covariance given by weight can be recovered");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/apricov.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(ApriCov::APRICOV_comparison_with_obsxyz);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		// TODO repair copy constructor for the member that stores the apriori covariance matrix
		LGCAdjustablePoint& P1 = projTest->getPoints().getObject("P1");
		// final estimated position of P1 should be at 0,0,0 because apri covariance and obsxyz cancel each other

		TPositionVector result = P1.getEstimatedValue();
		TVector aux(3);
		aux << result.getX(), result.getY(), result.getZ();
		double resNorm = aux.norm();

		ensure_equals("Estimated position of P1 should be at (0,0,0)", resNorm < 1e-12, true);

	}

	template<>
	template<>
	void object::test<3>()
	{

		set_test_name("Testing if frame covariance given by weight can be recovered");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/apricov.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(ApriCov::APRICOV_recovering_frame_covariance);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		//const TLGCData& dataset = calcul.getData();
		// TODO repair copy constructor for the member that stores the apriori covariance matrix
		
		TSparseMatrix fullCovar = *projTest.get()->getCovMatByConst();
		// test first frame, only translations
		TDataTreeIterator it = projTest.get()->getTree().begin();
		it++;
		TAdjustableHelmertTransformation &trafo = it.node->data.get()->frame;
		int first = trafo.getFirstUidx();
		std::vector<int> idx;
		for (int j = 0; j < trafo.getNumUnkn(); j++)
		{
			idx.push_back(j + first);
		}
		std::for_each(idx.begin(), idx.end(), [first](int x) { return x + first; });
		TDenseMatrix retrievedCovar = fullCovar.toDense()(idx,idx);
		TDenseMatrix suppliedCovar = trafo.getApriCovar();
		//std::cout << "supplied covar =" << std::endl << suppliedCovar << std::endl;
		//std::cout << "retrieved covar =" << std::endl << retrievedCovar << std::endl;
		ensure_equals("Retrieved Covariance does not match supplied covariance", retrievedCovar.isApprox(suppliedCovar), true);

		// test second frame, translations and rotations mixed
		it++;
		TAdjustableHelmertTransformation &trafo2 = it.node->data.get()->frame;
		int first2 = trafo2.getFirstUidx();
		std::vector<int> idx2;
		for (int j = 0; j < trafo2.getNumUnkn(); j++)
		{
			idx2.push_back(j + first2);
		}
		TDenseMatrix retrievedCovar2 = fullCovar.toDense()(idx2,idx2);
		TDenseMatrix suppliedCovar2 = trafo2.getApriCovar();

		ensure_equals("Retrieved Covariance does not match supplied covariance", retrievedCovar2.isApprox(suppliedCovar2), true);

	}
	template<>
	template<>
	void object::test<4>()
	{
		set_test_name("Testing if frame covariance given by weight can be recovered, 6 degrees of freedom");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/apricov.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(ApriCov::APRICOV_recovering_frame_covariance_2);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		//const TLGCData& dataset = calcul.getData();
		
		TSparseMatrix fullCovar = *projTest.get()->getCovMatByConst();
		TDataTreeIterator it = projTest.get()->getTree().begin();
		it++;
		TAdjustableHelmertTransformation &trafo = it.node->data.get()->frame;
		int first = trafo.getFirstUidx();
		std::vector<int> idx;
		for (int j = 0; j < trafo.getNumUnkn(); j++)
		{
			idx.push_back(j + first);
		}
		std::for_each(idx.begin(), idx.end(), [first](int x) { return x + first; });
		TDenseMatrix retrievedCovar = fullCovar.toDense()(idx,idx);
		TDenseMatrix suppliedCovar = trafo.getApriCovar();
		TVector result(7);
		result << trafo.getEstTranslation(0).getMetresValue(), trafo.getEstTranslation(1).getMetresValue(), trafo.getEstTranslation(2).getMetresValue(),
			trafo.getEstRotation(0).getGonsValue(), trafo.getEstRotation(1).getGonsValue(), trafo.getEstRotation(2).getGonsValue(), trafo.getEstScale();
		TVector expectedResult(7);
		expectedResult << 1, 2, 3, 4, 5, 6, 1;
		ensure_equals("Estimate does not match provisional values.", result.isApprox(expectedResult), true);
		ensure_equals("Retrieved Covariance does not match supplied covariance", retrievedCovar.isApprox(suppliedCovar), true);
		
	}
	template<>
	template<>
	void object::test<5>()
	{
		set_test_name("Testing compatibility of slave feature and apriCov feature for frames.");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/apricov.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(ApriCov::APRICOV_slave_vs_apriCov);

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		
		TSparseMatrix fullCovar = *projTest.get()->getCovMatByConst();
		TDataTreeIterator it = projTest.get()->getTree().begin();
		it++;
		TAdjustableHelmertTransformation &trafo1 = it.node->data.get()->frame;
		it++;
		TAdjustableHelmertTransformation &trafo2 = it.node->data.get()->frame;
		it++;
		TAdjustableHelmertTransformation &trafo3 = it.node->data.get()->frame;
		it++;
		TAdjustableHelmertTransformation &trafo4 = it.node->data.get()->frame;
		// first slave then apricov
		int first1 = trafo1.getFirstUidx();
		std::vector<int> idx1;
		for (int j = 0; j < trafo1.getNumUnkn(); j++)
		{
			idx1.push_back(j + first1);
		}
		std::for_each(idx1.begin(), idx1.end(), [first1](int x) { return x + first1; });
		TDenseMatrix retrievedCovar1 = fullCovar.toDense()(idx1, idx1);
		TDenseMatrix suppliedCovar1 = trafo1.getApriCovar();	
		// get the retrieved covar from the second frame
		int first2 = trafo2.getFirstUidx();
		std::vector<int> idx2;
		for (int j = 0; j < trafo2.getNumUnkn(); j++)
		{
			idx2.push_back(j + first2);
		}
		std::for_each(idx2.begin(), idx2.end(), [first2](int x) { return x + first2; });
		TDenseMatrix retrievedCovar2 = fullCovar.toDense()(idx1, idx1);
		ensure_equals("Retrieved Covariance does not match supplied covariance", retrievedCovar1.isApprox(suppliedCovar1), true);
		ensure_equals("Retrieved Covariance of second frame needs to match covar from first frame due to slave constraint", retrievedCovar1.isApprox(retrievedCovar2), true);

		// first apricov then slave
		int first3 = trafo3.getFirstUidx();
		std::vector<int> idx3;
		for (int j = 0; j < trafo3.getNumUnkn(); j++)
		{
			idx3.push_back(j + first3);
		}
		std::for_each(idx3.begin(), idx3.end(), [first3](int x) { return x + first3; });
		TDenseMatrix retrievedCovar3 = fullCovar.toDense()(idx3, idx3);
		TDenseMatrix suppliedCovar4 = trafo4.getApriCovar();	
		// get the retrieved covar from the second frame
		int first4 = trafo4.getFirstUidx();
		std::vector<int> idx4;
		for (int j = 0; j < trafo4.getNumUnkn(); j++)
		{
			idx4.push_back(j + first4);
		}
		std::for_each(idx4.begin(), idx4.end(), [first4](int x) { return x + first4; });
		TDenseMatrix retrievedCovar4 = fullCovar.toDense()(idx4, idx4);
		ensure_equals("Retrieved Covariance does not match supplied covariance", retrievedCovar3.isApprox(suppliedCovar4), true);
		ensure_equals("Retrieved Covariance of second frame needs to match covar from first frame due to slave constraint", retrievedCovar3.isApprox(retrievedCovar4), true);

	}


};
