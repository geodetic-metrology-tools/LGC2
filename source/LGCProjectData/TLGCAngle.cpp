#include "TLGCAngle.h"
#include "Global.h"

/*

LGC::TAngle::~TAngle() {}

struct LGC::TAngle::D {
	::TAngle a;
};

::TAngle const& LGC::TAngle::getSLAngle()const{
	return d->a;
}

LGC::TAngle::TAngle(const LGC::TAngle& rhs) :
	d(std::unique_ptr<D>(new D)) {
	d->a = rhs.d->a;
}

LGC::TAngle::TAngle() :
	d(std::unique_ptr<D>(new D)) {
}

LGC::TAngle& LGC::TAngle::operator=(const LGC::TAngle& rhs) {
	d->a = rhs.d->a;
	return *this;
}

LGC::TAngle::TAngle(EUnits unit, TReal v) :
	d(std::unique_ptr<D>(new D)) {
	switch(unit) {
		case kRadians:
			d->a = ::TAngle(v);
			break;
		case kDegrees:
			d->a = ::TAngle(v*DEG2RAD);
			break;
		case kGons:
			d->a = ::TAngle(v*GON2RAD);
			break;
		case kCC:
			d->a = ::TAngle(v*CC2RAD);
			break;
	}
}

bool LGC::TAngle::operator==(const LGC::TAngle& a) const {
	return d->a == a.d->a;
}

bool LGC::TAngle::operator!=(const TAngle& a) const {
	return d->a.getRadiansValue() != a.d->a.getRadiansValue();
}

bool LGC::TAngle::operator<=(const TAngle& a) const {
	return d->a.getRadiansValue() <= a.d->a.getRadiansValue();
}

bool LGC::TAngle::operator>=(const TAngle& a) const {
	return d->a.getRadiansValue() >= a.d->a.getRadiansValue();
}

bool LGC::TAngle::operator<(const TAngle& a)  const {
	return d->a.getRadiansValue() < a.d->a.getRadiansValue();
}

bool LGC::TAngle::operator>(const TAngle& a)  const {
	return d->a.getRadiansValue() > a.d->a.getRadiansValue();
}


LGC::TAngle LGC::TAngle::operator+(const TAngle& a) const {
	LGC::TAngle x(*this);
	x.d->a += a.d->a;
	return x;
}

LGC::TAngle LGC::TAngle::operator-(const TAngle& a) const {
	LGC::TAngle x(*this);
	x.d->a -= a.d->a;
	return x;
}

LGC::TAngle LGC::TAngle::operator*(const TAngle& a) const {
	LGC::TAngle x(*this);
	x.d->a = ::TAngle(d->a.getRadiansValue() * a.d->a.getRadiansValue());
	return x;
}

LGC::TAngle LGC::TAngle::operator*(TReal v) {
	LGC::TAngle x(*this);
	x.d->a = ::TAngle(d->a.getRadiansValue() * v);
	return x;
}

LGC::TAngle LGC::TAngle::operator/(const TAngle& a) const {
	LGC::TAngle x(*this);
	x.d->a = ::TAngle(d->a.getRadiansValue() / a.d->a.getRadiansValue());
	return x;
}


LGC::TAngle& LGC::TAngle::operator+=(const TAngle& a) {
	d->a.setRadiansValue(d->a.getRadiansValue() + a.d->a.getRadiansValue());
	return *this;
}

LGC::TAngle& LGC::TAngle::operator-=(const TAngle& a) {
	d->a.setRadiansValue(d->a.getRadiansValue() - a.d->a.getRadiansValue());
	return *this;
}

LGC::TAngle& LGC::TAngle::operator*=(const TAngle& a) {
	d->a.setRadiansValue(d->a.getRadiansValue() * a.d->a.getRadiansValue());
	return *this;
}

LGC::TAngle& LGC::TAngle::operator*=(const TReal v) {
	d->a.setRadiansValue(d->a.getRadiansValue() * v);
	return *this;
}

LGC::TAngle& LGC::TAngle::operator/=(const TAngle& a) {
	d->a.setRadiansValue(d->a.getRadiansValue() / a.d->a.getRadiansValue());
	return *this;
}

LGC::TAngle& LGC::TAngle::operator/=(const TReal v) {
	d->a.setRadiansValue(d->a.getRadiansValue() / v);
	return *this;
}


TReal LGC::TAngle::sin() const {
	return d->a.sine();
}

TReal LGC::TAngle::cos() const {
	return d->a.cosine();
}

TReal LGC::TAngle::tan() const {
	return d->a.tangent();
}

LGC::TAngle LGC::TAngle::asin(TReal v) {
	TAngle x;
	x.d->a = ::TAngle::aSin(v);
	return x;
}
LGC::TAngle LGC::TAngle::acos(TReal v) {
	TAngle x;
	x.d->a = ::TAngle::aCos(v);
	return x;

}
LGC::TAngle LGC::TAngle::atan(TReal v) {
	TAngle x;
	x.d->a = ::TAngle::aTan(v);
	return x;
}
LGC::TAngle LGC::TAngle::atan2(TReal a, TReal b) {
	TAngle x;
	x.d->a = ::TAngle::aTan2(a, b);
	return x;
}


TReal  LGC::TAngle::cc() const {
	return d->a.getSignedCCValue();
}

TReal LGC::TAngle::gon() const {
	return d->a.getGonsValue();
}

TReal LGC::TAngle::deg() const {
	return d->a.getDegreesValue();
}

TReal LGC::TAngle::rad() const {
	return d->a.getRadiansValue();
}


void LGC::TAngle::set(EUnits unit, TReal v) {
	TAngle x(unit, v);
	d->a = x.d->a;
}

/// only for debugging/testing prurposes
TReal LGC::TAngle::raw() {
	return d->a.getRadiansValue();
}

*/