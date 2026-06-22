// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdexcept>

#include <LGCAdjustableSag.h>
#include <TAdjustableLength.h>
#include <TAdjustablePoint.h>

#include "LGCAdjustablePoint.h"

LGCAdjustableSag LGCAdjustableSag::createUninitialized(const std::string &name)
{
	std::bitset<kNumSagParams> isFixed;
	LGCAdjustableSag sag(name, "", TLength(), TLength(), TLength(), TLength(), isFixed);
	sag.fInitialized = false;
	return sag;
}

int LGCAdjustableSag::getFirstUidx() const
{
	if (isFixed())
		throw std::logic_error("Trying to get unknown index of fixed element.");
	// Variable parameters get consecutive indices in setFirstUidx, so the first
	// non-fixed parameter holds the lowest unknown index.
	for (int j = 0; j < kNumSagParams; j++)
		if (uidx[j] != -1)
			return uidx[j];
	return -1;
}

int LGCAdjustableSag::getLastUidx() const
{
	// Variable parameters occupy a consecutive index range, so the last index is
	// the first plus the number of unknowns. Applied on fully fixed element it throws as intended.
	return getFirstUidx() + getNumUnkn() - 1;
}

const std::vector<int> LGCAdjustableSag::getRelativeUnknIndices() const
{
	std::vector<int> activeIndices;
	for (int j = 0; j < kNumSagParams; j++)
	{
		if (!fIsFixed[j])
			activeIndices.push_back(j);
	}
	return activeIndices;
}

int LGCAdjustableSag::getUnknIndex(int d) const
{
	return uidx[d];
}

void LGCAdjustableSag::setFirstUidx(int idx)
{
	if (isFixed())
		throw std::logic_error("Trying to assign unknown index to fixed sag element.");
	// iterate over the bitset, assigning consecutive indices to the variable parameters
	int setIdx = idx;
	for (int j = 0; j < kNumSagParams; j++)
	{
		if (!fIsFixed[j])
		{
			uidx[j] = setIdx;
			if (j == 0)
				fZSag.setFirstUidx(setIdx);
			else if (j == 1)
				fZCurv.setFirstUidx(setIdx);
			else if (j == 2)
				fXSag.setFirstUidx(setIdx);
			else if (j == 3)
				fXCurv.setFirstUidx(setIdx);
			setIdx++;
		}
		else
		{
			// fixed parameter has no unknown index; clear any stale value
			uidx[j] = -1;
		}
	}
}

Eigen::VectorXd LGCAdjustableSag::getEstVector() const
{
	Eigen::VectorXd estVect(kNumSagParams);
	estVect << fZSag.getEstimatedValue().getMetresValue(), fZCurv.getEstimatedValue().getMetresValue(), fXSag.getEstimatedValue().getMetresValue(),
		fXCurv.getEstimatedValue().getMetresValue();
	return estVect;
}

TReal LGCAdjustableSag::getValue(int idx) const
{
	if (idx < getFirstUidx() || idx > getLastUidx())
		throw std::logic_error("Attempting to get variable with wrong index.");
	TReal value = 0;
	if (uidx[0] == idx)
		value = fZSag.getValue(idx);
	else if (uidx[1] == idx)
		value = fZCurv.getValue(idx);
	else if (uidx[2] == idx)
		value = fXSag.getValue(idx);
	else if (uidx[3] == idx)
		value = fXCurv.getValue(idx);
	else
		throw std::logic_error("Sag unknown index in range but not mapped to any parameter.");

	return value;
}

void LGCAdjustableSag::setValue(int idx, TReal value)
{
	if (idx < getFirstUidx() || idx > getLastUidx())
		throw std::logic_error("Attempting to set variable with wrong index.");

	if (uidx[0] == idx)
		fZSag.setValue(idx, value);
	else if (uidx[1] == idx)
		fZCurv.setValue(idx, value);
	else if (uidx[2] == idx)
		fXSag.setValue(idx, value);
	else if (uidx[3] == idx)
		fXCurv.setValue(idx, value);
	else
		throw std::logic_error("Sag unknown index in range but not mapped to any parameter.");
}

void LGCAdjustableSag::reInitialise()
{
	fZSag.reInitialise();
	fZCurv.reInitialise();
	fXSag.reInitialise();
	fXCurv.reInitialise();
	fCovar.setConstant(NO_VALf);
}

#if USE_SERIALIZER
void LGCAdjustableSag::serialize(ObjectSerializer &obj) const
{
	TVAdjustableObject::serialize(obj);
	obj.addProperty("fName", fName);
	obj.addProperty("fBaseFrame", fBaseFrame);
	obj.addProperty("fZSag", fZSag);
	obj.addProperty("fZCurv", fZCurv);
	obj.addProperty("fXSag", fXSag);
	obj.addProperty("fXCurv", fXCurv);
	obj.addProperty("fCovar", fCovar);
	obj.addProperty("line", line);
	obj.addProperty("uidx", uidx);
	std::vector<bool> isFixed(kNumSagParams);
	for (int i = 0; i < kNumSagParams; ++i)
		isFixed[i] = fIsFixed[i];
	obj.addProperty("fIsFixed", isFixed);
	obj.addProperty("fInitialized", fInitialized);
}
#endif // USE_SERIALIZER
