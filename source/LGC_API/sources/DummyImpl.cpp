#include "Dummy.h"
#include "DummyImpl.h"

#include <iostream>


// constructor
DummyImpl::DummyImpl() : result(0), number(0)
{
	std::cout << "Hello"<< std ::endl;
}
void DummyImpl::setNumber(int k)
{
	number = k;
}
void DummyImpl::printResult()
{
	compute();
	std::cout << "square of " << number<< "="<< result << std::endl;
}
void DummyImpl::compute()
{
	result = number*number;
}

/*
void Dummy::setNumber(int k)
{
	impl_->setNumber(k);
}
void Dummy::printResult()
{
	impl_->printResult();
}

Dummy::Dummy() : impl_(new DummyImpl){}
Dummy::~Dummy() = default;
*/
