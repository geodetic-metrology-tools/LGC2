#include "TLOR2LOR.h"
#include <bitset>
#include<Eigen/Dense>
#include "TLGCData.h"
#include "Legacy.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLOR2LOR::TLOR2LOR(const TDataTree& tree, const std::string&  from, const std::string&  to, const std::string& name) : fName(name), fTurningPointLocated(false), transfo(){
	locateNodes(tree, from, to);
	locateTurningPoint();
	initialize();
}

TLOR2LOR::TLOR2LOR(TDataTreeIterator from, TDataTreeIterator to, const std::string& name) : fName(name), fFromNode(from), fToNode(to),fTurningPointLocated(false), transfo(){
	locateTurningPoint();
	initialize();
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
void TLOR2LOR::updateTree(){
	for(unsigned int i=0;i<transformationChain.size();i++){
		if(transformationChain[i].direct)
			static_pointer_cast<TDirectTransformation>(transformationChain[i].trafo)->setMatrix(transformationChain[i].adjTrafo->getEstParam());
		else
			static_pointer_cast<TInverseTransformation>(transformationChain[i].trafo)->setMatrix(transformationChain[i].adjTrafo->getEstParam());
	}

	updateTransformation();
}

TPositionVector& TLOR2LOR::transform(TPositionVector& p) const{
	transfo.transform(p);
	return p;
}

 TFreeVector TLOR2LOR::partDerivWRespToX0() const{
	try{
		TReal globalScale = 1;
		for (int positionInChain = 0; positionInChain< transformationChain.size(); positionInChain++)
		{
			//Make partial derivative of the transformation
			if(transformationChain[positionInChain].direct){ 
				std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else{
				std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1/inverse_transformation->getScaleFactor());
			}
		}

		return TFreeVector(globalScale*transfo.getMmatrixIJPosition(0,0), globalScale*transfo.getMmatrixIJPosition(1,0), globalScale*transfo.getMmatrixIJPosition(2,0),TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	catch(std::runtime_error ex){throw ex;}
 }

 TReal TLOR2LOR::partDerivWRespToX0(int pos) const
 {
	 assert3D(pos);
	 try{
		TReal globalScale = 1;
		for (int positionInChain = 0; positionInChain< transformationChain.size(); positionInChain++)
		{
			//Make partial derivative of the desired transformation
			if(transformationChain[positionInChain].direct){ 
				std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else{
				std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1/inverse_transformation->getScaleFactor());
			}
		}

		return globalScale*transfo.getMmatrixIJPosition(pos,0);
	}
	catch(std::runtime_error ex){throw ex;}
 }

  TFreeVector TLOR2LOR::partDerivWRespToY0() const{
	try{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain< transformationChain.size(); positionInChain++)
		{
			//Make partial derivative of the desired transformation
			if(transformationChain[positionInChain].direct){ 
				std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else{
				std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1/inverse_transformation->getScaleFactor());
			}
		}

		return TFreeVector(globalScale*transfo.getMmatrixIJPosition(0,1), globalScale*transfo.getMmatrixIJPosition(1,1), globalScale*transfo.getMmatrixIJPosition(2,1),TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	catch(std::runtime_error ex){throw ex;}
 }

  TReal TLOR2LOR::partDerivWRespToY0(int pos) const
 {
	assert3D(pos);
	try{
		TReal globalScale = 1;
		for (int positionInChain = 0; positionInChain< transformationChain.size(); positionInChain++)
		{
			//Make partial derivative of the desired transformation
			if(transformationChain[positionInChain].direct){ 
				std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else{
				std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1/inverse_transformation->getScaleFactor());
			}
		}

		return globalScale*transfo.getMmatrixIJPosition(pos,1);
	}
	catch(std::runtime_error ex){throw ex;}
 }

 TFreeVector TLOR2LOR::partDerivWRespToZ0() const{
	try{
		TReal globalScale = 1;
		for (int positionInChain = 0; positionInChain< transformationChain.size(); positionInChain++)
		{
			//Make partial derivative of the desired transformation
			if(transformationChain[positionInChain].direct){ 
				std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else{
				std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1/inverse_transformation->getScaleFactor());
			}
		}

		return TFreeVector(globalScale*transfo.getMmatrixIJPosition(0,2), globalScale*transfo.getMmatrixIJPosition(1,2), globalScale*transfo.getMmatrixIJPosition(2,2),TCoordSysFactory::ECoordSys::k3DCartesian);
	}
	catch(std::runtime_error ex){throw ex;}
 }

 TReal TLOR2LOR::partDerivWRespToZ0(int pos) const
 {
	assert3D(pos);
	try{
		TReal globalScale = 1;
		for (unsigned int positionInChain = 0; positionInChain< transformationChain.size(); positionInChain++)
		{
			//Make partial derivative of the desired transformation
			if(transformationChain[positionInChain].direct){ 
				std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= direct_transformation->getScaleFactor();
			}
			else{
				std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
				globalScale *= (1/inverse_transformation->getScaleFactor());
			}
		}

		return globalScale*transfo.getMmatrixIJPosition(pos,2);
	}
	catch(std::runtime_error ex){throw ex;}
 }


TFreeVector TLOR2LOR::partialDerivativesAngle(const std::string& transfoName, const TPositionVector& p, int angle)const{
	assert3D(angle);
	try{
		int positionInChain = getTransformationPosition(transfoName);

		TTransformation t;
		TTransformation cumulativTransformationBegin = getCumulativeBegin(positionInChain); //Pre multiplication
		TTransformation cumulativTransformationEnd = getCumulativeEnd(positionInChain);	//Post multiplication

		TFreeVector derivated_vector(0.0,0.0,0.0,TCoordSysFactory::ECoordSys::k3DCartesian);

		// Make partial derivative of the transformation
		if(transformationChain[positionInChain].direct){ 
			std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = direct_transformation->differentiatedTransformationAngle(angle);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}
		else{
			std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = inverse_transformation->differentiatedTransformationAngle(angle);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		} 

		return derivated_vector;
	}
	catch(std::runtime_error ex){throw ex;}
}

TFreeVector TLOR2LOR::partialDerivativesTranslation(const std::string& transfoName, const TPositionVector& p, int translation) const{
	assert3D(translation);
	try{
		int positionInChain = getTransformationPosition(transfoName);

		TTransformation t;
		TTransformation cumulativTransformationBegin = getCumulativeBegin(positionInChain); //Pre multiplication
		TTransformation cumulativTransformationEnd = getCumulativeEnd(positionInChain);	//Post multiplication

		TFreeVector derivated_vector(0.0,0.0,0.0,TCoordSysFactory::ECoordSys::k3DCartesian);

		// Make partial derivative of the transformation
		if(transformationChain[positionInChain].direct){ 
			std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = direct_transformation->differentiatedTransformationTranslation(translation);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}
		else{
			std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = inverse_transformation->differentiatedTransformationTranslation(translation);
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		} 

		return derivated_vector;
	}
	catch(std::runtime_error ex){throw ex;}
}

 TFreeVector TLOR2LOR::partialDerivativesScale(const std::string& transfoName, const TPositionVector& p) const{
	try{
		int positionInChain = getTransformationPosition(transfoName);

		TTransformation t;
		TTransformation cumulativTransformationBegin = getCumulativeBegin(positionInChain); //Pre multiplication
		TTransformation cumulativTransformationEnd = getCumulativeEnd(positionInChain);	//Post multiplication

		TFreeVector derivated_vector(0.0,0.0,0.0,TCoordSysFactory::ECoordSys::k3DCartesian);

		// Make partial derivative of the transformation
		if(transformationChain[positionInChain].direct){ 
			std::shared_ptr<TDirectTransformation> direct_transformation = static_pointer_cast<TDirectTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = direct_transformation->differentiatedTransformationScaleFactor();
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		}
		else{
			std::shared_ptr<TInverseTransformation> inverse_transformation = static_pointer_cast<TInverseTransformation>(transformationChain[positionInChain].trafo);
			// Derivative transformation is used to represent a partial derivative. Translation is set to 0 during transformation
			TDerivativeTransformation dt = inverse_transformation->differentiatedTransformationScaleFactor();
			derivated_vector = cumulativTransformationBegin * (dt * (cumulativTransformationEnd * p));
		} 
		
		return derivated_vector;
	}
	catch(std::runtime_error ex){throw ex;}
 }

///////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
///////////////////////////////////////////////////////////////////////////
 void TLOR2LOR::locateTurningPoint(){
		std::vector<int> fFromID = fFromNode.node->data.get()->ID;
	    std::vector<int> fToID = fToNode.node->data.get()->ID;	

		//Length of the ID indicates the depth of particular node (fFromID.size() = depth+1)
		int depthFrom = fFromID.size();
		int depthTo = fToID.size();

		int smallerDepth = depthFrom < depthTo ? depthFrom : depthTo;
		fTurningPointLocated = false;

		for(int i = 0;i<smallerDepth;i++){
			if(fFromID[i] == fToID[i])
				fTurningPoint.push_back (fFromID[i]);
			else{
				fTurningPointLocated = true;
				break;
			}
		}
}

 int TLOR2LOR::getTransformationPosition(const std::string& transfoName) const{
	bool transfoFound = false;
	int positionInChain = 0;
	for(unsigned int i=0; i<transformationChain.size();i++){
		if(transformationChain[i].adjTrafo->getName() == transfoName){
			positionInChain = i;
			transfoFound = true;
			break;
		}
	}

	if(!transfoFound)
		throw std::runtime_error("Transformation does not belong to the transformation chain.");
	else
		return positionInChain;
 }

 void TLOR2LOR::locateNodes(const TDataTree& tree, const std::string& from, const std::string& to){
	 	TDataTreeIterator currentNodeIter = tree.begin();
		TDataTreeIterator end = tree.end();

		bool fFromFound = false;
		bool fToFound = false;

		//Iteration over the tree in order to locate the "from" and the "to" transformations in the tree
		while (currentNodeIter != end){
			auto& node(*(currentNodeIter->get()));
				
			if(node.frame.getName() == from)
			{
				fFromNode = currentNodeIter.node;
				fFromFound = true;
			}

			if(node.frame.getName() == to)
			{
				fToNode = currentNodeIter.node;
				fToFound = true;
			}

			if(fFromFound && fToFound){
				break;
			}
			++currentNodeIter;
			if (currentNodeIter==end) throw std::runtime_error("\"From\" or \"To\" destination frame was not found in the tree");
		}
 }

 void TLOR2LOR::initialize(){
		TDataTreeIterator currentNode = fFromNode;

		//If turningPoint located, we need at first GO UP from "fFromNode" to the turning point, and then GO DOWN to the "fToNode" (treated in this if)
		if (fTurningPointLocated){
			while(currentNode.node->data->ID != fTurningPoint){
				//If we go up, DIRECT transformation of the one stored in the node is needed
				TDirectTransformation directTransfo;
				directTransfo.setMatrix(currentNode.node->data->frame.getEstParam());
				//Store pointer to an adjustable transformation, the DIRECT transformation and information that it was DIRECT in a transformationChain vector
				TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TDirectTransformation>(directTransfo), true};
				transformationChain.push_back(trPar);

				currentNode = currentNode.node->parent; //Move up
			}

			currentNode = fToNode;
			std::vector<TransformAndParams> transformationChainDown;

			while(currentNode.node->data->ID != fTurningPoint){
				//If we go down, INVERSE transformation of the one stored in the node is needed (actually we are going up but it just for convinience, later we push the INVERSE transformation in an opposite order)
				TInverseTransformation inverseTransfo;
				inverseTransfo.setMatrix(currentNode.node->data->frame.getEstParam());

				//Store pointer to adjustable transformation, the direct transformation and information that it was direct in a transformationChain
				TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TInverseTransformation>(inverseTransfo), false};
				transformationChainDown.push_back(trPar);

				currentNode = currentNode.node->parent; // Move up
			}
			//Transformation DOWN has to be pushed to the 'transformationChain' in opposite order to connect the chain correctly
			for(int i = transformationChainDown.size()-1; i>=0; i--)
				transformationChain.push_back(transformationChainDown[i]);			
		}
		//If turningPoint was not found it means that one of these frames is subset of the second (treated in this else)
		else{
			//If "fFromNode" is BELOW "fToNode" we need only to GO UP to the "fToNode"
			if( fFromNode.node->data.get()->ID.size() > fToNode.node->data.get()->ID.size()){
				while(currentNode.node->data->ID != fToNode.node->data->ID){
					//If we go up, DIRECT transformation of the one stored in the node is needed
					TDirectTransformation directTransfo;
					directTransfo.setMatrix(currentNode.node->data->frame.getEstParam());

					//Store pointer to adjustable transformation, the direct transformation and information that it was direct in a transformationChain
					TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TDirectTransformation>(directTransfo), true};
					transformationChain.push_back(trPar);

					currentNode = currentNode.node->parent; //Move up
				}
			}
			//If "fFromNode" is ABOVE "fToNode" we need only to GO DOWN to the "fToNode"
			else{
				currentNode = fToNode;
				std::vector<TransformAndParams> transformationChainDown;

				while(currentNode.node->data->ID != fFromNode.node->data->ID){
					//If we go down, INVERSE transformation of the one stored in the node is needed (actually we are going up but it just for convinience, later we push the INVERSE transformation in an opposite order)
					TInverseTransformation inverseTransfo;
					inverseTransfo.setMatrix(currentNode.node->data->frame.getEstParam());

					//Store pointer to adjustable transformation, the direct transformation and information that it was direct in a transformationChain
					TransformAndParams trPar = {&currentNode.node->data->frame, std::make_shared<TInverseTransformation>(inverseTransfo), false};
					transformationChainDown.push_back(trPar);

					currentNode = currentNode.node->parent; //Move up
				}
				//Transformation down has to be pushed in the right order
				for(int i = transformationChainDown.size()-1; i>=0; i--)
					transformationChain.push_back(transformationChainDown[i]);	
			}
		}

		updateTransformation();
}

void TLOR2LOR::updateTransformation()
{
	// Establish the transformation between the 2 LORs
	transfo.setIdentityTransformation(); 
	for(int i = transformationChain.size() - 1;i >= 0;i--)
		transfo *= *transformationChain[i].trafo;
}

 TTransformation TLOR2LOR::getCumulativeBegin(int positionInChain) const{
	TTransformation cumulativTransformationBegin; //Identity transformation

	for (int i = transformationChain.size()-1; i>positionInChain ; i--)
		cumulativTransformationBegin *= *transformationChain[i].trafo;

	return cumulativTransformationBegin;
}


 TTransformation TLOR2LOR::getCumulativeEnd(int positionInChain) const{
	TTransformation cumulativTransformationEnd; //Identity transformation
		
	for (int i = positionInChain - 1 ; i>=0; i--)
			cumulativTransformationEnd *= *transformationChain[i].trafo;

	return cumulativTransformationEnd;
 }
