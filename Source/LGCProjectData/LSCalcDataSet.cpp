////////////////////////////////////////////////////////////////////
//    LSCalcDataSet.cpp    :Implementation    file
//    Container    for    the    LGC    least    squares    calculation    data    set
//
//    Copyright    2003-2008    M.Jones,    CERN,    TS/SU.    All    rights    reserved.
////////////////////////////////////////////////////////////////////////////////////



#include    "LSCalcDataSet.h"

/////////////////////////////////////////////////////
//    CONSTRUCTOR    /    DESTRUCTOR
/////////////////////////////////////////////////////
LSCalcDataSet::LSCalcDataSet()
{//    Default    Constructor

    fConvergenceIsOk    =    false;

    fUEOIndices.UIndex    =    0;
    fUEOIndices.EIndex    =    0;
    fUEOIndices.OIndex    =    0;
    fS0APosteriori    =    TDouble(LITERAL(0.0));
    fChiUpLimit    =    LITERAL(0.0);
    fChiLoLimit    =    LITERAL(0.0);

    TLSConstraintIdentifier    dft;
    fLSFreeCnstr    =    dft;
    fUsedLibrOption    =    false;

    fLSWorkHorAngObs    =    new    TLSCalcWorkingHorAngObs();
    fLSWorkZenDistObs    =    new    TLSCalcWorkingZenDistObs();
    fLSWorkSpaDistObs    =    new    TLSCalcWorkingSpaDistObs();
    fLSWorkHorDistObs    =    new    TLSCalcWorkingHorDistObs();
    fLSWorkVertDistObs=    new    TLSCalcWorkingVertDistObs();
    fLSWorkDLEVDistObs=    new    TLSCalcWorkingVertDistObs();

    fLSWorkOffsetToVerLineObs=    new    TLSCalcWorkingOffsetToVerLineObs();
    fLSWorkOffsetToSpaLineObs=    new    TLSCalcWorkingOffsetToSpaLineObs();
    fLSWorkOffsetToVerPlaneObs=    new    TLSCalcWorkingOffsetToVerPlaneObs();
    fLSWorkOffsetToTheoPlaneObs=    new    TLSCalcWorkingOffsetToTheoPlaneObs();

    fLSWorkGyroOrieObs=    new    TLSCalcWorkingGyroOrientationObs();

    fLSWorkRadOffCnstrObs=    new    TLSCalcWorkingRadialOffsetCnstrObs();
    fLSWorkOrieCnstrObs=    new    TLSCalcWorkingOrientationCnstrObs();

    fLSWorkOrient    =    new    TLSCalcWorkingOrientation();
    fLSWorkPosVec    =    new    TLSCalcWorkingPosVec();
    fLSWorkLength    =    new    TLSCalcWorkingLength();
    
    fLSWorkRelErrors    =    new    TLSCalcWorkingRelativeErrors();
	
	fSimResults    =    new    TLGCSimResults(fLSWorkPosVec);

	fDeformationAnalysis = false;
}


LSCalcDataSet::~LSCalcDataSet()
{//    Destructor

    delete    fLSWorkHorAngObs;
    delete    fLSWorkZenDistObs;
    delete    fLSWorkSpaDistObs;
    delete    fLSWorkHorDistObs;
    delete    fLSWorkVertDistObs;
    delete    fLSWorkDLEVDistObs;

    delete    fLSWorkOffsetToVerLineObs;
    delete    fLSWorkOffsetToSpaLineObs;
    delete    fLSWorkOffsetToVerPlaneObs;
    delete    fLSWorkOffsetToTheoPlaneObs;

    delete    fLSWorkGyroOrieObs;

    delete    fLSWorkRadOffCnstrObs;
    delete    fLSWorkOrieCnstrObs;

    delete    fLSWorkOrient;
    delete    fLSWorkPosVec;
    delete    fLSWorkLength;

    delete    fLSWorkRelErrors;
}

////////////////////////////////////////////////////////////////
//PUBLIC    MEMBER    FUNCTIONS
///////////////////////////////////////////////////////////////
LSCalcDataSet&    LSCalcDataSet::operator=(    const    LSCalcDataSet&    original)
{//!    Copy    Assignment    Operator

    fConvergenceIsOk    =    original.fConvergenceIsOk;

    fLSWorkHorAngObs    =    original.fLSWorkHorAngObs;    
    fLSWorkZenDistObs    =    original.fLSWorkZenDistObs;
    fLSWorkSpaDistObs    =    original.fLSWorkSpaDistObs;
    fLSWorkHorDistObs    =    original.fLSWorkHorDistObs;
    fLSWorkVertDistObs    =    original.fLSWorkVertDistObs;
    fLSWorkDLEVDistObs    =    original.fLSWorkDLEVDistObs;

    fLSWorkOffsetToVerLineObs    =    original.fLSWorkOffsetToVerLineObs;
    fLSWorkOffsetToSpaLineObs    =    original.fLSWorkOffsetToSpaLineObs;
    fLSWorkOffsetToVerPlaneObs    =    original.fLSWorkOffsetToVerPlaneObs;
    fLSWorkOffsetToTheoPlaneObs    =    original.fLSWorkOffsetToTheoPlaneObs;

    fLSWorkGyroOrieObs    =    original.fLSWorkGyroOrieObs;

    fLSWorkRadOffCnstrObs    =    original.fLSWorkRadOffCnstrObs;
    fLSWorkOrieCnstrObs    =    original.fLSWorkOrieCnstrObs;

    fLSWorkOrient    =    original.fLSWorkOrient;
    fLSWorkPosVec    =    original.fLSWorkPosVec;
    fLSWorkLength    =    original.fLSWorkLength;    

    fS0APosteriori    =    original.fS0APosteriori;
    fUEOIndices    =    original.fUEOIndices;

    fLSFreeCnstr    =    original.fLSFreeCnstr;
    fUsedLibrOption    =    original.fUsedLibrOption;

    fLSWorkRelErrors    =    original.fLSWorkRelErrors;

    return    (*this);
}
    


UEOIndices    LSCalcDataSet::getDimensions()    const
{/*return    the    number    of    unknowns,    equations    and    observations
    of    the    project    as    a    UEOIndices    object    */
    return    fUEOIndices;
}


void    LSCalcDataSet::updateIndices()
{//    updates    the    different    indices
    setUIndex();
    setEOIndices();
}


bool    LSCalcDataSet::indicesConsistency()
{//    Controls    the    indices    consistency
    bool    consistent    =    true;
    //    for    standard    parametric    compensation
    if    (    fUEOIndices.EIndex    ==    fUEOIndices.OIndex    )    {
    if    (fUEOIndices.OIndex    <    fUEOIndices.UIndex)    
    consistent    =    false;
    }
    //    for    Gauss-Helmert    compensation    (parametric    with    conditions)
    else    {
    if    (    fUEOIndices.OIndex    >    (fUEOIndices.EIndex    -    fUEOIndices.UIndex)    )
    consistent    =    false;
    }

    return    consistent;
}


void    LSCalcDataSet::setUIndex()
{/*updates    the    parameters    unknown    index:
    ONLY    IF    FUEOINDICES    IS    INITIALIZED    BY    ZERO    */

    if    (fUEOIndices.UIndex    ==    0)    
    {
    //    updates    the    pos.    vector    parameters    unknown    indices
    LSPosVecIter    iterPV    =    fLSWorkPosVec->begin();
    while    (    iterPV!=    fLSWorkPosVec->end()    )    
    {
    fUEOIndices    =    iterPV->setUIndex(fUEOIndices);
    iterPV++;
    }

    //    updates    the    orientation    parameters    unknown    indices
    LSOrientIter    iterOr    =    fLSWorkOrient->begin();
    while    (    iterOr!=    fLSWorkOrient->end()    )    
    {
    fUEOIndices    =    iterOr->setUIndex(fUEOIndices);
    iterOr++;
    }

    //    updates    the    length    parameters    unknown    indices
    LSLengthIter    iterL    =    fLSWorkLength->begin();
    while    (    iterL!=    fLSWorkLength->end()    )    
    {
    fUEOIndices    =    iterL->setUIndex(fUEOIndices);
    iterL++;
    }
    }
}


void    LSCalcDataSet::setEOIndices()
{/*updates    the    observations    and    equations    index    :
    ONLY    IF    FUEOINDICES    IS    INITIALIZED    BY    ZERO*/

    if    (fUEOIndices.EIndex    ==    0    &&    fUEOIndices.OIndex    ==    0)    {

    //    updates    indices    for    hor.    angle    observations
    LSHorAngIter    iterHa    =    fLSWorkHorAngObs->begin();
    while    (    iterHa!=    fLSWorkHorAngObs->end()    )    {
    fUEOIndices    =    iterHa->setEOIndices(fUEOIndices);
    iterHa++;
    }
    
    //    updates    indices    for    zenithal    dist    observations
    LSZenDistIter    iterZd    =    fLSWorkZenDistObs->begin();
    while    (    iterZd!=    fLSWorkZenDistObs->end()    )    {
    fUEOIndices    =    iterZd->setEOIndices(fUEOIndices);
    iterZd++;
    }

    //    updates    indices    for    spatial    dist    observations
    LSSpaDistIter    iterSd    =    fLSWorkSpaDistObs->begin();
    while    (    iterSd!=    fLSWorkSpaDistObs->end()    )    {
    fUEOIndices    =    iterSd->setEOIndices(fUEOIndices);
    iterSd++;
    }

    //    updates    indices    for    hor.    dist    observations
    LSHorDistIter    iterHd    =    fLSWorkHorDistObs->begin();
    while    (    iterHd!=    fLSWorkHorDistObs->end()    )    {
    fUEOIndices    =    iterHd->setEOIndices(fUEOIndices);
    iterHd++;
    }
	
    //    updates    indices    for    vertical    dist    observations
    LSVertDistIter    iterVd    =    fLSWorkVertDistObs->begin();
    while    (    iterVd!=    fLSWorkVertDistObs->end()    )    {
    fUEOIndices    =    iterVd->setEOIndices(fUEOIndices);
    iterVd++;
    }
    //    updates    indices    for    vertical    dist    observations
    iterVd    =    fLSWorkDLEVDistObs->begin();
    while    (    iterVd!=    fLSWorkDLEVDistObs->end()    )    {
    fUEOIndices    =    iterVd->setEOIndices(fUEOIndices);
    iterVd++;
    }

    //    updates    indices    for    offset    to    vertical    line    observations
    LSOffsetToVerLineIter    ecveIt    =    fLSWorkOffsetToVerLineObs->begin();
    LSOffsetToVerLineIter    ecveItEnd    =    fLSWorkOffsetToVerLineObs->end();
    while    (    ecveIt!=    ecveItEnd    )
    {
    fUEOIndices    =    ecveIt->setEOIndices(fUEOIndices);
    ecveIt++;
    }

    //    updates    indices    for    offset    to    spatial    line    observations
    LSOffsetToSpaLineIter    ecspIt    =    fLSWorkOffsetToSpaLineObs->begin();
    LSOffsetToSpaLineIter    ecspItEnd    =    fLSWorkOffsetToSpaLineObs->end();
    while    (    ecspIt!=    ecspItEnd    )
    {
    fUEOIndices    =    ecspIt->setEOIndices(fUEOIndices);
    ecspIt++;
    }

    //    updates    indices    for    offset    to    vertical    plane    observations
    LSOffsetToVerPlaneIter    echoIt    =    fLSWorkOffsetToVerPlaneObs->begin();
    LSOffsetToVerPlaneIter    echoItEnd    =    fLSWorkOffsetToVerPlaneObs->end();
    while    (    echoIt!=    echoItEnd    )
    {
    fUEOIndices    =    echoIt->setEOIndices(fUEOIndices);
    echoIt++;
    }

    //    updates    indices    for    offset    to    theodolite    plane    observations
    LSOffsetToTheoPlaneIter    ecthIt    =    fLSWorkOffsetToTheoPlaneObs->begin();
    LSOffsetToTheoPlaneIter    ecthItEnd    =    fLSWorkOffsetToTheoPlaneObs->end();
    while    (    ecthIt!=    ecthItEnd    )
    {
    fUEOIndices    =    ecthIt->setEOIndices(fUEOIndices);
    ecthIt++;
    }

    //    updates    indices    for    gyro.    obsevation    observations
    LSGyroOrieIter    orieIt    =    fLSWorkGyroOrieObs->begin();
    LSGyroOrieIter    orieItEnd    =    fLSWorkGyroOrieObs->end();
    while    (    orieIt!=    orieItEnd    )
    {
    fUEOIndices    =    orieIt->setEOIndices(fUEOIndices);
    orieIt++;
    }

    //    updates    indices    for    radial    offset    constraints
    LSRadOffCnstrIter    cnstrIt    =    fLSWorkRadOffCnstrObs->begin();
    LSRadOffCnstrIter    cnstrItEnd    =    fLSWorkRadOffCnstrObs->end();
    while    (    cnstrIt!=    cnstrItEnd    )
    {
    fUEOIndices    =    cnstrIt->setEOIndices(fUEOIndices);
    cnstrIt++;
    }

    //    updates    indices    for    orientation    constraints
    LSOrieCnstrIter    cnstrOIt    =    fLSWorkOrieCnstrObs->begin();
    LSOrieCnstrIter    cnstrOItEnd    =    fLSWorkOrieCnstrObs->end();
    while    (    cnstrOIt!=    cnstrOItEnd    )
    {
    fUEOIndices    =    cnstrOIt->setEOIndices(fUEOIndices);
    cnstrOIt++;
    }

    }
}

void    LSCalcDataSet::setFreeConstraints(const    TLSConstraintIdentifier&    cnstr)
{
    fLSFreeCnstr    =    cnstr;
    fUsedLibrOption    =    true;
    return;
}


TLSConstraintIdentifier    LSCalcDataSet::getFreeConstraints()    const
{
    return    fLSFreeCnstr;
}


bool    LSCalcDataSet::isLibrOptionUsed()    const
{
    return    fUsedLibrOption;
}


/////////////////////////////////////////////////
//    METHODS    FOR    HOR.    ANGLE    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list    
LSHorAngIter    LSCalcDataSet::beginLSHorAng()    {

    return    fLSWorkHorAngObs->begin();
}


//    Returns    a    const    iterator    to    the    first    item    in    the    list    
LSHorAngConstIter    LSCalcDataSet::beginLSHorAng()    const    {

    return    fLSWorkHorAngObs->begin();
}

//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSHorAngIter    LSCalcDataSet::endLSHorAng()    {

    return    fLSWorkHorAngObs->end();
}
    
//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSHorAngConstIter    LSCalcDataSet::endLSHorAng()    const    {

    return    fLSWorkHorAngObs->end();
}


//    Adds    a    new    ls    hor.    ang.    observation    to    the    list
LSHorAngIter    LSCalcDataSet::push_backLSHorAng(const TLSCalcHorAngleObservation&    ha)    
{
    return    fLSWorkHorAngObs->push_back(ha);
}


//    Get    a    copy    of    the    Horizontal    Angle    summary    statistics
TLGCObsSummary    LSCalcDataSet::getHorAngSummary()    const
{
    return    fLSWorkHorAngObs->getObsSummary();
}


/////////////////////////////////////////////////
//    METHODS    FOR    ZENITHAL    DISTANCE    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list    
LSZenDistIter    LSCalcDataSet::beginLSZenDist()    {

    return    fLSWorkZenDistObs->begin();
}
    
//    Returns    a    const    iterator    to    the    first    item    in    the    list    
LSZenDistConstIter    LSCalcDataSet::beginLSZenDist()    const    {

    return    fLSWorkZenDistObs->begin();
}
    
//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSZenDistIter    LSCalcDataSet::endLSZenDist()    {

    return    fLSWorkZenDistObs->end();
}
    
//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSZenDistConstIter    LSCalcDataSet::endLSZenDist()    const    {

    return    fLSWorkZenDistObs->end();
}

//    Adds    a    new    ls    zen.    dist.    observation    to    the    list
LSZenDistIter    LSCalcDataSet::push_backLSZenDist(const TLSCalcZenithDistObservation&    zd)    {

    return    fLSWorkZenDistObs->push_back(zd);
}

//    Get    a    copy    of    the    zenith    distance    (ZENH)    summary    statistics
TLGCObsSummary    LSCalcDataSet::getZENHSummary()    const
{
    return    fLSWorkZenDistObs->getZENHObsSummary();
}


//    Get    a    copy    of    the    zenith    distance    (ZENI)    summary    statistics
TLGCObsSummary    LSCalcDataSet::getZENISummary()    const
{
    return    fLSWorkZenDistObs->getZENIObsSummary();
}


/////////////////////////////////////////////////
//    METHODS    FOR    SPATIAL    DISTANCE    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list    
LSSpaDistIter    LSCalcDataSet::beginLSSpaDist()    {

    return    fLSWorkSpaDistObs->begin();
}
    
//    Returns    a    const    iterator    to    the    first    item    in    the    list    
LSSpaDistConstIter    LSCalcDataSet::beginLSSpaDist()    const    {

    return    fLSWorkSpaDistObs->begin();
}

//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSSpaDistIter    LSCalcDataSet::endLSSpaDist()    {

    return    fLSWorkSpaDistObs->end();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSSpaDistConstIter    LSCalcDataSet::endLSSpaDist()    const    {

    return    fLSWorkSpaDistObs->end();
}

//    Adds    a    new    ls    spa.    dist.    observation    to    the    list
LSSpaDistIter    LSCalcDataSet::push_backLSSpaDist(const TLSCalcSpatialDistObservation&    sd)    {

    return    fLSWorkSpaDistObs->push_back(sd);
}

//    Get    a    copy    of    the    spatial    distance    (DMES)    summary    statistics
TLGCObsSummary    LSCalcDataSet::getDMESSummary()    const
{
    return    fLSWorkSpaDistObs->getDMESObsSummary();
}

//    Get    a    copy    of    the    spatial    distance    (DTHE)    summary    statistics
TLGCObsSummary    LSCalcDataSet::getDTHESummary()    const
{
    return    fLSWorkSpaDistObs->getDTHEObsSummary();
}


/////////////////////////////////////////////////
//    METHODS    FOR    HORIZONTAL    DISTANCE    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list    
LSHorDistIter    LSCalcDataSet::beginLSHorDist()    {

    return    fLSWorkHorDistObs->begin();
}

//    Returns    a    const    iterator    to    the    first    item    in    the    list    
LSHorDistConstIter    LSCalcDataSet::beginLSHorDist()    const    {

    return    fLSWorkHorDistObs->begin();
}
    
//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSHorDistIter    LSCalcDataSet::endLSHorDist()    {

    return    fLSWorkHorDistObs->end();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSHorDistConstIter    LSCalcDataSet::endLSHorDist()    const    {

    return    fLSWorkHorDistObs->end();
}

//    Adds    a    new    ls    hor.    dist.    observation    to    the    list
LSHorDistIter    LSCalcDataSet::push_backLSHorDist(const TLSCalcHorDistObservation&    hd)    {

    return    fLSWorkHorDistObs->push_back(hd);
}

//    Get    a    copy    of    the    horizontal    distance    summary    statistics
TLGCObsSummary    LSCalcDataSet::getHorDistSummary()    const
{
    return    fLSWorkHorDistObs->getObsSummary();
}

/////////////////////////////////////////////////
//    METHODS    FOR    VERTICAL    DISTANCE    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list    
LSVertDistIter    LSCalcDataSet::beginLSVertDist(bool isDLEV)    {

    return isDLEV ? fLSWorkDLEVDistObs->begin() : fLSWorkVertDistObs->begin();
}

//    Returns    a    const    iterator    to    the    first    item    in    the    list    
LSVertDistConstIter    LSCalcDataSet::beginLSVertDist(bool isDLEV)    const    {

    return isDLEV ? fLSWorkDLEVDistObs->begin() : fLSWorkVertDistObs->begin();
}

//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSVertDistIter    LSCalcDataSet::endLSVertDist(bool isDLEV)    {

    return isDLEV ? fLSWorkDLEVDistObs->end() : fLSWorkVertDistObs->end();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSVertDistConstIter    LSCalcDataSet::endLSVertDist(bool isDLEV)    const    {

    return isDLEV ? fLSWorkDLEVDistObs->end() : fLSWorkVertDistObs->end();
}

//    Adds    a    new    ls    vert.    dist.    observation    to    the    list
LSVertDistIter    LSCalcDataSet::push_backLSVertDist(const TLSCalcVertDistObservation&    vd, bool isDLEV)    {

    return    isDLEV ? fLSWorkDLEVDistObs->push_back(vd) : fLSWorkVertDistObs->push_back(vd);
}

//    Get    a    copy    of    the    horizontal    distance    summary    statistics
TLGCObsSummary    LSCalcDataSet::getVertDistSummary(bool isDLEV)    const
{
    return    isDLEV ? fLSWorkDLEVDistObs->getObsSummary() : fLSWorkVertDistObs->getObsSummary();
}


/////////////////////////////////////////////////
//    METHODS    FOR    OFFSET    TO    VERTICAL    LINE    CONTAINERS
/////////////////////////////////////////////////
LSOffsetToVerLineIter    LSCalcDataSet::beginLSOffsetToVerLine()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToVerLineObs->begin();
}


LSOffsetToVerLineConstIter    LSCalcDataSet::beginLSOffsetToVerLine()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToVerLineObs->begin();
}


LSOffsetToVerLineIter    LSCalcDataSet::endLSOffsetToVerLine()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToVerLineObs->end();
}


LSOffsetToVerLineConstIter    LSCalcDataSet::endLSOffsetToVerLine()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToVerLineObs->end();
}


LSOffsetToVerLineIter    LSCalcDataSet::push_backLSOffsetToVerLine(const TLSCalcOffsetToVerLineObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkOffsetToVerLineObs->push_back(obs);
}

//    Get    a    copy    of    the    Offset    to    a    Vertical    Line    summary    statistics
TLGCObsSummary    LSCalcDataSet::getOffsetToVerLineSummary()    const
{
    return    fLSWorkOffsetToVerLineObs->getObsSummary();
}


/////////////////////////////////////////////////
//    METHODS    FOR    OFFSET    TO    SPATIAL    LINE    CONTAINERS
/////////////////////////////////////////////////
LSOffsetToSpaLineIter    LSCalcDataSet::beginLSOffsetToSpaLine()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToSpaLineObs->begin();
}


LSOffsetToSpaLineConstIter    LSCalcDataSet::beginLSOffsetToSpaLine()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToSpaLineObs->begin();
}


LSOffsetToSpaLineIter    LSCalcDataSet::endLSOffsetToSpaLine()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToSpaLineObs->end();
}


LSOffsetToSpaLineConstIter    LSCalcDataSet::endLSOffsetToSpaLine()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToSpaLineObs->end();
}


LSOffsetToSpaLineIter    LSCalcDataSet::push_backLSOffsetToSpaLine(const TLSCalcOffsetToSpaLineObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkOffsetToSpaLineObs->push_back(obs);
}

//    Get    a    copy    of    the    Offset    to    a    Spatial    Line    summary    statistics
TLGCObsSummary    LSCalcDataSet::getOffsetToSpaLineSummary()    const
{
    return    fLSWorkOffsetToSpaLineObs->getObsSummary();
}


//////////////////////////////////////////////////
//    METHODS    FOR    OFFSET    TO    VERTICAL    PLANE    CONTAINERS
//////////////////////////////////////////////////
LSOffsetToVerPlaneIter    LSCalcDataSet::beginLSOffsetToVerPlane()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToVerPlaneObs->begin();
}


LSOffsetToVerPlaneConstIter    LSCalcDataSet::beginLSOffsetToVerPlane()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToVerPlaneObs->begin();
}


LSOffsetToVerPlaneIter    LSCalcDataSet::endLSOffsetToVerPlane()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToVerPlaneObs->end();
}


LSOffsetToVerPlaneConstIter    LSCalcDataSet::endLSOffsetToVerPlane()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToVerPlaneObs->end();
}


LSOffsetToVerPlaneIter    LSCalcDataSet::push_backLSOffsetToVerPlane(const TLSCalcOffsetToVerPlaneObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkOffsetToVerPlaneObs->push_back(obs);
}

//    Get    a    copy    of    the    Offset    to    a    Vertical    Plane    summary    statistics
TLGCObsSummary    LSCalcDataSet::getOffsetToVerPlaneSummary()    const
{
    return    fLSWorkOffsetToVerPlaneObs->getObsSummary();
}


/////////////////////////////////////////////////////
//    METHODS    FOR    OFFSET    TO    THEODOLITE    PLANE    CONTAINERS
/////////////////////////////////////////////////////
LSOffsetToTheoPlaneIter    LSCalcDataSet::beginLSOffsetToTheoPlane()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToTheoPlaneObs->begin();
}


LSOffsetToTheoPlaneConstIter    LSCalcDataSet::beginLSOffsetToTheoPlane()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOffsetToTheoPlaneObs->begin();
}


LSOffsetToTheoPlaneIter    LSCalcDataSet::endLSOffsetToTheoPlane()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToTheoPlaneObs->end();
}


LSOffsetToTheoPlaneConstIter    LSCalcDataSet::endLSOffsetToTheoPlane()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOffsetToTheoPlaneObs->end();
}


LSOffsetToTheoPlaneIter    LSCalcDataSet::push_backLSOffsetToTheoPlane(const TLSCalcOffsetToTheoPlaneObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkOffsetToTheoPlaneObs->push_back(obs);
}

//    Get    a    copy    of    the    Offset    to    a    Theodolite    Vertical    Plane    summary    statistics
TLGCObsSummary    LSCalcDataSet::getOffsetToTheoPlaneSummary()    const
{
    return    fLSWorkOffsetToTheoPlaneObs->getObsSummary();
}


/////////////////////////////////////////////////////
//    METHODS    FOR    GYRO.    ORIENTATION    CONTAINERS
/////////////////////////////////////////////////////
LSGyroOrieIter    LSCalcDataSet::beginLSGyroOrie()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkGyroOrieObs->begin();
}


LSGyroOrieConstIter    LSCalcDataSet::beginLSGyroOrie()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkGyroOrieObs->begin();
}


LSGyroOrieIter    LSCalcDataSet::endLSGyroOrie()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkGyroOrieObs->end();
}


LSGyroOrieConstIter    LSCalcDataSet::endLSGyroOrie()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkGyroOrieObs->end();
}


LSGyroOrieIter    LSCalcDataSet::push_backLSGyroOrie(const TLSCalcGyroOrientationObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkGyroOrieObs->push_back(obs);
}

//    Get    a    copy    of    the    Gyro    Orientation    summary    statistics
TLGCObsSummary    LSCalcDataSet::getGyroOrieSummary()    const
{
    return    fLSWorkGyroOrieObs->getObsSummary();
}


/////////////////////////////////////////////////////
//    METHODS    FOR    RADIAL    OFFSET    CONSTRAINT    CONTAINERS
/////////////////////////////////////////////////////
LSRadOffCnstrIter    LSCalcDataSet::beginLSRadOffCnstr()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkRadOffCnstrObs->begin();
}


LSRadOffCnstrConstIter    LSCalcDataSet::beginLSRadOffCnstr()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkRadOffCnstrObs->begin();
}


LSRadOffCnstrIter    LSCalcDataSet::endLSRadOffCnstr()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkRadOffCnstrObs->end();
}


LSRadOffCnstrConstIter    LSCalcDataSet::endLSRadOffCnstr()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkRadOffCnstrObs->end();
}


LSRadOffCnstrIter    LSCalcDataSet::push_backLSRadOffCnstr(const TLSCalcRadialOffsetCnstrObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkRadOffCnstrObs->push_back(obs);
}

//    Get    a    copy    of    the    Offset    to    a    Vertical    Line    summary    statistics
TLGCObsSummary    LSCalcDataSet::getRadOffCnstrSummary()    const
{
    return    fLSWorkRadOffCnstrObs->getObsSummary();
}


/////////////////////////////////////////////////////
//    METHODS    FOR    ORIENTATION    CONSTRAINT    CONTAINERS
/////////////////////////////////////////////////////
LSOrieCnstrIter    LSCalcDataSet::beginLSOrieCnstr()
{//    Returns    an    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOrieCnstrObs->begin();
}


LSOrieCnstrConstIter    LSCalcDataSet::beginLSOrieCnstr()    const
{//    Returns    a    const    iterator    to    the    first    item    in    the    list    
    return    fLSWorkOrieCnstrObs->begin();
}


LSOrieCnstrIter    LSCalcDataSet::endLSOrieCnstr()
{//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOrieCnstrObs->end();
}


LSOrieCnstrConstIter    LSCalcDataSet::endLSOrieCnstr()    const
{//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
    return    fLSWorkOrieCnstrObs->end();
}


LSOrieCnstrIter    LSCalcDataSet::push_backLSOrieCnstr(const TLSCalcOrientationCnstrObservation&    obs)
{//    Adds    a    new    ls    offset    observation    to    the    list
    return    fLSWorkOrieCnstrObs->push_back(obs);
}


/////////////////////////////////////////////////
//    METHODS    FOR    ORIENTATION    PARAMETERS    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list
LSOrientIter    LSCalcDataSet::beginOrient()    {

    return    fLSWorkOrient->begin();
}

//    Returns    a    const    iterator    to    the    first    item    in    the    list
LSOrientConstIter    LSCalcDataSet::beginOrient()    const    {

    return    fLSWorkOrient->begin();
}

//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSOrientIter    LSCalcDataSet::endOrient()    {

    return    fLSWorkOrient->end();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSOrientConstIter    LSCalcDataSet::endOrient()    const    {

    return    fLSWorkOrient->end();
}

//    Adds    a    new    ls    calc    orientation    param    to    the    list,    and    returns    a    iterator
LSOrientIter    LSCalcDataSet::push_backOrient(const TLSCalcOrientationParam&    orient)    {

    return    fLSWorkOrient->push_back(orient);
}

/////////////////////////////////////////////////
//    METHODS    FOR    LENGTH    PARAMETERS    CONTAINERS
////////////////////////////////////////////////
//    Returns    an    iterator    to    the    first    item    in    the    list
LSLengthIter    LSCalcDataSet::beginLength()    {

    return    fLSWorkLength->begin();
}

//    Returns    a    const    iterator    to    the    first    item    in    the    list
LSLengthConstIter    LSCalcDataSet::beginLength()    const    {

    return    fLSWorkLength->begin();
}

//    Returns    an    iterator    to    the    position    after    the    last    item    of    the    list    
LSLengthIter    LSCalcDataSet::endLength()    {

    return    fLSWorkLength->end();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    item    of    the    list    
LSLengthConstIter    LSCalcDataSet::endLength()    const    {

    return    fLSWorkLength->end();
}
//    Adds    a    new    ls    calc    length    param    to    the    list,    and    returns    a    iterator
LSLengthIter    LSCalcDataSet::push_backLength(const TLSCalcLengthParam&    length)    {

    return    fLSWorkLength->push_back(length);
}

/////////////////////////////////////////////////
//    METHODS    FOR    POSITION    VECTOR    PARAMETERS    CONTAINERS
////////////////////////////////////////////////
string    LSCalcDataSet::checkAllPointsAreUsed(const    TWorkingPoints*    ptList)
{/*!\class the list like the input point'list*/

    string error = "";

    error += this->orderPVList(ptList);

    if(ptList->numberOfPoints() != this->numPosVectorParam())
    {//all points are not used

        //list iterator
        PointConstIter inputPtIt, endInputPtIt;
        LSPosVecIter outputPtIt, endOutputPtIt;
        outputPtIt = this->beginPV();
        endOutputPtIt = this->endPV();
        inputPtIt = ptList->getPointsBeginIterator();
        endInputPtIt = ptList->getPointsEndIterator();

        while(inputPtIt != endInputPtIt)
        {
            string nameInput, nameOutput;
            nameInput = inputPtIt->getName().getName();
			if (outputPtIt != endOutputPtIt)
			{
				nameOutput = outputPtIt->getName();
			}
			else
			{
				nameOutput = "";
			}


            if(nameInput != nameOutput)
            {
                if( inputPtIt->getPosition().getObjectStatus() == TSpatialStatus::kCala )
                {
                    TPositionVector posVec(inputPtIt->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian));
                    if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
                    {
                        TSpatialPosition* spos = new TSpatialPosition(inputPtIt->getPosition());
                        spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
                        posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
                        delete spos;
                    }

                    struct LSParaStatus pvStatus;
                    pvStatus.first = TALSCalcParameter::kFixed;
                    pvStatus.second = TALSCalcParameter::kFixed;
                    pvStatus.third = TALSCalcParameter::kFixed;

                    //fCalaPtNotUsed.push_back(TLSCalcPosVectorParam(posVec,pvStatus,nameInput));

                    fLSWorkPosVec->insert(outputPtIt, TLSCalcPosVectorParam(posVec,pvStatus,nameInput));
                    // outputPtIt++;
                }
                else
                {
                    error = error + "Aucune observation concernant le point " + nameInput + '\n';
                }
            }
            else
            {
                outputPtIt++;
            }

            inputPtIt++;
        }
    }

    return error;
}




    
string    LSCalcDataSet::orderPVList(const    TWorkingPoints*    ptList)
{/*!\class the list like the input point'list*/

	string error = "";

    //list iterator
    PointConstIter pointIterator, endPointIterator;
    LSPosVecIter posVecIterator, endPosVecIterator;
    endPosVecIterator = this->endPV();
    pointIterator = ptList->getPointsBeginIterator();
    endPointIterator = ptList->getPointsEndIterator();

    while(pointIterator != endPointIterator)
    {//for each point of the input's list

        string nameInput, name;
        nameInput = pointIterator->getName().getName();

        //write point in the same order that in the input file
		posVecIterator = fLSWorkPosVec->getPoint(nameInput);
		if (posVecIterator != endPosVecIterator)
		{
            fLSWorkPosVec->splice(this->beginPV(),posVecIterator);/*splice pos2 front of the NEW begin*/
		}
        else if (pointIterator->getPosition().getObjectStatus() != TSpatialStatus::kCala)
		{
			error += "Les coordonnées du point, " + nameInput + ", ne peuvent pas être déterminées." +'\n';
        }

        pointIterator++;
    }

    fLSWorkPosVec->reverse();

    return error;
}


/////////////////////////////////////////////////
//    METHODS    FOR    RELATIVE    ERROR    POINT    PAIRS
////////////////////////////////////////////////

//    Returns    an    iterator    to    the    first    item    in    the    container    
LSRelErrorIter    LSCalcDataSet::beginRelError()
{
    return    fLSWorkRelErrors->begin();
}

//    Returns    a    const    iterator    to    the    first    item    in    the    container    
LSRelErrorConstIter    LSCalcDataSet::beginRelError()    const
{
    return    fLSWorkRelErrors->begin();
}

//    Returns    an    iterator    to    the    position    after    the    last    item    in    the    container    
LSRelErrorIter    LSCalcDataSet::endRelError()
{
    return    fLSWorkRelErrors->end();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    item    in    the    container    
LSRelErrorConstIter    LSCalcDataSet::endRelError()    const
{
    return    fLSWorkRelErrors->end();
}

//    Adds    a    new    item    to    the    container    
LSRelErrorIter    LSCalcDataSet::push_backRelError(const TLSCalcRelativeError&    erel)
{
    return    fLSWorkRelErrors->push_back(erel);
}

//    get    any    warning    related    to    the    relative    errors    
string    LSCalcDataSet::getRelErrorWarning()    const
{
    return    fLSWorkRelErrors->getWarning();
}

//    set    a    warning    related    to    the    relative    errors    
void    LSCalcDataSet::setRelErrorWarning(const    string&    warning)
{
    return    fLSWorkRelErrors->setWarning(warning);
}


/////////////////////////////////////////////////
//    METHODS    FOR    CALCULATION    STATISTICS
////////////////////////////////////////////////
TDouble    LSCalcDataSet::computeOverallNetworkReliability()    {

    TDouble    F(LITERAL(0.0));
    TReal    FValue(LITERAL(0.0));

    LSHorAngIter    iterHa    =    beginLSHorAng();
    while    (iterHa!=endLSHorAng())    {
    if    (iterHa->paramsCanBeDetermined())    {
    FValue    +=    powq(iterHa->getT(),2)    -    1;
    iterHa++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterHa    =    endLSHorAng();
    }
    }

    LSZenDistIter    iterZd    =    beginLSZenDist();
    while    ((iterZd!=endLSZenDist())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterZd->paramsCanBeDetermined())    {
    FValue    +=    powq(iterZd->getT(),2)    -    1;
    iterZd++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterZd    =    endLSZenDist();
    }
    }

    LSSpaDistIter    iterSd    =    beginLSSpaDist();
    while    ((iterSd!=endLSSpaDist())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterSd->paramsCanBeDetermined())    {
    FValue    +=    powq(iterSd->getT(),2)    -    1;
    iterSd++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterSd    =    endLSSpaDist();
    }
    }

    LSHorDistIter    iterHd    =    beginLSHorDist();
    while    ((iterHd!=endLSHorDist())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterHd->paramsCanBeDetermined())    {
    FValue    +=    powq(iterHd->getT(),2)    -    1;
    iterHd++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterHd    =    endLSHorDist();
    }
    }
    
	// Cover DVER and DLEV observations
	for (int i = 0; i< 2; i++) {
		LSVertDistIter    iterVd    =    beginLSVertDist(i==1);
		while    ((iterVd!=endLSVertDist(i==1))    &&    (F.getStatus()!=TVNumericValue::kNull))    {
		if    (iterVd->paramsCanBeDetermined())    {
		FValue    +=    powq(iterVd->getT(),2)    -    1;
		iterVd++;
		}
		else    {
		F.setStatus(TVNumericValue::kNull);
		iterVd    =    endLSVertDist(i==1);
		}
		}
	}

    LSOffsetToVerLineIter    iterECVE    =    beginLSOffsetToVerLine();
    while    ((iterECVE!=endLSOffsetToVerLine())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterECVE->paramsCanBeDetermined())    {
    FValue    +=    powq(iterECVE->getT(),2)    -    1;
    iterECVE++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterECVE    =    endLSOffsetToVerLine();
    }
    }

    LSOffsetToSpaLineIter    iterECSP    =    beginLSOffsetToSpaLine();
    while    ((iterECSP!=endLSOffsetToSpaLine())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterECSP->paramsCanBeDetermined())    {
    FValue    +=    powq(iterECSP->getT(),2)    -    1;
    iterECSP++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterECSP    =    endLSOffsetToSpaLine();
    }
    }

    LSOffsetToVerPlaneIter    iterECHO    =    beginLSOffsetToVerPlane();
    while    ((iterECHO!=endLSOffsetToVerPlane())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterECHO->paramsCanBeDetermined())    {
    FValue    +=    powq(iterECHO->getT(),2)    -    1;
    iterECHO++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterECHO    =    endLSOffsetToVerPlane();
    }
    }

    LSOffsetToTheoPlaneIter    iterECTH    =    beginLSOffsetToTheoPlane();
    while    ((iterECTH!=endLSOffsetToTheoPlane())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterECTH->paramsCanBeDetermined())    {
    FValue    +=    powq(iterECTH->getT(),2)    -    1;
    iterECTH++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterECTH    =    endLSOffsetToTheoPlane();
    }
    }

    LSGyroOrieIter    iterOrie    =    beginLSGyroOrie();
    while    ((iterOrie!=endLSGyroOrie())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterOrie->paramsCanBeDetermined())    {
    FValue    +=    powq(iterOrie->getT(),2)    -    1;
    iterOrie++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterOrie    =    endLSGyroOrie();
    }
    }

    LSRadOffCnstrIter    iterRADI    =    beginLSRadOffCnstr();
    while    ((iterRADI!=endLSRadOffCnstr())    &&    (F.getStatus()!=TVNumericValue::kNull))    {
    if    (iterRADI->paramsCanBeDetermined())    {
    FValue    +=    powq(iterRADI->getT(),2)    -    1;
    iterRADI++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterRADI    =    endLSRadOffCnstr();
    }
    }
    
    if    (F.getStatus()!=TVNumericValue::kNull)    {
    
    int    nobs    =    totalSize();
    F.setValue(FValue/nobs);
    }

    return    F;
}


/////////////////////////////////////////////////
//    METHODS    FOR    CALCULATION    STATISTICS
////////////////////////////////////////////////
//    initialise    the    summaries    for    simulation    calculation    results    
bool    LSCalcDataSet::initSimResults()
{
    bool    success    =    false;

    if(fSimResults    !=    0)    delete    fSimResults;

    fSimResults    =    new    TLGCSimResults(fLSWorkPosVec);

    if(    fSimResults    !=    0)    success    =    true;

    return    success;
}

    
//    Reinitialse    the    values    set    during    and    after    a    calculation
void    LSCalcDataSet::reInitialiseForSim()
{
    //    calculation    results    flag;
    fConvergenceIsOk    =    false;

    //    reinitialise    sigma    zero    parameters
    fS0APosteriori    =    TDouble(LITERAL(0.0));
    fChiUpLimit    =    LITERAL(0.0);
    fChiLoLimit    =    LITERAL(0.0);

    //    Re-initialise    the    LSCalc    parameters
    reInitialiseParams();

    //    Re-initialise    the    LSCalc    observations
    reInitObsForSim();

    return;
}


/*    update    the    summaries    for    simulation    calculation    results    */
bool    LSCalcDataSet::updateSimResults()
{
    bool    success    =    false;
    CalcRecordIter    calcRecord;

    if(fSimResults    !=    0)    
    {
    //
    //    add    point    results    to    the    point    summaries
    //
    fSimResults->updateResValues(fLSWorkPosVec);

    //
    //    add    calculation    results    to    the    calculation    records
    //
    //    add    a    new    sim    calculation    record
    calcRecord    =    fSimResults->addSimCalc();

    //    set    the    calculation    summary    parameters
    calcRecord->setS0APosteriori(fS0APosteriori);
    calcRecord->setChiHiLimit(fChiUpLimit);
    calcRecord->setChiLoLimit(fChiLoLimit);

    //    block    to    add    the    unknown    point    calculation    results
    {
    LSPosVecIter    iter    =    beginPV();
    LSPosVecIter    iterE    =    endPV();

    //    iterate    through    all    the    position    vectors
    while    (    iter    !=    iterE    )
    {
    //    reset    position    vector    estimated    values    to    the    provisional    values
    calcRecord->push_backPoint(*iter);
    iter++;
    }
    }

    //    block    to    add    the    observation    summary    results
    {
    //    Check    to    see    if    there    are    any    observations    of    a    given    type    
    //    and    if    there    are    get    the    observations    summary    data    from    the
    //    TLSWorking*Obs    object    and    save    a    copy    in    the    calculation    record
    
    //    Horizontal    Angles
    if(numHorAngObs()!=0)    
    calcRecord->addObsSumm(fLSWorkHorAngObs->getObsSummary(),    TALGCConverter::kANGL);
    //    Zenith    Distances    -fixed    instrument    height
    if(numZENHObs()    !=    0)    
    calcRecord->addObsSumm(fLSWorkZenDistObs->getZENHObsSummary(),    TALGCConverter::kZENH);
    //    Zenith    Distances    -unknown    instrument    height
    if(numZENIObs()    !=    0)
    calcRecord->addObsSumm(fLSWorkZenDistObs->getZENIObsSummary(),    TALGCConverter::kZENI);
    //    Spatial    Distances
    if(numDMESObs()!=0)    
    calcRecord->addObsSumm(fLSWorkSpaDistObs->getDMESObsSummary(),    TALGCConverter::kDMES);
    //    DTHE    observations
    if(numDTHEObs()!=0)    
    calcRecord->addObsSumm(fLSWorkSpaDistObs->getDTHEObsSummary(),    TALGCConverter::kDTHE);
    //    Azimuth    measurements
    if(numGyroOrieObs()!=0)
    calcRecord->addObsSumm(fLSWorkGyroOrieObs->getObsSummary(),    TALGCConverter::kORIE);
    //    Vertical    Distances
    if(numVertDistObs(0)!=0)    
    calcRecord->addObsSumm(fLSWorkVertDistObs->getObsSummary(),    TALGCConverter::kDVER);
    if(numVertDistObs(1)!=0)    
    calcRecord->addObsSumm(fLSWorkVertDistObs->getObsSummary(),    TALGCConverter::kDLEV);
    //    Horizontal    distances
    if(numHorDistObs()!=0)    
    calcRecord->addObsSumm(fLSWorkHorDistObs->getObsSummary(),    TALGCConverter::kDHOR);
    //    ECHO    observations
    if(numOffsetToVerPlaneObs()!=0)    
    calcRecord->addObsSumm(fLSWorkOffsetToVerPlaneObs->getObsSummary(),    TALGCConverter::kECHO);
    //    ECSP    observations
    if(numOffsetToSpaLineObs()!=0)    
    calcRecord->addObsSumm(fLSWorkOffsetToSpaLineObs->getObsSummary(),    TALGCConverter::kECSP);
    //    ECTH    observations
    if(numOffsetToTheoPlaneObs()!=0)    
    calcRecord->addObsSumm(fLSWorkOffsetToTheoPlaneObs->getObsSummary(),    TALGCConverter::kECTH);
    //    ECVE    observations
    if(numOffsetToVerLineObs()!=0)    
    calcRecord->addObsSumm(fLSWorkOffsetToVerLineObs->getObsSummary(),    TALGCConverter::kECVE);
    //    radial    constraints
    if(numRadOffCnstrObs()!=0)    
    calcRecord->addObsSumm(fLSWorkRadOffCnstrObs->getObsSummary(),    TALGCConverter::kRADI);
    }

    success    =    true;
    }
    else
    {
    //    no    object    to    store    the    simulation    results
    success    =    false;
    }

    return    success;
}


//////////////////////////////////////////////////////////////////////////
//Protected    Access    methods
////////////////////////////////////////////////////////////////////////

//    Returns    the    total    number    of    observation,    PDOR    excluded
int    LSCalcDataSet::totalSize()    {

    int    n(0);

    n    =    numHorAngObs()    +    numZenDistObs()    +    numSpaDistObs    ()
    +    numHorDistObs()    +    numVertDistObs(0)    +    numVertDistObs(1)    +    numOffsetToVerLineObs()
    +    numOffsetToSpaLineObs()    +    numOffsetToVerPlaneObs()    +    numOffsetToTheoPlaneObs()
    +    numGyroOrieObs()    +    numRadOffCnstrObs();

    return    n;
}



//////////////////////////////////////////////////////////////////////////
//Private    Access    methods
////////////////////////////////////////////////////////////////////////

//    Re-initialise    the    LSCalc    parameters
void    LSCalcDataSet::reInitialiseParams()
{
    //////////////////////////////////////////////////////
    //    block    to    re-initialise    the    LSCalc    position    vector    parameters
    {
    LSPosVecIter    iterB    =    beginPV();
    LSPosVecIter    iterE    =    endPV();

    //    iterate    through    all    the    position    vectors
    while    (    iterB    !=    iterE    )
    {
    //    reset    position    vector    estimated    values    to    the    provisional    values
    iterB->reInitialise();
    iterB++;
    }
    }
    //    end    of    block

    //////////////////////////////////////////////////
    //    block    to    re-initialise    the    LSCalc    orientation    parameters
    {
    LSOrientIter    iterB    =    beginOrient();
    LSOrientIter    iterE    =    endOrient();

    //    iterate    through    all    the    orientations
    while    (    iterB    !=    iterE    )
    {
    //    reset    position    vector    estimated    values    to    the    provisional    values
    iterB->reInitialise();
    iterB++;
    }
    }
    //    end    of    block

    /////////////////////////////////////////////
    //    block    to    re-initialise    the    LSCalc    length    parameters
    {
    LSLengthIter    iterB    =    beginLength();
    LSLengthIter    iterE    =    endLength();

    //    iterate    through    all    the    position    vectors
    while    (    iterB    !=    iterE    )
    {
    //    reset    position    vector    estimated    values    to    the    provisional    values
    iterB->reInitialise();
    iterB++;
    }
    }
    //    end    of    block

    return;
}


//    Re-initialise    the    LSCalc    observations
void    LSCalcDataSet::reInitObsForSim()
{
    //    set    the    observations'    summary    statistics    to    be    re-determined
    fLSWorkHorAngObs->statisticsBad();
    fLSWorkZenDistObs->statisticsBad();
    fLSWorkSpaDistObs->statisticsBad();
    fLSWorkHorDistObs->statisticsBad();
    fLSWorkVertDistObs->statisticsBad();

    fLSWorkOffsetToVerLineObs->statisticsBad();
    fLSWorkOffsetToSpaLineObs->statisticsBad();
    fLSWorkOffsetToVerPlaneObs->statisticsBad();
    fLSWorkOffsetToTheoPlaneObs->statisticsBad();

    fLSWorkGyroOrieObs->statisticsBad();

    fLSWorkRadOffCnstrObs->statisticsBad();

    return;
}

//    Returns    an    iterator    to    the    first    Simulation    Calculation    Record    in    the    container    
CalcRecordIter    LSCalcDataSet::beginSimCalcRec()
{
    return    fSimResults->beginSimCalc();
}

//    Returns    a    const    iterator    to    the    first    Simulation    Calculation    Record    in    the    container    
CalcRecordConstIter    LSCalcDataSet::beginSimCalcRec()    const
{
    return    fSimResults->beginSimCalc();
}

//    Returns    an    iterator    to    the    position    after    the    last    Simulation    Calculation    Record    
//in    the    container    
CalcRecordIter    LSCalcDataSet::endSimCalcRec()
{
    return    fSimResults->endSimCalc();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    Simulation    Calculation    Record    
//in    the    container    
CalcRecordConstIter    LSCalcDataSet::endSimCalcRec()    const
{
    return    fSimResults->endSimCalc();
}

//    Returns    an    iterator    to    the    first    Simulation    Point    Statistic    in    the    container    
SimPointIterator    LSCalcDataSet::beginSimResPoint()
{
    return    fSimResults->beginPoint();
}

//    Returns    a    const    iterator    to    the    first    Simulation    Point    Statistic    in    the    container    
SimPointConstIter    LSCalcDataSet::beginSimResPoint()    const
{
    return    fSimResults->beginPoint();
}

//    Returns    an    iterator    to    the    position    after    the    last    Simulation    Point    Statistic    
//in    the    container    
SimPointIterator    LSCalcDataSet::endSimResPoint()
{
    return    fSimResults->endPoint();
}

//    Returns    a    const    iterator    to    the    position    after    the    last    Simulation    Point    Statistic    
//    in    the    container    
SimPointConstIter    LSCalcDataSet::endSimResPoint()    const
{
    return    fSimResults->endPoint();
}



