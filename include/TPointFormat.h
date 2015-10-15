// TPointFormat.h
/*!
Class used by a text stream to store and get all necessary formatting information
for the points and positions

Patterns:

Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TPointFormat
#define SU_TPointFormat


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
// typedefs
#include  "TSpatialStatus.h"
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup projectframework
@{*/

//Class definition
class  TPointFormat 
{
public:

	/*!name Constant*/
	//@!{
	/*!Coordinate Precision*/
	enum ECoordPrecision {kMetre, k100Millimetres, k10Millimetres, kMillimetre, k100Micrometres, k10Micrometres, kMicrometre, k100Nanometres};
	//@}


	/*!name Constructor / Destructor*/
	//@!{
	/*!Constructor*/
	TPointFormat();

	/*!Constructor
	\param name Width
	\param coord Width
	\param coord Precision
	\param coord error Width
	\param coord error Precision
	\param show Sign (bool)
	*/
	TPointFormat(int nameWidth, int coordWidth, ECoordPrecision coordPrecision, int coordErrorWidth, ECoordPrecision coordErrorPrecision, bool showSign, TSpatialStatus::ESpatialStatus status);

	/*!Copy constructor*/
	TPointFormat(const  TPointFormat&);
	
	/*!Destructor*/
	virtual  ~TPointFormat();
	//@}


	/*!name Public Member Function*/
	//@!{
	/*!Copy Assignment Operator
	\param point Format*/
	TPointFormat& operator=( const TPointFormat& );

	void	setSpatialStatus(TSpatialStatus::ESpatialStatus status);

	/*!\return the width reserved for the point name fields*/
	virtual int					getNameWidth() const;

	/*!\return the width reserved for the position coordinate fiels*/
	virtual int					getCoordWidth() const;

	/*!\return the precision with wich the positions will be displayed*/
	virtual ECoordPrecision		getCoordPrecision() const;

	/*!\return the width reserved for the position error fiels*/
	virtual int					getCoordErrorWidth() const;

	/*!\return the precision with wich the positions error will be displayed*/
	virtual ECoordPrecision		getCoordErrorPrecision() const;


	/*!\return a true boolean if the number's signs are to be shown*/
	virtual bool				getShowSign() const;

	/*!\return the point's spatial status*/
	virtual TSpatialStatus::ESpatialStatus				getSpatialStatus() const;


	/*!\return the width reserved for the point name fields*/
	virtual void  setNameWidth(int i) {fNameWidth = i; return;}

	/*!\return the width reserved for the position coordinate fiels*/
	virtual void  setCoordWidth(int i) {fCoordWidth = i; return;}

	/*!\return the precision with wich the positions will be displayed*/
	virtual void  setCoordPrecision(ECoordPrecision e) {fCoordPrecision = e; return;}

	/*!\return the width reserved for the position error fiels*/
	virtual void  setCoordErrorWidth(int i) {fCoordErrorWidth = i; return;}

	/*!\return the precision with wich the positions error will be displayed*/
	virtual void  setCoordErrorPrecision(ECoordPrecision e) {fCoordErrorPrecision = e; return;}

	//@}
	//JH@@
	static double getPrecision(ECoordPrecision currPrec);
	static ECoordPrecision getPrecisionFromNumber(int prec);



private:

	int								fNameWidth;				/*!<name width*/
	int								fCoordWidth;			/*!<coordinate width*/
	ECoordPrecision					fCoordPrecision;		/*!<coordinate precision*/
	int								fCoordErrorWidth;		/*!<coordinate error width*/
	ECoordPrecision					fCoordErrorPrecision;	/*!<coordinate error precision*/
	bool							fShowSign;				/*!<show sign (bool)*/
	TSpatialStatus::ESpatialStatus	fStatus;				/*!<point's spatial status*/

	//ClassDef(TPointFormat, 1)

};

/*@}*/


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
inline int TPointFormat::getNameWidth() const{ return fNameWidth;}

inline int TPointFormat::getCoordWidth() const{ return fCoordWidth;}

inline int TPointFormat::getCoordErrorWidth() const{ return fCoordErrorWidth;}

inline bool TPointFormat::getShowSign() const{ return fShowSign; }

inline TSpatialStatus::ESpatialStatus TPointFormat::getSpatialStatus() const{ return fStatus; }

#endif // SU_TPointFormat
