#ifndef TV_ABSTRACT_ALGORITHM
#define TV_ABSTRACT_ALGORITHM

class TLGCData;

/*!
\ingroup AbractAlgorithm
\brief Abstract class to run the chosen algorithm process
*/
class TVAbractAlgorithm{

public:
	TVAbractAlgorithm() : resultMatrices(nullptr){};

	/// This virtual base class destructor does nothing since this is an interface.
	~TVAbractAlgorithm() {}


	/// abstarct funtion to run the calculation
	virtual bool run(TLGCData& data, int fMaxIterations) = 0;

	TLSResultsMatrices* resultMatrices;

};

#endif