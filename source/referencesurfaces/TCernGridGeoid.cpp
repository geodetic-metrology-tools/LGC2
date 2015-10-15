#include "TSpatialPosition.h"
#include "TGraph.h"
#include "TAReferenceFrame.h"


#include "TCernGridGeoid.h"

#include <TNotInLepGridException.h>

#include <math.h>
#include <valarray>
#include <vector>






/////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////////////////////
TCernGridGeoid::TCernGridGeoid() : fName(0), fDownLeft(0,0,0,TCoordSysFactory::k3DCartesian),
 fUpRight(0,0,0,TCoordSysFactory::k3DCartesian)
{//Default constructor
	fNMatrix = 0;
	fEtaMatrix = 0;
	fXiMatrix = 0;

	fDefRFPtr = 0;
	fDefEllPtr = 0;
	fCalcRFPtr = 0;
}


TCernGridGeoid::TCernGridGeoid( const string& name,
								const TMatrix* N, const TMatrix* Eta, const TMatrix* Xsi,
								const TPositionVector downLeft, const TPositionVector upRight,
								TAReferenceFrame* def, TReferenceEllipsoid* ell,
								TAReferenceFrame* calc)
	: fName(name), fDownLeft(downLeft), fUpRight(upRight)
{//Constructor
	

	fNMatrix.setDimensions(15, 14);
	fEtaMatrix.setDimensions(15, 14);
	fXiMatrix.setDimensions(15, 14);
	
	
	fNMatrix = *N;
	fEtaMatrix = *Eta;
	fXiMatrix = *Xsi;


	fDefRFPtr = def;
	fDefEllPtr = ell;
	fCalcRFPtr = calc;
	
}


TCernGridGeoid::~TCernGridGeoid()
{//Destructor
}



///////////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////

TLength TCernGridGeoid::getN ( const TSpatialPosition& sp) const
{
	// deep copy of TSpatialPosition
	TSpatialPosition spos(sp);

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if ( (spos.getRefFrame() != fCalcRFPtr)
		&& (spos.getRefFrame() != TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHe))
		&& (spos.getRefFrame() != TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg00))
		&& (spos.getRefFrame() != TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg00Topo))
		&& (spos.getRefFrame() != TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg00Machine))
		&& (spos.getRefFrame() != TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg85))
		&& (spos.getRefFrame() != TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg85Machine)) )
	{
		spos.transform(fCalcRFPtr);
	}

	TLength NValue;


	// the spatial position must be in the LEP grid
	TReal x,y,xdl,ydl,xur,yur;
	x = spos.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = spos.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();
	xdl = fDownLeft.getX().getValue();
	ydl = fDownLeft.getY().getValue();
	xur = fUpRight.getX().getValue();
	yur = fUpRight.getY().getValue();
	

	if ( (x>=xdl) && (x<=xur) && (y>=ydl) && (y<=yur) )
	{
		NValue.setMetresValue( splineInterpolation(fNMatrix, spos) );
		return NValue;
	}

	else
	{
		char posstr[512];
		_snprintf(posstr, 512, " (%.3g,%.3g,%.3g) ",x, y, spos.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue());
		throw TNotInLepGridException("TNotInLepGridException: getEta function problem with coordinate " + string(posstr) + ".");
		//exit(EXIT_FAILURE);
	}

	
}


TAngle TCernGridGeoid::getEta ( const TSpatialPosition& spatialPosition) const
{

	// deep copy of TSpatialPosition
	TSpatialPosition spos(spatialPosition);

	TAngle eta;

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if ( (spos.getRefFrame() != fCalcRFPtr) )
	{
		spos.transform(fCalcRFPtr);
	}


	// the spatial position must be in the LEP grid
	TReal x,y,xdl,ydl,xur,yur;
	x = spos.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = spos.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();
	xdl = fDownLeft.getX().getValue();
	ydl = fDownLeft.getY().getValue();
	xur = fUpRight.getX().getValue();
	yur = fUpRight.getY().getValue();
	

	if ( (x>=xdl) && (x<=xur) && (y>=ydl) && (y<=yur) )
	{
		// round to LITERAL(0.01) cc 
		TReal interpolated = splineInterpolation(fEtaMatrix, spos) * 100;
		int temp = (int) interpolated;
		if ( ( interpolated - temp ) >= LITERAL(0.5))
			temp += 1;

		TReal newTemp = temp;
		newTemp = newTemp/100;

		eta.setGonsValue( newTemp * LITERAL(0.0001) );
		return eta;
	}

	else
	{
		cerr << "SURVEYLIB LOG MESSAGE: Error : spatial position not in the LEP grid (2)" << endl;
		
		char posstr[512];
		_snprintf(posstr, 512, " (%.3g,%.3g,%.3g) ",x, y, spos.getCoordinates(TCoordSysFactory::k3DCartesian).getZ().getValue());
		throw TNotInLepGridException("TNotInLepGridException: getEta function problem with coordinate " + string(posstr) + ".");
		
		//cerr << endl << "Error : spatial position not in the LEP grid" << endl;

		///
		//TODO@*@
		///
		//exit(EXIT_FAILURE);
	}

	
}


TAngle TCernGridGeoid::getXi ( const TSpatialPosition& sp) const
{

	// deep copy of TSpatialPosition
	TSpatialPosition spos(sp);

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if (spos.getRefFrame() != fCalcRFPtr)
	{
		spos.transform(fCalcRFPtr);
	}

	TAngle xsi;


	// the spatial position must be in the LEP grid
	TReal x,y,xdl,ydl,xur,yur;
	x = spos.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = spos.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();
	xdl = fDownLeft.getX().getValue();
	ydl = fDownLeft.getY().getValue();
	xur = fUpRight.getX().getValue();
	yur = fUpRight.getY().getValue();
	

	if ( (x>=xdl) && (x<=xur) && (y>=ydl) && (y<=yur) )
	{
		// round to LITERAL(0.01) cc 
		TReal interpolated = splineInterpolation(fXiMatrix, spos) * 100;
		int temp = (int) interpolated;
		if ( ( interpolated - temp ) >= LITERAL(0.5))
			temp += 1;

		TReal newTemp = temp;
		newTemp = newTemp/100;

		xsi.setGonsValue( newTemp * LITERAL(0.0001) );
		return xsi;
	}

	else
	{
		char posstr[512];
		_snprintf(posstr, 512, " (%.3g,%.3g,%.3g) ",x, y, spos.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue());
		throw TNotInLepGridException("TNotInLepGridException: getEta function problem with coordinate " + string(posstr) + ".");
		
		//cerr << endl << "Error : spatial position not in the LEP grid" << endl;
		///
		//TODO@*@
		///
		//exit(EXIT_FAILURE);
	}
	
}


TAngle	TCernGridGeoid::getDAlpha ( const TSpatialPosition& sp ) const
{
	// deep copy of TSpatialPosition
	TSpatialPosition position(sp);

	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if (position.getRefFrame() != fDefRFPtr)
	{
		position.transform(fDefRFPtr);
	}

	
	// computation of phi (latitude for the spatial position)
	TAngle latitude;
	TReal phi;
	TAngle eta, fDAlphaValue;
	
	latitude = position.getCoordinates(TCoordSysFactory::kGeodetic).getPhiEllipsoid();
	phi = latitude.getRadiansValue();


	// transformation in the calculation RF 
	position.transform(fCalcRFPtr);


	// the spatial position must be in the LEP grid
	TReal x,y,xdl,ydl,xur,yur;
	x = position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue();
	y = position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue();
	xdl = fDownLeft.getX().getValue();
	ydl = fDownLeft.getY().getValue();
	xur = fUpRight.getX().getValue();
	yur = fUpRight.getY().getValue();
	

	if ( (x>=xdl) && (x<=xur) && (y>=ydl) && (y<=yur) )
	{
		
		eta = getEta(position); // / (LITERAL(6.366) * 100000);
		fDAlphaValue = eta*tanq(phi);
		return fDAlphaValue;
	}

	else
	{
		char posstr[512];
		_snprintf(posstr, 512, " (%.3g,%.3g,%.3g) ",position.getCoordinates(TCoordSysFactory::k3DCartesian).getZ().getValue());
		throw TNotInLepGridException("TNotInLepGridException: getEta function problem with coordinate " + string(posstr) + ".");
		
		//cerr << "Error : spatial position not in the LEP grid" << endl;
		///
		//TODO@*@
		///
		//exit(EXIT_FAILURE);
	}
	
	
}

TAngle	TCernGridGeoid::getDAlpha ( const TSpatialPosition& sp, const TAngle& latitude ) const
{
	// deep copy of TSpatialPosition
	TSpatialPosition position(sp);

	TReal phi;
	TAngle eta, fDAlphaValue;
	
	phi = latitude.getRadiansValue();


	// RF of TSpatialPosition must be the same as the geoid CalculationRF
	if (position.getRefFrame() != fCalcRFPtr)
	{
		position.transform(fCalcRFPtr);
	}

	// the spatial position must be in the LEP grid
	if ( (position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue() >= fDownLeft.getX().getValue()) && (position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue() <= fUpRight.getX().getValue())
		&& (position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue() >= fDownLeft.getY().getValue()) && (position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue() <= fUpRight.getY().getValue()) )
	{
		
		eta = getEta(position); // / (LITERAL(6.366) * 100000);
		fDAlphaValue = eta*tanq(phi);
		return fDAlphaValue;
	}

	else
	{
		char posstr[512];
		_snprintf(posstr, 512, " (%.3g,%.3g,%.3g) ", 
			position.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue(), 
			position.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue(), 
			position.getCoordinates(TCoordSysFactory::k3DCartesian).getZ().getValue());
		throw TNotInLepGridException("TNotInLepGridException: getEta function problem with coordinate " + string(posstr) + ".");
		
		//cerr << "Error : spatial position not in the LEP grid" << endl;
		///
		//TODO@*@
		///
		//exit(EXIT_FAILURE);
	}
	
	
}


void TCernGridGeoid::setGeoidId(const TRefSystemFactory::EGeoid geoidId)
{
	fGeoidId = geoidId;
}


//////////////////////////////////////////////////////////////////////
// Private Member Functions
//////////////////////////////////////////////////////////////////////

TReal TCernGridGeoid::splineInterpolation(const TMatrix& matrix, const TSpatialPosition& spos) const
{

	
	TReal N = std::numeric_limits<TReal>::infinity();
	TReal Xo(spos.getCoordinates(TCoordSysFactory::k3DCartesian).getX().getValue()/1000), Yo(spos.getCoordinates(TCoordSysFactory::k3DCartesian).getY().getValue()/1000);
	valarray<TReal> x(matrix.numCols()), y(matrix.numRows()), absx(matrix.numCols()), absy(matrix.numRows());
	TReal xmin, ymin, t;
	int I(-1),J(-1);
	TMatrix base(4,4), c(4,3), T(1,4), q(1,3), Q(4,3);
	vector<int> L(4), K(4);


	
	int i;
	// coordinates from the interpolated point
	for(i = 0; i<matrix.numRows(); i++)
	{
		y[i] = (i-1)-Yo;
		absy[i] = fabsq(y[i]);
	}


	for(i = 0; i<matrix.numCols(); i++)
	{
		x[i] = (i-6)-Xo;
		absx[i] = fabsq(x[i]);
	}



	// determination of the nearest point in the LEP grid
	//cout << y[0] << endl;
	//cout << absy[0] << endl;
	
	xmin = absx.min();
	ymin = absy.min();

	for(i = 0; i<matrix.numCols(); i++)
	{
		if (absx[i] == xmin)
			J = i;
	}

	for(i = 0; i<matrix.numRows(); i++)
	{
		if (absy[i] == ymin)
			I = i;
	}


	// Catmull Rom splines
	// base matrix
	base(0,0) = -LITERAL(0.5);
	base(0,1) = LITERAL(1.5);
	base(0,2) = -LITERAL(1.5);
	base(0,3) = LITERAL(0.5);
	base(1,0) = 1;
	base(1,1) = -LITERAL(2.5);
	base(1,2) = 2;
	base(1,3) = -LITERAL(0.5);
	base(2,0) = -LITERAL(0.5);
	base(2,1) = 0;
	base(2,2) = LITERAL(0.5);
	base(2,3) = 0;
	base(3,0) = 0;
	base(3,1) = 1;
	base(3,2) = 0;
	base(3,3) = 0;

	// elements of the matrix used for the interpolation
	// if Xo, Yo is not a point of the grid
	if (x[J] > 0)
	{
		L[0] = J-2;
	}
	else
	{
		if(x[J] < 0)
		{
			L[0] = J-1;

		}
	}


	if (x[J] != 0)
	{
		for(i = 1; i < 4; i++)
			L[i] = L[i-1]+1;
	}


	//cout << L[0] << "  " << L[1] << "  " << L[2] << "  " << L[3] << endl;


	if (y[I] > 0)
	{
		K[0] = I-2;

	}
	else 
	{
		if(y[I] < 0)
		{
			K[0] = I-1;

		}
	}


	if (y[I] != 0)
	{
		for(i = 1; i < 4; i++)
			K[i] = K[i-1]+1;
	}

	//cout << K[0] << "  " << K[1] << "  " << K[2] << "  " << K[3] << endl;



	if ( (x[J] != 0) && (y[I] != 0) )
	{	
		// interpolation
		for(i = 0; i < 4; i++)
		{
			int j;
			for(j = 0; j < 4; j++)
			{
				c(j,0) = x[L[j]]+Xo;
				c(j,1) = y[K[i]]+Yo;
				c(j,2) = matrix(K[i],L[j]);
			}
			/*for(j=0; j<4; j++)
				cout << c(j,0) << " " << c(j,1) << " " << c(j,2) << endl;*/

			//cout << endl;


			t = -x[L[1]];

			T(0,0) = (double) powq(t,3);
			T(0,1) = (double) powq(t,2);
			T(0,2) = t;
			T(0,3) = 1;

			q = T*base*c;


			for(j=0; j<3; j++)
				Q(i,j) = q(0,j);

			
		}

		//for(i=0; i<4; i++)
		//	cout << Q(i,0) << " " << Q(i,1) << " " << Q(i,2) << endl;

		//cout << endl;

		t = -y[K[1]];

		T(0,0) = (double) powq(t,3);
		T(0,1) = (double) powq(t,2);
		T(0,2) = t;
		T(0,3) = 1;

		//cout << T(0,0) << " " << T(0,1) << " " << T(0,2) << " " << T(0,3) << endl << endl;


		q = T*base*Q;

		//cout << q(0,0) << " " << q(0,1) << " " << q(0,2) << endl;

		N = q(0,2);

	}


	if (x[J] == 0)
	{

		if (y[I] == 0)
			N = matrix(I,J);

		else
		{
			for( i = 0; i < 4; i++ )
			{
				Q(i,0) = x[J]+Xo;
				Q(i,1) = y[K[i]]+Yo;
				Q(i,2) = matrix(K[i],J);
			}


			//for(i=1; i<=4; i++)
			//	cout << Q(i,1) << " " << Q(i,2) << " " << Q(i,3) << endl;

			//cout << endl;



			t = -y[K[1]];

			T(0,0) = (double) powq(t,3);
			T(0,1) = (double) powq(t,2);
			T(0,2) = t;
			T(0,3) = 1;

			//cout << T(1,1) << " " << T(1,2) << " " << T(1,3) << " " << T(1,4) << endl << endl;


			q = T*base*Q;

			//cout << q(1,1) << " " << q(1,2) << " " << q(1,3) << endl;

			N = q(0,2);
		}
	}
		


	if ( (y[I] == 0) && (x[J] != 0) )
	{
		for(int j = 0; j < 4; j++)
		{
			c(j,0) = x[L[j]]+Xo;
			c(j,1) = y[I]+Yo;
			c(j,2) = matrix(I,L[j]);
		}


		t = -x[L[1]];

		T(0,0) = (double) powq(t,3);
		T(0,1) = (double) powq(t,2);
		T(0,2) = t;
		T(0,3) = 1;

		
		q = T*base*c;

		N = q(0,2);
	}
	

	return N;
}

////////////////////////////////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////////////////////////////////
