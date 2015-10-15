#include "Quad.h"

#include <iostream>

#if _DEBUG && USE_QUAD

Quad::Quad()
{
}

Quad::Quad(_Quad other)
{
	doubleValue = (double) other;
	value = other;
}

Quad::Quad(double other)
{
	doubleValue = other;
	value = other;
}

Quad::Quad(int other)
{
	doubleValue = other;
	value = other;
}

Quad::Quad(const Quad& other)
{
	doubleValue = (double) other.value;
	value = other.value;
}

Quad& Quad::operator =(const Quad& other)
{
	doubleValue = (double) other.value;
	value = other.value;
	return *this;
}

Quad& Quad::operator =(_Quad other)
{
	doubleValue = (double) other;
	value = other;
	return *this;
}

Quad& Quad::operator =(double other)
{
	doubleValue = other;
	value = other;
	return *this;
}

Quad& Quad::operator =(int other)
{
	doubleValue = other;
	value = other;
	return *this;
}

Quad operator +(const Quad& first, const Quad& second)
{
	Quad res(first.value + second.value);

	return res;
}

Quad operator -(const Quad& first, const Quad& second)
{
	Quad res(first.value - second.value);

	return res;
}

Quad operator *(const Quad& first, const Quad& second)
{
	Quad res(first.value * second.value);

	return res;
}

Quad operator /(const Quad& first, const Quad& second)
{
	Quad res(first.value / second.value);

	return res;
}

Quad operator +(double first, const Quad& second)
{
	Quad res(first + second.value);

	return res;
}

Quad operator -(double first, const Quad& second)
{
	Quad res(first - second.value);

	return res;
}

Quad operator *(double first, const Quad& second)
{
	Quad res(first * second.value);

	return res;
}

Quad operator /(double first, const Quad& second)
{
	Quad res(first / second.value);

	return res;
}

Quad operator +(const Quad& first, double second)
{
	Quad res(first.value + second);

	return res;
}

Quad operator -(const Quad& first, double second)
{
	Quad res(first.value - second);

	return res;
}

Quad operator *(const Quad& first, double second)
{
	Quad res(first.value * second);

	return res;
}

Quad operator /(const Quad& first, double second)
{
	Quad res(first.value / second);

	return res;
}

Quad operator +(int first, const Quad& second)
{
	Quad res(first + second.value);

	return res;
}

Quad operator -(int first, const Quad& second)
{
	Quad res(first - second.value);

	return res;
}

Quad operator *(int first, const Quad& second)
{
	Quad res(first * second.value);

	return res;
}

Quad operator /(int first, const Quad& second)
{
	Quad res(first / second.value);

	return res;
}

Quad operator +(const Quad& first, int second)
{
	Quad res(first.value + second);

	return res;
}

Quad operator -(const Quad& first, int second)
{
	Quad res(first.value - second);

	return res;
}

Quad operator *(const Quad& first, int second)
{
	Quad res(first.value * second);

	return res;
}

Quad operator /(const Quad& first, int second)
{
	Quad res(first.value / second);

	return res;
}

Quad operator +(_Quad first, const Quad& second)
{
	Quad res(first + second.value);

	return res;
}

Quad operator -(_Quad first, const Quad& second)
{
	Quad res(first - second.value);

	return res;
}

Quad operator *(_Quad first, const Quad& second)
{
	Quad res(first * second.value);

	return res;
}

Quad operator /(_Quad first, const Quad& second)
{
	Quad res(first / second.value);

	return res;
}

Quad operator +(const Quad& first, _Quad second)
{
	Quad res(first.value + second);

	return res;
}

Quad operator -(const Quad& first, _Quad second)
{
	Quad res(first.value - second);

	return res;
}

Quad operator *(const Quad& first, _Quad second)
{
	Quad res(first.value * second);

	return res;
}

Quad operator /(const Quad& first, _Quad second)
{
	Quad res(first.value / second);

	return res;
}

Quad& Quad::operator +=(const Quad& second)
{
	value += second.value;
	doubleValue += (double) second.value;

	return *this;
}

Quad& Quad::operator -=(const Quad& second)
{
	value -= second.value;
	doubleValue -= (double) second.value;

	return *this;
}

Quad& Quad::operator *=(const Quad& second)
{
	value *= second.value;
	doubleValue *= (double) second.value;

	return *this;
}

Quad& Quad::operator /=(const Quad& second)
{
	value /= second.value;
	doubleValue /= (double) second.value;

	return *this;
}

Quad& Quad::operator +=(double second)
{
	value += second;
	doubleValue += second;

	return *this;
}

Quad& Quad::operator -=(double second)
{
	value -= second;
	doubleValue -= second;

	return *this;
}

Quad& Quad::operator *=(double second)
{
	value *= second;
	doubleValue *= second;

	return *this;
}

Quad& Quad::operator /=(double second)
{
	value /= second;
	doubleValue /= second;

	return *this;
}

Quad& Quad::operator +=(int second)
{
	value += second;
	doubleValue += second;

	return *this;
}

Quad& Quad::operator -=(int second)
{
	value -= second;
	doubleValue -= second;

	return *this;
}

Quad& Quad::operator *=(int second)
{
	value *= second;
	doubleValue *= second;

	return *this;
}

Quad& Quad::operator /=(int second)
{
	value /= second;
	doubleValue /= second;

	return *this;
}

Quad& Quad::operator +=(_Quad second)
{
	value += second;
	doubleValue += (double) second;

	return *this;
}

Quad& Quad::operator -=(_Quad second)
{
	value -= second;
	doubleValue -= (double) second;

	return *this;
}

Quad& Quad::operator *=(_Quad second)
{
	value *= second;
	doubleValue *= (double) second;

	return *this;
}

Quad& Quad::operator /=(_Quad second)
{
	value /= second;
	doubleValue /= (double) second;

	return *this;
}

bool operator <(const Quad& first, const Quad& second)
{
	return first.value < second.value;
}

bool operator >(const Quad& first, const Quad& second)
{
	return first.value > second.value;
}

bool operator ==(const Quad& first, const Quad& second)
{
	return first.value == second.value;
}

bool operator !=(const Quad& first, const Quad& second)
{
	return first.value != second.value;
}

bool operator <=(const Quad& first, const Quad& second)
{
	return first.value <= second.value;
}

bool operator >=(const Quad& first, const Quad& second)
{
	return first.value >= second.value;
}

bool operator <(double first, const Quad& second)
{
	return first < second.value;
}

bool operator >(double first, const Quad& second)
{
	return first > second.value;
}

bool operator ==(double first, const Quad& second)
{
	return first == second.value;
}

bool operator !=(double first, const Quad& second)
{
	return first != second.value;
}

bool operator <=(double first, const Quad& second)
{
	return first <= second.value;
}

bool operator >=(double first, const Quad& second)
{
	return first >= second.value;
}

bool operator <(const Quad& first, double second)
{
	return first.value < second;
}

bool operator >(const Quad& first, double second)
{
	return first.value > second;
}

bool operator ==(const Quad& first, double second)
{
	return first.value == second;
}

bool operator !=(const Quad& first, double second)
{
	return first.value != second;
}

bool operator <=(const Quad& first, double second)
{
	return first.value <= second;
}

bool operator >=(const Quad& first, double second)
{
	return first.value >= second;
}

bool operator <(int first, const Quad& second)
{
	return first < second.value;
}

bool operator >(int first, const Quad& second)
{
	return first > second.value;
}

bool operator ==(int first, const Quad& second)
{
	return first == second.value;
}

bool operator !=(int first, const Quad& second)
{
	return first != second.value;
}

bool operator <=(int first, const Quad& second)
{
	return first <= second.value;
}

bool operator >=(int first, const Quad& second)
{
	return first >= second.value;
}

bool operator <(const Quad& first, int second)
{
	return first.value < second;
}

bool operator >(const Quad& first, int second)
{
	return first.value > second;
}

bool operator ==(const Quad& first, int second)
{
	return first.value == second;
}

bool operator !=(const Quad& first, int second)
{
	return first.value != second;
}

bool operator <=(const Quad& first, int second)
{
	return first.value <= second;
}

bool operator >=(const Quad& first, int second)
{
	return first.value >= second;
}

bool operator <(_Quad first, const Quad& second)
{
	return first < second.value;
}

bool operator >(_Quad first, const Quad& second)
{
	return first > second.value;
}

bool operator ==(_Quad first, const Quad& second)
{
	return first == second.value;
}

bool operator !=(_Quad first, const Quad& second)
{
	return first != second.value;
}

bool operator <=(_Quad first, const Quad& second)
{
	return first <= second.value;
}

bool operator >=(_Quad first, const Quad& second)
{
	return first >= second.value;
}

bool operator <(const Quad& first, _Quad second)
{
	return first.value < second;
}

bool operator >(const Quad& first, _Quad second)
{
	return first.value > second;
}

bool operator ==(const Quad& first, _Quad second)
{
	return first.value == second;
}

bool operator !=(const Quad& first, _Quad second)
{
	return first.value != second;
}

bool operator <=(const Quad& first, _Quad second)
{
	return first.value <= second;
}

bool operator >=(const Quad& first, _Quad second)
{
	return first.value >= second;
}

Quad Quad::operator -()
{
	return Quad(-this->value);
}

Quad::operator _Quad() const
{
	return value;
}

Quad::operator _Quad*()
{
	return &value;
}

std::ostream& operator<<(std::ostream& stream, const TReal & value) 
{
    return stream << (long double)value.value;
}

std::istream& operator>>(std::istream& stream, TReal & value) 
{
	long double foo;
	stream >> foo;
	value.value = foo;
	value.doubleValue = foo;
	return stream;
}


#elif USE_QUAD

std::ostream& operator<<(std::ostream& stream, const TReal & value) 
{
    return stream << (long double)value;
}

std::istream& operator>>(std::istream& stream, TReal & value) 
{
	long double foo;
	stream >> foo;
	value = foo;
	return stream;
}

#endif
