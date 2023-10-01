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

}

void LGCPointConstraintGroup::setConstraintSignature(constraintSignature usedConstraints)
{
	fConstraints = usedConstraints;
	constraintDim = usedConstraints.count();
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


#if USE_SERIALIZER
void LGCPointConstraintGroup::serialize(SerializerObject::SerializationHelper &obj) const
{
	//obj.addProperty("groupName", groupName);
	obj.addProperty("constraintDim", constraintDim);
	//obj.addProperty("firstCIndex", firstCIndex);
	//obj.addProperty("slaves", slaves);
}
#endif // USE_SERIALIZER
