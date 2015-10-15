// TLSLocalSys2DOffMeasCG.h
//
/** Concrete Class for a LS contrib generator processing 2D offset measurements
 defined in a local system */
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_TLSLocalSys2DOffMeasCG
#define SU_TLSLocalSys2DOffMeasCG


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
#include  "TALS2DOffMeasContribGenerator.h"
#include "dist.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TLSLocalSys2DOffMeasCG  : public TALS2DOffMeasContribGenerator//: public TObject  
{
public:
	//constants

	/**@name Constructors and Destructors */
	//@{
		/// Default Constructor 
		TLSLocalSys2DOffMeasCG();

		///Constructor
		explicit	TLSLocalSys2DOffMeasCG(TLSInputMatrices* im);

		/// Copy Constructor 
		TLSLocalSys2DOffMeasCG(const  TLSLocalSys2DOffMeasCG&);

		/// Destructor
		virtual  ~TLSLocalSys2DOffMeasCG();
	//@}


	/**@name Member Functions */
	//@{
		/// Copy Assignment Operator 
		TLSLocalSys2DOffMeasCG& operator=( const TLSLocalSys2DOffMeasCG& );

		///processes the WPS measurement
		virtual void		process2DOffsetMeas(TLSCalc2DOffsetROM::Calc2DOffsetIterator om);
	//@}


private:

	//ClassDef(TLSLocalSys2DOffMeasCG, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////


#endif // SU_TLSLocalSys2DOffMeasCG