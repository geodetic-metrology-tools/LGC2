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

//class TLSInputMatricesFiller;
//class TLSResultsMatrices;

/*!
\ingroup Evaluator
\brief For evaluating the mathematical models at arbitrary values
*/
class TLSEvaluator 
{

public:
	TLSEvaluator(std::shared_ptr<TLGCData> data);

	~TLSEvaluator();
	
	Eigen::VectorXd evaluateMisclosure(Eigen::VectorXd parameter);
	Eigen::SparseMatrix<double> getA(Eigen::VectorXd parameter);
	Eigen::VectorXd getEstParams();
	void setParameters(Eigen::VectorXd para);
	UEOIndices dimensions;
	void testSetterAndGetter();
	bool testSetterEffect();

private:
	// a copy of data for maipulating parameter and observation values.
	std::shared_ptr<TLGCData> fData;
	//TLGCData fData;
	TLSInputMatricesFiller* fMatFiller;
	TLSInputMatrices* iMat;
	// update iMat objects by evaluating at current parameter
	bool evaluate();
	// indicating thatiMat corrsponds to eval at current parameter. reset to false in any setParam method call
	bool isUptoDate = false;
	// setter helpers
	void setPointParams(Eigen::VectorXd para);
	void setAngleParams(Eigen::VectorXd para);
	void setPlaneParams(Eigen::VectorXd para);
	void setLengthParams(Eigen::VectorXd para);
	void setTransformationParams(Eigen::VectorXd para);
	void setLineParams(Eigen::VectorXd para);
	
	// getter helpers
	void getPointParams(Eigen::VectorXd &para);
	void getAngleParams(Eigen::VectorXd &para);
	void getPlaneParams(Eigen::VectorXd &para);
	void getLengthParams(Eigen::VectorXd &para);
	void getTransformationParams(Eigen::VectorXd &para);
	void getLineParams(Eigen::VectorXd &para);




};

#endif
