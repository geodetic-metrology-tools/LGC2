// SPDX-FileCopyrightText: 2025 CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLGCFrameConstraintGroup.h"

void TLGCFrameConstraintGroup::addFrameToGroup(std::string slave, const TLGCData &projData)
{
	TDataTreeIterator slaveIt = projData.locateNode(slave);
	TAdjustableHelmertTransformation &slaveFrame = projData.locateNode(slave).node->data.get()->frame;
	// check dimension. Fixed frames are not allowed
	if (slaveFrame.getNumUnkn() == 0)
	{
		logWarning() << "The frame" << slave << "cannot be added to frame-constraint group" << groupName << "because it is already fixed via the supplied provisional values.";
	}

	// if the group already exist, check the compatibility of the degrees of freedom
	if (slaves.size() > 0)
	{
		TAdjustableHelmertTransformation &masterFrame = projData.locateNode(slaves.front()).node->data.get()->frame;
		const std::string &masterName = masterFrame.getName();
		const std::string &slaveName = slaveFrame.getName();
		// check if both master and slave have compatible freedom signature
		for (int j = 0; j < 3; j++)
		{
			if (masterFrame.isTranslationFixed(j) != slaveFrame.isTranslationFixed(j))
			{
				throw std::runtime_error("The freedoms of translation parameter " + std::to_string(j) + " of master frame " + masterName + " and slave frame " + slaveName
					+ " in slave group " + groupName + "are incompatible.");
			}
			if (masterFrame.isRotationFixed(j) != slaveFrame.isRotationFixed(j))
			{
				throw std::runtime_error("The freedoms of rotation parameter " + std::to_string(j) + " of master frame " + masterName + " and slave frame " + slaveName
					+ " in slave group " + groupName + "are incompatible.");
			}
		}
		if (masterFrame.isScaleFixed() != slaveFrame.isScaleFixed())
		{
			throw std::runtime_error(
				"The freedoms of the scale parameter of master frame " + masterName + " and slave frame " + slaveName + " in slave group " + groupName + " are incompatible.");
		}

		constraintDim += slaveFrame.getNumUnkn();
	}
	slaves.push_back(slave);
}

bool TLGCFrameConstraintGroup::isPartOfGroup(std::string frame)
{
	return std::count(slaves.begin(), slaves.end(), frame) > 0;
}

#if USE_SERIALIZER
void TLGCFrameConstraintGroup::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("groupName", groupName);
	obj.addProperty("constraintDim", constraintDim);
	obj.addProperty("firstCIndex", firstCIndex);
	obj.addProperty("slaves", slaves);
}
#endif // USE_SERIALIZER
