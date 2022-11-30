#ifndef DUMMY
#define DUMMY



// STL
#include <memory>

class Dummy
{
public:
	// constructor
	Dummy();
	~Dummy();
	void setNumber(int);
	void printResult();

private:	
	class DummyImpl;
	std::shared_ptr<DummyImpl> impl_;
};

#endif
