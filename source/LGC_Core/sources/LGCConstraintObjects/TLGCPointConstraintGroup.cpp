// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLGCPointConstraintGroup.h"

TLGCPointConstraintGroup::TLGCPointConstraintGroup(const TLGCData &projData, const std::array<bool, 7> &usedConstraints) : data(projData)
{
	setConstraintSignature(usedConstraints);
	allPointsAreAffected();
}

TLGCPointConstraintGroup::TLGCPointConstraintGroup(const TLGCData &projData, const std::array<bool, 7> &usedConstraints, const std::set<std::string> &affectedPoints) :
	data(projData)
{
	setConstraintSignature(usedConstraints);
	setAffectedPoints(affectedPoints);
}


void TLGCPointConstraintGroup::setAffectedPoints(const std::set<std::string> &affectedPoints)
{
	if (affectedPoints.size() == 0)
	{
		throw std::logic_error("Can not create empty constraint group.");
	}
	// set the affected points
	fAffectedPoints = affectedPoints;
	// compute and store the coordinates of the points in Root. These values are necessary to evaluate the constraint functions
	Eigen::Vector3d cogInRoot = Eigen::Vector3d::Zero();

	for (auto pointName : affectedPoints)
	{
		if (!data.getPoints().doesObjectExist(pointName))
		{
			throw std::logic_error("Can not add uninitialized point to constraint group. Point " + pointName);
		}
		LGCAdjustablePoint point = data.getPoints().getObject(pointName);
		// transform point to root coordinates
		TLOR2LOR sub2Root(point.getFrameTreePosition(), data.getTree().begin(), "sub2Root");
		TPositionVector positionInRoot = point.getEstimatedValue();
		sub2Root.transform(positionInRoot);
		Eigen::Vector3d rootPos =  positionInRoot.toRealVector();
		fProvPosInRoot[pointName] = rootPos;
		cogInRoot += rootPos;
	}
	cogInRoot /= fAffectedPoints.size();
	provCOG = cogInRoot;

	// compute provisional scale = sum(|p_i-cog|^2)
	provScale = 0;
	for (auto pointName : affectedPoints)
	{
		provScale += pow2((fProvPosInRoot[pointName] - provCOG).norm());
	}


}

void TLGCPointConstraintGroup::allPointsAreAffected()
{
	// create a set containing all points
	std::set<std::string> allPoints;
	for (auto point : data.getPoints())
	{
		allPoints.insert(point.getName());
	}
	// call the setAffectedPoints method with this set
	setAffectedPoints(allPoints);
}

void TLGCPointConstraintGroup::setConstraintSignature(const std::array<bool, 7>& usedConstraints)
{
	fConstraints = usedConstraints;
	int dim = std::count(usedConstraints.begin(), usedConstraints.end(), true);
	if (dim == 0)
	{
		throw std::logic_error("Defining a group of constrained points without imposing any constraint is not possible.");
	}
	constraintDim = dim;
}

Eigen::Vector3d TLGCPointConstraintGroup::getProvRootPos(std::string pointName) const
{
	auto it = fProvPosInRoot.find(pointName);
	if (it != fProvPosInRoot.end())
	{
		return it->second; // Found the key, return the corresponding value
	}
	else
	{
		// Handle the case where the key is not found in the map
		throw std::runtime_error("Can't get the provisionla value in root of point " + pointName);
	}
}

Eigen::Vector3d TLGCPointConstraintGroup::computeEstCOG() const
{
	int numberOfPoints = fAffectedPoints.size();
	if (numberOfPoints == 0)
	{
		throw std::logic_error("Contribution of point constraint group undefined for empty group.");
	}
	double averagingFactor = 1.0 / double(numberOfPoints);
	Eigen::Vector3d estCOGinRoot = Eigen::Vector3d::Zero();
	for (auto pointName : fAffectedPoints)
	{
		// compute the position of the point in Root
		LGCAdjustablePoint point = data.getPoints().getObject(pointName);
		// transform point to root coordinates
		TLOR2LOR sub2Root(point.getFrameTreePosition(), data.getTree().begin(), "sub2Root");
		TPositionVector positionInRoot = point.getEstimatedValue();
		sub2Root.transform(positionInRoot);
		Eigen::Vector3d rootPos = positionInRoot.toRealVector();
		estCOGinRoot += rootPos;
	}
	estCOGinRoot *= averagingFactor;

	return estCOGinRoot;
}

void TLGCPointConstraintGroup::plotGroupData()
{
	// write to log file
	std::vector<std::string> constraintNames({"TX", "TY", "TZ", "RX", "RY", "RZ", "SCL"});

	logWarning() << "LIBR constraint group blocking the following: ";
	std::string constraintString;
	for (int j = 0; j < 7; j++)
	{
		constraintString += fConstraints[j] ? constraintNames[j] + "|" : " |";
	}
	logWarning() << constraintString;
	logWarning() << " affecting the following" << std::to_string(fAffectedPoints.size()) << "points : ";
	for (auto pointName : fAffectedPoints)
	{
		logWarning() << pointName;
	}
}


#if USE_SERIALIZER
void TLGCPointConstraintGroup::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("affectedPoints", fAffectedPoints);
	obj.addProperty("constraints", fConstraints);
}
#endif // USE_SERIALIZER
