// TCoordSysFactory.h
//
/*! Singleton class for use of coordinate systems

	Patterns:

	Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_T_COORDSYS_FACTORY
#define SU_T_COORDSYS_FACTORY


#if _MSC_VER >= 1000
#pragma once
#pragma warning (disable:4786)
#endif


////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////
//
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stddef.h>
//using namespace std;

class TVCoordinateSystem;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup MathematicalConcepts
	@{*/

//! Singleton class: produce one unique instance listing coordinate systems

class TCoordSysFactory
{
public:
		
		//!@name Constants
		//@{
			/*! Enumerator for coordinate systems implemented in the factory */
			enum  ECoordSys 
            {
                k3DCartesian = 1,
                kGeodetic = 2,
                k2DPlusH = 4,
                k2DCartesian = 8,
                kGeodeticSphere = 16
            };
		//@}
		
		//!@name Member Functions		
			/*!return the unique object of TCoordSysFactory */
			static TCoordSysFactory* getCoordSysFactoryPtr();

			/*! Return a pointer to the coordinate system asked for 
			\param CoordSysId an element of the existing coordinate system enumeration */
			TVCoordinateSystem* getCoordSys(const ECoordSys CoordSysId);

			void deleteCoordSysFactory();
		//@}

protected:
	
	//!@name Constructor and Destructor
	//@{
		/*! Default Constructor */
		TCoordSysFactory();

		/*! Destructor */
		virtual ~TCoordSysFactory();
	//@}

private:

		void	init();


private:

	static TCoordSysFactory*			fCoordSysFactoryPtr; /*!< static member that contains a pointer to the unique instance of TCoordSysFactory */
    std::vector<TVCoordinateSystem*>	fCoordSysList; /*!< vector of pointers to coordinate systems*/

};
/*@}*/

#endif
