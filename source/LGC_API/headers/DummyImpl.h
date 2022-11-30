#ifndef DUMMYIMPL
#define DUMMYIMPL



// STL
#include <Eigen/Dense>
#include "Dummy.h"

class DummyImpl
{
public:
	// constructor
	DummyImpl();
	void setNumber(int);
	void printResult();

private:	
	void compute();
	int number;
	int result;
};

#endif
