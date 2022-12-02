#include "Dummy.h"
#include "DummyImpl.h"

#include <iostream>


// constructor
void Dummy::DummyImpl::setNumber(int k)
{
	number = k;
}
void Dummy::DummyImpl::printResult()
{
	compute();
	std::cout << "square of " << number<< "="<< result << std::endl;
}
void Dummy::DummyImpl::compute()
{
	result = number*number;
}

void Dummy::setNumber(int k)
{
	impl_->setNumber(k);
}
void Dummy::printResult()
{
	impl_->printResult();
}

Dummy::Dummy() : impl_(new DummyImpl()){}
Dummy::~Dummy() = default;
