/*!
	TDist.h : definitions of distance functions taking TLength
	objects as argument

	Patterns:
	
	Copyright 2002 CERN EST/SU. All rights reserved
*/

#ifndef SU_DIST_FCTS
#define SU_DIST_FCTS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "TLength.h"

/*! \name Distance Fonction*/
//@{
/*! computation of the distance from 2D coordinates
	\param TLength x1, coordinate  x of the first point
	\param TLength y1, coordinate  y of the first point
	\param TLength x2, coordinate  x of the second point
	\param TLength y2, coordinate  y of the second point
	\return Distance as TReal
	*/
	/*!\addtogroup spatialobjects
	@{*/
	TReal	dist(TLength x1, TLength y1, TLength x2, TLength y2);
	/*@}*/

/*! computation of the distance from 2D coordinates
	\param TReal x1, coordinate  x of the first point
	\param TReal y1, coordinate  y of the first point
	\param TReal x2, coordinate  x of the second point
	\param TReal y2, coordinate  y of the second point
	\return Distance as TReal
	*/
	/*!\addtogroup spatialobjects
	@{*/
	TReal	dist(TReal x1, TReal y1, TReal x2, TReal y2);
	/*@}*/



/*! computation of the distance from 3D coordinates
	\param TLength x1, coordinate  x of the first point
	\param TLength y1, coordinate  y of the first point
	\param TLength z1, coordinate  z of the first point
	\param TLength x2, coordinate  x of the second point
	\param TLength y2, coordinate  y of the second point
	\param TLength z2, coordinate  z of the second point
	\return Distance as TReal
	*/
	/*!\addtogroup spatialobjects
	@{*/
	TReal	dist3D(TLength x1, TLength y1, TLength z1, TLength x2, TLength y2, TLength z2);
	/*@}*/

/*! computation of the distance from 3D coordinates
	\param TReal x1, coordinate  x of the first point
	\param TReal y1, coordinate  y of the first point
	\param TReal z1, coordinate  z of the first point
	\param TReal x2, coordinate  x of the second point
	\param TReal y2, coordinate  y of the second point
	\param TReal z2, coordinate  z of the second point
	\return Distance as TReal
	*/
	/*!\addtogroup spatialobjects
	@{*/
	TReal	dist3D(TReal x1, TReal y1, TReal z1, TReal x2, TReal y2, TReal z2);
	/*@}*/


//@}



#endif
