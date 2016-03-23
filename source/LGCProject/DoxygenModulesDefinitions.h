/*!
   \file
   \ingroup LGCProject
   Definitions of the Doxygen groups.
*/


/** \defgroup LGC2
 
  \brief A major update of the LGC (C++ version 1), provides extended functionalities required for the HIE-Isolde project and more. 
 
   The new features are documented in the EDMS document number: 1465539.

   Main shifts: 
	- Complex (adjustable) objects introduced (planes, lines, transformations). 
	- Mathematical observations models described in terms of these complex objects and therefore have been reformulated, new measurement types introduced.
	- Introduced camera instrument and camera measurements.
	- Hierarchy of local frames has been added.
	- New input file format.
 */

/**
	\defgroup ConstantsDefaults Constants and Defaults
	\ingroup LGC2

	\brief Summary of all constants used within the project. Taken from Defaults.h and GeodeticConstants.h header files.
*/

/**
	\defgroup LGCProject
	\ingroup LGC2

	\brief Responsible for the execution of the application.

	The TLGCApp class, contains the execution method and manages the main processes: reading, calculation and writing. It also 
	mediates the communication with the user and keeps global information about the program, such as the version or copyright. 

	Apart from this class, header files, including Doxygen description of all the projects or global application definitions
	shared among other classes are kept.
*/

/**
	\defgroup LGCProjectData
	\ingroup LGC2

	\brief Groups classes which are used to store the read data or are linked to the main TLGCData class.

	The TLGCData class contains all the information needed for the calculation and all the other classes
	within this project are part of this class. 

	The tree.h includes an external STL-like templated tree implementation. Instrument data are stored in the TInstrumentData class, 
	project configuration in the TLGCConfig and TTreeEntry represents a one node in the tree of local frames.

	\note TLGCAngle shall be moved to the SurveyLib, TLSConstraintIdentifier probably moved to the LGCCalculation.
*/

/**
	\defgroup LGCCalculation
	\ingroup LGC2

	\brief Classes which treat the various calculation options of the program.

	Responsible for the whole calculation process, manage the filling of the input matrices, 
	running the LS iteration, extracte the results, calculate statistics taking into account different 
	calculation options (NORMAL, SIMU, LIBR, ALLFIXED).

*/

/** \defgroup AdjustableObjects
    \ingroup LGC2

	\brief Adjustable objects within the module: point, angle, scalar, plane, line and transformation.

	This project includes classes used in the adjustment process, representing objects which components are to be determined. 
	These classes store the provisional and estimated values of each object, enabling to set up the corrections and holding 
	information about the variability of its components, indices of the unknown values in the least squares design matrices, 
	estimated precision or covariances. New description of observation models require, apart from the fundamental 
	objects, also more general ones. Because of that, following adjustable objects have been introduced: point, angle, scalar, plane, line and transformation.

	A collection of these objects is also part of this package (TAdjustableObjectCollection) as well as a common interface (TVAdjustableObject).
 */

/** \defgroup LocalTransformations 
   \ingroup LGC2
   \brief Groups everything what is needed for a local reference frames transformations and partial derivatives of these transformations.

   The module includes the general Helmert transformation (TTransformation) and the particular ones for the step up and down 
   (TDirectTransformation, TInverseTransformation) in the tree.

   - TransformParameters keeps the transformation parameters.
   - TDerivativeTransformation is a class representing partial derivatives of a transformation.
   - TLOR2LOR Transformations between frames which are not directly linked as well as partial derivatives of these transformations.
 */

/** \defgroup ContributionsGenerators
    \ingroup LGC2
	 \brief This module is responsible for calculation of contributions for every observation type.
	 
	 Classes in this project are responsible for calculation of the partial derivatives, observation values, 
	 standard deviations or misclosure vectors for all the observations treated within the LGC. 

	 These contributions are inserted into the input matrices in each iteration of the least square process, which
	 is done in the TLSInputMatricesFiller class.
 
 	 Methods for returning only the calculation measurement values are implemented in order to be used in the simulation, where nothing else is needed for
	 computing the 'measured values'.

	 In the file ContributionStructures.h, the structures including all what is needed for every particular observation type are defined.
 */


/** \defgroup LGCObjectWriters
   \ingroup LGC2
   \brief Classes responsible for writing LGC-specific objects into a file.

   They enable to write out FRAME, stations: TTSTN, TCAM, TSCALE, TEDM or TLEVEL, points and measurements.
   Points are written out inside the FRAME object and measurement inside the specific station if not defined standalone.

   These classes are used inside all the LGC output file writers and use writing functionalities provided by the 
   StreamFormatters and Converters SurveyLib projects.
 */

/** \defgroup Measurements 
   \ingroup LGC2
   \brief This module groups classes related to measurements, i.e. stations, actual measurements and related statistics.
   
   Documentation for each measurement type can be found in the Mathematical Observation File, EDMS number: 1465539.

   There is an abstract measurement class (TAMeas), from which an abstract scalar (TAScalarMeas) and abstract vector (TAVectorMeas) classes inherit. 
   All other concrete measurement classes inherit from one of these two later mentioned. TDVER is standalone, since it does not have instrument nor target.

   The module includes stations (TCAM, TEDM, TLEVEL, TTSTN), which perform these measurements. They include the station definition and vectors of 
   measurements which are applicable to these stations.

   TLGCObsSummary class includes general summary for each observation type and can be acquired from the stations.

   Some measurements does not belong to any of these stations (in the input file, there is no station definition) and structures representing round of measurements of these observations are defined (TECHOROM, TORIEROM).

   Finally, there is TMeasurements class, which represents all measurements made in any node of the tree and therefore consists of vector 
   of stations and round of measurements, which bundle all possible measurements together. 
  */


/** \defgroup Readers 
   \ingroup LGC2
   \brief Module for reading the input file and storing the information into the LGC objects, based on keyword handling.
 */

/** \defgroup MeasurementReaders 
   \ingroup Readers
   \brief Includes classes for reading all the measurements related keywords.
 */

/** \defgroup OptionReaders 
   \ingroup Readers
   \brief Includes classes for reading all the option keywords.
 */

/** \defgroup InstrumentReaders 
   \ingroup Readers
   \brief Includes classes for reading all the instrument keywords.
 */

/** \defgroup AdjObjectsReader 
   \ingroup Readers
   \brief Includes classes for reading the adjustable objects.
 */

/** \defgroup InputFileReader 
   \ingroup Readers
   \brief Classes responsible for the reading process and abstract definition of keyword.
 */


 /**
	\defgroup LSCalculation
	\ingroup LGC2
	\brief Classes responsible for the LS process in LGC2. 
	- TLSInputMatricesFiller is responsible for filling contribution into the input LS matrices.
	- TLSResultMatricesExtractor is responsible for extracting values from the LS result matrices and updating respective adjustable objects or measurements.
	- TLSAlgorithm is responsible for the LS iteration process and finding the solution.
	- TLAllfixed calculated additionnal parameter after the LS process.
	- TLSSimulation allows to run N LS calculation with simulated data.
*/



/**
	\defgroup SurveyLib
	\ingroup LGC2
	\brief Projects which were part of the SurveyLib and are used in LGC2. The often have been modified, thus the changes need to be pushed back to the SurveyLib.
*/

/** \defgroup CalcElements 
   \ingroup SurveyLib
   \brief Algorithms used in the adjustment process, in particular the Least Squares.
 */

 /** \defgroup RefFrameTransformations 
   \ingroup SurveyLib
   \brief Includes classes for geodetic reference frames transfansformations.

   New classes added for the LGC2, only these are included in this documentation of the module since the others are not actually used, but also do not
   contain any modifications.
 */

/** \defgroup Converters 
   \ingroup SurveyLib
   \brief Provides way to convert and output objects.
 */

 /** \defgroup StreamFormatters 
   \ingroup SurveyLib
   \brief Used for formatting the output files.
 */

  /** \defgroup FiltersAndFormats 
   \ingroup SurveyLib
   \brief Used for formatting the output files, filters for different units.
 */

 /** \defgroup MathematicalConcepts 
   \ingroup SurveyLib
   \brief Definitions of mathematical objects.

   Some modifications made, especially in the TPositionVector and the TFreeVector.
 */

 /** \defgroup ShortestPath 
   \ingroup SurveyLib
   No class is used except of the TRefSystemFactory, which is used for global definition of geoids, ellispoids and other objects.
 */

  /** \defgroup SpatialObjectsAndSystems 
   \ingroup SurveyLib
   \brief NOT USED, just because the StreamFormatters involve it and some other dependencies. Groupes (SpatialObj and SpatialSystems systems).
 */

   /** \defgroup ReferenceSurfaces 
   \ingroup SurveyLib
   \brief Basically not used, some of them initialized in the TRefSystemFactory. Almost everything can be deleted, left just for the dependecies reasons. Classes not attached here, but see the LGC2 ReferenceSurfaces project.
 */

   /** \defgroup ProjectData 
   \ingroup SurveyLib
   \brief Only the TADataSet class is used in a tricky way, only to provide it to the StreamFormatters because they expect it. FIND A BETTER SOLUTION and delete this
   project. Classes not included here but see the ProjectData project in LGC2.
 */

 /** \defgroup ReferenceSurfaces 
   \ingroup SurveyLib
   \brief Basically not used, some of them initialized in the TRefSystemFactory. Almost everything can be deleted, left just for the dependecies reasons. Classes not attached here, but see the LGC2 ReferenceSurfaces project.
 */