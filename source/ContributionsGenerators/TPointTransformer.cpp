#include "TPointTransformer.h"
#include "TTreeEntry.h"
#include "TXYH2CCS.h"
#include "GeodeticConstants.h"

TPointTransformer::TPointTransformer(const TDataTree* tree, const TRefSystemFactory::ERefFrame& refFrame) : fTree(tree), fRefFrame(refFrame),
fMLAused(false), fCGRFused(false), fIsSphere(false), fccs2cgrf()
{
	fLastStationPtName = ""; //No point can have empty name

	if (refFrame == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS){
		fIsSphere = true;
		fGeoidModel = TRefSystemFactory::EGeoid::kCGSphere;
		fccs2cgrf.reInitialize(fIsSphere); //Implicitly the ccs2cgrf transformation uses ellipsoidal CGRF, if SPHE is used, a SPHE CGRF (cgrfs) shoud be used
	}
	else if (refFrame == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG2000Machine;
	else if (refFrame == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG1985Machine;
	else
		fGeoidModel = TRefSystemFactory::EGeoid::kNoGeoid;
}

//////////////////////////////
//Tree update
//////////////////////////////
void TPointTransformer::updateTransformations(){
	for (std::list<TLOR2LOR>::iterator it = fLORTrafo.begin(); it != fLORTrafo.end(); ++it){
		it->updateTree();
	}
	fLastStationPtName = "";
	fMLAused = false;
}

void TPointTransformer::transformPointsToMLASystem(std::string originName, TPositionVector& originOfMLAPos, TPositionVector& additPointPos){
	if (!(fLastStationPtName == originName) || !fMLAused){
		set2MLATransformation(originOfMLAPos);
		fLastStationPtName = originName;
	}
	transform2MLA(additPointPos);
	originOfMLAPos.setX(TLength(0.0));
	originOfMLAPos.setY(TLength(0.0));
	originOfMLAPos.setZ(TLength(0.0));
}

////////////////////////////////////////
// Transformations related functions
////////////////////////////////////////
void TPointTransformer::transform2MLA(TPositionVector& pv){
	fccs2cgrf.transform(pv);
	fcgrf2ilg.transform(pv);
	filg2ila.transform(pv);
	fla2mla.transform(pv);
}

void TPointTransformer::transform2MLA(TFreeVector& fv){
	fccs2cgrf.transform(fv);
	fcgrf2ilg.transform(fv);
	filg2ila.transform(fv);
	fla2mla.transform(fv);
}

// used only for the dver measurements
void TPointTransformer::transformMLA2CGRF(TFreeVector& fv){
	fla2mla.transformInverse(fv);
	filg2ila.transformInverse(fv);
	fcgrf2ilg.transformInverse(fv);
}


// \returns index of a transformation with given name in the 'fLORTrafo' vector, if vector does not include this transformation, function \returns -1
int TPointTransformer::getTransformationIndex(const std::string& transfName) const{
	int index = 0;
	for (auto itLOR2LOR(fLORTrafo.begin()); itLOR2LOR != fLORTrafo.end(); ++itLOR2LOR){
		if (itLOR2LOR->getName() == transfName)
			return index;
		index++;
	}
	return -1;
}

// Set a new origin of LA system, initialize the transformation
void	TPointTransformer::set2MLATransformation(TPositionVector originInCCS){
	//Use origin CGRF position as an origin of the Local Geodetic system
	fcgrf2ilg = TCGRF2LGTransformation(originInCCS, fIsSphere);
	//Use origin CCS position as an origin of the Local Astronomical system
	filg2ila = TILG2ILATransformation(originInCCS, fGeoidModel);
	//Use origin CCS position as an origin of the Local Astronomical system
	fla2mla = TLA2MLATransformation(originInCCS, fGeoidModel, TAngle(0.0), TAngle(0.0));
}

TLOR2LOR TPointTransformer::getIdentityTransformation(){
	//ROOT2ROOT transformation index == an identity transformation
	auto rootFramePosition(fTree->begin());
	return TLOR2LOR(rootFramePosition, rootFramePosition, "IDENTITY");
}

const TLOR2LOR& TPointTransformer::getLORTransformation(TDataTreeIterator originalTreePos, TDataTreeIterator destinationTreePos){
	std::string originalFrameName = originalTreePos->get()->frame.getName();
	std::string destinationFrameName = destinationTreePos->get()->frame.getName();

	int trIndex = getTransformationIndex(originalFrameName + destinationFrameName);

	//If transformation is not defined yet (i.e. trIndex == -1), it needs to be added into the vector of transformations
	if (trIndex == -1){
		fLORTrafo.emplace_back(TLOR2LOR(originalTreePos, destinationTreePos, originalFrameName + destinationFrameName));
		trIndex = (int)fLORTrafo.size() - 1; // Index of the last transformation in the vector, i.e. the one we added on the line above
	}
	std::list<TLOR2LOR>::iterator it = fLORTrafo.begin();
	std::advance(it, trIndex);
	return *it;
}