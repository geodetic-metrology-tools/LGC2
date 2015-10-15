// TSpatialLineName.h : header file
/*!	  
	Class modelising a line	

	Copyright 2003, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////


#ifndef SU_SPATIAL_LINE_NAME
#define SU_SPATIAL_LINE_NAME

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////
//forward declarations and includes

#include <list>
#include <sstream>
//using namespace std;

#include "TAngle.h"
#include "TSpatialPointName.h"
/////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/


class TSpatialLineName //: public
{

public:



	/*!@name Constructors / Destructor*/
	//@{
		/*!Default Constructors */
		TSpatialLineName();
		/*!Constructors of a vertical line
		\param firstEndName the name of the line's first end point*/
		TSpatialLineName(const TSpatialPointName firstEndName);
		/*!Constructors line
		\param firstEndName the name of the line's first end point
		<param angle the line's orientation*/
		TSpatialLineName(const TSpatialPointName firstEndName, const TSpatialPointName secondEndName);
		//!Copy constructor
		TSpatialLineName(const TSpatialLineName& source);
		//!Destructor
		~TSpatialLineName();
	//@}

	/*!@member function*/
	//@{
		//!Equality operator
		virtual bool				operator==(const TSpatialLineName& right) const;
		//!operator =
		virtual void				operator=(const TSpatialLineName& right);
		//!Less than operator
		bool						operator<( const TSpatialLineName & ) const;


		/*!\return true if the line is define with the new arguments*/
		virtual bool				ifNotDoneDefineLine(const TSpatialPointName firstPointName);
		/*!\return true if the line is define with the new arguments*/
		virtual bool				ifNotDoneDefineLine(const TSpatialPointName firstPointName, const TSpatialPointName secondEndName);

		/*!\return the name of the line's first end point*/
		virtual	TSpatialPointName	getFirstEndName() const;
		/*!\rreturn the name of the line's second end point*/
		virtual	TSpatialPointName	getSecondEndName() const;
		/*!\return the line's name*/
		virtual std::string				getName() const;

		/*!build the name (used only for TWorkingLines
		if the line which is added to the list is not defined)*/
		virtual void					buildErrorName();
	//@}


private:

	/*!@name Member function*/
	//@{
	/*!\return true if the line is define*/
		bool								isDefine() const;
	//@}


	TSpatialPointName			fFirstEndName;
	TSpatialPointName			fSecondEndName;
	std::string					fName;

};
/*@}*/



#endif

