//TLSConstraintIdentifier.h : implementation file


#include "TSpatialPosition.h"
#include "TLGCDataSet.h"
#include "TLSConstraintIdentifier.h"


///////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////
TLSConstraintIdentifier::TLSConstraintIdentifier()
{//Default constructor
	fCnstrVector.dx = 1;
	fCnstrVector.dy = 1;
	fCnstrVector.dz = 1;
	fCnstrVector.rx = 1;
	fCnstrVector.ry = 1;
	fCnstrVector.rz = 1;
	fCnstrVector.k  = 1;

	TLength zero (0);
	fXcg = zero;
	fYcg = zero;
	fZcg = zero;
	fXcgEst = zero;
	fYcgEst = zero;
	fZcgEst = zero;

	fCnstrNumber.dx = 0;
	fCnstrNumber.dy = 0;
	fCnstrNumber.dz = 0;
	fCnstrNumber.rx = 0;
	fCnstrNumber.ry = 0;
	fCnstrNumber.rz = 0;
	fCnstrNumber.k  = 0;
}


TLSConstraintIdentifier::~TLSConstraintIdentifier()
{// Destructor
}



///////////////////////////////////////////////////////////////////////////////////////
//PUBLIC MEMBER FUNCTION
///////////////////////////////////////////////////////////////////////////////////////
void	TLSConstraintIdentifier::initCnstrIdentifier(TLSCalcWorkingPosVec* ptList, const ObservationSet& obs)
{
	//Initialise elements
	bool T, F;
	T = true;
	F = false;

	bool bx = false;
	struct freeCnstr vx;
		vx.dx = T;
		vx.dy = F;
		vx.dz = F;
		vx.rx = F;
		vx.ry = F;
		vx.rz = F;
		vx.k = F;

	bool by = false;
	struct freeCnstr vy;
		vy.dx = F;
		vy.dy = T;
		vy.dz = F;
		vy.rx = F;
		vy.ry = F;
		vy.rz = F;
		vy.k = F;

	bool bz = false;
	struct freeCnstr vz;
		vz.dx = F;
		vz.dy = F;
		vz.dz = T;
		vz.rx = F;
		vz.ry = F;
		vz.rz = F;
		vz.k = F;

	bool bxy = false;
	struct freeCnstr vxy;
		vxy.dx = T;
		vxy.dy = T;
		vxy.dz = F;
		vxy.rx = F;
		vxy.ry = F;
		vxy.rz = T;
		vxy.k = F;

	bool bxz = false;
	struct freeCnstr vxz;
		vxz.dx = T;
		vxz.dy = F;
		vxz.dz = T;
		vxz.rx = F;
		vxz.ry = T;
		vxz.rz = F;
		vxz.k = F;

	bool byz = false;
	struct freeCnstr vyz;
		vyz.dx = F;
		vyz.dy = T;
		vyz.dz = T;
		vyz.rx = T;
		vyz.ry = F;
		vyz.rz = F;
		vyz.k = F;

	bool bxyz = false;
	struct freeCnstr vxyz;
		vxyz.dx = T;
		vxyz.dy = T;
		vxyz.dz = T;
		vxyz.rx = T;
		vxyz.ry = T;
		vxyz.rz = T;
		vxyz.k = T;

	bool bcala = false;
	struct freeCnstr vcala;
		vcala.dx = F;
		vcala.dy = F;
		vcala.dz = F;
		vcala.rx = F;
		vcala.ry = F;
		vcala.rz = F;
		vcala.k = F;

	bool bscale = false;
	struct freeCnstr vscale;
		vscale.dx = T;
		vscale.dy = T;
		vscale.dz = T;
		vscale.rx = T;
		vscale.ry = T;
		vscale.rz = T;
		vscale.k = F;

	bool brotation = false;
	struct freeCnstr vrotation;
		vrotation.dx = T;
		vrotation.dy = T;
		vrotation.dz = T;
		vrotation.rx = T;
		vrotation.ry = T;
		vrotation.rz = F;
		vrotation.k = T;

	//get point's type used for calculation
	LSPosVecConstIter iter = ptList->begin();
	LSPosVecConstIter iterEnd = ptList->end();
	int numberOfPoints = ptList->size();

	//n accepte pas la division par un entier???
	double div = numberOfPoints;
	double factor = 1 / div;


	//constraints on points
	while( iter != iterEnd)
	{
		TSpatialStatus::ESpatialStatus status = TSpatialStatus::kUnknown;

		status = iter->getGlobalStatus();
		switch (status)
		{
			case TSpatialStatus::kVx:
				if(bx == false)
				{bx =true;}
				break;
			case TSpatialStatus::kVy:
				if(by == false)
				{by =true;}
				break;

			case TSpatialStatus::kVz:
				if(bz == false)
				{bz =true;}
				break;

			case TSpatialStatus::kVxy:
				if(bxy == false)
				{bxy =true;}
				break;

			case TSpatialStatus::kVxz:
				if(bxz == false)
				{bxz =true;}
				break;

			case TSpatialStatus::kVyz:
				if(byz == false)
				{byz =true;}
				break;

			case TSpatialStatus::kVxyz:
				if(bxyz == false)
				{bxyz =true;}
				break;

			case TSpatialStatus::kCala:
				if(bcala == false)
				{bcala =true;}
				break;

			default:
				break;
		}

		//build gravity center
		fXcg = fXcg + (iter->getProvisionalValue().getX() * factor);
		fYcg = fYcg + (iter->getProvisionalValue().getY() * factor);
		fZcg = fZcg + (iter->getProvisionalValue().getZ() * factor);

		iter++;
	}

	setEstimatedGravityCenterCoord(fXcg, fYcg, fZcg);

	//constraints on distances
	if(	obs.sizeSpaDist() > 0 ||
		obs.sizeHorDist() > 0 ||
		obs.sizeVertDist() > 0 ||
		obs.sizeOffsetToVerLine() > 0 ||
		obs.sizeOffsetToSpaLine() > 0 ||
		obs.sizeOffsetToVerPlane() > 0 ||
		obs.sizeOffsetToTheoPlane() > 0 )
	{	
		if(bscale == false)
		{bscale =true;}
	}

	//constraints on orientation
	if( obs.sizeGyroOrie() > 0 ||
		TLGCDataSet::usedOrieCnstr())
	{
		if(brotation == false)
		{brotation =true;}
	}


	//build constraint vector
	setCnstrIdentifier(T, T, T, T, T, T, T);

	if(bx == true)
	{	addCnstr(vx);	}

	if(by == true)
	{	addCnstr(vy);	}

	if(bz == true)
	{	addCnstr(vz);	}

	if(bxy == true)
	{	addCnstr(vxy);	}

	if(bxz == true)
	{	addCnstr(vxz);	}

	if(byz == true)
	{	addCnstr(vyz);	}

	if(bxyz == true)
	{	addCnstr(vxyz);	}

	if(bcala == true)
	{	addCnstr(vcala);}

	if(bscale == true)
	{	addCnstr(vscale);}

	if(brotation == true)
	{	addCnstr(vrotation);}


	//Calc the number of constraint
	int i = -1;
	if(fCnstrVector.dx)
	{	i = i+1;
		fCnstrNumber.dx = i;
	}
	if(fCnstrVector.dy)
	{	i = i+1;
		fCnstrNumber.dy = i;
	}
	if(fCnstrVector.dz)
	{	i = i+1;
		fCnstrNumber.dz = i;
	}
	if(fCnstrVector.rx)
	{	i = i+1;
		fCnstrNumber.rx = i;
	}
	if(fCnstrVector.ry)
	{	i = i+1;
		fCnstrNumber.ry = i;
	}
	if(fCnstrVector.rz)
	{	i = i+1;
		fCnstrNumber.rz = i;
	}
	if(fCnstrVector.k)
	{	i = i+1;
		fCnstrNumber.k = i;
	}

	return;
}


void		TLSConstraintIdentifier::setEstimatedGravityCenterCoord(TLength x, TLength y, TLength z)
{
	fXcgEst = x;
	fYcgEst = y;
	fZcgEst = z;
	return;
}


int		TLSConstraintIdentifier::getNumberOfConstraint() const
{
	int i = 0;
	i = fCnstrVector.dx + fCnstrVector.dy + fCnstrVector.dz +
	fCnstrVector.rx + fCnstrVector.ry + fCnstrVector.rz +
	fCnstrVector.k;
	return i;
}

///////////////////////////////////////////////////////////////////////////////////////
//PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////
void	TLSConstraintIdentifier::setCnstrIdentifier(const bool dx, const bool dy, const bool dz, 
													const bool rx, const bool ry, const bool rz, 
													const bool k)
{
	fCnstrVector.dx = dx;
	fCnstrVector.dy = dy;
	fCnstrVector.dz = dz;
	fCnstrVector.rx = rx;
	fCnstrVector.ry = ry;
	fCnstrVector.rz = rz;
	fCnstrVector.k = k;

	return;
}

void	TLSConstraintIdentifier::addCnstr(const struct freeCnstr cnstr)
{
	fCnstrVector.dx = fCnstrVector.dx && cnstr.dx;
	fCnstrVector.dy = fCnstrVector.dy && cnstr.dy;
	fCnstrVector.dz = fCnstrVector.dz && cnstr.dz;
	fCnstrVector.rx = fCnstrVector.rx && cnstr.rx;
	fCnstrVector.ry = fCnstrVector.ry && cnstr.ry;
	fCnstrVector.rz = fCnstrVector.rz && cnstr.rz;
	fCnstrVector.k  = fCnstrVector.k  && cnstr.k;

	return;
}


///////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////