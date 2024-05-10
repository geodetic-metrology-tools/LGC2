/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_GAUSSNEWTONSOLVER
#define TLS_GAUSSNEWTONSOLVER


//LGC
#include <TLSEvaluator.h>

#include <Eigen/Dense>


/*!
\ingroup GaussNewtonSolver
\brief Gauss Newton method
*/
struct GNresult: public Serializable
{
	Eigen::VectorXd solution;
	Eigen::VectorXd residual;
	double objective = -1;
	int nIterations = 0;
	bool success = false;
	bool isInLimits = false;
	double sigma0Aposteriori;
	void serialize(SerializerObject::SerializationHelper &obj) const;

//	GNresult() = default;
};
struct solverConfig : public Serializable
{
	// plotLevel
	// 0 -> show no information
	// 1 -> show information at end of iterations
	// 2 -> show information after each step
	int plotLevel = 0;
	bool useArmijo = false;
	bool useLM = false;
	double LMpenalty = 0;
	int maxIter = 0;
	double terminationTol = 0;
	std::vector<int> penalizedIndices;
	virtual void serialize(SerializerObject::SerializationHelper &obj) const override;
	//
	solverConfig(int plotLevel, bool useArmijo, bool useLM, double LMpenalty, int maxIter, double terminationTol) :
		plotLevel(plotLevel), useArmijo(useArmijo), useLM(useLM), LMpenalty(LMpenalty), maxIter(maxIter), terminationTol(terminationTol){};

	solverConfig() = default;
};


class TLSGaussNewtonSolver //public Serializable
{
public:
	TLSGaussNewtonSolver(std::shared_ptr<TLSEvaluator> evaluator);
	//~TLSGaussNewtonSolver();
	// apply GN solver
	GNresult solve();
	void setOption(std::string, bool flag);
	void setOption(std::string, double value);
	void setOption(std::string, int value);
	void setConfig(solverConfig config) { fConfig = config; };
	void resetOptions();


private:
	std::shared_ptr<TLSEvaluator> fEvaluator;
	// solve min ||r+J dx||^2 + sqrt(lambda)||dx||^2, which has normal matrix J^T J +lambda*Id
	Eigen::VectorXd getGNDirection(Eigen::VectorXd r, Eigen::MatrixXd J, double LMLambda);
	// compute armijo stepsize via backtracking
	double backtrackingArmijoStepsize(double sigma0 , Eigen::VectorXd x0, Eigen::VectorXd direction);
	Eigen::SparseMatrix<double> getDiagonalLMScaleFactor(Eigen::SparseMatrix<double>& M);

	solverConfig fConfig;

//	struct
//	{
//		// plotLevel 
//		// 0 -> show no information
//		// 1 -> show information at end of iterations
//		// 2 -> show information after each step
//		int plotLevel;
//		bool useArmijo;
//		bool useLM;
//		double LMpenalty;
//		int maxIter;
//		double terminationTol;
//	} config;


};



#endif
