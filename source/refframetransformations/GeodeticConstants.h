#ifndef GeodeticConstants_H
#define GeodeticConstants_H

#include <Global.h>

//False coordinates of P0
const double XP0 = 2000.00000;
const double YP0 = 2097.79265;
const double ZP0 = 2433.66000;


//Geodetic ellipsoidal coordinates of P0 and Azimuth
const double PHIP0 = 51.3692;
const double LambdaP0 = 6.72124;
const double HP0 = 433.65921;
const double AzimuthCCSYaxis = 37.77864;

//GRS80 ellipsoid definition
const double SemiMajorAxisGRS80 = 6378137.0;
const double SemiMinorAxisGRS80 = 6356752.314140;
const double eccentricity = 0.0066943800229;

/*! Enumerator for Reference Frames implemented in the factory */
enum  ERefFrame {
			kCCS                 =  0, 
			kLAp0                =  1,
			kLGp0                =  2, 
			kCGRF                =  3, 
			kITRF97              =  4, 
			kWGS84               =  5, 
			kROMA40              =  6,
            kETRF93              =  7,
            kCH1903plus          =  8, /*!< Swiss coordinate system */
            // Projections:
			kCernXYHe            = 100, 
			kCernX0Y0He          = 101, 
			kCernXYHg00          = 102, 
			kCernXYHg00Topo      = 103, 
			kCernXYHg00Machine   = 104, 
			kCernXYHg85          = 105, 
			kCernXYHg85Machine   = 106, 
			kCERNXYHsSphereSPS   = 107, 
			kCGRFSphere          = 108,
#ifdef USE_SWISSTOPO
            kSwissLV95           = 109,
            kSwissLV03           = 110,
#endif
            kFrenchRGF93Zone5    = 205,
			kLambert93			 = 206,
			//
			//The same as ETRF93
			kRGF93               = 207,
			kCHTRF95             = 208,
			//
            // Local Reference Frames:
            kMLA1985Machine      = 1000,
            kMLA2000Machine      = 1001,
            kMLASphere           = 1002,
            kLA1985Machine       = 1010,
            kLA2000Machine       = 1011,
            kLASphere            = 1012,

            kMLGGRS80            = 2000,
            kMLGSphere           = 2001,
            kLGGRS80             = 2010,
            kLGSphere            = 2011,

            kLocalRefFrame       = -2 /*!< This was present in the TDataParameters variant. Referenced in LGC in few places */
};

enum  EGeoid {kCG2000, kCG2000Machine, kCG2000topo, kCG1985, 
			          kCG1985Machine, kNoGeoid, kCGSphere};

#endif