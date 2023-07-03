/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_LMSOLVER
#define TLS_LMSOLVER


//LGC
#include <TLSEvaluator.h>

#include <Eigen/Dense>


/*!
\ingroup LMSolver
\brief Gauss Newton method
*/
class TLSLMSolver
{
public:
	TLSLMSolver(std::shared_ptr<TLGCData> data);
	//~TLSLMSolver();
	Eigen::VectorXd solveLM();

private:
	TLSEvaluator fEvaluator;
	
	// Generic functor
	template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
	struct Functor
	{
		typedef _Scalar Scalar;
		enum
		{
			InputsAtCompileTime = NX,
			ValuesAtCompileTime = NY
		};
		typedef Eigen::Matrix<Scalar, InputsAtCompileTime, 1> InputType;
		typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, 1> ValueType;
		typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, InputsAtCompileTime> JacobianType;

		int m_inputs, m_values;
		TLSEvaluator& f_evaluator;

	//	Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
		Functor(TLSEvaluator evaluator) : m_inputs(evaluator.dimensions.UIndex), m_values(evaluator.dimensions.OIndex), f_evaluator(evaluator) {}
		int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
		{
		//	std::cout << "eval of function" << std::endl;
			f_evaluator.setParameters(x);
			fvec = f_evaluator.getWeightedResidual();
			return 0;
		};
		int df(const Eigen::VectorXd &x, Eigen::MatrixXd &fjac) const
		{
			//std::cout << "start eval of Jacobian" << std::endl;
			f_evaluator.setParameters(x);
			//std::cout << "getting A" << std::endl;
			Eigen::SparseMatrix<double> A = *f_evaluator.getA();
			//std::cout << "A=" << std::endl << A.toDense() << std::endl;
			//std::cout << "getting Binv" << std::endl;
			Eigen::SparseMatrix<double> Binv = *f_evaluator.getBinv();
			//std::cout << "Binv=" << std::endl << Binv.toDense() << std::endl;
			//std::cout << "getting Pv" << std::endl;
			Eigen::SparseMatrix<double> Pv = *f_evaluator.getPv();
			//std::cout << "Pv=" << std::endl << Pv.toDense() << std::endl;
			//std::cout << "diagonal of Pv" << std::endl;
			Eigen::VectorXd diagEntries = Pv.diagonal().cwiseSqrt();
			Eigen::SparseMatrix<double> aux = Binv * A;
			Eigen::MatrixXd diagMat(f_evaluator.dimensions.OIndex, f_evaluator.dimensions.OIndex);
			diagMat.setZero();
			diagMat.diagonal() = diagEntries;
			//std::cout << "diagonal matrix" << std::endl << diagMat << std::endl;

			//std::cout << "getting Binv*A" << std::endl;
			Eigen::SparseMatrix<double> temp = Binv * A;

			//std::cout << "temp=" << std::endl << temp.toDense() << std::endl;
			//std::cout << "scaling with diagmat" << std::endl;
			fjac = -(diagMat.sparseView() * temp).toDense();
			//std::cout << "finish eval of Jacobian" << std::endl;
			//std::cout << fjac << std::endl;
			return 0;
		}



		int inputs() const { return m_inputs; }
		int values() const { return m_values; }
	};


};



#endif
