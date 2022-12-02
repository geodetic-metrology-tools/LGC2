#ifndef DUMMYIMPL
#define DUMMYIMPL



// STL
#include <Eigen/Dense>
#include "Dummy.h"

class Dummy::DummyImpl
{
public:
	// constructor
	void setNumber(int);
	void printResult();

private:	
	void compute();
	int number;
	int result;
};

#endif
