#include "TLOR2LOR.h"

#include "TAdjustableHelmertTransformation.h"
#include "TDirectTransformation.h"
#include "TInverseTransformation.h"
#include "TTreeEntry.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLOR2LOR::TLOR2LOR(const TDataTree &tree, const std::string &from, const std::string &to, const std::string &name) : fName(name), fTurningPointLocated(false), transfo()
{
	locateNodes(tree, from, to);
	locateTurningPoint();
	initialize();
}

TLOR2LOR::TLOR2LOR(TDataTreeIterator from, TDataTreeIterator to, const std::string &name) :
	fName(name), fFromNode(from), fToNode(to), fTurningPointLocated(false), transfo()
{
	locateTurningPoint();
	initialize();
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
void TLOR2LOR::updateTree()
{
	for (unsigned int i = 0; i < transformationChain.size(); i++)
	{
		if (transformationChain[i].direct)
			std::static_pointer_cast<TDirectTransformation>(transformationChain[i].trafo)->setTransformParam(transformationChain[i].adjTrafo->getEstParam());
		else
			std::static_pointer_cast<TInverseTransformation>(transformationChain[i].trafo)->setTransformParam(transformationChain[i].adjTrafo->getEstParam());
	}

	updateTransformation();
}

TPositionVector &TLOR2LOR::transform(TPositionVector &p) const
{
	transfo.transform(p);
	return p;
}

TFreeVector &TLOR2LOR::transform(TFreeVector &p) const
{
	transfo.transform(p);
	return p;
}

TFreeVector TLOR2LOR::partDerivWRespToX0() const
{
	try
	{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain < transformationChain.size(); positionInChain++)
		{
			// Make partial derivative of the transformation
			if (transformationChain[positionInChain].direct)
			{
				std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else
			{
				std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1 / inverse_transformation->getScaleFactor());
			}
		}

		return TFreeVector(globalScale * transfo.getMmatrixIJPosition(0, 0), globalScale * transfo.getMmatrixIJPosition(1, 0),
			globalScale * transfo.getMmatrixIJPosition(2, 0), TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TReal TLOR2LOR::partDerivWRespToX0(int pos) const
{
	assert3D(pos);
	try
	{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain < transformationChain.size(); positionInChain++)
		{
			// Make partial derivative of the desired transformation
			if (transformationChain[positionInChain].direct)
			{
				std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else
			{
				std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1 / inverse_transformation->getScaleFactor());
			}
		}

		return globalScale * transfo.getMmatrixIJPosition(pos, 0);
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TFreeVector TLOR2LOR::partDerivWRespToY0() const
{
	try
	{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain < transformationChain.size(); positionInChain++)
		{
			// Make partial derivative of the desired transformation
			if (transformationChain[positionInChain].direct)
			{
				std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else
			{
				std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1 / inverse_transformation->getScaleFactor());
			}
		}

		return TFreeVector(globalScale * transfo.getMmatrixIJPosition(0, 1), globalScale * transfo.getMmatrixIJPosition(1, 1),
			globalScale * transfo.getMmatrixIJPosition(2, 1), TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TReal TLOR2LOR::partDerivWRespToY0(int pos) const
{
	assert3D(pos);
	try
	{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain < transformationChain.size(); positionInChain++)
		{
			// Make partial derivative of the desired transformation
			if (transformationChain[positionInChain].direct)
			{
				std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else
			{
				std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1 / inverse_transformation->getScaleFactor());
			}
		}

		return globalScale * transfo.getMmatrixIJPosition(pos, 1);
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TFreeVector TLOR2LOR::partDerivWRespToZ0() const
{
	try
	{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain < transformationChain.size(); positionInChain++)
		{
			// Make partial derivative of the desired transformation
			if (transformationChain[positionInChain].direct)
			{
				std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else
			{
				std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1 / inverse_transformation->getScaleFactor());
			}
		}

		return TFreeVector(globalScale * transfo.getMmatrixIJPosition(0, 2), globalScale * transfo.getMmatrixIJPosition(1, 2),
			globalScale * transfo.getMmatrixIJPosition(2, 2), TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TReal TLOR2LOR::partDerivWRespToZ0(int pos) const
{
	assert3D(pos);
	try
	{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain < transformationChain.size(); positionInChain++)
		{
			// Make partial derivative of the desired transformation
			if (transformationChain[positionInChain].direct)
			{
				std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else
			{
				std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1 / inverse_transformation->getScaleFactor());
			}
		}

		return globalScale * transfo.getMmatrixIJPosition(pos, 2);
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TFreeVector TLOR2LOR::partialDerivativesAngle(const std::string &transfoName, const TPositionVector &p, int angle) const
{
	assert3D(angle);
	try
	{
		int positionInChain = getTransformationPosition(transfoName);

		TTransformation t;
		TTransformation cumulativTransformationBegin = getCumulativeBegin(positionInChain); // Pre multiplication
		TTransformation cumulativTransformationEnd = getCumulativeEnd(positionInChain); // Post multiplication

		TFreeVector derivated_vector(0.0, 0.0, 0.0, TCoordSysFactory::ECoordSys::k3DCartesian);

		// Make partial derivative of the transformation
		if (transformationChain[positionInChain].direct)
		{
			std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = direct_transformation->differentiatedTransformationAngle(angle);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}
		else
		{
			std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = inverse_transformation->differentiatedTransformationAngle(angle);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}

		return derivated_vector;
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TFreeVector TLOR2LOR::partialDerivativesAngle(const std::string &transfoName, const TFreeVector &v, int angle) const
{
	return partialDerivativesAngle(transfoName, (TPositionVector)v, angle)
		- partialDerivativesAngle(transfoName, TPositionVector(0, 0, 0, TCoordSysFactory::ECoordSys::k3DCartesian), angle);
}

TFreeVector TLOR2LOR::partialDerivativesTranslation(const std::string &transfoName, const TPositionVector &p, int translation) const
{
	assert3D(translation);
	try
	{
		int positionInChain = getTransformationPosition(transfoName);

		TTransformation t;
		TTransformation cumulativTransformationBegin = getCumulativeBegin(positionInChain); // Pre multiplication
		TTransformation cumulativTransformationEnd = getCumulativeEnd(positionInChain); // Post multiplication

		TFreeVector derivated_vector(0.0, 0.0, 0.0, TCoordSysFactory::ECoordSys::k3DCartesian);

		// Make partial derivative of the transformation
		if (transformationChain[positionInChain].direct)
		{
			std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = direct_transformation->differentiatedTransformationTranslation(translation);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}
		else
		{
			std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = inverse_transformation->differentiatedTransformationTranslation(translation);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}

		return derivated_vector;
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TFreeVector TLOR2LOR::partialDerivativesScale(const std::string &transfoName, const TPositionVector &p) const
{
	try
	{
		int positionInChain = getTransformationPosition(transfoName);

		TTransformation t;
		TTransformation cumulativTransformationBegin = getCumulativeBegin(positionInChain); // Pre multiplication
		TTransformation cumulativTransformationEnd = getCumulativeEnd(positionInChain); // Post multiplication

		TFreeVector derivated_vector(0.0, 0.0, 0.0, TCoordSysFactory::ECoordSys::k3DCartesian);

		// Make partial derivative of the transformation
		if (transformationChain[positionInChain].direct)
		{
			std::shared_ptr<TDirectTransformation> direct_transformation = std::static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = direct_transformation->differentiatedTransformationScaleFactor();
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}
		else
		{
			std::shared_ptr<TInverseTransformation> inverse_transformation = std::static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = inverse_transformation->differentiatedTransformationScaleFactor();
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}

		return derivated_vector;
	}
	catch (std::runtime_error &ex)
	{
		throw ex;
	}
}

TFreeVector TLOR2LOR::partialDerivativesScale(const std::string &transfoName, const TFreeVector &v) const
{
	return partialDerivativesScale(transfoName, (TPositionVector)v) - partialDerivativesScale(transfoName, TPositionVector(0, 0, 0, TCoordSysFactory::ECoordSys::k3DCartesian));
}

std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> TLOR2LOR::getPartialDerivativesWrtHelmertParameters(const TPositionVector &pos) const
{
	// create the Jacobian of the Helmert transformation with respect to all 7 parameters
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> trafoContrib;
	TFreeVector omegaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector phiDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector kappaDerivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t1Derivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t2Derivative(TCoordSysFactory::k3DCartesian);
	TFreeVector t3Derivative(TCoordSysFactory::k3DCartesian);
	TFreeVector scaleDeriv(TCoordSysFactory::k3DCartesian);
	auto toVector = [](TFreeVector vIn) {
		TVector vector(3);
		vector << vIn.getX(), vIn.getY(), vIn.getZ();
		return vector;
	};

	// Iterate through the transformations, calculate contributions and store the contributiojn for every transformation
	for (auto it(transformationChain.begin()); it != transformationChain.end(); ++it)
	{
		std::string transformationName = it->adjTrafo->getName();

		// Contributions for rotations : Omega, Phi and Kappa
		omegaDerivative = partialDerivativesAngle(transformationName, pos, 0);
		phiDerivative = partialDerivativesAngle(transformationName, pos, 1);
		kappaDerivative = partialDerivativesAngle(transformationName, pos, 2);

		// Contributions for translation: X, Y and Z coordinate
		t1Derivative = partialDerivativesTranslation(transformationName, pos, 0);
		t2Derivative = partialDerivativesTranslation(transformationName, pos, 1);
		t3Derivative = partialDerivativesTranslation(transformationName, pos, 2);

		scaleDeriv = partialDerivativesScale(transformationName, pos);

		TDenseMatrix singleContrib(3, 7);
		singleContrib.setZero();
		singleContrib.col(0) = toVector(t1Derivative);
		singleContrib.col(1) = toVector(t2Derivative);
		singleContrib.col(2) = toVector(t3Derivative);
		singleContrib.col(3) = toVector(omegaDerivative);
		singleContrib.col(4) = toVector(phiDerivative);
		singleContrib.col(5) = toVector(kappaDerivative);
		singleContrib.col(6) = toVector(scaleDeriv);
		trafoContrib.push_back(std::pair<TAdjustableHelmertTransformation, TDenseMatrix>(*it->adjTrafo, singleContrib));
	}

	return trafoContrib;
}

TDenseMatrix TLOR2LOR::getPartialDerivativeWrtPosition() const
{
	auto toVector = [](TFreeVector vIn) {
		TVector vector(3);
		// getMetresValue() not necessary because default is meter
		vector << vIn.getX(), vIn.getY(), vIn.getZ();
		return vector;
	};

	TFreeVector partDerWRespToX0 = partDerivWRespToX0();
	TFreeVector partDerWRespToY0 = partDerivWRespToY0();
	TFreeVector partDerWRespToZ0 = partDerivWRespToZ0();
	TDenseMatrix pointContrib(3, 3);
	pointContrib.setZero();
	pointContrib.col(0) = toVector(partDerWRespToX0);
	pointContrib.col(1) = toVector(partDerWRespToY0);
	pointContrib.col(2) = toVector(partDerWRespToZ0);

	return pointContrib;
}

TSparseMatrix TLOR2LOR::getPointDerivative(const TLGCData *projData, const LGCAdjustablePoint &point)
{
	// provide derivatives
	TPositionVector ptInSF = point.getEstimatedValue();

	TDenseMatrix ptJac = getPartialDerivativeWrtPosition();
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> trafoDerivatives = getPartialDerivativesWrtHelmertParameters(ptInSF);
	TSparseMatrix jac(3, projData->fUEOIndices.UIndex);
	jac.setZero();
	// assemble unknown Jacobian of transformation
	// sensitivity wrt trafo parameters
	for (auto &pair : trafoDerivatives)
	{
		TAdjustableHelmertTransformation trafo = pair.first;
		TDenseMatrix trafoJac = pair.second;
		if (trafo.getNumUnkn() > 0)
		{
			jac.middleCols(trafo.getFirstUidx(), trafo.getNumUnkn()) = (trafoJac(Eigen::indexing::all, trafo.getRelativeUnknIndices())).sparseView();
		}
	}
	// sensitivity wrt transformed point
	if (point.getNumUnkn() > 0)
	{
		jac.middleCols(point.getFirstUidx(), point.getNumUnkn()) = (ptJac(Eigen::indexing::all, point.getRelativeUnknIndices())).sparseView();
	}

	return jac;
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////
void TLOR2LOR::locateTurningPoint()
{
	std::vector<int> fFromID = fFromNode.node->data.get()->ID;
	std::vector<int> fToID = fToNode.node->data.get()->ID;

	// Length of the ID indicates the depth of particular node (fFromID.size() = depth+1)
	int depthFrom = (int)fFromID.size();
	int depthTo = (int)fToID.size();

	int smallerDepth = depthFrom < depthTo ? depthFrom : depthTo;
	fTurningPointLocated = false;

	for (int i = 0; i < smallerDepth; i++)
	{
		if (fFromID[i] == fToID[i])
			fTurningPoint.push_back(fFromID[i]);
		else
		{
			fTurningPointLocated = true;
			break;
		}
	}
}

int TLOR2LOR::getTransformationPosition(const std::string &transfoName) const
{
	bool transfoFound = false;
	int positionInChain = 0;
	for (unsigned int i = 0; i < transformationChain.size(); i++)
	{
		if (transformationChain[i].adjTrafo->getName() == transfoName)
		{
			positionInChain = i;
			transfoFound = true;
			break;
		}
	}

	if (!transfoFound)
		throw std::runtime_error("Transformation does not belong to the transformation chain.");
	else
		return positionInChain;
}

void TLOR2LOR::locateNodes(const TDataTree &tree, const std::string &from, const std::string &to)
{
	TDataTreeIterator currentNodeIter = tree.begin();
	TDataTreeIterator end = tree.end();

	bool fFromFound = false;
	bool fToFound = false;

	// Iteration over the tree in order to locate the "from" and the "to" transformations in the tree
	while (currentNodeIter != end)
	{
		auto &node(*(currentNodeIter->get()));

		if (node.frame.getName() == from)
		{
			fFromNode = currentNodeIter.node;
			fFromFound = true;
		}

		if (node.frame.getName() == to)
		{
			fToNode = currentNodeIter.node;
			fToFound = true;
		}

		if (fFromFound && fToFound)
		{
			break;
		}
		++currentNodeIter;
		if (currentNodeIter == end)
			throw std::runtime_error("\"From\" or \"To\" destination frame was not found in the tree");
	}
}

void TLOR2LOR::initialize()
{
	TDataTreeIterator currentNode = fFromNode;

	// If turningPoint located, we need at first GO UP from "fFromNode" to the turning point, and then GO DOWN to the "fToNode" (treated in this if)
	if (fTurningPointLocated)
	{
		while (currentNode.node->data->ID != fTurningPoint)
		{
			// If we go up, DIRECT transformation of the one stored in the node is needed
			TDirectTransformation directTransfo;
			directTransfo.setTransformParam(currentNode.node->data->frame.getEstParam());
			// Store pointer to an adjustable transformation, the DIRECT transformation and information that it was DIRECT in a transformationChain vector
			TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TDirectTransformation>(directTransfo), true};
			transformationChain.push_back(trPar);

			currentNode = currentNode.node->parent; // Move up
		}

		currentNode = fToNode;
		std::vector<TransformAndParams> transformationChainDown;

		while (currentNode.node->data->ID != fTurningPoint)
		{
			// If we go down, INVERSE transformation of the one stored in the node is needed (actually we are going up but it just for convinience, later we push the INVERSE transformation in an opposite order)
			TInverseTransformation inverseTransfo;
			inverseTransfo.setTransformParam(currentNode.node->data->frame.getEstParam());

			// Store pointer to adjustable transformation, the direct transformation and information that it was direct in a transformationChain
			TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TInverseTransformation>(inverseTransfo), false};
			transformationChainDown.push_back(trPar);

			currentNode = currentNode.node->parent; // Move up
		}
		// Transformation DOWN has to be pushed to the 'transformationChain' in opposite order to connect the chain correctly
		for (int i = (int)transformationChainDown.size() - 1; i >= 0; i--)
			transformationChain.push_back(transformationChainDown[i]);
	}
	// If turningPoint was not found it means that one of these frames is subset of the second (treated in this else)
	else
	{
		// If "fFromNode" is BELOW "fToNode" we need only to GO UP to the "fToNode"
		if (fFromNode.node->data.get()->ID.size() > fToNode.node->data.get()->ID.size())
		{
			while (currentNode.node->data->ID != fToNode.node->data->ID)
			{
				// If we go up, DIRECT transformation of the one stored in the node is needed
				TDirectTransformation directTransfo;
				directTransfo.setTransformParam(currentNode.node->data->frame.getEstParam());

				// Store pointer to adjustable transformation, the direct transformation and information that it was direct in a transformationChain
				TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TDirectTransformation>(directTransfo), true};
				transformationChain.push_back(trPar);

				currentNode = currentNode.node->parent; // Move up
			}
		}
		// If "fFromNode" is ABOVE "fToNode" we need only to GO DOWN to the "fToNode"
		else
		{
			currentNode = fToNode;
			std::vector<TransformAndParams> transformationChainDown;

			while (currentNode.node->data->ID != fFromNode.node->data->ID)
			{
				// If we go down, INVERSE transformation of the one stored in the node is needed (actually we are going up but it just for convinience, later we push the INVERSE transformation in an opposite order)
				TInverseTransformation inverseTransfo;
				inverseTransfo.setTransformParam(currentNode.node->data->frame.getEstParam());

				// Store pointer to adjustable transformation, the direct transformation and information that it was direct in a transformationChain
				TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TInverseTransformation>(inverseTransfo), false};
				transformationChainDown.push_back(trPar);

				currentNode = currentNode.node->parent; // Move up
			}
			// Transformation down has to be pushed in the right order
			for (int i = (int)transformationChainDown.size() - 1; i >= 0; i--)
				transformationChain.push_back(transformationChainDown[i]);
		}
	}

	updateTransformation();
}

void TLOR2LOR::updateTransformation()
{
	// Establish the transformation between the 2 LORs
	transfo.setIdentityTransformation();
	for (int i = (int)transformationChain.size() - 1; i >= 0; i--)
		transfo *= *transformationChain[i].trafo;
}

TTransformation TLOR2LOR::getCumulativeBegin(int positionInChain) const
{
	TTransformation cumulativTransformationBegin; // Identity transformation

	for (int i = (int)transformationChain.size() - 1; i > positionInChain; i--)
		cumulativTransformationBegin *= *transformationChain[i].trafo;

	return cumulativTransformationBegin;
}

TTransformation TLOR2LOR::getCumulativeEnd(int positionInChain) const
{
	TTransformation cumulativTransformationEnd; // Identity transformation

	for (int i = positionInChain - 1; i >= 0; i--)
		cumulativTransformationEnd *= *transformationChain[i].trafo;

	return cumulativTransformationEnd;
}
