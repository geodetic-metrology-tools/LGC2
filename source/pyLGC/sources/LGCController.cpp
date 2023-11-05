// class for python LGC wrapper
// for starting it will contain methods
// initialize: with lgc file name string
// set and get Parameter
// evaluate A(1 and 2) matrix, B matrix, P + all misclosures


#include "LGCController.h"

LGCController::LGCController(const std::string path) : inputFilePath(path)
{
	initialize();
}

Eigen::VectorXd LGCController::getMisclosure(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	return fEvaluator->getMisclosure();
}

Eigen::VectorXd LGCController::getConstraintMisclosure(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	return fEvaluator->getConstraintMisclosure();
}

Eigen::SparseMatrix<double> LGCController::getFirstDesignMatrix(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	return fEvaluator->getA();
}

Eigen::SparseMatrix<double> LGCController::getFirstConstraintDesignMatrix(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	return fEvaluator->getA2();
}

Eigen::SparseMatrix<double> LGCController::getSecondDesignMatrix(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	return fEvaluator->getB();
}

Eigen::VectorXd LGCController::getPv(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	return fEvaluator->getPv();
}

Eigen::VectorXd LGCController::getParameters()
{
	Eigen::VectorXd result = fEvaluator->getEstParams();
	return result;
}
void LGCController::setParameters(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
}


void LGCController::initialize()
{
	Behavior successCalculation;
	std::cout << "Creating controller object with LGC input file " << inputFilePath << std::endl;
	std::shared_ptr<TLGCData> project(new TLGCData);
	fProject = project;
	TReader r(fProject);

	fProject->getFileLogger().setOutputfileLocation("C:/Temp/pyLog.txt");
	fProject->getFileLogger().writeReportHeader("Dummy output file python lib");


	std::ifstream inputFileStream(inputFilePath, std::ifstream::in);
	bool succesReading = r.read(inputFileStream);
	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*fProject.get());
	bool configSuccess = analyzer.dataConsistent();
	if (!configSuccess)
		throw std::runtime_error("Error during problem initialization");

	// projectData object is ready now.
	// create the evaluator object
	TLSEvaluator evaluator(fProject);
	fEvaluator = std::make_shared<TLSEvaluator>(evaluator);
	std::cout << "initialization done" << std::endl;
}