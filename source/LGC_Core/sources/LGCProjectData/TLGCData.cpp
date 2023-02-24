#include <TLGCData.h>	

TLGCData::TLGCData() 
    : fileLogger(std::make_shared<TFileLogger>())
    , fhasStandardDeviations(false) 
    , fUEOIndices({0,0,0,0})
{
        fLSRelatedInfo.fNumberOfLSIterations = 0;
	    setDefaultValues();
        config.referential = TRefSystemFactory::ERefFrame::kNotInGraph;

	// Create an empty root node
	tree.insert(tree.begin(), TDataSPtr(new TTreeEntry()));

	// This is the current node of the tree
	pos = tree.begin();
	auto& n(getCurrentNode());

	//ROOT frame of the tree
	n.ID = std::vector<int>(1);
	n.ID[0] = 1;
	n.frame.setName("ROOT");

    // covar Matrix
	fCovMat = new TSparseMatrix;
}

TLGCData::~TLGCData(){
    // should be done in addition to default destructor
	delete fCovMat;
	fCovMat = nullptr;
}

TDataTreeIterator TLGCData::locateNode(std::string frameName) const
{
	// Find the frame iterator with the corresponding frame name
	TDataTreeIterator lastNodeIter = getTree().end();
	TDataTreeIterator currentNodeIter = getTree().begin();
	while (currentNodeIter != lastNodeIter)
	{
		if (currentNodeIter.node->data.get()->frame.getName() == frameName)
		{
			break;
		}
		currentNodeIter++;
	}
	if (currentNodeIter == lastNodeIter)
		throw std::runtime_error("Frame not found");

	return currentNodeIter;
}

TTreeEntry &TLGCData::addChild(TAdjustableHelmertTransformation *transfo)
{
	pos = tree.append_child(pos, TDataSPtr(new TTreeEntry()));
	auto& n(getCurrentNode());
	n.frame = *transfo;
	//Assign unique ID to the node
	int depth = tree.depth(pos);
	int nOfSiblings = tree.number_of_siblings(pos);
	n.ID = std::vector<int>(depth + 1);
	TDataTreeIterator parent = TDataTree::parent(pos);

	for (int i = 0; i < depth; i++) {
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
	switch (status) {
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
	switch (status) {
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
		case TMeasurementsGlobal::kECDIR:
			fMeasInfo.fNumECDIR++;
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
		case TMeasurementsGlobal::kOBSXYZ:
			fMeasInfo.fNumOBSXYZ++;
			break;
		case TMeasurementsGlobal::kINCLY:
			fMeasInfo.fNumINCLY++;
			break;
		case TMeasurementsGlobal::kECWS:
			fMeasInfo.fNumECWS++;
			break;
		case TMeasurementsGlobal::kECWI:
			fMeasInfo.fNumECWI++;
			break;
	}
}


int TLGCData::getMeasurementDimension(TMeasurementsGlobal::EMeasurementType type) const
{
	switch (type)
	{
	case TMeasurementsGlobal::kANGL:
		return fMeasInfo.fNumANGL;
	case TMeasurementsGlobal::kZEND:
		return fMeasInfo.fNumZEND;
	case TMeasurementsGlobal::kDIST:
		return fMeasInfo.fNumDIST;
	case TMeasurementsGlobal::kPLR3D:
		return fMeasInfo.fNumPLR3D;
	case TMeasurementsGlobal::kDHOR:
		return fMeasInfo.fNumDHOR;
	case TMeasurementsGlobal::kDSPT:
		return fMeasInfo.fNumDSPT;
	case TMeasurementsGlobal::kECTH:
		return fMeasInfo.fNumECTH;
	case TMeasurementsGlobal::kECDIR:
		return fMeasInfo.fNumECDIR;
	case TMeasurementsGlobal::kDLEV:
		return fMeasInfo.fNumDLEV;
	case TMeasurementsGlobal::kDVER:
		return fMeasInfo.fNumDVER;
	case TMeasurementsGlobal::kUVEC:
		return fMeasInfo.fNumUVEC;
	case TMeasurementsGlobal::kUVD:
		return fMeasInfo.fNumUVD;
	case TMeasurementsGlobal::kECHO:
		return fMeasInfo.fNumECHO;
	case TMeasurementsGlobal::kECSP:
		return fMeasInfo.fNumECSP;
	case TMeasurementsGlobal::kECVE:
		return fMeasInfo.fNumECVE;
	case TMeasurementsGlobal::kORIE:
		return fMeasInfo.fNumORIE;
	case TMeasurementsGlobal::kRADI:
		return fMeasInfo.fNumRADI;
	case TMeasurementsGlobal::kOBSXYZ:
		return fMeasInfo.fNumOBSXYZ;
	case TMeasurementsGlobal::kINCLY:
		return fMeasInfo.fNumINCLY;
	case TMeasurementsGlobal::kECWS:
		return fMeasInfo.fNumECWS;
	case TMeasurementsGlobal::kECWI:
		return fMeasInfo.fNumECWI;
	default:
		return 0;
	}
}

void TLGCData::setDefaultValues()
{
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
	fMeasInfo.fNumECDIR = 0;
	fMeasInfo.fNumDSPT = 0;
	fMeasInfo.fNumUVEC = 0;
	fMeasInfo.fNumUVD = 0;
	fMeasInfo.fNumECHO = 0;
	fMeasInfo.fNumECSP = 0;
	fMeasInfo.fNumECVE = 0;
	fMeasInfo.fNumORIE = 0;
	fMeasInfo.fNumPDOR = 0;
	fMeasInfo.fNumRADI = 0;
	fMeasInfo.fNumOBSXYZ = 0;
	fMeasInfo.fNumINCLY = 0;
	fMeasInfo.fNumECWS = 0;
	fMeasInfo.fNumECWI = 0;
}

const TSparseMatrix *TLGCData::getCovMatByConst() const noexcept
{
	return fCovMat;
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

	for (auto nodeIt(tree.begin()); nodeIt != tree.end(); ++nodeIt)
		nodeIt->get()->frame.reInitialise();


	/*Eventually also all the statistics but they will be rewritten anyway, so it seems not to be necessary*/
}


std::shared_ptr<TLGCData> TLGCData::clone() const {
	auto d = std::make_shared<TLGCData>();

	d->islgc1 = islgc1;

	// Copy adjustable objects collections:
	d->points = points;
	d->lines = lines;
	d->planes = planes;
	d->angles = angles;
	d->lengths = lengths;

	// Copy tree and reset position:
	copyTree(this, d.get());
	d->pos = d->tree.begin();

	// Reset the frameTreePositions in LGCAdjustablePoints and
	// move the pointers to correct objects in lines and planes:
	updateAdjustableObjectsPointers(d.get());

    // Copy configuration:
    d->config = config;
    d->fhasStandardDeviations = fhasStandardDeviations;
    d->fUEOIndices = fUEOIndices;
    d->fLSRelatedInfo = fLSRelatedInfo;
    d->fPointInfo = fPointInfo;
    d->fMeasInfo = fMeasInfo;

	// Copy instruments:
	copyInstruments(this, d.get());

    // Copy filelogger:
    d->fileLogger = fileLogger;
    
    // Copy statistics:
    d->stat = stat;
	d->fCovMat = fCovMat;

	// Copy relative errors:
	for (const auto& erelPair : fRelError) {
		// Get the correct points from the new container:
		auto p1 = d->points.getObject(erelPair.getPoint1Name());
		auto p2 = d->points.getObject(erelPair.getPoint2Name());

		// Create the erel pair, set sigmas:
		TLSCalcRelativeError erel(p1, p2);
		erel.setSigmaL(erelPair.getSigmaL());
		erel.setSigmaR(erelPair.getSigmaR());
		erel.setSigmaZ(erelPair.getSigmaZ());
		erel.setSigmaG(erelPair.getSigmaG());
		erel.setSigmaV(erelPair.getSigmaV());

		// Add to the new container:
		d->fRelError.push_back(erel);
	}

	// Copy comments
	d->comments = comments;

	return d;
}


void TLGCData::copyTree(TLGCData const* const src, TLGCData* tgt) {

	// Copy the tree structure/form:
	tgt->tree = src->tree;

	// Loop the tree in order to create a deep copy of each node
	// (NB. Iterator needed for the OBSXYZ measurement)
	for (auto tree_iter = tgt->tree.begin(); tree_iter != tgt->tree.end(); ++tree_iter) {

		auto& entry = *tree_iter;

		// First replace the contents of the node:
		entry.reset(new TTreeEntry(*entry));

		// Necessary to manage all measurements separately thanks to pointers and references

		// TSTN
		for (auto& tstn : entry->measurements.fTSTN) {

			// Replace the tstn:
			tstn.reset(new TTSTN(*tstn));

			// Reset the pointers to point to objects in the target core:
			tstn->instrumentPos = &tgt->points.getObject(tstn->instrumentPos->getName());

			if (tstn->instrumentHeightAdjustable)
				tstn->instrumentHeightAdjustable = &tgt->lengths.getObject(tstn->instrumentHeightAdjustable->getName());

			if (tstn->rotX)
				tstn->rotX = &tgt->angles.getObject(tstn->rotX->getName());

			if (tstn->rotY)
				tstn->rotY = &tgt->angles.getObject(tstn->rotY->getName());

			for (auto& plrTgt : tstn->instrument.targets)
				if (plrTgt.second->distCorrectionAdjustable)
					plrTgt.second->distCorrectionAdjustable = &tgt->lengths.getObject(plrTgt.second->distCorrectionAdjustable->getName());

			// TSTN::ROM
			for (auto& rom : tstn->roms) {

				// Replace the rom:
				rom.reset(new TTSTN::TROM(*rom));

				// Reset the pointers:
				if (rom->v0)
					rom->v0 = &tgt->angles.getObject(rom->v0->getName());

				// Measurements in this rom

				// PLR3D
				for (auto& meas : rom->measPLR3D) {
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

					if (meas.target.distCorrectionAdjustable)
						meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
				}

				// ANGL
				for (auto& meas : rom->measANGL) {
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

					if (meas.target.distCorrectionAdjustable)
						meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
				}

				// ZEND
				for (auto& meas : rom->measZEND) {
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

					if (meas.target.distCorrectionAdjustable)
						meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
				}

				// DIST
				for (auto& meas : rom->measDIST) {
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

					if (meas.target.distCorrectionAdjustable)
						meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
				}

				// DHOR
				for (auto& meas : rom->measDHOR) {
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

					if (meas.target.distCorrectionAdjustable)
						meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
				}

				// ECTH
				for (auto& meas : rom->measECTH)
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

				// ECDIR
				for (auto& meas : rom->measECDIR)
					if (meas.targetPos)
						meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

			}
		}

		// CAM
		for (auto& cam : entry->measurements.fCAM) {

			// Reset the pointers to point to objects in the target core:
			cam.instrumentPos = &tgt->points.getObject(cam.instrumentPos->getName());

			// UVD
			for (auto& meas : cam.measUVD)
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

			// UVEC
			for (auto& meas : cam.measUVEC)
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
		}


		// EDM
		for (auto& edm : entry->measurements.fEDM) {

			// Reset the pointers to point to objects in the target core:
			edm.instrumentPos = &tgt->points.getObject(edm.instrumentPos->getName());

			for (auto& edmTgt : edm.instrument.targets)
				if (edmTgt.second->distCorrectionAdjustable)
					edmTgt.second->distCorrectionAdjustable = &tgt->lengths.getObject(edmTgt.second->distCorrectionAdjustable->getName());

			// DSPT
			for (auto& meas : edm.measDSPT) {
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

				if (meas.target.distCorrectionAdjustable)
					meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
			}
		}

		// LEVEL
		for (auto& level : entry->measurements.fLEVEL) {

			// Reset the pointers to point to objects in the target core:
			if (level.fRefPt)
				level.fRefPt = &tgt->points.getObject(level.fRefPt->getName());

			if (level.fMeasuredPlane)
				level.fMeasuredPlane = &tgt->planes.getObject(level.fMeasuredPlane->getName());

			if (level.instrument.collAngleAdjustable)
				level.instrument.collAngleAdjustable = &tgt->angles.getObject(level.instrument.collAngleAdjustable->getName());

			// DLEV
			for (auto& meas : level.measDLEV) {

				// If DHOR specified, replace it and reset pointer:
				if (meas.dhor) {

					meas.dhor.reset(new TDLEV::TDHOR(*meas.dhor));

					if (meas.dhor->targetPos)
						meas.dhor->targetPos = &tgt->points.getObject(meas.dhor->targetPos->getName());
				}

				// Reset the pointer
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
			}
		}

		// ORIE
		for (auto& orierom : entry->measurements.fORIE) {

			// Reset the pointers to point to objects in the target core:
			orierom.instrumentPos = &tgt->points.getObject(orierom.instrumentPos->getName());

			for (auto& plrTgt : orierom.instrument.targets)
				if (plrTgt.second->distCorrectionAdjustable)
					plrTgt.second->distCorrectionAdjustable = &tgt->lengths.getObject(plrTgt.second->distCorrectionAdjustable->getName());

			// Measurements
			for (auto& meas : orierom.measORIE) {
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());

				if (meas.target.distCorrectionAdjustable)
					meas.target.distCorrectionAdjustable = &tgt->lengths.getObject(meas.target.distCorrectionAdjustable->getName());
			}
		}

		// ECHO
		for (auto& echorom : entry->measurements.fECHO) {

			// Reset the pointers to point to objects in the target core:
			if (echorom.fMeasuredPlane)
				echorom.fMeasuredPlane = &tgt->planes.getObject(echorom.fMeasuredPlane->getName());

			// Measurements
			for (auto& meas : echorom.measECHO)
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
		}

		// ECVE
		for (auto& ecverom : entry->measurements.fECVE) {

			// Reset the pointers to point to objects in the target core:
			if (ecverom.fPtLine)
				ecverom.fPtLine = &tgt->points.getObject(ecverom.fPtLine->getName());

			if (ecverom.fMeasuredLine)
				ecverom.fMeasuredLine = &tgt->lines.getObject(ecverom.fMeasuredLine->getName());

			// Measurements
			for (auto& meas : ecverom.measECVE)
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
		}

		// ECSP
		for (auto& ecsprom : entry->measurements.fECSP) {

			// Reset the pointers to point to objects in the target core:
			if (ecsprom.p1)
				ecsprom.p1 = &tgt->points.getObject(ecsprom.p1->getName());

			if (ecsprom.p2)
				ecsprom.p2 = &tgt->points.getObject(ecsprom.p2->getName());

			// Measurements
			for (auto& meas : ecsprom.measECSP)
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
		}

		// DVER
		for (auto& dver : entry->measurements.fDVER) {

			// Reset the pointers to point to objects in the target core:

			if (dver.station)
				dver.station = &tgt->points.getObject(dver.station->getName());

			if (dver.targetPos)
				dver.targetPos = &tgt->points.getObject(dver.targetPos->getName());
		}

		// RADI
		for (auto& radi : entry->measurements.fRADI) {

			// Reset the pointers to point to objects in the target core:

			if (radi.station)
				radi.station = &tgt->points.getObject(radi.station->getName());

			if (radi.targetPos)
				radi.targetPos = &tgt->points.getObject(radi.targetPos->getName());
		}

		// OBSXYZ
		for (auto& obsxyz : entry->measurements.fOBSXYZ) {

			// Reset the pointers to point to objects in the target core:

			if (obsxyz.station)
				obsxyz.station = &tgt->points.getObject(obsxyz.station->getName());

			if (obsxyz.targetPos)
				obsxyz.targetPos = &tgt->points.getObject(obsxyz.targetPos->getName());

			// Update the tree position
			obsxyz.positionInTree = tree_iter;
		}

		// INCLY
		for (auto& inclyrom : entry->measurements.fINCLY) {

			// Measurements
			for (auto& meas : inclyrom.measINCLY)
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
		}

		// PDOR 
		auto& pdor = entry->measurements.fPDOR;

		// Reset the pointers to point to objects in the target core:
		if (pdor.calaPt)
			pdor.calaPt = &tgt->points.getObject(pdor.calaPt->getName());

		if (pdor.orientationPt)
			pdor.orientationPt = &tgt->points.getObject(pdor.orientationPt->getName());

        if(pdor.targetPos)
            pdor.targetPos = &tgt->points.getObject(pdor.targetPos->getName());

        // ECWS
        for (auto& ecwsrom : entry->measurements.fECWS) {

            // Measurements
            for (auto& meas : ecwsrom.measECWS) {
                if (meas.targetPos)
                    meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
            }      
        }

		// ECWI
		for (auto &ecwirom : entry->measurements.fECWI)
		{
			// Measurements
			for (auto &meas : ecwirom.measECWI)
			{
				if (meas.targetPos)
					meas.targetPos = &tgt->points.getObject(meas.targetPos->getName());
			}
		}
    }
}

void TLGCData::copyInstruments(TLGCData const* const src, TLGCData* tgtData) {

	// Copy the instruments and targets:
	tgtData->instruments = src->instruments;

	// Reset the pointers to instruments and targets and to adjustable objects in each concerned instrument/target:

	// Polar targets:
	for (auto& polar : tgtData->instruments.fPOLAR) {

		// Replace the instrument in the memory
		polar.second.reset(new TInstrumentData::TPOLAR(*polar.second));

		for (auto& tgt : polar.second->targets) {

			// Replace the target in the memory
			tgt.second.reset(new TInstrumentData::TPOLAR::TTarget(*tgt.second));

			if (tgt.second->distCorrectionAdjustable)
				tgt.second->distCorrectionAdjustable = &tgtData->lengths.getObject(tgt.second->distCorrectionAdjustable->getName());
		}
	}

	for (auto& camd : tgtData->instruments.fCAMD) {

		// Replace the instrument in the memory
		camd.second.reset(new TInstrumentData::TCAMD(*camd.second));

		for (auto& tgt : camd.second->targets)
			// Replace the target in the memory
			tgt.second.reset(new TInstrumentData::TCAMD::TTarget(*tgt.second));
	}

	for (auto& incl : tgtData->instruments.fINCL) {

		// Replace the instrument in the memory
		incl.second.reset(new TInstrumentData::TINCL(*incl.second));

	}

	// EDM targets:
	for (auto& edm : tgtData->instruments.fEDM) {

		// Replace the instrument in the memory
		edm.second.reset(new TInstrumentData::TEDM(*edm.second));

		for (auto& tgt : edm.second->targets) {

			// Replace the target in the memory
			tgt.second.reset(new TInstrumentData::TEDM::TTarget(*tgt.second));

			if (tgt.second->distCorrectionAdjustable)
				tgt.second->distCorrectionAdjustable = &tgtData->lengths.getObject(tgt.second->distCorrectionAdjustable->getName());
		}
	}

	// Levelling instrument:
	for (auto& level : tgtData->instruments.fLEVEL) {

		// Replace the instrument in the memory
		level.second.reset(new TInstrumentData::TLEVEL(*level.second));

		if (level.second->collAngleAdjustable)
			level.second->collAngleAdjustable = &tgtData->angles.getObject(level.second->collAngleAdjustable->getName());

		for (auto& tgt : level.second->targets)
			// Replace the target in the memory
			tgt.second.reset(new TInstrumentData::TLEVEL::TTarget(*tgt.second));
	}

    for(auto &scale : tgtData->instruments.fSCALE)
        // Replace the instrument in the memory
        scale.second.reset(new TInstrumentData::TSCALE(*scale.second));

    //HLSR instrument
    for (auto& hlsr : tgtData->instruments.fHLSR)
        // Replace the instrument in the memory
        hlsr.second.reset(new TInstrumentData::THLSR(*hlsr.second));

	// WPSR instrument
	for (auto &wpsr : tgtData->instruments.fWPSR)
		// Replace the instrument in the memory
		wpsr.second.reset(new TInstrumentData::TWPSR(*wpsr.second));
}

void TLGCData::updateAdjustableObjectsPointers(TLGCData* d) {

	// Update the iterators in points:
	for (auto& p : d->points) {

		// If frameTreePosition is unknown, continue
		if (p.getFrameTreePosition() == TDataTreeIterator()) continue;

		// Get the id of the node:
		auto nodeId = (*p.getFrameTreePosition())->ID;

		// Loop the tree and find the node with the same id,
		// update the frameTreePosition to the found node:
		for (auto it = d->tree.begin(); it != d->tree.end(); ++it)
			if ((*it)->ID == nodeId) {
				p.setFrameTreePosition(it);
				break;
			}
	}

	// Update the pointers in lines:
	for (auto& l : d->lines)
		if (l.getLinePoint())
			l.setLinePoint(&d->points.getObject(l.getLinePoint()->getName()));

	// Update the pointers in planes:
	for (auto& p : d->planes)
		if (p.getReferencePoint())
			p.setReferencePoint(&d->points.getObject(p.getReferencePoint()->getName()));

}

#if USE_SERIALIZER

void TLGCData::serialize(SerializerObject::SerializationHelper& obj) const
{
	obj.addProperty("angles", angles);
	obj.addProperty("comments", comments);
	obj.addProperty("config", config);
	obj.addProperty("fhasStandardDeviations", fhasStandardDeviations);
	obj.addProperty("fLSRelatedInfo", fLSRelatedInfo);
	obj.addProperty("fMeasInfo", fMeasInfo);
	obj.addProperty("fPointInfo", fPointInfo);
	obj.addProperty("fRelError", fRelError);
	obj.addProperty("fUEOIndices", fUEOIndices);
	obj.addProperty("instruments", instruments);
	obj.addProperty("islgc1", islgc1);
	obj.addProperty("lengths", lengths);
	obj.addProperty("lines", lines);
	obj.addProperty("planes", planes);
	obj.addProperty("points", points);
	obj.addProperty("stat", stat);
	obj.addProperty("tree", tree);
	obj.addProperty("fCovMat", fCovMat);
}


void TMeasurementsGlobal::serialize(SerializerObject::SerializationHelper& obj) const
{
	obj.addProperty("fNumANGL", fNumANGL);
	obj.addProperty("fNumZEND", fNumZEND);
	obj.addProperty("fNumDIST", fNumDIST);
	obj.addProperty("fNumPLR3D", fNumPLR3D);
	obj.addProperty("fNumDLEV", fNumDLEV);
	obj.addProperty("fNumDHOR", fNumDHOR);
	obj.addProperty("fNumECTH", fNumECTH);
	obj.addProperty("fNumECDIR", fNumECDIR);
	obj.addProperty("fNumDSPT", fNumDSPT);
	obj.addProperty("fNumDVER", fNumDVER);
	obj.addProperty("fNumUVEC", fNumUVEC);
	obj.addProperty("fNumUVD", fNumUVD);
	obj.addProperty("fNumECHO", fNumECHO);
	obj.addProperty("fNumECSP", fNumECSP);
	obj.addProperty("fNumECVE", fNumECVE);
	obj.addProperty("fNumORIE", fNumORIE);
	obj.addProperty("fNumPDOR", fNumPDOR);
	obj.addProperty("fNumRADI", fNumRADI);
	obj.addProperty("fNumOBSXYZ", fNumOBSXYZ);
	obj.addProperty("fNumINCLY", fNumINCLY);
}

void TPointGlobal::serialize(SerializerObject::SerializationHelper& obj) const
{
	obj.addProperty("fNumCala", fNumCala);
	obj.addProperty("fNumVx", fNumVx);
	obj.addProperty("fNumVy", fNumVy);
	obj.addProperty("fNumVz", fNumVz);
	obj.addProperty("fNumVxy", fNumVxy);
	obj.addProperty("fNumVxz", fNumVxz);
	obj.addProperty("fNumVyz", fNumVyz);
	obj.addProperty("fNumVxyz", fNumVxyz);
}

void TLGCData::TLSRelatedInfo::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("fNumberOfLSIterations", fNumberOfLSIterations);
	obj.addProperty("fS0APosteriori", fS0APosteriori);
	obj.addProperty("fChiLoLimit", fChiLoLimit);
	obj.addProperty("fChiUpLimit", fChiUpLimit);
};

#endif // USE_SERIALIZER
