// TLSLocalSysWTWMeasCG.h
//
/** Concrete Class for a LS contrib generator processing WTW distance measurements
 defined in a local system */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TLSLocalSysWTWMeasCG
#define SU_TLSLocalSysWTWMeasCG


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
#include  "TALSWTWMeasContribGenerator.h"
#include "dist.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TLSLocalSysWTWMeasCG  : public TALSWTWMeasContribGenerator//: public TObject  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TLSLocalSysWTWMeasCG();

		///Constructor
		explicit	TLSLocalSysWTWMeasCG(TLSInputMatrices* im);

		/// Copy Constructor 
		TLSLocalSysWTWMeasCG(const  TLSLocalSysWTWMeasCG&);

		/// Destructor
		virtual  ~TLSLocalSysWTWMeasCG();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TLSLocalSysWTWMeasCG& operator=( const TLSLocalSysWTWMeasCG& );

		///processes the WTW measurement
		virtual void		processWTWDistMeas(TLSCalcWTWStation::CalcWTWDistMeasIterator wm);
	//@}


private:

	//ClassDef(TLSLocalSysWTWMeasCG, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_TLSLocalSysWTWMeasCG
