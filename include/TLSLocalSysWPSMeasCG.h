// TLSLocalSysWPSMeasCG.h
//
/** Concrete Class for a LS contrib generator processing WPS 2D measurements
 defined in a local system */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TLSLocalSysWPSMeasCG
#define SU_TLSLocalSysWPSMeasCG


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
//class  **classname**;
#include  "TALSWPSMeasContribGenerator.h"
#include "dist.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TLSLocalSysWPSMeasCG  : public TALSWPSMeasContribGenerator//: public TObject  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TLSLocalSysWPSMeasCG();

		///Constructor
		explicit	TLSLocalSysWPSMeasCG(TLSInputMatrices* im);

		/// Copy Constructor 
		TLSLocalSysWPSMeasCG(const  TLSLocalSysWPSMeasCG&);

		/// Destructor
		virtual  ~TLSLocalSysWPSMeasCG();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TLSLocalSysWPSMeasCG& operator=( const TLSLocalSysWPSMeasCG& );

		///processes the WPS measurement
		virtual void		processWPSMeas(TLSCalcWPSStation::CalcWPSMeasIterator wm);
	//@}


private:

	//ClassDef(TLSLocalSysWPSMeasCG, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_TLSLocalSysWPSMeasCG
