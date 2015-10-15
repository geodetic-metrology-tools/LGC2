// Define the variables necessary for the export and import of DLL functions

/*
#ifdef SU_MAKEDLL
   #define  SU_DECLSPEC  __declspec(dllexport)
#elif SU_USEDLL
   #define  SU_DECLSPEC  __declspec(dllimport)
#else
   #define 	SU_DECLSPEC
#endif*/


//#ifdef SU_MAKEDLL
   #define  SU_DECLSPEC  __declspec(dllexport)
//#else
  // #define  SU_DECLSPEC  __declspec(dllimport)
//#endif


#define DLLAPI __stdcall


//Doxygen documentation: modules definition

/*!	\defgroup surveylib SurveyLib
	\brief documentation of the survey library
*/

/*!	\defgroup spatialmeasurements SpatialMeasurements
	\ingroup surveylib
	\brief Module SpatialMeasurements: group all the methods concerning measurements and
	instruments management. The module is used to store the data to be used in calculations
*/

/*!	\defgroup spatialobjects SpatialObjects
	\ingroup surveylib
	\brief Module SpatialObjects: group all the classes defining reference frames and coordinate
	systems and the corresponding transformations. It also defines the basic mathematical
	concepts used for algebric calculations.
	DOCUMENTATION STILL UNDER CONSTRUCTION...
*/

/*!	\defgroup lsalgorithms LSAlgorithms
	\ingroup surveylib
	\brief Documentation under construction
*/

/*! \defgroup projectframework ProjectFramework
	\ingroup surveylib
	\brief Documentation under construction
*/



