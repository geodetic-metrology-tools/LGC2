//  TRefSystemFactory.h
//
/*! A singleton class listing reference surface, reference frame and reference frame transformation.
	A "server" of these systems. 
  
    Patterns: a flyweight factory of reference systems
  
   
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_REF_SYS_FACTORY
#define SU_REF_SYS_FACTORY


#if _MSC_VER >= 1000
#pragma once
#pragma warning (disable:4786)
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include <string>
#include <vector>

#include <iostream>
#include <iomanip>

#include <stddef.h>
//using namespace std;

class TAGeoidModel;
class TReferenceEllipsoid;
class TAReferenceFrame;
class TGeodeticRefFrame;
class TModifiedLocalAstronomicalRF;

class TARefFrameTransformation;
class TSpatialPosition;

#include <TLocalSystemOrigin.h>
#include <refframetransformations\TCCS2CGRFTransformation.h>
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/



//#define PHIP0 LITERAL(51.3692);

//! Singleton class: produce one unique instance listing ref. surfaces, ref.frames and transformations
class TRefSystemFactory
{
public:
	
	//!@name Constants
	//@{
		/*! Enumerator for ellipsoids implemented in the factory */
		enum  ERefEll {kGRS80, kWGSEll, kInternationalEll, kSphere, kNoEllipsoid, kBessel1841};
		/*! Enumerator for geoids implemented in the factory */
		enum  EGeoid {kCG2000, kCG2000Machine, kCG2000topo, kCG1985, 
			          kCG1985Machine, kNoGeoid, kCGSphere};
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

            // Reserved value:
			kNotInGraph			 = -1,
            kLocalRefFrame       = -2 /*!< This was present in the TDataParameters variant. Referenced in LGC in few places */
        };

		/*! Enumerator for transformations between Ref.Frames implemented in the factory */
		enum  ERefFrameTransform {	kCCS2CGRF, kLAp02LGp0, kLGp02CGRF, kLAp02CCS,
									kROMA2WGS, kWGS2CGRF, kXYHe2CCS, kX0Y0He2XYHe,
									kXYHg2XYHe, kXYHg2XYHe00Topo, kXYHg2XYHe00Machine, 
									kXYHg2XYHe85, kXYHg2XYHe85Machine, kXYHsSphereSPS2CCS,
									kCCS2CGRFSphere, 
									kCGRF2CCS, kLGp02LAp0, kCGRF2LGp0, kCCS2LAp0,
									kWGS2ROMA, kCGRF2WGS, kCCS2XYHe, kXYHe2X0Y0He,
									kXYHe2XYHg, kXYHe00Topo2XYHg, kXYHe00Machine2XYHg, 
									kXYHe852XYHg, kXYHe85Machine2XYHg, kCCS2XYHsSphereSPS,
									kCGRFSphere2CCS, kITRF972CGRF, kCGRF2ITRF97, 
									kITRF972ETRF93, kETRF932ITRF97,
                                    kETRF932CH1903plus, kCH1903plus2ETRF93,
#ifdef USE_SWISSTOPO
                                    kCH1903plus2SwissLV95, kSwissLV952CH1903plus,
                                    kSwissLV952SwissLV03, kSwissLV032SwissLV95,
#endif
                                    kETRF932FrenchRGF93, kFrenchRGF932ETRF93,
									kETRF932kLambert93, kLambert932ETRF93,
									kETRF932kCHTRF95, kCHTRF952kETRF93,
									kETRF932kRGF93, kRGF932kETRF93
		};
	//@}
	

	/*!@name Member Functions */
	//@{
		/*! Instance method to obtain a pointer to the TRefSystemFactory instance */
		static TRefSystemFactory* getRefSystemFactory();	
		
		void	deleteRefSystemFactory();

		/*! Return a pointer to the geoid asked for 
		\param geoidId an element of the existing geoid enumeration */
		TAGeoidModel* getGeoid(const EGeoid geoidId);

		/*! Return a pointer to the reference ellipsoid asked for 
		\param ellId an element of the existing ellipsoid enumeration */
		TReferenceEllipsoid* getEllipsoid(const ERefEll ellId);
		
		/*! Return a pointer to the reference frame asked for 
		\param refFrameId an element of the existing Ref. Frame enumeration */
		TAReferenceFrame* getRefFrame(const ERefFrame refFrameId);

		/*! Return a pointer to the reference frame transformation asked for 
		\param transformId an element of the existing transformations enumeration */
		TARefFrameTransformation* getTransformation(const ERefFrameTransform transformId);

		/*! Return the transformation between two reference frames
			\return a vector of Reference Frame transformations */
		vector<TARefFrameTransformation&> getTransform(const ERefFrame refFrameIdfrom, const ERefFrame refFrameIdto);
	
		/*! test if the reference frame is in graph or not 
		\param pointer to a Ref. Frame
		\return true if the ref. frame is in graph*/
		bool isInRFFactory( const TAReferenceFrame* );

		/*! Return a pointer to TGeodeticRefFrame
		\param reference frame id
		\return an error message (cerr) if the id is not cooresponding to a TGeodeticRefFrame*/
		TGeodeticRefFrame*	getGeoRefFrame(const ERefFrame refFrameId);

		//return a pointer to a new Local ref Frame and put it in a list in order to be deleted
		TAReferenceFrame* getNewLocalRefFrame();

		//return a pointer to a new Modified Local Astronomic ref Frame and put it in a list in order to be deleted
		TAReferenceFrame* getNewLocalRefFrame(const TLocalSystemOrigin & LSO, EGeoid geoid, ERefFrame frame);

		//return a CCS2CGRF transformation
		TCCS2CGRFTransformation* getCCS2CGRFTransformation() {return &pCCS2CGRFTransformation;}

	//@}


protected:

	/*!@name Constructors and Destructors */
	//@{
		/*! Default Constructor: build the unique instance of TRefSystemFactory */
		TRefSystemFactory();

		/*! Destructor */
		virtual  ~TRefSystemFactory();

private:

	/*! Copy constructor */
	TRefSystemFactory( const TRefSystemFactory& );
	//@}

	void	init();

	/*! Copy Assigment Operator */
	TRefSystemFactory& operator=( const TRefSystemFactory& );

	
private:
	static TRefSystemFactory* fRefSystemFactory; /*!< static member that contains a pointer to the unique instance of TRefSystemFactory */

	vector<TAGeoidModel*> fGeoidList;	/*!< list of pointers to the to-be-defined geoids */
	vector<TReferenceEllipsoid*> fRefEllList; /*!< list of pointers to the to-be-defined ellipsoids */
	vector<TAReferenceFrame*> fRefFrameList; /*!< list of pointers to the to-be-defined Ref.Frames */

	vector<TAReferenceFrame*> fLocalRefFrameList; /*!< list of pointers to the local Ref.Frames */

	vector<TARefFrameTransformation*> fTransformList; /*!< list of pointers to the to-be-defined Ref.Frame transformations */
	//il est important que ce soit une classe TA plutot que TV pour avoir acces au destructeur

	TCCS2CGRFTransformation pCCS2CGRFTransformation;


	TGeodeticRefFrame *fCGRF, *fCGRFSphere;
	//ClassDef(TRefSystemFactory, 1)
};
/*@}*/

#endif // SU_REF_SYS_FACTORY
