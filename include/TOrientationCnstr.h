// TOrientationCnstr.h
//
/*! 
	Class for a radial constraint 

	Patterns:

	Copyright 2003 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TOrientationCnstr
#define SU_TOrientationCnstr


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
class  TOrientationCnstr //: public TObject  
{
public:
	//constants

	/*!@name Constructors and Destructors */
	//@{
		/*! Default Constructor */ 
		TOrientationCnstr();

		//! Constructor
		/*!@param cPtName the name of the constrained point
		@param cWidth the width of the radial constraint
		@param cBearing the bearing of the radial constraint*/
		TOrientationCnstr(int obsID, TSpatialPointName refPtName, TAngle cBearing);

		/*! Copy Constructor */
		TOrientationCnstr(const  TOrientationCnstr&);

		/*! Destructor */
		virtual  ~TOrientationCnstr();
	//@}


	/*!@name Overloaded operators */
	//@{
		/*! Copy Assignment Operator */
		TOrientationCnstr& operator=( const TOrientationCnstr& );
		/*! Less than operator */
		bool operator<( const TOrientationCnstr& ) const;
		/*! Equality operator */
		bool operator==( const TOrientationCnstr& ) const;

	//@}

	/*!@name Access methods*/
	//@{
		/*!Gets the constraint's width */
		TAngle							getSigma() const;
		/*!Gets the constraint's bearing */
		TAngle							getCnstrBearing() const;
		/*!Gets the constraint's point name*/
		TSpatialPointName				getPoint() const;
		/*!Gets the constraint's name */
		string							getName() const;

	//@}

	/*!@name setting methods*/
	//@{
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

	TSpatialPointName				fRefPoint;/*!<constraint's point name*/
	string							fName; /*!< name of the constrained point */
	TAngle							fConstraintBearing; /*!< bearing of the radial constraint */

	int								fIdentifier; /*< identifier from geode */
	string							fComment; /*< comment from geode*/
	string							fHeaderComment; /*< head comment line (starting with % in input file)*/

	int observationID;

	//ClassDef(TOrientationCnstr, 1)
};
/*@}*/

	/*!@name Typedefs*/
	//@{
		/*!Type of the container storing the radial offset constraints */
		typedef list< TOrientationCnstr > OrieCnstrContainer;
		/*!Type of the iterators pointing to radial offset constraints */
		typedef OrieCnstrContainer::iterator OrieCnstrIterator;
		/*! Type of the constant iterators pointing to radial offset constraints */
		typedef OrieCnstrContainer::const_iterator OrieCnstrConstIter;
	//@}

#endif // SU_TOrientationCnstr

