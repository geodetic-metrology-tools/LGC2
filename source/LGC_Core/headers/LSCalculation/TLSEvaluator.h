/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TLS_EVALUATOR
#define TLS_EVALUATOR

// SURVEYLIB
#include <Behavior.h>
#include <TALSComputer.h>
// LGC
#include <memory>

#include <Eigen/Dense>

#include <TLSConsistencyCheck.h>
#include <TLSResultsMatricesExtractor.h>
#include <TVAbstractAlgorithm.h>

#include "TLSInputMatricesFiller.h"

/*!
\ingroup Evaluator
\brief For evaluating the mathematical models at arbitrary values
original LS problem: min |PV|^2 s.t. F(x,L+V)=0 (& C(x)=0)
for iterative GN method in LGC, inputMatrixFiller evaluates
main parts:
- "A-matrix" A = dF/dx(x,L)
- constraint A matrix A2 = dC/dx(x)
- "Misclosure" W = F(x,L)(=F(x)-L for the relevant parametric case)
- constraint misclosure C = F(x)
*/

class TLSEvaluator
{
public:
	// construct with a prepared project data
	TLSEvaluator(std::shared_ptr<TLGCData> data);
	// construct with a lgc input file
	TLSEvaluator(std::stringstream& fileStream);

	~TLSEvaluator();

	// setting and getting the parameter vector
	void setParameters(const Eigen::VectorXd &para);
	Eigen::VectorXd getEstParams() const;
	bool testParameterSetAndGet();
	bool testEvaluate();
	// compute finite differenc matrices, default finite difference step = 1e-6 (mainly used for testing)
	Eigen::MatrixXd getFiniteDifferenceA(double finiteDiffEpsilon = 1e-6);

	// this will attempt a normal LGC solve operation
	bool tryLGCSolve(TVector& solution);
	// triggers evaluation at the current set parameter
	bool evaluate();
	// functions evaluations at current parameter (whch can be controlled by get/set)
	const Eigen::VectorXd &getMisclosure() const;
	const TSparseMatrix &getAMatrix() const;
	const TSparseMatrix &getBMatrix() const;
	const TSparseMatrix &getInvBMatrix() const;
	const Eigen::VectorXd &getConstraintMisclosure() const;
	const TSparseMatrix &getA2Matrix() const;
	const TSparseMatrix &getPMatrix() const;
	const UEOIndices getIndices() const { return fData->fUEOIndices; };

	// observation index to input file line number mapping
	const std::map<int, int> &getObsIndexToLineNumber() const { return fData->fObsIndexToLineNumber; }

	// access to project data (for Python bindings)
	const TLGCData &getData() const { return *fData; }

	// get the whole inputmatrices object
	const TLSInputMatrices &getInputMatricesRef() const;
	// get a copy
	TLSInputMatrices getInputMatricesCopy();

private:
	// indicates that results are ready for access
	bool isUptoDate{false};
	// helper method
	void checkIsUptoDate() const;
	
	// a pointer to the data for manipulating parameters
	std::shared_ptr<TLGCData> fData;
	std::unique_ptr<TLSInputMatricesFiller> fMatFiller;
	std::unique_ptr<TLSInputMatrices> iMat;

};

#endif