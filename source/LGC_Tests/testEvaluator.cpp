#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <vector>

#include <Behavior.h>
#include <TLGCData.h>
#include <TReader.h>

#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSEvaluator.h"
#include "TLSGaussNewtonSolver.h"
#include "testEvaluator.h"

namespace tut
{
	struct test_Evaluator
	{
		test_Evaluator() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
		std::shared_ptr<TLGCData> projTest;
		TReader reader;
	};
	typedef test_group<test_Evaluator> factory;
	typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test Evaluator");
}

namespace tut
{
		template<>
		template<>
		void object::test<1>()
		{
			set_test_name("Testing standard Evaluator and Gauss newton with armijo stepsize");
			projTest->getFileLogger().setOutputfileLocation("C:/Temp/Evaluator.txt");
			projTest->getFileLogger().writeReportHeader("LGC output file");
		
			std::stringstream infiler(Evaluator::Evaluator_test);
		
			bool succesReading = reader.read(infiler);
			ensure_equals("Reading file successful", succesReading, true);
			TDataAnalyzer analyzer(*projTest.get());
			// Checks whether the data are consistent, assign unknown indices and initialize uninitialized objects (points, lines, planes), reference poiints for certain observations, etc.
			analyzer.dataConsistent();
		
			// create a Evaluator Object
			TLSEvaluator evalObject(projTest);
			std::shared_ptr<TLSEvaluator> evaluator = std::make_shared<TLSEvaluator>(evalObject);
			// create GN solver object
			TLSGaussNewtonSolver gnObject(evaluator);
			// solve the problem
			Eigen::VectorXd firstSolution = gnObject.solve();
			// solution needs to be 0 0 0 0 0 0
			ensure_equals("all parameters need to be 0", firstSolution.norm(), 0);
		
			// deactivate the second B observation
			evaluator->currentMask.equationIndices = std::vector<int>{0, 1, 2, 3, 4, 5};
			Eigen::VectorXd secondSolution = gnObject.solve();
			Eigen::VectorXd expectedSol(6);
			expectedSol << 0, 0, 0, -1, -2, -3;
			ensure_equals("B coordinates should match -1,-2,-3", expectedSol, secondSolution);
		
			// activate all parameters and remove all observations of point B. Should raise an exception
		
			evaluator->currentMask.parameterIndices = std::vector<int>{0, 1, 2, 3, 4, 5};
			evaluator->currentMask.equationIndices = std::vector<int>{0, 1, 2};
			//	try
			//	{
			//		gnObject.solve();
			//		fail("Expect exception because A matrix has not full coulmn rank.");
			//	}
			//	catch (std::exception const &excp)
			//	{
			//		std::string test = excp.what();
			//	}
		}
		
	//	template<>
	//	template<>
	//	void object::test<2>()
	//	{
	//		set_test_name("Testing Armijo stepsize regularization with a simple example (that diverges using full step GN)");
	//		projTest->getFileLogger().setOutputfileLocation("C:/Temp/Evaluator.txt");
	//		projTest->getFileLogger().writeReportHeader("LGC output file");
	//	
	//		std::stringstream infiler(Evaluator::armijo_test);
	//	
	//		bool succesReading = reader.read(infiler);
	//		ensure_equals("Reading file successful", succesReading, true);
	//		TDataAnalyzer analyzer(*projTest.get());
	//		// Checks whether the data are consistent, assign unknown indices and initialize uninitialized objects (points, lines, planes), reference poiints for certain observations, etc.
	//		analyzer.dataConsistent();
	//		// create a Evaluator Object
	//		TLSEvaluator evalObject(projTest);
	//		std::shared_ptr<TLSEvaluator> evaluator = std::make_shared<TLSEvaluator>(evalObject);
	//		// create GN solver object
	//		TLSGaussNewtonSolver gnObject(evaluator);
	//		// solve the problem
	//		Eigen::VectorXd firstSolution = gnObject.solve();
	//		ensure_equals("z component should be estimated at 10", firstSolution(0), 1);
	//	}

}; // namespace tut
