#include "LGCPointConstraintGroup.h"

void LGCPointConstraintGroup::setAffectedPoints(std::set<std::string> affectedPoints)
{
	// set the affected points
	fAffectedPoints = affectedPoints;
	// compute and store the coordinates of the points in Root. These values are necessary to evaluate the constraint functions
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
		fPositionsInRoot[pointName] = positionInRoot.toRealVector();
	}
}

void LGCPointConstraintGroup::setConstraints(constraintSignature usedConstraints)
{
	fConstraints = usedConstraints;
	constraintDim = usedConstraints.tx + usedConstraints.ty + usedConstraints.tz + usedConstraints.rx + usedConstraints.ry + usedConstraints.rz + usedConstraints.scl;
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
