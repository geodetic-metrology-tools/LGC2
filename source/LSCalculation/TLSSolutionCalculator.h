#ifndef LS_SOLUTION_CALCULATOR
#define LS_SOLUTION_CALCULATOR

class TLGCData;
class TLSInputMatrices;
class TALSComputer;
class TLSResultsMatricesExtractor;
class TLSResultsMatrices;

typedef double TReal;

/*! 
	\ingroup LSCalculation

	\brief Class which is responsible for the LS algorithm and iterating to the solution.
*/
class TLSSolutionCalculator{
	public:
		TLSSolutionCalculator();

		/// Iterates to the solution
		bool	iterate2Solution(TLGCData& data,  int fMaxIterations, TReal convCrit);

		/// Returns number of iterations made
		int		getNumberOfIterations() {return fNumberOfIterations;}

	private:
		int		 fNumberOfIterations;
		bool	 fS0APosterioriVariances;

		void	 computeVarCovarAndReliability(TLGCData* data,
						 TLSInputMatrices* inputMtr,
						 TALSComputer* computer,
						 TLSResultsMatricesExtractor* extractor,
						 TLSResultsMatrices* resultMatrices);

};

#endif