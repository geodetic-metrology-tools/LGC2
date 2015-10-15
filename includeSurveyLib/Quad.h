#ifndef QUAD_H
#define QUAD_H

#include <limits>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <iosfwd>

#if _MSC_VER
#include <float.h>
#else
#include <cmath>
#endif

// explicitly ignore unused parameters
template <typename T>
void ignoring(T &&)
{ }

#define USE_QUAD __INTEL_COMPILER && ENABLE_QUAD

#if USE_QUAD
// We need to bring a handful of Intel _Quad functions in:
extern "C" {
	_Quad __ceilq(_Quad);
	_Quad __floorq(_Quad);
	_Quad __fmodq(_Quad, _Quad);
	_Quad __fabsq(_Quad);
	
	_Quad __expq(_Quad);
	_Quad __frexpq(_Quad, int *);
	_Quad __ldexpq(_Quad, int);
	_Quad __modfq(_Quad, _Quad *);
	_Quad __logq(_Quad);
	_Quad __log10q(_Quad);

	_Quad powq(_Quad, _Quad);
	_Quad __sqrtq(_Quad);
	
	_Quad __cosq(_Quad);
	_Quad __sinq(_Quad);
	_Quad __tanq(_Quad);
	_Quad __acosq(_Quad);
	_Quad __asinq(_Quad);
	_Quad __atanq(_Quad);
	_Quad __atan2q(_Quad, _Quad);

	_Quad __coshq(_Quad);
	_Quad __sinhq(_Quad);
	_Quad __tanhq(_Quad);	
}


//// And define useful overloads, as it's cumbersome to use __fabsq...
// NOTE: We could have used #define to achieve the same goal
//   However, using inline functions gives the same performance, 
//   and gives more flexibility (overloads) - see the QuadDebug.h
inline _Quad ceilq(_Quad _X) {return (__ceilq(_X)); }
inline _Quad floorq(_Quad _X) {return (__floorq(_X)); }
inline _Quad fmodq(_Quad _X, _Quad _Y) {return (__fmodq(_X, _Y)); }
inline _Quad fabsq(_Quad _X) {return (__fabsq(_X)); }

inline _Quad expq(_Quad _X) {return (__expq(_X)); }
inline _Quad frexpq(_Quad _X, int * _Y) {return (__frexpq(_X, _Y)); }
inline _Quad ldexpq(_Quad _X, int _Y) {return (__ldexpq(_X, _Y)); }
inline _Quad modfq(_Quad _X, _Quad * _Y) {return (__modfq(_X, _Y)); }
inline _Quad logq(_Quad _X) {return (__logq(_X)); }
inline _Quad log10q(_Quad _X) {return (__log10q(_X)); }

inline _Quad powq(_Quad _X, _Quad _Y) {return (powq(_X, _Y)); }
inline _Quad sqrtq(_Quad _X) {return (__sqrtq(_X)); }

inline _Quad cosq(_Quad _X) {return (__cosq(_X)); }
inline _Quad sinq(_Quad _X) {return (__sinq(_X)); }
inline _Quad tanq(_Quad _X) {return (__tanq(_X)); }
inline _Quad acosq(_Quad _X) {return (__acosq(_X)); }
inline _Quad asinq(_Quad _X) {return (__asinq(_X)); }
inline _Quad atanq(_Quad _X) {return (__atanq(_X)); }
inline _Quad atan2q(_Quad _X, _Quad _Y) {return (__atan2q(_X, _Y)); }
inline _Quad coshq(_Quad _X) {return (__coshq(_X)); }
inline _Quad sinhq(_Quad _X) {return (__sinhq(_X)); }
inline _Quad tanhq(_Quad _X) {return (__tanhq(_X)); }

#define LITERAL(x) x ## q

#else /* NOT INTEL COMPILER */

// Here ordinary #defines will do. We don't need any flexibility
#define ceilq ceil
#define floorq floor
#define fmodq fmod
#define fabsq fabs

#define expq exp
#define frexpq frexp
#define ldexpq ldexp
#define modfq modf
#define logq log
#define log10q log10

#define powq pow
#define sqrtq sqrt
#define pow2q pow2

#define cosq cos
#define sinq sin
#define tanq tan
#define acosq acos
#define asinq asin
#define atanq atan
#define atan2q atan2

#define coshq cosh
#define sinhq sinh
#define tanhq tanh

#define LITERAL(x) x

#endif


#if _DEBUG && USE_QUAD

class Quad
{
public:
	Quad(_Quad other);
	Quad(double other);
	Quad(int other);
	Quad(const Quad& other);
	Quad();

	Quad& operator =(const Quad& other);
	Quad& operator =(_Quad other);
	Quad& operator =(double other);
	Quad& operator =(int other);

	friend Quad operator +(const Quad& first, const Quad& second);
	friend Quad operator -(const Quad& first, const Quad& second);
	friend Quad operator *(const Quad& first, const Quad& second);
	friend Quad operator /(const Quad& first, const Quad& second);

	friend Quad operator +(double first, const Quad& second);
	friend Quad operator -(double first, const Quad& second);
	friend Quad operator *(double first, const Quad& second);
	friend Quad operator /(double first, const Quad& second);

	friend Quad operator +(const Quad& first, double second);
	friend Quad operator -(const Quad& first, double second);
	friend Quad operator *(const Quad& first, double second);
	friend Quad operator /(const Quad& first, double second);

	friend Quad operator +(int first, const Quad& second);
	friend Quad operator -(int first, const Quad& second);
	friend Quad operator *(int first, const Quad& second);
	friend Quad operator /(int first, const Quad& second);

	friend Quad operator +(const Quad& first, int second);
	friend Quad operator -(const Quad& first, int second);
	friend Quad operator *(const Quad& first, int second);
	friend Quad operator /(const Quad& first, int second);

	friend Quad operator +(_Quad first, const Quad& second);
	friend Quad operator -(_Quad first, const Quad& second);
	friend Quad operator *(_Quad first, const Quad& second);
	friend Quad operator /(_Quad first, const Quad& second);

	friend Quad operator +(const Quad& first, _Quad second);
	friend Quad operator -(const Quad& first, _Quad second);
	friend Quad operator *(const Quad& first, _Quad second);
	friend Quad operator /(const Quad& first, _Quad second);
	
	friend bool operator <(const Quad& first, const Quad& second);
	friend bool operator >(const Quad& first, const Quad& second);
	friend bool operator ==(const Quad& first, const Quad& second);
	friend bool operator !=(const Quad& first, const Quad& second);
	friend bool operator <=(const Quad& first, const Quad& second);
	friend bool operator >=(const Quad& first, const Quad& second);
	
	friend bool operator <(double first, const Quad& second);
	friend bool operator >(double first, const Quad& second);
	friend bool operator ==(double first, const Quad& second);
	friend bool operator !=(double first, const Quad& second);
	friend bool operator <=(double first, const Quad& second);
	friend bool operator >=(double first, const Quad& second);
	
	friend bool operator <(const Quad& first, double second);
	friend bool operator >(const Quad& first, double second);
	friend bool operator ==(const Quad& first, double second);
	friend bool operator !=(const Quad& first, double second);
	friend bool operator <=(const Quad& first, double second);
	friend bool operator >=(const Quad& first, double second);
	
	friend bool operator <(int first, const Quad& second);
	friend bool operator >(int first, const Quad& second);
	friend bool operator ==(int first, const Quad& second);
	friend bool operator !=(int first, const Quad& second);
	friend bool operator <=(int first, const Quad& second);
	friend bool operator >=(int first, const Quad& second);
	
	friend bool operator <(const Quad& first, int second);
	friend bool operator >(const Quad& first, int second);
	friend bool operator ==(const Quad& first, int second);
	friend bool operator !=(const Quad& first, int second);
	friend bool operator <=(const Quad& first, int second);
	friend bool operator >=(const Quad& first, int second);
	
	friend bool operator <(_Quad first, const Quad& second);
	friend bool operator >(_Quad first, const Quad& second);
	friend bool operator ==(_Quad first, const Quad& second);
	friend bool operator !=(_Quad first, const Quad& second);
	friend bool operator <=(_Quad first, const Quad& second);
	friend bool operator >=(_Quad first, const Quad& second);
	
	friend bool operator <(const Quad& first, _Quad second);
	friend bool operator >(const Quad& first, _Quad second);
	friend bool operator ==(const Quad& first, _Quad second);
	friend bool operator !=(const Quad& first, _Quad second);
	friend bool operator <=(const Quad& first, _Quad second);
	friend bool operator >=(const Quad& first, _Quad second);

	Quad operator -();

	Quad& operator +=(const Quad& second);
	Quad& operator -=(const Quad& second);
	Quad& operator *=(const Quad& second);
	Quad& operator /=(const Quad& second);

	Quad& operator +=(double second);
	Quad& operator -=(double second);
	Quad& operator *=(double second);
	Quad& operator /=(double second);

	Quad& operator +=(int second);
	Quad& operator -=(int second);
	Quad& operator *=(int second);
	Quad& operator /=(int second);

	Quad& operator +=(_Quad second);
	Quad& operator -=(_Quad second);
	Quad& operator *=(_Quad second);
	Quad& operator /=(_Quad second);

	operator _Quad() const;
	operator _Quad*();

	double doubleValue;
	_Quad value;
};

// This function works with _Quad pointer. Thus a special overload for Quad is needed
inline _Quad modfq(const Quad & _X, Quad * _Y)
{
    // Do the calculations on proper _Quads.
    _Quad result = modfq(_X.value, &_Y->value);
    // Synchronize the double value in Y
    _Y->doubleValue = _Y->value;
    return result;
}


typedef Quad TReal;
std::ostream& operator<<(std::ostream& stream, const TReal & value);
std::istream& operator>>(std::istream& stream, TReal & value);

#elif USE_QUAD

typedef _Quad TReal;
std::ostream& operator<<(std::ostream& stream, const TReal & value);
std::istream& operator>>(std::istream& stream, TReal & value);

#else /* NOT INTEL COMPILER */

typedef double TReal;

#endif

static inline bool notZero(TReal v) {
	return (fabsq(v) > std::numeric_limits<TReal>::epsilon());
}

// squaring values is done frequently
static inline TReal pow2(TReal v) {
	return v*v;
}

static inline bool equalq(TReal a, TReal b) {
	return (fabsq(a-b) < std::numeric_limits<TReal>::epsilon());
}

#endif  //QUAD_H
