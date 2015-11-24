#include "TLGCData.h"	

TLGCData::TLGCData() : 
	fCombinedCase(false),
	fhasStandardDeviations(false),
	fNumberOfConstraints(0)
	{
	setDefaultValues();
	// Create an empty root node
	tree.insert(tree.begin(), TDataSPtr(new TTreeEntry())) ;	
	
	// This is the current node of the tree
	pos = tree.begin();
	auto& n(getCurrentNode());

	//ROOT frame of the tree
	n.ID = std::vector<int> (1);
	n.ID[0] = 1;
	n.frame.setName("ROOT");
}

TTreeEntry& TLGCData::addChild(TAdjustableHelmertTransformation* transfo) {
	pos = tree.append_child(pos, TDataSPtr(new TTreeEntry()));
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


void TLGCData::setChiS0Limits(TReal chiLo, TReal chiUp)
{
		fLSRelatedInfo.fChiLoLimit = chiLo;
		fLSRelatedInfo.fChiUpLimit = chiUp;
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
		default:
            return 0;
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
		case TMeasurementsGlobal::kDIST :
			fMeasInfo.fNumDIST++;
			break;
		case TMeasurementsGlobal::kPLR3D:
			fMeasInfo.fNumPLR3D++;
			break;
		case TMeasurementsGlobal::kECTH:
			fMeasInfo.fNumECTH++;
			break;
		case TMeasurementsGlobal::kDLEV:
			fMeasInfo.fNumDLEV++;
			break;
		case TMeasurementsGlobal::kDVER:
			fMeasInfo.fNumDVER++;
			break;
		case TMeasurementsGlobal::kDSPT:
			fMeasInfo.fNumDSPT++;
			break;
		case TMeasurementsGlobal::kUVEC:
			fMeasInfo.fNumUVEC++;
			break;
		case TMeasurementsGlobal::kUVD:
			fMeasInfo.fNumUVD++;
			break;
		case TMeasurementsGlobal::kECHO:
			fMeasInfo.fNumECHO++;
			break;
		case TMeasurementsGlobal::kECSP:
			fMeasInfo.fNumECSP++;
			break;
		case TMeasurementsGlobal::kECVE:
			fMeasInfo.fNumECVE++;
			break;
		case TMeasurementsGlobal::kORIE:
			fMeasInfo.fNumORIE++;
			break;
		case TMeasurementsGlobal::kPDOR:
			fMeasInfo.fNumPDOR++;
			break;
		case TMeasurementsGlobal::kRADI:
			fMeasInfo.fNumRADI++;
			break;
	}
}


int TLGCData::getMeasurementDimension(TMeasurementsGlobal::EMeasurementType type) const{
	switch (type){
	case TMeasurementsGlobal::kANGL:
		return fMeasInfo.fNumANGL;
	case TMeasurementsGlobal::kZEND:
		return fMeasInfo.fNumZEND;
	case TMeasurementsGlobal::kDIST:
		return fMeasInfo.fNumDIST;
	case TMeasurementsGlobal::kPLR3D:
		return fMeasInfo.fNumPLR3D;
	case TMeasurementsGlobal::kDHOR :
		return fMeasInfo.fNumDHOR;
	case TMeasurementsGlobal::kDSPT:
		return fMeasInfo.fNumDSPT;
	case TMeasurementsGlobal::kECTH:
		return fMeasInfo.fNumECTH;
	case TMeasurementsGlobal::kDLEV:
		return fMeasInfo.fNumDLEV;
	case TMeasurementsGlobal::kDVER:
		return fMeasInfo.fNumDVER;
	case TMeasurementsGlobal::kUVEC :
		return fMeasInfo.fNumUVEC;
	case TMeasurementsGlobal::kUVD :
		return fMeasInfo.fNumUVD;
	case TMeasurementsGlobal::kECHO :
		return fMeasInfo.fNumECHO;
	case TMeasurementsGlobal::kECSP:
		return fMeasInfo.fNumECSP;
	case TMeasurementsGlobal::kECVE:
		return fMeasInfo.fNumECVE;
	case TMeasurementsGlobal::kORIE :
		return fMeasInfo.fNumORIE;
	case TMeasurementsGlobal::kRADI:
		return fMeasInfo.fNumRADI;
	default:
        return 0;
	}
}

void TLGCData::setDefaultValues() {
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
	fMeasInfo.fNumDIST = 0;
	fMeasInfo.fNumDLEV = 0;
	fMeasInfo.fNumPLR3D = 0;
	fMeasInfo.fNumZEND = 0;
	fMeasInfo.fNumDHOR = 0;
	fMeasInfo.fNumDVER = 0;
	fMeasInfo.fNumECTH = 0;
	fMeasInfo.fNumDSPT = 0;
	fMeasInfo.fNumUVEC = 0;
	fMeasInfo.fNumUVD = 0;
	fMeasInfo.fNumECHO = 0;
	fMeasInfo.fNumECSP = 0;
	fMeasInfo.fNumECVE = 0;
	fMeasInfo.fNumORIE = 0;
	fMeasInfo.fNumPDOR = 0;
	fMeasInfo.fNumRADI = 0;

	config.referential = TRefSystemFactory::ERefFrame::kNotInGraph;
}

void TLGCData::reInitForSIMU(){
	for (auto& point : points)
		point.reInitialise();

	for (auto& plane : planes)
		plane.reInitialise();

	for (auto& angle : angles)
		angle.reInitialise();
	
	for (auto& length : lengths)
		length.reInitialise();

	for (auto& line : lines)
		line.reInitialise();

	for (auto nodeIt( tree.begin()); nodeIt != tree.end(); ++nodeIt)
		nodeIt->get()->frame.reInitialise();


	/*Eventually also all the statistics but they will be rewritten anyway, so it seems not to be necessary*/
}
