#ifndef GeodeticConstants_H
#define GeodeticConstants_H
/*!
   \ingroup ConstantsDefaults
   @{

/*!@name P0 false coordinates*/
//@{
	/// in [m]
	static const double XP0 = 2000.00000;
	/// in [m]
	static const double YP0 = 2097.79265;
	/// in [m]
	static const double ZP0 = 2433.66000;
//@}

/*!@name Geodetic ellipsoidal coordinates of P0 and azimuth. */
//@{
	/// in [gon]
	static const double PHIP0 = 51.3692;
	/// in [gon]
	static const double LambdaP0 = 6.72124;
	/// in [m]
	static const double HP0 = 433.65921;
	/// in [gon]
	static const double AzimuthCCSYaxis = 37.77864; 
//@}

/*!@name GRS80 ellipsoid definition. */
//@{
	/// in [m]
	static const double SemiMajorAxisGRS80 = 6378137.0;
	/// in [m]
	static const double SemiMinorAxisGRS80 = 6356752.314140;
	static const double eccentrGRS80 = 0.0066943800229;
//@}

/*!@name Sphere definition. */
//@{
	static const double sphereRadius = 6371000.0;

//@}
/*! @} End of Doxygen Groups*/
#endif