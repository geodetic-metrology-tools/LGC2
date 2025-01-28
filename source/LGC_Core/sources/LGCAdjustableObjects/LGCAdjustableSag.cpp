#include <stdexcept>

#include <LGCAdjustableSag.h>
#include <TAdjustablePoint.h>
#include <TAdjustableAngle.h>
#include <TAdjustableLength.h>

#include "LGCAdjustablePoint.h"

// LGCAdjustableSag::LGCAdjustableSag(const TPositionVector &lowPoint, const std::string &name)
// {};

LGCAdjustableSag LGCAdjustableSag::createUninitialized(const std::string name)
{
	std::bitset<5> isFixed;
	LGCAdjustableSag sag(name, "", TLength(), TLength(), TLength(), TLength(), isFixed);
	sag.setCovar(Eigen::Matrix<double, 5, 5>::Constant(NO_VALf));
	sag.fInitialized = false;
	return sag;
}

inline int LGCAdjustableSag::getNumUnkn() const
{
	return fIsFixed.size() - fIsFixed.count();
}

inline int LGCAdjustableSag::getFirstUidx() const
{
	if (isFixed())
		throw std::logic_error("Trying to get unknown index of fixed element.");
	int firstIdx = -1;
	for (int j = 0; j < 5; j++)
	{
		firstIdx = fUidx[j];
		if (firstIdx != -1)
		{
			break;
		}
	}
	return firstIdx;
}

inline int LGCAdjustableSag::getLastUidx() const
{
	if (isFixed())
		throw std::logic_error("Trying to get unknown index of fixed element.");
	int idx = -1;
	for (int j = 4; j >= 0; j--)
	{
		idx = fUidx[j];
		if (idx != -1)
		{
			break;
		}
	}
	return idx;
}

const std::vector<int> LGCAdjustableSag::getRelativeUnknIndices() const
{
	// for now: bearing, curvature and slack free always
	std::vector<int> activeIndices;
	for (int j = 0; j < 5; j++)
	{
		if (!fIsFixed[j])
			activeIndices.push_back(j);
	}
	return activeIndices;
}

void LGCAdjustableSag::setFirstUidx(int idx)
{
	// setting the first indices of the contained adjustable objects
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed sag element.");
	// iterate over the bitset
	int setIdx = idx;
	for (int j = 0; j < 5; j++)
	{
		if (!fIsFixed[j])
		{
			fUidx[j] = setIdx;
			if (j == 0)
				fBearing.setFirstUidx(setIdx);
			else if (j == 1)
				fVertSag.setFirstUidx(setIdx);
			else if (j == 2)
				fVertCurv.setFirstUidx(setIdx);
			else if (j == 3)
				fRadSag.setFirstUidx(setIdx);
			else if (j == 4)
				fRadCurv.setFirstUidx(setIdx);
			setIdx++;
		}
	}
}

void LGCAdjustableSag::setCorrection(int idx, TReal value)
{
	if (idx < getFirstUidx() || idx > getLastUidx())
		throw std::logic_error("Attempting to set variable with wrong index.");

	if (fUidx[0] == idx)
		fBearing.setCorrection(idx, value);
	else if (fUidx[1] == idx)
		fVertSag.setCorrection(idx, value);
	else if (fUidx[2] == idx)
		fVertCurv.setCorrection(idx, value);
	else if (fUidx[3] == idx)
		fRadSag.setCorrection(idx, value);
	else if (fUidx[4] == idx)
		fRadCurv.setCorrection(idx, value);
}

	
#if USE_SERIALIZER
void LGCAdjustableSag::serialize(ObjectSerializer &obj) const
{
	TVAdjustableObject::serialize(obj);
	obj.addProperty("fName", fName);
	obj.addProperty("fBaseFrame", fBaseFrame);
	obj.addProperty("fVertCurv", fVertCurv);
	obj.addProperty("fVertSag", fVertSag);
	obj.addProperty("fRadCurv", fRadCurv);
	obj.addProperty("fRadSag", fRadSag);
	obj.addProperty("fCovar", fCovar);
	obj.addProperty("firsCIndex", firstCIndex);

}
#endif // USE_SERIALIZER
