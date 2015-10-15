#include "TLOR2LOR.h"
#include <vector>
#include <bitset>
#include<Eigen/Dense>

#include "Global.h"
#include "Legacy.h"

TLOR2LOR::TLOR2LOR(const TDataTree& tree, const std::string&  from, const std::string&  to, const std::string& name):trafo(name){
	initialize(tree, from, to);
}

void TLOR2LOR::initialize(const TDataTree& tree, const std::string& from, const std::string& to){

		TDataTreeIterator currentNodeIter = tree.begin();
		TDataTreeIterator end = tree.end();

		bool fFromFound = false;
		bool fToFound = false;
		std::vector<int> fFromID;
	    std::vector<int> fToID;

		//Iteration over the tree in order to locate the "from" and the "to" transformations in the tree
		while (currentNodeIter != end){
			auto& node(*(currentNodeIter->get()));
				
			if(node.frame.getName() == from)
			{
				fFromNode = *(currentNodeIter.node);
				fFromID=fFromNode.data->ID;
				fFromFound = true;
			}

			if(node.frame.getName() == to)
			{
				fToNode = *(currentNodeIter.node);
				fToID=fToNode.data->ID;
				fToFound = true;
			}

			if(fFromFound && fToFound){
				break;
			}
			++currentNodeIter;
			if (currentNodeIter==end) throw std::runtime_error("\"From\" or \"To\" destination frame was not found in the tree");
		}

		//Length of the ID indicates the depth of particular node (fFromID.size() = depth+1)
		int sizeFrom = fFromID.size();
		int sizeTo = fToID.size();
		int smallerSize = sizeFrom < sizeTo ? sizeFrom : sizeTo;
		bool found = false;

		//Find the turningPoint, i.e. node in which we stop going up from "from" and start going down to "to" 
		std::vector<int> turningPoint;
		for(int i = 0;i<smallerSize;i++){
			if(fFromID[i] == fToID[i]){
				turningPoint.push_back (fFromID[i]);
			}
			else{
				found = true;
				break;
			}
		}

		Node currentNode = fFromNode;
		//If turningPoint was found, we need at first GO UP from "from node" to the turning point, and then GO DOWN to the "to node" 
		if (found){
			while(currentNode.data->ID != turningPoint){
				//If we go up, inverse transformation of the one stored in the node is needed
				transformationChain.push_back(currentNode.data->frame.inverse());
				currentNode = *currentNode.parent;
			}
			currentNode = fToNode;
			std::vector<TLocalTransformation> transformationChainDown;
			while(currentNode.data->ID != turningPoint){
				transformationChainDown.push_back(currentNode.data->frame);
				currentNode = *currentNode.parent;
			}
			//Transformation down has to be pushed in the right order
			for(int i = transformationChainDown.size()-1; i>=0; i--){
				transformationChain.push_back(transformationChainDown[i]);
			}
		}
		//If turningPoint was not found it means that one of these frames is subset of the second
		else{
			//If "fFromNode" is BELOW "fToNode" we need only to GO UP to the "fToNode"
			if(sizeFrom > sizeTo){
				while(currentNode.data->ID != fToNode.data->ID){
				transformationChain.push_back(currentNode.data->frame.inverse());
				currentNode = *currentNode.parent;
				}
			}
			//If "fFromNode" is ABOVE "fToNode" we need only to GO DOWN to the "fToNode"
			else{
				currentNode = fToNode;
				std::vector<TLocalTransformation> transformationChainDown;
				while(currentNode.data->ID != fFromNode.data->ID){
				transformationChainDown.push_back(currentNode.data->frame);
				currentNode = *currentNode.parent;
				}
				//Transformation down has to be pushed in opposite order
				for(int i = transformationChainDown.size()-1; i>=0; i--){
					transformationChain.push_back(transformationChainDown[i]);
				}
			}
			}
	// Establish the transformation
	for(int i = transformationChain.size()-1;i>=0;i--){
		trafo = trafo * transformationChain[i];
	}
}

TLOR2LOR::TLOR2LOR(const  TLOR2LOR &lor2lor):trafo(lor2lor.getTransformation().getName()){
	trafo = lor2lor.getTransformation();
	transformationChain = lor2lor.getTransformationChain();
	fFromNode = lor2lor.getFromNode();
	fToNode = lor2lor.getToNode();
}

TspatialPoint& TLOR2LOR::transform(TspatialPoint& p){
	return trafo.transform(p);
}

TReal TLOR2LOR::partialDerivativesAngle(const std::string& trafoName, int pos, const TspatialPoint& p, int angle){
	assert4D(pos);
	assert3D(angle);
	try{
		int positionInChain = getTransformationPosition(trafoName);
		TReal derivative = 0.0;
		std::vector<TLocalTransformation> transformationChainDerived = transformationChain;

		transformationChainDerived[positionInChain] = transformationChainDerived[positionInChain].differentiatedTransformationAngle(angle);
		TLocalTransformation derivedTrafo = transformationChainDerived[0];

		for(unsigned int i = 0;i<transformationChainDerived.size()-1;i++){
			derivedTrafo = transformationChainDerived[i+1] * derivedTrafo;
		}

		Eigen::Vector4d pTemp(p[0],p[1],p[2],1.0);
		for(unsigned int i=0;i<4;i++){
			derivative = derivative + derivedTrafo.getMmatrixIJPosition(pos,i)*pTemp(i);
		}
		derivative *=trafo.getScaleFactor();
		return derivative;
	}
	catch(std::runtime_error ex){throw ex;}
}

TReal TLOR2LOR::partialDerivativesTranslation(const std::string& trafoName, int pos, const TspatialPoint& p, int translation){
	assert4D(pos);
	assert3D(translation);
	try{
		int positionInChain = getTransformationPosition(trafoName);
		TReal derivative = 0.0;
		std::vector<TLocalTransformation> transformationChainDerived = transformationChain;

		transformationChainDerived[positionInChain] = transformationChainDerived[positionInChain].differentiatedTransformationTranslation(translation);
		TLocalTransformation derivedTrafo = transformationChainDerived[0];

		for(unsigned int i = 0;i<transformationChainDerived.size()-1;i++){
			derivedTrafo = transformationChainDerived[i+1] * derivedTrafo;
		}

		Eigen::Vector4d pTemp(p[0],p[1],p[2],1.0);
		for(unsigned int i=0;i<4;i++){
			derivative = derivative + derivedTrafo.getMmatrixIJPosition(pos,i)*pTemp(i);
		}
		derivative *=trafo.getScaleFactor();
		return derivative;
	}
	catch(std::runtime_error ex){throw ex;}
}

 TReal TLOR2LOR::partialDerivativesScale(const std::string& trafoName, int pos, const TspatialPoint& p){
	assert4D(pos);
	try{
		int positionInChain = getTransformationPosition(trafoName);
		TReal derivative = 0.0;
		std::vector<TLocalTransformation> transformationChainDerived = transformationChain;

		transformationChainDerived[positionInChain] = transformationChainDerived[positionInChain].differentiatedTransformationScaleFactor();
		TLocalTransformation derivedTrafo = transformationChainDerived[0];

		for(unsigned int i = 0;i<transformationChainDerived.size()-1;i++){
			derivedTrafo = transformationChainDerived[i+1] * derivedTrafo;
		}

		Eigen::Vector4d pTemp(p[0],p[1],p[2],1.0);
		for(unsigned int i=0;i<4;i++){
			derivative = derivative + derivedTrafo.getMmatrixIJPosition(pos,i)*pTemp(i);
		}
		derivative *=trafo.getScaleFactor();
		return derivative;
	}
	catch(std::runtime_error ex){throw ex;}
 }

 int TLOR2LOR::getTransformationPosition(const std::string& trafoName){
	bool trafoFound = false;
	int positionInChain = 0;
	for(unsigned int i=0; i<transformationChain.size();i++){
		if(transformationChain[i].getName() == trafoName){
			positionInChain = i;
			trafoFound = true;
			break;
		}
	}
	if(!trafoFound){
		throw std::runtime_error("Transformation does not belong to the transformation chain.");}
	else{
		return positionInChain;}
 }

