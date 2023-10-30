
#ifndef _LGCController_H_
#define _LGCController_H_

#include "LGCController.h"
#include "TAStreamFormatter.h"
#include <iostream>

// STL
#include <Eigen/Dense>

#include <Behavior.h>
#include <TLGCData.h>
#include <TLSAlgorithm.h>
#include <TReader.h>

#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"
#include "TInputFileWriter.h"
#include "TSimFileWriter.h"
#include "TStreamFormatterFactory.h"
#include <iomanip> // put_time
#include <chrono>
#include "TLSEvaluator.h"



class LGCController
{
public:
	LGCController(const std::string path);
	Eigen::VectorXd getParameters();
	void setParameters(Eigen::VectorXd par);
	Eigen::VectorXd getMisclosure(Eigen::VectorXd par);
	Eigen::VectorXd getConstraintMisclosure(Eigen::VectorXd par);
	Eigen::SparseMatrix<double> getFirstDesignMatrix(Eigen::VectorXd par);
	Eigen::SparseMatrix<double> getFirstConstraintDesignMatrix(Eigen::VectorXd par);
	Eigen::SparseMatrix<double> getSecondDesignMatrix(Eigen::VectorXd par);
	Eigen::SparseMatrix<double> getPv(Eigen::VectorXd par);

private:
	void initialize();
	std::string inputFilePath;
	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> fProject;
	std::shared_ptr<TLSEvaluator> fEvaluator;
};


#endif
