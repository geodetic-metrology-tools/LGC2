/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_EVALUATOR
#define TLS_EVALUATOR


//SURVEYLIB
#include <TALSComputer.h>
#include <Behavior.h>
//LGC
#include <TVAbstractAlgorithm.h>
#include <TLSResultsMatricesExtractor.h>
#include <TLibrCnstrGenerator.h>
#include <TLSConsistencyCheck.h>
#include "TLSInputMatricesFiller.h"

#include <Eigen/Dense>

class TLSInputMatricesFiller;
class TLSResultsMatrices;

/*!
\ingroup Evaluator
\brief For evaluating the mathematical models at arbitrary values
*/
class TLSEvaluator 
{

public:
	TLSEvaluator(std::shared_ptr<TLGCData> data);
	Eigen::VectorXd evaluate(Eigen::VectorXd parameter);

private:
	// a copy of data for maipulating parameter and observation values.
	std::shared_ptr<TLGCData> fData;
	//TLGCData fData;
	TLSInputMatricesFiller fMatFiller;
	void setParameters(Eigen::VectorXd para);
	// void setPointParams(Eigen::VectorXd para);
	// void setAngleParams(Eigen::VectorXd para);
	// void setPlaneParams(Eigen::VectorXd para);
	// void setLengthParams(Eigen::VectorXd para);
	// void setTransformationParams(Eigen::VectorXd para);
	// void setLineParams(Eigen::VectorXd para);




};

#endif
