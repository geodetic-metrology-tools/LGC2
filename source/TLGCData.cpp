#include "TLGCData.h"	


TLGCData::TLGCData() : 
	ignmeas(cfg.sim.ignoreMeasurements),
	fCombinedCase(false){
/////////////////////////////////////////////////////////////  TADataSet stuff       //////////////
	TDataParameters dataParam;
	dataParam.setRefFrame(TRefSystemFactory::kCCS);
	dataParam.setCoordSys(TCoordSysFactory::k3DCartesian);
	dataParam.setLenUnits(TLength::kMetres);
	dataParam.setAngUnits(TAngle::kGons);
	dataParam.setCoordPrecision(TPointFormat::k10Micrometres);
	dataParam.setAnglePrecision(TObservationFormat::k10Microgons);
	dataParam.setLengthPrecision(TObservationFormat::k10Micrometres);
	setDataParams(dataParam);

	setDefaults();
	// Create an empty root node
	tree.insert(tree.begin(), TDataSPtr(new TTreeEntry(ignmeas))) ;	
	
	// This is the current node of the tree
	pos = tree.begin();
	auto& n(getCurrentNode());

	//ROOT frame of the tree
	n.ID = std::vector<int> (1);
	n.ID[0] = 1;
	n.frame.setName("ROOT");
}

TTreeEntry& TLGCData::addChild(TAdjustableHelmertTransformation* transfo) {
	pos = tree.append_child(pos, TDataSPtr(new TTreeEntry(ignmeas)));
	auto& n(getCurrentNode());
	n.frame = *transfo;
	//Assign unique ID to the node
	int depth = tree.depth(pos);
	int nOfSiblings = tree.number_of_siblings(pos);
	n.ID = std::vector<int> (depth+1);
	TDataTreeIterator parent = TDataTree::parent(pos);

	for(int i = 0;i<depth;i++){
		n.ID[i] = parent.node->data->ID[i];
	}
	n.ID[depth] = nOfSiblings + 1;

	return n;
}

///////////////////////////////////////////////////////////////////
// increment the number of read points for the corresponding status
///////////////////////////////////////////////////////////////////
void TLGCData::addToPointNum(TSpatialStatus::ESpatialStatus status)
{
	switch (status){
	case TSpatialStatus::kCala:
		fPointInfo.fNumCala++;
		break;
	case TSpatialStatus::kVx:
		fPointInfo.fNumVx++;
		break;
	case TSpatialStatus::kVy:
		fPointInfo.fNumVy++;
		break;
	case TSpatialStatus::kVz:
		fPointInfo.fNumVz++;
		break;
	case TSpatialStatus::kVxy:
		fPointInfo.fNumVxy++;
		break;
	case TSpatialStatus::kVxz:
		fPointInfo.fNumVxz++;
		break;
	case TSpatialStatus::kVyz:
		fPointInfo.fNumVyz++;
		break;
	case TSpatialStatus::kVxyz:
		fPointInfo.fNumVxyz++;
		break;
	}
}

////////////////////////////////////////////////////////////////
// return the number of read points for the corresponding status
////////////////////////////////////////////////////////////////
int TLGCData::getPointsDimension(TSpatialStatus::ESpatialStatus status) const
{
	switch (status){
	case TSpatialStatus::kCala:
		return fPointInfo.fNumCala;
	case TSpatialStatus::kVx:
		return fPointInfo.fNumVx;
	case TSpatialStatus::kVy:
		return fPointInfo.fNumVy;
	case TSpatialStatus::kVz:
		return fPointInfo.fNumVz;
	case TSpatialStatus::kVxy:
		return fPointInfo.fNumVxy;
	case TSpatialStatus::kVxz:
		return fPointInfo.fNumVxz;
	case TSpatialStatus::kVyz:
		return fPointInfo.fNumVyz;
	case TSpatialStatus::kVxyz:
		return fPointInfo.fNumVxyz;
	}
}


void TLGCData::addToMeasurementNum(TMeasurementsGlobal::EMeasurementType type){
	switch (type){
	case TMeasurementsGlobal::kANGL:
		fMeasInfo.fNumANGL++;
		break;
	case TMeasurementsGlobal::kZEND:
		fMeasInfo.fNumZEND++;
		break;
	case TMeasurementsGlobal::kDHOR :
		fMeasInfo.fNumDHOR++;
		break;
	case TMeasurementsGlobal::kDIR3D:
		fMeasInfo.fNumDIR3D++;
		break;
	case TMeasurementsGlobal::kDIST :
		fMeasInfo.fNumDIST++;
		break;
	case TMeasurementsGlobal::kDLEV:
		fMeasInfo.fNumDLEV++;
		break;
	case TMeasurementsGlobal::kPLR3D :
		fMeasInfo.fNumPLR3D++;
		break;
	case TMeasurementsGlobal::kDVER:
		fMeasInfo.fNumDVER++;
		break;
	case TMeasurementsGlobal::kDSPT:
		fMeasInfo.fNumDSPT++;
		break;
	}
}


int TLGCData::getMeasurementDimension(TMeasurementsGlobal::EMeasurementType type) const{
	switch (type){
	case TMeasurementsGlobal::kANGL:
		return fMeasInfo.fNumANGL;
	case TMeasurementsGlobal::kDHOR :
		return fMeasInfo.fNumDHOR;
	case TMeasurementsGlobal::kDIR3D:
		return fMeasInfo.fNumDIR3D;
	case TMeasurementsGlobal::kDIST:
		return fMeasInfo.fNumDIST;
	case TMeasurementsGlobal::kDLEV:
		return fMeasInfo.fNumDLEV;
	case TMeasurementsGlobal::kPLR3D:
		return fMeasInfo.fNumPLR3D;
	case TMeasurementsGlobal::kZEND:
		return fMeasInfo.fNumZEND;
	case TMeasurementsGlobal::kECTH:
		return fMeasInfo.fNumECTH;
	case TMeasurementsGlobal::kDVER :
		return fMeasInfo.fNumDVER;
	case TMeasurementsGlobal::kDSPT :
		return fMeasInfo.fNumDSPT;
	}
}

void TLGCData::setDefaults(){
	fLSRelatedInfo.fNumberOfLSIterations = 0;

	fUEOIndices.EIndex = 0;
	fUEOIndices.OIndex = 0;
	fUEOIndices.UIndex = 0;

	fPointInfo.fNumCala = 0;
	fPointInfo.fNumVx = 0;
	fPointInfo.fNumVy = 0;
	fPointInfo.fNumVz = 0;
	fPointInfo.fNumVxy = 0;
	fPointInfo.fNumVxz = 0;
	fPointInfo.fNumVyz = 0;
	fPointInfo.fNumVxyz = 0;


	fMeasInfo.fNumANGL = 0;
	fMeasInfo.fNumDIR3D = 0;
	fMeasInfo.fNumDIST = 0;
	fMeasInfo.fNumDLEV = 0;
	fMeasInfo.fNumPLR3D = 0;
	fMeasInfo.fNumZEND = 0;
	fMeasInfo.fNumDHOR = 0;
	fMeasInfo.fNumDVER = 0;
	fMeasInfo.fNumECTH = 0;
	fMeasInfo.fNumDSPT = 0;
}