// TSpatialPointName
//
// Class for a spatial point name tag.
//
// breaks the name tag down into three components, a name, a point, and a class.
// The name represents the points name.
// The point allows us to distinguish closely related points which otherwise
//     have the same name.
// The class represents the type of spatial point, or the element on which it
//     is placed.
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////////

//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
// other forward declarations
#include	"TSpatialPointName.h"
////////////////////////////////////////////////////////////////



//ClassImp(TSpatialPointName)
TSpatialPointName::ENameType TSpatialPointName::fNamingConvention = kName;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


TSpatialPointName::TSpatialPointName()
	: fName(""), fClassification(""), fElement(""), /*fMultiPart(false),*/
	 fPointId(), fPointComment("") 
{//default constructor
}


TSpatialPointName::TSpatialPointName(  const std::string &name )
	: fName(name), fClassification(""), fElement(""), /*fMultiPart(false),*/ fPointId(), fPointComment("")
{//constructor of a point name with just the name
}


TSpatialPointName::TSpatialPointName(  const std::string &name, const std::string &classifier, const std::string &element )
	: fName(name), fClassification(classifier), fElement(element),  /*fMultiPart(true),*/ fPointId(), fPointComment("")
{//constructor of a point name with all three components
	fNamingConvention=kDBName;
}


TSpatialPointName::TSpatialPointName(const TSpatialPointName& name)
{// copy constructor
	*this = name;
}


TSpatialPointName::~TSpatialPointName()
{//destructor
}


///////////////////////////////////////////////////////////////////
// copy assignement operator
///////////////////////////////////////////////////////////////////

TSpatialPointName& TSpatialPointName::operator=(const TSpatialPointName& name)
{
	if (this != &name)
	{
		fName = name.fName;
		fClassification = name.fClassification;
		fElement = name.fElement;
		fPointId = name.fPointId;
		fPointComment = name.fPointComment;
		fNamingConvention = name.fNamingConvention;
		//fMultiPart = name.composite();
	}
	return *this;
}

///////////////////////////////////////////////////////////////////
// Member functions
///////////////////////////////////////////////////////////////////

/*void TSpatialPointName::setName( const string &name, const string &classifier, const string& element )
{//set the name, the classe and the type of the point
	fName = name;
	fClassification = classifier;
	fElement = element;
//	fMultiPart = true;
}*/


void	TSpatialPointName::setName( const std::string &name )
{
	if (fNamingConvention == kDBName)
	{
		//set the name, the classe and the type of a point as defined	for the database
		int i;

		//test the name length
		if (name.length() != 13) throw "point name conversion not possible";

		//cut 6 first caracteres to give the classe
		fClassification = (name.substr(0, 6));
		i = (fClassification.find_last_of('_', (fClassification.length())));
		fClassification = (fClassification.substr(0, i));
	
		//cut 6 next caracteres to give the name
		fElement  = (name.substr(6, 6));
		i = (fElement.find_first_not_of('_',0)); 
		fElement = (fElement.substr(i, (fElement.length())-i));
	
		//the last caractere gives the type
		if (name[12]=='_') fName="";
		else fName = (name[12]);
	}

	else
	{
		fName = name;
		fElement = "";
		fClassification = "";
	}
		
	return;
}
	
std::string	TSpatialPointName::getName() const
{

	if (fNamingConvention == kDBName)
	{
		// test classe & name length and replace void by _
		if (fClassification.length() > 6 || fElement.length() > 6 
			||  fName.length() > 1)
		throw "point name conversion not possible";

		std::string s(fClassification);
		if (fClassification.length() <6) s.resize(6,'_');
		if (fElement.length() <6) 
		{
			for (unsigned int i=0; i<(6-fElement.length());i++)
			s = (s+'_');
		}
		s+=fElement;

		//concatenate the name
		if (fName == "")
		{
			s+='_';
		}
		else
		{
			s+=fName;
		}
		return s;
	}
	else
		return fName;
}
		




///////////////////////////////////////////////////////////////////
// Overloaded == operator definition
///////////////////////////////////////////////////////////////////

bool TSpatialPointName::operator==( const TSpatialPointName &right ) const{

	if ( (fName==right.fName)  
		      &&  (fClassification==right.fClassification)
			  &&  (fElement==right.fElement) )
		return true;
	else return false;

}


///////////////////////////////////////////////////////////////////
// Overloaded < operator definition
///////////////////////////////////////////////////////////////////

bool TSpatialPointName::operator<( const TSpatialPointName &right ) const{

	bool answer=false;
	bool answerFound=false;

	//Name Point et Class ne sont pas concaténés pour la comparaison car si par ex. les
	//deux Name n'ont pas la même longueur des caractères de l'un vont être comparés 
	//avec des caractères qui sont de Point ou Class de l'autre  

	if (fName<right.fName){
		answer=true;
		answerFound=true;
	}
	if (fName>right.fName){
		answer=false;
		answerFound=true;
	}
	
	if (!answerFound){

		if (fClassification<right.fClassification){
			answer=true;
			answerFound=true;
		}
		if (fClassification>right.fClassification){
			answer=false;
			answerFound=true;
		}
	}

	if (!answerFound){

		if (fElement<right.fElement){
			answer=true;
			answerFound=true;
		}
		if (fElement>right.fElement){
			answer=false;
			answerFound=true;
		}
	}


	return answer;
}






