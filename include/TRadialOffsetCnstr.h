// TRadialOffsetCnstr.h
//
/*! 
	Class for a radial constraint 

	Patterns:

	Copyright 2003 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TRadialOffsetCnstr
#define SU_TRadialOffsetCnstr


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//#include  <**classname**>
//using namespace std;
//
#include <list>

#include "TAngle.h"
#include "TSpatialPointName.h"
#include "TLength.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a radial constraint
class  TRadialOffsetCnstr //: public TObject  
{
public:
	//constants

	/*!@name Constructors and Destructors */
	//@{
		/*! Default Constructor */ 
		TRadialOffsetCnstr();

		//! Constructor
		/*!@param cPtName the name of the constrained point
		@param cWidth the width of the radial constraint
		@param cBearing the bearing of the radial constraint*/
		TRadialOffsetCnstr(int obsID, TSpatialPointName cPtName, TLength cWidth, TAngle cBearing);

		/*! Copy Constructor */
		TRadialOffsetCnstr(const  TRadialOffsetCnstr&);

		/*! Destructor */
		virtual  ~TRadialOffsetCnstr();
	//@}


	/*!@name Overloaded operators */
	//@{
		/*! Copy Assignment Operator */
		TRadialOffsetCnstr& operator=( const TRadialOffsetCnstr& );
		/*! Less than operator */
		bool operator<( const TRadialOffsetCnstr& ) const;
		/*! Equality operator */
		bool operator==( const TRadialOffsetCnstr& ) const;

	//@}

	/*!@name Access methods*/
	//@{
		/*!Gets the constraint's width */
		TLength							getCnstrSigma() const;
		/*!Gets the constraint's bearing */
		TAngle							getCnstrBearing() const;
		/*!Gets the constraint's point name*/
		TSpatialPointName				getPoint() const;
		/*!Gets the constraint's name */
		string							getName() const;

	//@}

	/*!@name setting methods*/
	//@{
		/*!Sets the constraint's width if it isn't done yet
		\param cWidth the width to set */
		virtual void				ifNotDoneSetSigma(TLength cWidth);
		/*!Sets the constraint's bearing if it isn't done yet
		\param cBear the constraint's bearing to set */
		virtual void				ifNotDoneSetCnstrBearing(TAngle cBear);
		/*!Sets the constraint's name if it isn't done yet
		\param name the constraint's name to set */
		virtual void				ifNotDoneSetName(const string name);
	//@}

	/*!@name Comments methods*/
	//@{
		/*!@return the identifier of the constraint*/
		virtual int					getId() const { return fIdentifier;}
		/*!sets the identifier of the constraint
		@param id an int*/
		virtual void				setId(int id) { fIdentifier = id; return;}

		/*!@return the comment of the constraint*/
		virtual string				getComment() const { return fComment;}
		/*!sets the comment of the constraint
		\param com a string*/
		virtual void				setComment(string com) { fComment = com; return;}

		/*!@return the head comment of the constraint*/
		virtual string				getHeaderComment() const { return fHeaderComment;}
		/*!sets the head comment of the constraint
		\param com the comment line as a string*/
		virtual void				setHeaderComment(string com) { fHeaderComment = com; return;}

		int getObservationID() const { return observationID; }



private:

	TSpatialPointName				fPoint;/*!<constraint's point name*/
	string							fName; /*!< name of the constrained point */
	TLength							fConstraintWidth; /*!< width of the radial constraint */
	TAngle							fConstraintBearing; /*!< bearing of the radial constraint */
	int								fIdentifier; /*< identifier from geode */
	string							fComment; /*< comment from geode*/
	string							fHeaderComment; /*< head comment line (starting with % in input file)*/

	int observationID;

	//ClassDef(TRadialOffsetCnstr, 1)
};
/*@}*/

	/*!@name Typedefs*/
	//@{
		/*!Type of the container storing the radial offset constraints */
		typedef list< TRadialOffsetCnstr > RadOffCnstrContainer;
		/*!Type of the iterators pointing to radial offset constraints */
		typedef RadOffCnstrContainer::iterator RadOffCnstrIterator;
		/*! Type of the constant iterators pointing to radial offset constraints */
		typedef RadOffCnstrContainer::const_iterator RadOffCnstrConstIter;
	//@}

#endif // SU_TRadialOffsetCnstr

