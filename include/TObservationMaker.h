//  TObservationMaker.h
//
/*! A spatial point data set for geodetic transformation programs.  
  
    Patterns:
  
   
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_TOBSERVATION_MAKER
#define SU_TOBSERVATION_MAKER


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations

#include "TLGCDataSet.h"
#include "ObservationSet.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup lgc

@{*/


//Class definition
class  TObservationMaker  // : public   
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//!Default Constructor 
		TObservationMaker();

		//!Copy Constructor 
		TObservationMaker(const  TObservationMaker& original);

		//!Destructor
		virtual  ~TObservationMaker();
	//@}


	/*!@name Member Functions */
	//@{
		void	operator = (const TObservationMaker& copy);
		
		string	getError() const;
		
		bool	processData(TLGCDataSet& ds, ObservationSet& obs);

		void	processTheoStations(const TLGCDataSet& ds, ObservationSet& obs);

		void	processDistStations(TLGCDataSet& ds, ObservationSet& obs);

		void	processLevelStations(const TLGCDataSet& ds, ObservationSet& obs);

		void	processConstraints(const TLGCDataSet& ds, ObservationSet& obs);

		void	setError(string);
	//@}


private:

		void	processHorAng(TheodStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		void	processZenithDist(TheodStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		void	processSpatialDist(DistStIterator, TLGCDataSet& ds, ObservationSet& obs);

		void	processHorizontalDist(DistStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		void	processVerticalDist(LevelStConstIter, const TLGCDataSet& ds, ObservationSet& obs, bool isDLEV);

		void	processOffsetToVerLine(DistStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		void	processOffsetToSpaLine(DistStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		void	processOffsetToVerPlane(DistStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		void	processOffsetToTheoPlane(DistStIterator, TLGCDataSet& ds, ObservationSet& obs);

		void	processGyroOrie(TheodStConstIter, const TLGCDataSet& ds, ObservationSet& obs);

		TheodStIterator		getAssociatedTheoStToSpaDist(DistStIterator distStIt, TLGCDataSet& ds);

		TheodStIterator		getAssociatedTheoStToOffset(DistStIterator distStIt, PointConstIter theo, TLGCDataSet& ds, ObservationSet& obs);

	/*!name Private Attributs*/
	//@{
		/*!\return error's message*/
		string					fError;
		TheodStIterator			fLastTheoIt;

		int obsCount;
	//@}

	//ClassDef(TObservationMaker, 1)
};

/*@}*/

#endif // SU_TOBSERVATION_MAKER
