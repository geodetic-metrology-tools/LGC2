// TSpatialPointName.h
/*! 
Breaks the name tag down into three components, a name, a point, and a class.
The name represents the points name.
the point allows us to distinguish closely related points which otherwise
have the same name.
The class represents the type of spatial point, or the element on which it
is placed.

Copyright 1999,2000, CERN, EST/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////////

#ifndef SU_SPATIAL_POINT_NAME
#define SU_SPATIAL_POINT_NAME

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <string>
//using namespace std;

/*!\ingroup spatialobjects
	@{*/
//! Class for a spatial point name tag.
class	TSpatialPointName// : public TObject
{

public:

	/*!@name Typedefs */
	//@{
		//! Enumerator for naming convention
		enum ENameType{kName, kDBName};
	//@}


	/*!@name Constructors/Destructor*/
	//@{
		//!Default constructor
		TSpatialPointName();

		//!Constructor
		/*!@param name a string that will be assigned to the name component of this object*/
		explicit  TSpatialPointName( const std::string& name );

		//!Constructor
		/*!@param name the Name component of this object
		@param element the Element component of this object
		@param classifier the Classifier component of this object*/
		TSpatialPointName( const std::string& name, const std::string& classifier, const std::string& element );

		//! Copy constructor
		TSpatialPointName( const TSpatialPointName& );

		//! Destructor
		virtual  ~TSpatialPointName();
	//@}

	//!Copy assignement operator
	TSpatialPointName& operator=(const TSpatialPointName&);

	/*!@name Overloaded operators*/
	//@{
		//!Equality operator
		bool  operator==( const TSpatialPointName & ) const;//compare equal

		//!Less than operator
		bool  operator<( const TSpatialPointName & ) const;//compare less than

		//!Sets the name component
		/*!@param name string assigned to the Name component of this object*/
		void	setName( const std::string& name );

		//!Sets the element component
		/*!@param elt string assigned to the Element component of this object*/
		void	setElement( const std::string& elt) {fElement = elt; /*fMultiPart = true;*/ return;}

		//!Sets the classifier component
		/*!@param clsfr string assigned to the Classifier component of this object*/
		void	setClassifier( const std::string& c) {fClassification = c; /*fMultiPart = true;*/ return;}//set the classe of the point

		//!Sets the name type
		/*!@param nameType enum corresponding to the naming convention used*/
		static void	setNamingConvention( const ENameType type ) { fNamingConvention = type; return;}
		
		//!Set's this object's components
		/*!@param name the Name component of this object
		@param element the Element component of this object
		@param classifier the Classifier component of this object*/
		//void	setName( const string& name, const string& classifier, const string& element );
		/*!Sets the point's Id (for file from geode)
		\param id the point's id as an integer*/
		virtual  void  setId( const long int& id) {fPointId = id; return;};

		/*!Sets the point's comment (for file from geode)
		\param id the point's comment as a string*/
		virtual  void  setComment( const std::string& id) {fPointComment = id; return;};
	
		/*!@return the Name component as a string*/
		std::string	getName() const;

		/*!@return the Element component as a string*/
		std::string	getElement() const {return fElement;}//return the type of the point

		/*!@return the Classifier component as a string*/
		std::string	getClassifier() const {return fClassification;}//return the classe of the point

		/*!Returns the point's Id */
		virtual  long int  getId()  const {return fPointId;};

		/*!Returns the point's comment */
		virtual  std::string  getComment()  const {return fPointComment;};

		//!Returns the name type*/
		ENameType	getNamingConvention() const {return fNamingConvention;};
	//@}


private:
	
	std::string				fName;
	std::string				fElement;
	std::string				fClassification;

	long int			fPointId; /*!< identifier of the point */
	std::string				fPointComment; /*!< comments on the point */

	static ENameType	fNamingConvention; /*!< indicate if the point is named with DB's convention or not */
	//bool			fMultiPart;     // indicates that the point name has more than the name component
	

	//ClassDef(TSpatialPointName, 1)
};
#endif
