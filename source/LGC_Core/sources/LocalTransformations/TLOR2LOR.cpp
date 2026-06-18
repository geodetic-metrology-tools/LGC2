// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLOR2LOR.h"

#include "TAdjustableHelmertTransformation.h"
#include "TDirectTransformation.h"
#include "TInverseTransformation.h"
#include "TLGCData.h"
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
		singleContrib.col(0) = t1Derivative.toRealVector();
		singleContrib.col(1) = t2Derivative.toRealVector();
		singleContrib.col(2) = t3Derivative.toRealVector();
		singleContrib.col(3) = omegaDerivative.toRealVector();
		singleContrib.col(4) = phiDerivative.toRealVector();
		singleContrib.col(5) = kappaDerivative.toRealVector();
		singleContrib.col(6) = scaleDeriv.toRealVector();
		trafoContrib.push_back(std::pair<TAdjustableHelmertTransformation, TDenseMatrix>(*it->adjTrafo, singleContrib));
	}

	return trafoContrib;
}

std::vector<std::pair<TAdjustableHelmertTransformation, Eigen::Matrix<double, 7, 7>>> TLOR2LOR::computeComposedTransformationDerivatives() const
{
	// The parameters that represent the complete chain as single Helmert transformation (transformation H(x)=Hrot(x) + Htrans) can be expressed as function of the
	// transformed unit vectors (by the trafo chain) this way also the derivatives with respect to the parameters can be computed

	// define unit vectors as free vectors. When transforming a free vector by the chain, only the rotation and scale components act on them.
	// This allows access to the derivatives of the M matrix elements with respect to the chain parameters.
	// As the Helmert parameters are a function of these M matrix elements, we can also compute their derivatives using the chainrule.
	TFreeVector ux(1, 0, 0, TCoordSysFactory::k3DCartesian), uy(0, 1, 0, TCoordSysFactory::k3DCartesian), uz(0, 0, 1, TCoordSysFactory::k3DCartesian);

	// the transformed (free) unit vectors are named HUx, HUy, HUz
	Eigen::Vector3d HUx = transform(ux).toRealVector();
	Eigen::Vector3d HUy = transform(uy).toRealVector();
	Eigen::Vector3d HUz = transform(uz).toRealVector();

	// derivative of helper function v=(x,y,z)->x/norm(x,y,z)
	auto dHelpFun = [](Eigen::Vector3d v) -> Eigen::Matrix<double, 1, 3> {
		Eigen::Matrix<double, 1, 3> result;
		result.setZero();
		double normXYZ = v.norm();
		result << normXYZ - v(0) * v(0) / normXYZ, -v(1) * v(0) / normXYZ, -v(2) * v(0) / normXYZ;
		result *= 1.0 / (normXYZ * normXYZ);
		return result;
	};

	// computing the derivatives of the composed helmert parameter with respect to all helmert parameters forming the chain
	// each of the 7 helmert parameters representing the chain can be interpreted as a function of the images (HUx etc.. ) of the unit vectors (ux etc..)
	// for example the rx rotation can be expressed as f_rx(HUz)= atan2(HUz(2),HUz(1))
	// the derivatives then can be computed using the chain rule
	// d f_rx/dp = d f_rx / dv (HUz) * dHUz/dp

	// computing "prefactors" interpreting the f_rx etc all as functions of HUx, HUy, HUz
	// such that we can compute df_* /dp = prefactor_* * d (HUx, HUy, HUz)/dp

	// rx rotation
	// this factor picks the right derivatives from  d (HUx, HUy, HUz)/dp
	Eigen::Matrix<double, 2, 9> rxAuxFactor = Eigen::Matrix<double, 2, 9>::Zero();
	// corresponds to HUz(2)
	rxAuxFactor(0, 8) = 1;
	// corresponds to HUz(1)
	rxAuxFactor(1, 7) = 1;
	Eigen::Matrix<double, 1, 9> rxPreFactor = TAngle::dATan2(HUz(2), HUz(1)) * rxAuxFactor;

	// ry rotation
	// this factor picks the right derivatives from  d (HUx, HUy, HUz)/dp
	Eigen::Matrix<double, 3, 9> ryAuxFactor = Eigen::Matrix<double, 3, 9>::Zero();
	// corresponds to HUz
	ryAuxFactor(0, 6) = 1;
	ryAuxFactor(1, 7) = 1;
	ryAuxFactor(2, 8) = 1;
	double auxVal = HUz(0) / HUz.norm();
	Eigen::Matrix<double, 1, 9> ryPreFactor = -TAngle::dASin(auxVal) * dHelpFun(HUz) * ryAuxFactor;

	// rz rotation
	// this factor picks the right derivatives from  d (HUx, HUy, HUz)/dp
	Eigen::Matrix<double, 2, 9> rzAuxFactor = Eigen::Matrix<double, 2, 9>::Zero();
	// corresponds to HUx(0)
	rzAuxFactor(0, 0) = 1;
	// corresponds to HUy(0)
	rzAuxFactor(1, 3) = 1;
	Eigen::Matrix<double, 1, 9> rzPreFactor = TAngle::dATan2(HUx(0), HUy(0)) * rzAuxFactor;

	// scale
	Eigen::Matrix<double, 1, 9> sclPreFactor = Eigen::Matrix<double, 1, 9>::Zero();
	// this factor picks the right derivatives from  d (HUx, HUy, HUz)/dp
	Eigen::Matrix<double, 3, 9> sclAuxFactor = Eigen::Matrix<double, 3, 9>::Zero();
	// corresponds to HUx
	sclAuxFactor(0, 0) = 1;
	sclAuxFactor(1, 1) = 1;
	sclAuxFactor(2, 2) = 1;
	Eigen::Matrix<double, 1, 3> normJacobian = Eigen::Matrix<double, 1, 3>::Zero();
	normJacobian = HUx.transpose() / HUx.norm();

	sclPreFactor = normJacobian * sclAuxFactor;

	Eigen::Matrix<double, 4, 9> stackedRotAndScalePreFactor;
	stackedRotAndScalePreFactor << rxPreFactor, ryPreFactor, rzPreFactor, sclPreFactor;

	// translations

	// the translation parameters are simply the image of a zero (positionvector)
	TPositionVector zero(0, 0, 0, TCoordSysFactory::k3DCartesian);

	// to get the partial derivatives of the image of the zero position vector we can use the getPartialDerivativesWrtHelmertParameters() method
	// as the getPartialDerivativesWrtHelmertParameters() takes position vectors we need to subtract the derivatives of the zero position
	// vector to eliminate the effect of translations

	TPositionVector uxPos(1, 0, 0, TCoordSysFactory::k3DCartesian);
	TPositionVector uyPos(0, 1, 0, TCoordSysFactory::k3DCartesian);
	TPositionVector uzPos(0, 0, 1, TCoordSysFactory::k3DCartesian);
	TPositionVector zeroPos(0, 0, 0, TCoordSysFactory::k3DCartesian);

	// prepare derivatives of the transformed unit vectors with respect to each Helmert parameter in the chain
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> trafoDerivativesUx = getPartialDerivativesWrtHelmertParameters(uxPos);
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> trafoDerivativesUy = getPartialDerivativesWrtHelmertParameters(uyPos);
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> trafoDerivativesUz = getPartialDerivativesWrtHelmertParameters(uzPos);
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> trafoDerivativesZero = getPartialDerivativesWrtHelmertParameters(zeroPos);

	// create a vector containing the sensitivities of the composed helmert parameter with respect to all helmert parameters defining the chain
	std::vector<std::pair<TAdjustableHelmertTransformation, Eigen::Matrix<double, 7, 7>>> helmertParameterChainDerivatives;
	int chainLength = transformationChain.size();

	// loop over the chain and compute the derivatives of the helmert parameters.
	for (int j = 0; j < chainLength; j++)
	{
		Eigen::Matrix<double, 7, 7> helmertDerivatives;

		Eigen::Matrix<double, 3, 7> uxDerivative = trafoDerivativesUx.at(j).second;
		Eigen::Matrix<double, 3, 7> uyDerivative = trafoDerivativesUy.at(j).second;
		Eigen::Matrix<double, 3, 7> uzDerivative = trafoDerivativesUz.at(j).second;
		// as the translation corresponds to the image of the zero position vector, we can use its derivative to compute the erivative of the translation parameters
		Eigen::Matrix<double, 3, 7> zeroDerivative = trafoDerivativesZero.at(j).second;

		Eigen::Matrix<double, 9, 7> stackedUnitVectorDerivatives;

		// as we need the derivatives of the action on free vectors, we need to eliminate the translation effect by subtracting the derivative of the image of the zero vector with respect to the the parameters
		stackedUnitVectorDerivatives << uxDerivative - zeroDerivative, uyDerivative - zeroDerivative, uzDerivative - zeroDerivative;

		Eigen::Matrix<double, 4, 7> rotAndScaleDerivatives;
		rotAndScaleDerivatives = stackedRotAndScalePreFactor * stackedUnitVectorDerivatives;

		// combining all derivatives, first 3 rows represent derivatives wrt translation, last 4 rows are rx ry rz and scale
		// the derivatives of the composed translation parameters can be recovered as the derivatives of the image of the zero position vector
		helmertDerivatives << zeroDerivative, rotAndScaleDerivatives;
		helmertParameterChainDerivatives.push_back(std::pair<TAdjustableHelmertTransformation, TDenseMatrix>(trafoDerivativesUx.at(j).first, helmertDerivatives));
	}

	return helmertParameterChainDerivatives;
}

TSparseMatrix TLOR2LOR::getComposedParameterDerivatives(const TLGCData *projData) const
{
	std::vector<std::pair<TAdjustableHelmertTransformation, Eigen::Matrix<double, 7, 7>>> parameterChainDerivatives = computeComposedTransformationDerivatives();
	TSparseMatrix jac(7, projData->fUEOIndices.UIndex);
	jac.setZero();
	// assemble unknown Jacobian of composed trafo parameter
	// sensitivity wrt trafo parameters
	for (auto &pair : parameterChainDerivatives)
	{
		TAdjustableHelmertTransformation trafo = pair.first;
		Eigen::Matrix<double, 7, 7> trafoJac = pair.second;
		if (trafo.getNumUnkn() > 0)
		{
			jac.middleCols(trafo.getFirstUidx(), trafo.getNumUnkn()) = (trafoJac(Eigen::indexing::all, trafo.getRelativeUnknIndices())).sparseView();
		}
	}

	return jac;
}

TDenseMatrix TLOR2LOR::getComposedParameterCovariance(const TLGCData *projData) const
{
	TSparseMatrix frameDerivatives = getComposedParameterDerivatives(projData);
	TDenseMatrix frameDerivativesDense = frameDerivatives.toDense();
	TSparseMatrix parameterCovars = *projData->getCovMatByConst();
	TDenseMatrix result(7, 7);
	// using covariance propagation law
	result = frameDerivativesDense * parameterCovars.toDense() * frameDerivativesDense.transpose();
	return result;
}

TransformParameters TLOR2LOR::computeComposedTransformationParameters() const
{
	TransformParameters result = transfo.getTrafoParameters();
	return result;
}

TDenseMatrix TLOR2LOR::getPartialDerivativeWrtPosition() const
{
	TFreeVector partDerWRespToX0 = partDerivWRespToX0();
	TFreeVector partDerWRespToY0 = partDerivWRespToY0();
	TFreeVector partDerWRespToZ0 = partDerivWRespToZ0();
	TDenseMatrix pointContrib(3, 3);
	pointContrib.setZero();
	pointContrib.col(0) = partDerWRespToX0.toRealVector();
	pointContrib.col(1) = partDerWRespToY0.toRealVector();
	pointContrib.col(2) = partDerWRespToZ0.toRealVector();

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
