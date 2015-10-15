// TVNumericValue.h
//
/** Virtual class for Numerical Value */
//
// Patterns:
//
// 
// Copyright 2002-10 CERN SU, M.Jones. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_V_NUMERIC_VALUE
#define SU_V_NUMERIC_VALUE


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Quad.h"
#include <math.h>

////////////////////////////////////////////////////////////////
// Forward declarations
//
//using namespace std;
//
//
//typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TVNumericValue  //: public TObject

{
public:
	/**@name Constants */
	//@{
		enum  EStatus { kNull, kKnown };
	//@}

	virtual	EStatus	getStatus() const = 0 ;

	virtual void setStatus( EStatus ) = 0;

	virtual bool isNull() const = 0;

	virtual ~TVNumericValue() { }






	//ClassDef(TVNumericValue, 1)
};
#endif // SU_V_NUMERIC_VALUE
