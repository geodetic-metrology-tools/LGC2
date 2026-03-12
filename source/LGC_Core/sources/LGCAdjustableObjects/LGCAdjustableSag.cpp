#include <stdexcept>

#include <LGCAdjustableSag.h>
#include <TAdjustablePoint.h>
#include <TAdjustableLength.h>

#include "LGCAdjustablePoint.h"

LGCAdjustableSag LGCAdjustableSag::createUninitialized(const std::string name)
{
	std::bitset<4> isFixed;
	LGCAdjustableSag sag(name, "", TLength(), TLength(), TLength(), TLength(), isFixed);
	sag.setCovar(Eigen::Matrix<double, 4, 4>::Constant(NO_VALf));
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
	for (int j = 0; j < 4; j++)
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
	for (int j = 3; j >= 0; j--)
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
	std::vector<int> activeIndices;
	for (int j = 0; j < 4; j++)
	{
		if (!fIsFixed[j])
			activeIndices.push_back(j);
	}
	return activeIndices;
}

int LGCAdjustableSag::getUnknIndex(int d) const
{
	return fUidx[d];
}

void LGCAdjustableSag::setFirstUidx(int idx)
{
	// setting the first indices of the contained adjustable objects
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed sag element.");
	// iterate over the bitset
	int setIdx = idx;
	for (int j = 0; j < 4; j++)
	{
		if (!fIsFixed[j])
		{
			fUidx[j] = setIdx;
			if (j == 0)
				fVertSag.setFirstUidx(setIdx);
			else if (j == 1)
				fVertCurv.setFirstUidx(setIdx);
			else if (j == 2)
				fRadSag.setFirstUidx(setIdx);
			else if (j == 3)
				fRadCurv.setFirstUidx(setIdx);
			setIdx++;
		}
	}
}

Eigen::VectorXd LGCAdjustableSag::getEstVector() const
{
	Eigen::VectorXd estVect(4);
	estVect << fVertSag.getEstimatedValue().getMetresValue(), fVertCurv.getEstimatedValue().getMetresValue(),
		fRadSag.getEstimatedValue().getMetresValue(), fRadCurv.getEstimatedValue().getMetresValue();
	return estVect;
}

TReal LGCAdjustableSag::getValue(int idx) const
{
	if (idx < getFirstUidx() || idx > getLastUidx())
		throw std::logic_error("Attempting to get variable with wrong index.");
	TReal value = 0;
	if (fUidx[0] == idx)
		value = fVertSag.getValue(idx);
	else if (fUidx[1] == idx)
		value = fVertCurv.getValue(idx);
	else if (fUidx[2] == idx)
		value = fRadSag.getValue(idx);
	else if (fUidx[3] == idx)
		value = fRadCurv.getValue(idx);

	return value;
}

void LGCAdjustableSag::setValue(int idx, TReal value)
{
	if (idx < getFirstUidx() || idx > getLastUidx())
		throw std::logic_error("Attempting to set variable with wrong index.");

	if (fUidx[0] == idx)
		fVertSag.setValue(idx, value);
	else if (fUidx[1] == idx)
		fVertCurv.setValue(idx, value);
	else if (fUidx[2] == idx)
		fRadSag.setValue(idx, value);
	else if (fUidx[3] == idx)
		fRadCurv.setValue(idx, value);
}

void LGCAdjustableSag::setEstValue(int idx, TReal value)
{
	//0,1,2,3=vertsag,vertcurv,radsag,radcurv
	switch (idx)
	{
	case 0:
		fVertSag.setEstValue(value);
		break;
	case 1:
		fVertCurv.setEstValue(value);
		break;
	case 2:
		fRadSag.setEstValue(value);
		break;
	case 3:
		fRadCurv.setEstValue(value);
		break;
	}
}


#if USE_SERIALIZER
void LGCAdjustableSag::serialize(ObjectSerializer &obj) const
{
	TVAdjustableObject::serialize(obj);
	obj.addProperty("fName", fName);
	obj.addProperty("fBaseFrame", fBaseFrame);
	obj.addProperty("fVertSag", fVertSag);
	obj.addProperty("fVertCurv", fVertCurv);
	obj.addProperty("fRadSag", fRadSag);
	obj.addProperty("fRadCurv", fRadCurv);
	obj.addProperty("fCovar", fCovar);
}
#endif // USE_SERIALIZER
