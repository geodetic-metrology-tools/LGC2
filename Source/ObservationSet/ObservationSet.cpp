// ObservationSet.cpp

#include "ObservationSet.h"


//////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////
ObservationSet::ObservationSet()
{// Default constructor
	fWorkingHorAngObs = new TWorkingObservation<THorAngleObservation>();
	fWorkingZenDistObs = new TWorkingObservation<TZenithDistObservation>();
	fWorkingSpaDistObs = new TWorkingObservation<TSpatialDistObservation>();
	fWorkingHorDistObs = new TWorkingObservation<THorizontalDistObservation>();
	fWorkingVertDistObs = new TWorkingObservation<TVertDistObservation>();
	fWorkingDLEVDistObs = new TWorkingObservation<TVertDistObservation>();
	fWorkingOffsetToVerLineObs = new TWorkingObservation<TOffsetToVerLineObservation>();
	fWorkingOffsetToSpaLineObs = new TWorkingObservation<TOffsetToSpaLineObservation>();
	fWorkingOffsetToVerPlaneObs = new TWorkingObservation<TOffsetToVerPlaneObservation>();
	fWorkingOffsetToTheoPlaneObs = new TWorkingObservation<TOffsetToTheoPlaneObservation>();
	fWorkingGyroOrieObs = new TWorkingObservation<TGyroOrientationObservation>();
	fWorkingRadOffCnstrObs = new TWorkingObservation<TRadialOffsetCnstrObservation>();
	fWorkingOrieCnstrObs = new TWorkingObservation<TOrientationCnstrObservation>();
}


ObservationSet::~ObservationSet()
{// Destructor
	delete fWorkingHorAngObs;
	delete fWorkingZenDistObs;
	delete fWorkingSpaDistObs;
	delete fWorkingHorDistObs;
	delete fWorkingVertDistObs;
	delete fWorkingDLEVDistObs;
	delete fWorkingOffsetToVerLineObs;
	delete fWorkingOffsetToSpaLineObs;
	delete fWorkingOffsetToVerPlaneObs;
	delete fWorkingOffsetToTheoPlaneObs;
	delete fWorkingGyroOrieObs;
	delete fWorkingRadOffCnstrObs;
	delete fWorkingOrieCnstrObs;

}

///////////////////////////////////////////////////
// METHODS FOR HOR.ANG. OBSERVATIONS LIST
////////////////////////////////////////////////////

// Returns a pointer to the working horizontal angle observations 
TWorkingObservation<THorAngleObservation>* ObservationSet::getWorkingHorAngObs() const {

	return fWorkingHorAngObs;
}

// begin -> horAngIter
HorAngObsIter	ObservationSet::beginHorAng() {

	return fWorkingHorAngObs->begin();
}

// begin -> horAngConstIter
HorAngObsConstIter ObservationSet::beginHorAng() const {

	return fWorkingHorAngObs->begin();
}

// end -> horAngIter
HorAngObsIter ObservationSet::endHorAng() {

	return fWorkingHorAngObs->end();
}

// end -> horAngConstIter
HorAngObsConstIter ObservationSet::endHorAng() const {

	return fWorkingHorAngObs->end();
}

// push_back a new item to the list
void	ObservationSet::push_backHorAng(const THorAngleObservation& ha, bool noDuplicates) {

	fWorkingHorAngObs->push_back(ha, noDuplicates);
}

///////////////////////////////////////////////////
// METHODS FOR ZEN.ANG. OBSERVATIONS LIST
////////////////////////////////////////////////////

// Returns a pointer to the working zenithal angle observations
TWorkingObservation<TZenithDistObservation>* ObservationSet::getWorkingZenDistObs() const {

	return fWorkingZenDistObs;
}

// begin -> horAngIter
ZenDistObsIter	ObservationSet::beginZenDist() {

	return fWorkingZenDistObs->begin();
}

// begin -> horAngConstIter
ZenDistObsConstIter ObservationSet::beginZenDist() const {

	return fWorkingZenDistObs->begin();
}

// end -> horAngIter
ZenDistObsIter ObservationSet::endZenDist() {

	return fWorkingZenDistObs->end();
}

// end -> horAngConstIter
ZenDistObsConstIter ObservationSet::endZenDist() const {

	return fWorkingZenDistObs->end();
}

// push_back a new item to the list
void	ObservationSet::push_backZenDist(const TZenithDistObservation& zd, bool noDuplicates) {

	fWorkingZenDistObs->push_back(zd, noDuplicates);
}

///////////////////////////////////////////////////
// METHODS FOR SPATIAL DIST. OBSERVATIONS LIST
////////////////////////////////////////////////////


// Returns a pointer to the working horizontal dist observations
TWorkingObservation<TSpatialDistObservation>* ObservationSet::getWorkingSpaDistObs() const {

	return fWorkingSpaDistObs;
}

// begin -> SpaDistIter
SpaDistObsIter	ObservationSet::beginSpaDist() {

	return fWorkingSpaDistObs->begin();
}

// begin -> horAngConstIter
SpaDistObsConstIter ObservationSet::beginSpaDist() const {

	return fWorkingSpaDistObs->begin();
}

// end -> horAngIter
SpaDistObsIter ObservationSet::endSpaDist() {

	return fWorkingSpaDistObs->end();
}

// end -> horAngConstIter
SpaDistObsConstIter ObservationSet::endSpaDist() const {

	return fWorkingSpaDistObs->end();
}

// push_back a new item to the list
void	ObservationSet::push_backSpaDist(const TSpatialDistObservation& sd, bool noDuplicates) {

	fWorkingSpaDistObs->push_back(sd, noDuplicates);
}

///////////////////////////////////////////////////
// METHODS FOR HORIZONTAL DIST. OBSERVATIONS LIST
////////////////////////////////////////////////////


// Returns a pointer to the working horizontal dist. observations
TWorkingObservation<THorizontalDistObservation>* ObservationSet::getWorkingHorDistObs() const {

	return fWorkingHorDistObs;
}

// begin -> SpaDistIter
HorDistObsIter	ObservationSet::beginHorDist() {

	return fWorkingHorDistObs->begin();
}

// begin -> horAngConstIter
HorDistObsConstIter ObservationSet::beginHorDist() const {

	return fWorkingHorDistObs->begin();
}

// end -> horAngIter
HorDistObsIter ObservationSet::endHorDist() {

	return fWorkingHorDistObs->end();
}

// end -> horAngConstIter
HorDistObsConstIter ObservationSet::endHorDist() const {

	return fWorkingHorDistObs->end();
}

// push_back a new item to the list
void	ObservationSet::push_backHorDist(const THorizontalDistObservation& hd, bool noDuplicates) {

	fWorkingHorDistObs->push_back(hd, noDuplicates);
}

///////////////////////////////////////////////////
// METHODS FOR VERTICAL DIST. OBSERVATIONS LIST
////////////////////////////////////////////////////

// Returns a pointer to the working horizontal dist. observations
TWorkingObservation<TVertDistObservation>* ObservationSet::getWorkingVertDistObs(bool isDLEV) const {

	return isDLEV ? fWorkingDLEVDistObs : fWorkingVertDistObs;
}

// begin -> VertDistIter
VertDistObsIter	ObservationSet::beginVertDist(bool isDLEV) {

	return isDLEV ? fWorkingDLEVDistObs->begin() : fWorkingVertDistObs->begin();
}

// begin -> VertAngConstIter
VertDistObsConstIter ObservationSet::beginVertDist(bool isDLEV) const {

	return isDLEV ? fWorkingDLEVDistObs->begin() : fWorkingVertDistObs->begin();
}

// end -> VertAngIter
VertDistObsIter ObservationSet::endVertDist(bool isDLEV) {

	return isDLEV ? fWorkingDLEVDistObs->end() : fWorkingVertDistObs->end();
}

// end -> VertAngConstIter
VertDistObsConstIter ObservationSet::endVertDist(bool isDLEV) const {

	return isDLEV ? fWorkingDLEVDistObs->end() : fWorkingVertDistObs->end();
}

// push_back a new item to the list
void	ObservationSet::push_backVertDist(const TVertDistObservation& vd, bool noDuplicates, bool isDLEV) {
	if (isDLEV)
		fWorkingDLEVDistObs->push_back(vd, noDuplicates);
	else
		fWorkingVertDistObs->push_back(vd, noDuplicates);
}

int	 ObservationSet::sizeVertDist(bool isDLEV) const {
	return isDLEV ? fWorkingDLEVDistObs->size() : fWorkingVertDistObs->size();
}

bool ObservationSet::getWorkingVertDistObsError(bool isDLEV) const { 
	return isDLEV ? fWorkingDLEVDistObs->getListError() : fWorkingVertDistObs->getListError(); 
}



////////////////////////////////////////////////////////
// METHODS FOR OFFSET TO VERTICAL LINE OBSERVATIONS LIST
////////////////////////////////////////////////////////
TWorkingObservation<TOffsetToVerLineObservation>* ObservationSet::getWorkingOffsetToVerLineObs() const
{// Returns a pointer to the working offset to ver. line observations
	return fWorkingOffsetToVerLineObs;
}


OffsetToVerLineObsIter	ObservationSet::beginOffsetToVerLine()
{// begin -> OffsetToVerLineObsIter
	return fWorkingOffsetToVerLineObs->begin();
}


OffsetToVerLineObsConstIter ObservationSet::beginOffsetToVerLine() const
{// begin -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToVerLineObs->begin();
}


OffsetToVerLineObsIter ObservationSet::endOffsetToVerLine()
{// end -> OffsetToVerLineObsIter
	return fWorkingOffsetToVerLineObs->end();
}


OffsetToVerLineObsConstIter ObservationSet::endOffsetToVerLine() const
{// end -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToVerLineObs->end();
}


void	ObservationSet::push_backOffsetToVerLine(const TOffsetToVerLineObservation& obs, bool noDuplicates)
{// push_back a new item to the list
	fWorkingOffsetToVerLineObs->push_back(obs, noDuplicates);
}


///////////////////////////////////////////////////////
// METHODS FOR OFFSET TO SPATIAL LINE OBSERVATIONS LIST
///////////////////////////////////////////////////////
TWorkingObservation<TOffsetToSpaLineObservation>* ObservationSet::getWorkingOffsetToSpaLineObs() const
{// Returns a pointer to the working offset to ver. line observations
	return fWorkingOffsetToSpaLineObs;
}


OffsetToSpaLineObsIter	ObservationSet::beginOffsetToSpaLine()
{// begin -> OffsetToVerLineObsIter
	return fWorkingOffsetToSpaLineObs->begin();
}


OffsetToSpaLineObsConstIter ObservationSet::beginOffsetToSpaLine() const
{// begin -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToSpaLineObs->begin();
}


OffsetToSpaLineObsIter ObservationSet::endOffsetToSpaLine()
{// end -> OffsetToVerLineObsIter
	return fWorkingOffsetToSpaLineObs->end();
}


OffsetToSpaLineObsConstIter ObservationSet::endOffsetToSpaLine() const
{// end -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToSpaLineObs->end();
}


void	ObservationSet::push_backOffsetToSpaLine(const TOffsetToSpaLineObservation& obs, bool noDuplicates)
{// push_back a new item to the list
	fWorkingOffsetToSpaLineObs->push_back(obs, noDuplicates);
}


/////////////////////////////////////////////////////////
// METHODS FOR OFFSET TO VERTICAL PLANE OBSERVATIONS LIST
/////////////////////////////////////////////////////////
TWorkingObservation<TOffsetToVerPlaneObservation>* ObservationSet::getWorkingOffsetToVerPlaneObs() const
{// Returns a pointer to the working offset to ver. line observations
	return fWorkingOffsetToVerPlaneObs;
}


OffsetToVerPlaneObsIter	ObservationSet::beginOffsetToVerPlane()
{// begin -> OffsetToVerLineObsIter
	return fWorkingOffsetToVerPlaneObs->begin();
}


OffsetToVerPlaneObsConstIter ObservationSet::beginOffsetToVerPlane() const
{// begin -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToVerPlaneObs->begin();
}


OffsetToVerPlaneObsIter ObservationSet::endOffsetToVerPlane()
{// end -> OffsetToVerLineObsIter
	return fWorkingOffsetToVerPlaneObs->end();
}


OffsetToVerPlaneObsConstIter ObservationSet::endOffsetToVerPlane() const
{// end -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToVerPlaneObs->end();
}


void	ObservationSet::push_backOffsetToVerPlane(const TOffsetToVerPlaneObservation& obs, bool noDuplicates)
{// push_back a new item to the list
	fWorkingOffsetToVerPlaneObs->push_back(obs, noDuplicates);
}


///////////////////////////////////////////////////////////
// METHODS FOR OFFSET TO THEODOLITE PLANE OBSERVATIONS LIST
///////////////////////////////////////////////////////////
TWorkingObservation<TOffsetToTheoPlaneObservation>* ObservationSet::getWorkingOffsetToTheoPlaneObs() const
{// Returns a pointer to the working offset to ver. line observations
	return fWorkingOffsetToTheoPlaneObs;
}


OffsetToTheoPlaneObsIter	ObservationSet::beginOffsetToTheoPlane()
{// begin -> OffsetToVerLineObsIter
	return fWorkingOffsetToTheoPlaneObs->begin();
}


OffsetToTheoPlaneObsConstIter ObservationSet::beginOffsetToTheoPlane() const
{// begin -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToTheoPlaneObs->begin();
}


OffsetToTheoPlaneObsIter ObservationSet::endOffsetToTheoPlane()
{// end -> OffsetToVerLineObsIter
	return fWorkingOffsetToTheoPlaneObs->end();
}


OffsetToTheoPlaneObsConstIter ObservationSet::endOffsetToTheoPlane() const
{// end -> OffsetToVerLineObsConstIter
	return fWorkingOffsetToTheoPlaneObs->end();
}


void	ObservationSet::push_backOffsetToTheoPlane(const TOffsetToTheoPlaneObservation& obs, bool noDuplicates)
{// push_back a new item to the list
	fWorkingOffsetToTheoPlaneObs->push_back(obs, noDuplicates);
}


///////////////////////////////////////////////////////////
// METHODS FOR ORIENTATION OBSERVATIONS LIST
///////////////////////////////////////////////////////////
TWorkingObservation<TGyroOrientationObservation>* ObservationSet::getWorkingGyroOrieObs() const
{// Returns a pointer to the working offset to ver. line observations
	return fWorkingGyroOrieObs;
}


GyroOrieObsIter	ObservationSet::beginGyroOrie()
{// begin -> GyroOrieObsIter
	return fWorkingGyroOrieObs->begin();
}


GyroOrieObsConstIter ObservationSet::beginGyroOrie() const
{// begin -> GyroOrieObsConstIter
	return fWorkingGyroOrieObs->begin();
}


GyroOrieObsIter ObservationSet::endGyroOrie()
{// end -> GyroOrieObsIter
	return fWorkingGyroOrieObs->end();
}


GyroOrieObsConstIter ObservationSet::endGyroOrie() const
{// end -> GyroOrieObsConstIter
	return fWorkingGyroOrieObs->end();
}


void	ObservationSet::push_backGyroOrie(const TGyroOrientationObservation& obs, bool noDuplicates)
{// push_back a new item to the list
	fWorkingGyroOrieObs->push_back(obs, noDuplicates);
}


///////////////////////////////////////////////////////////
// METHODS FOR RADIAL OFFSET CONSTRAINTS OBSERVATIONS LIST
///////////////////////////////////////////////////////////
TWorkingObservation<TRadialOffsetCnstrObservation>* ObservationSet::getWorkingRadOffCnstrObs() const
{
	return fWorkingRadOffCnstrObs;
}


RadOffCnstrObsIter	ObservationSet::beginRadOffCnstr()
{
	return fWorkingRadOffCnstrObs->begin();
}


RadOffCnstrObsConstIter ObservationSet::beginRadOffCnstr() const
{
	return fWorkingRadOffCnstrObs->begin();
}


RadOffCnstrObsIter ObservationSet::endRadOffCnstr()
{
	return fWorkingRadOffCnstrObs->end();
}


RadOffCnstrObsConstIter ObservationSet::endRadOffCnstr() const
{
	return fWorkingRadOffCnstrObs->end();
}


void	ObservationSet::push_backRadOffCnstr(const TRadialOffsetCnstrObservation& obs, bool noDuplicates)
{
	fWorkingRadOffCnstrObs->push_back(obs, noDuplicates);
}


///////////////////////////////////////////////////////////
// METHODS FOR ORIENTATION CONSTRAINTS OBSERVATIONS LIST
///////////////////////////////////////////////////////////
TWorkingObservation<TOrientationCnstrObservation>* ObservationSet::getWorkingOrieCnstrObs() const
{
	return fWorkingOrieCnstrObs;
}


OrieCnstrObsIter	ObservationSet::beginOrieCnstr()
{
	return fWorkingOrieCnstrObs->begin();
}


OrieCnstrObsConstIter ObservationSet::beginOrieCnstr() const
{
	return fWorkingOrieCnstrObs->begin();
}


OrieCnstrObsIter ObservationSet::endOrieCnstr()
{
	return fWorkingOrieCnstrObs->end();
}


OrieCnstrObsConstIter ObservationSet::endOrieCnstr() const
{
	return fWorkingOrieCnstrObs->end();
}


void	ObservationSet::push_backOrieCnstr(const TOrientationCnstrObservation& obs, bool noDuplicates)
{
	fWorkingOrieCnstrObs->push_back(obs, noDuplicates);
}


///////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////

