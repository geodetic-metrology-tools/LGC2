#ifndef TV_ABSTRACT_ALGORITHM
#define TV_ABSTRACT_ALGORITHM

class TLGCData;

/*!
\ingroup AbractAlgorithm
\brief Abstract class to run the chosen algorithm process
*/
class TVAbractAlgorithm{

public:
	
	/// This virtual base class destructor does nothing since this is an interface.
	~TVAbractAlgorithm() {}

	virtual bool run(TLGCData& data, int fMaxIterations) = 0;

};

#endif