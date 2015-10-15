#include <TLV95Transformation.h>

#include <TRefFrameInfo.h>

#include <assert.h>
#include <math.h>
#include <TVNumericValue.h>

// Anonymous namespace for local constants
namespace
{
    // constants
    const double a = 6377397.155; 
    const double E2 = 0.006674372230614; 
    const double PI = 3.14159265358979323844;
    // Ellipsoidal coordinates of the projection centre in Bern
    const double phi0 = (46+57/60.0+8.66/3600)*PI/180.0;
    const double lam0 = (7+26/60.0+22.5/3600)*PI/180.0; 
    
    // Radius of the projection sphere
    const double R = a*sqrt(1-E2)/(1-E2*pow((sin(phi0)),2));

    // Relationship between longitude on sphere and ellipsoid:
    const double alpha = sqrt(1+E2/(1-E2)*pow(cos(phi0),4));

    // latitude of the fundamental point on the sphere
    const double b0 = asin(sin(phi0)/alpha); 

    // Constant of the latitude formula
    const double K=log(tan(PI/4+b0/2)) - alpha*log(tan(PI/4+phi0/2)) + alpha*sqrt(E2)/2*log((1+sqrt(E2)*sin(phi0))/(1-sqrt(E2)*sin(phi0)));

    const int LV95_Y_OFFSET = 2600000;
    const int LV95_X_OFFSET = 1200000;
}

TLV95Transformation::TLV95Transformation(bool fromCH1903plus)
: fFromCH1903plus(fromCH1903plus)
{
}

TLV95Transformation * TLV95Transformation::clone() const
{
    return new TLV95Transformation(*this);
}

TLV95Transformation * TLV95Transformation::inverse() const
{
    return new TLV95Transformation(!fFromCH1903plus);
}

TAReferenceFrame * TLV95Transformation::getSourceFrame() const
{
    return TRefFrameInfo::getReferenceFrame(
        fFromCH1903plus ? TRefSystemFactory::kCH1903plus : TRefSystemFactory::kSwissLV95);
}

TAReferenceFrame * TLV95Transformation::getDestinationFrame() const
{
    return TRefFrameInfo::getReferenceFrame(
        fFromCH1903plus ? TRefSystemFactory::kSwissLV95 : TRefSystemFactory::kCH1903plus);
}

bool TLV95Transformation::transform(TPositionVector & pv) const
{
    if(fFromCH1903plus)
        return transformFromCH1903plus(pv);
    return transformToCH1903plus(pv);
}

bool TLV95Transformation::transformFromCH1903plus(TPositionVector & pv) const
{
   //transform phi lambda h
    TSpatialPosition position(getSourceFrame());
	if(!position.setCoordinates(pv))
        return false;

    const double phi = position.getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid().getRadiansValue();
    const double lam = position.getCoordinates(TCoordSysFactory::kGeodetic).getLambdaEllipsoid().getRadiansValue();
    const double h   = position.getCoordinates(TCoordSysFactory::kGeodetic).getH().getMetresValue();
    
    // Auxiliary value:
    const double S = - alpha * log(tan(PI/4 - phi/2)) - (alpha*sqrt(E2))/2 * log((1+sqrt(E2)*sin(phi))/(1-sqrt(E2)*sin(phi)))+K;

    // spherical latitude
    const double b = 2*(atan(exp(S))-PI/4);

    // spherical longitude
    const double l = alpha*(lam - lam0);


    // Rotation to pseudo-equator system
    const double l_ = atan(sin(l)/(sin(b0)*tan(b)+cos(b0)*cos(l)));
    const double b_ = asin(cos(b0)*sin(b) - sin(b0)*cos(b)*cos(l));

    // sphere to plane Mercator projection
    double Y = R*l_;
    double X = R/2*log((1+sin(b_))/(1-sin(b_)));


    // Offset to LV95:
    Y+=LV95_Y_OFFSET;
    X+=LV95_X_OFFSET;

    pv = TPositionVector(X, Y, h, TCoordSysFactory::k2DPlusH);

    return true;
}

bool TLV95Transformation::transformToCH1903plus(TPositionVector & pv) const
{
    //transform phi lambda h
    TSpatialPosition position(getSourceFrame());
	if(!position.setCoordinates(pv))
        return false;

    const double x = position.getCoordinates(TCoordSysFactory::k2DPlusH).getX().getMetresValue();
    const double y = position.getCoordinates(TCoordSysFactory::k2DPlusH).getY().getMetresValue();
    const double h = position.getCoordinates(TCoordSysFactory::k2DPlusH).getH().getMetresValue();

    // projection plane (x, y) to sphere (l_, b_)
    const double Y = y - LV95_Y_OFFSET;
    const double X = x - LV95_X_OFFSET; 
    const double l_ = Y/R;
    const double b_ = 2 * (atan(exp(X/R)) - PI/4);

    // Pseudo-equator system -> to equator system
    const double b = asin(cos(b0)*sin(b_) + sin(b0)*cos(b_)*cos(l_));
    const double l = atan(sin(l_)/(cos(b0)*cos(l_) - sin(b0)*tan(b_)));

    // Sphere (b, l) -> to elipsoid (phi, lambda)
    const double lambda = lam0 + l/alpha;

    double phi = b;
    for (int i = 0; i != kMaxIter; i++)
    {
        double S=(log(tan(PI/4+b/2))-K)/alpha + sqrt(E2)*log(tan(PI/4+asin(sqrt(E2)*sin(phi))/2));
        double phi_new = 2*atan(exp(S))-PI/2;

        double delta = fabs(phi-phi_new);
        phi = phi_new;

        if(delta<1e-10)
            break;
    }

    //pv = TPositionVector(phi, lambda, h, TCoordSysFactory::kGeodetic);
	// We can't leave it in the Geodetic form. Other code expects to get Cartesian
    TPositionVector tmp = TPositionVector(phi, lambda, h, TCoordSysFactory::kGeodetic);
	TSpatialPosition outpos(getDestinationFrame());
	if(!outpos.setCoordinates(tmp))
        return false;
	// Project to Carthesian
	pv = outpos.getCoordinates(TCoordSysFactory::k3DCartesian);
	
    return true;
}
