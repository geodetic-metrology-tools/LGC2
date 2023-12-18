#include "LGCPointConstraintGroup.h"

void LGCPointConstraintGroup::setAffectedPoints(std::set<std::string> affectedPoints)
{
	// set the affected points
	fAffectedPoints = affectedPoints;
	// compute and store the coordinates of the points in Root. These values are necessary to evaluate the constraint functions
	// additionally compute the center of gravity in Root
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

void LGCPointConstraintGroup::allPointsAreAffected()
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

void LGCPointConstraintGroup::setConstraintSignature(std::array<bool, 7> usedConstraints)
{
	fConstraints = usedConstraints;
	int dim = std::count(usedConstraints.begin(), usedConstraints.end(), true);
	constraintDim = dim;
}

Eigen::Vector3d LGCPointConstraintGroup::getProvRootPos(std::string pointName) const
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

void LGCPointConstraintGroup::plotGroupData()
{

// print to std::cout for debugging
//	std::cout << "Point group constraint fixing constraints" << std::endl << "(TX, TY, TZ, RX, RY, RZ, SCL) " << std::endl;
//	for (auto used : fConstraints)
//	{
//		std::cout << used;
//	}
//	std::cout << std::endl;
//	std::cout << fAffectedPoints.size() << " Affected Points:" << std::endl;
//	for (auto pointName : fAffectedPoints)
//	{
//		std::cout << pointName << std::endl;
//	}
//	std::cout << std::endl;

	// write to log file
	std::vector<std::string> constraintNames({"TX", "TY", "TZ", "RX", "RY", "RZ", "SCL"});

	logWarning() << "LIBR constraint group blocking the following: ";
	// logWarning() << "(TX, TY, TZ, RX, RY, RZ, SCL) ";
	std::string constraintString;
	for (int j = 0; j < 7; j++)
	{
		constraintString += fConstraints[j] ? constraintNames[j] + "|" : " |";
	}
	logWarning() << constraintString;
	logWarning() << " affecting the following" << std::to_string(fAffectedPoints.size()) << "points : ";
	//logWarning() << std::to_string(fAffectedPoints.size()) + " affected Points:";
	for (auto pointName : fAffectedPoints)
	{
		logWarning() << pointName;
	}
}


#if USE_SERIALIZER
void LGCPointConstraintGroup::serialize(ObjectSerializer &obj) const
{
	//obj.addProperty("groupName", groupName);
	obj.addProperty("constraintDim", constraintDim);
	//obj.addProperty("firstCIndex", firstCIndex);
	//obj.addProperty("slaves", slaves);
}
#endif // USE_SERIALIZER
