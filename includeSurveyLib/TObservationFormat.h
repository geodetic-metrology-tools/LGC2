// TObservationFormat.h
/*!
Class used by a text stream to store and get all necessary formatting information
for observations

Patterns:

Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TObservationFormat
#define SU_TObservationFormat


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
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup Converters
	\brief Class used by a text stream to store and get all necessary formatting information for observations.
@{*/

//Class definition
class  TObservationFormat 
{
public:

	/*!name Constant*/
	//@!{
	/*!Length Precision*/
	enum ELengthPrecision {kMetre, k100Millimetres, k10Millimetres, kMillimetre, k100Micrometres, k10Micrometres, kMicrometre, k100Nanometres};

	/*!AnglePrecision*/
	enum EAnglePrecision {kGons, k100Milligons, k10Milligons, kMilligons, k100Microgons, k10Microgons, kMicrogon, k100Nanogons};
	//@}


	/*!name Constructor / Destructor*/
	//@!{
	/*!Constructor*/
	TObservationFormat();

	/*!Constructor
		\param name Width
		\param obs Width
		\param length Precision
		\param angle Precision
		\param obs Residual Width
		\param length Residual Precision
		\param angle Residual precision
		\param show Sign (bool)
	*/
	TObservationFormat(int nameWidth, int obsWidth, ELengthPrecision lengthPrecision, EAnglePrecision anglePrecision,
			int obsResidualWidth, ELengthPrecision lengthResidualPrecision, EAnglePrecision angleResidualprecision, bool showSign);

	/*!Copy constructor*/
	TObservationFormat(const  TObservationFormat&);

	/*!Destructor*/
	virtual  ~TObservationFormat();
	//@}


	/*!name Public Member Function*/
	//@!{
	/*!Copy Assignment Operator
	\param observation Format
	*/
	TObservationFormat& operator=( const TObservationFormat& obsFormat);

	/*!\return the width reserved for the point name fields*/
	virtual int					getNameWidth() const;

	/*!\return the width reserved for the observation fields*/
	virtual int					getObsWidth() const;

	/*!\return the width reserved for the observation residuals fields*/
	virtual int					getObsResidualWidth() const;

	/*!\return the precision with wich the observed lengths will be displayed*/
	virtual ELengthPrecision	getLengthPrecision() const;

	/*!\return the precision with wich the observed angles will be displayed*/
	virtual EAnglePrecision		getAnglePrecision() const;

	/*!\return the precision with wich the lengths' residuals will be displayed*/
	virtual ELengthPrecision	getLengthResidualPrecision() const;

	/*!\return the precision with wich the angles' residuals will be displayed*/
	virtual EAnglePrecision		getAngleResidualPrecision() const;

	/*!\@return a true boolean if the number's positive sign is to be shown*/
	virtual bool				getShowSign() const;
	//@}


private:

	int					fNameWidth;					/*!<name width*/
	int					fObsWidth;					/*!<observation width*/
	int					fObsResidualWidth;			/*!<observation residual width*/
	ELengthPrecision	fLengthPrecision;			/*!<length precision*/
	EAnglePrecision		fAnglePrecision;			/*!<angle precision*/
	ELengthPrecision	fLengthResidualPrecision;	/*!<length residual precision*/
	EAnglePrecision		fAngleResidualPrecision;	/*!<angle residual precision*/
	bool				fShowSign;					/*!<show sign (bool)*/

	//ClassDef(TObservationFormat, 1)
};

/*@}*/


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////
inline int TObservationFormat::getNameWidth() const{ return fNameWidth;}

inline int TObservationFormat::getObsWidth() const{ return fObsWidth;}

inline int TObservationFormat::getObsResidualWidth() const{ return fObsResidualWidth;}

inline bool TObservationFormat::getShowSign() const{ return fShowSign; }


#endif // SU_TObservationFormat
