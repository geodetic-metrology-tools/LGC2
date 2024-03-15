# Configure version when called from UI
# /!\ IMPORTANT : Modify it int ../CMakeLists.txt AS WELL !
set (LGC_VERSION_MAJOR "2")
set (LGC_VERSION_MINOR "7")
set (LGC_VERSION_PATCH "beta_8_CONSI_LIBR")

# Versions files
configure_file (
    ${CMAKE_SOURCE_DIR}/LGCVersion.in
    ${CMAKE_SOURCE_DIR}/Version.h
)

if(MSVC)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

# Add options to compile with GCC to get more opinions on the code
if(MINGW)
    message("    Using MinGW")
endif(MINGW)

# Define a variable that contains the path to LGC modules
set(LGCCore_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(LGCCore_INCLUDE_PATH ${LGCCore_MODULE_PATH}/headers)
set(SURVEYLIB_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../SurveyLib)

include_directories (
	SYSTEM
    ${SURVEYLIB_ROOT}/source/Tools/headers
	${SURVEYLIB_ROOT}/source/LSAlgorithms/headers/AdjustableObjects
	${SURVEYLIB_ROOT}/source/LSAlgorithms/headers/CalcElements
	${SURVEYLIB_ROOT}/source/LSAlgorithms/headers/CalcParams
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/Converters
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/FiltersAndFormats
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/ProjectData
	${SURVEYLIB_ROOT}/source/ProjectFramework/headers/StreamFormatters
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/AffineTransformations
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/MathematicalConcepts
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/ReferenceSurfaces
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/RefFrameTransformations
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/ShortestPath
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/SpatialMeasurments
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/SpatialObj
	${SURVEYLIB_ROOT}/source/SpatialObjects/headers/SpatialSystems
	${TUT_INCLUDE_PATH}
	${EIGEN_INCLUDE_PATH}
    ${LGCCore_INCLUDE_PATH}/LGCAdjustableObjects
	${LGCCore_INCLUDE_PATH}/LGCConstraintObjects
	${LGCCore_INCLUDE_PATH}/LSCalculation
	${LGCCore_INCLUDE_PATH}/Readers
	${LGCCore_INCLUDE_PATH}/ContributionsGenerators
	${LGCCore_INCLUDE_PATH}/Measurements
	${LGCCore_INCLUDE_PATH}/LocalTransformations
	${LGCCore_INCLUDE_PATH}/LGCProject
	${LGCCore_INCLUDE_PATH}/LGCProjectData
	${LGCCore_INCLUDE_PATH}/LGCCalculation
	${LGCCore_INCLUDE_PATH}/Writers
	${LGCCore_INCLUDE_PATH}/LGCObjectWriters
)

INCLUDE(${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake)

set(CORE_INCLUDE_PATH 	${LGCCore_INCLUDE_PATH})
set(CORE_SOURCES_PATH 	${LGCCore_MODULE_PATH}/sources)
					
					
# Create a variable containing all .h files:
set(Core_HEADERS 	
	${CORE_INCLUDE_PATH}/ContributionsGenerators/TContributionsGenerator.h
	${CORE_INCLUDE_PATH}/ContributionsGenerators/ContributionStructures.h
	${CORE_INCLUDE_PATH}/ContributionsGenerators/TPointTransformer.h
	${CORE_INCLUDE_PATH}/ContributionsGenerators/TObservationGenerator.h
	${CORE_INCLUDE_PATH}/ContributionsGenerators/TAllfixedParamGenerator.h
	${CORE_INCLUDE_PATH}/ContributionsGenerators/TLibrCnstrGenerator.h
	${CORE_INCLUDE_PATH}/LGCAdjustableObjects/LGCAdjustablePoint.h
	${CORE_INCLUDE_PATH}/LGCAdjustableObjects/LGCAdjustableLine.h
	${CORE_INCLUDE_PATH}/LGCAdjustableObjects/LGCAdjustablePlane.h
	${CORE_INCLUDE_PATH}/LGCAdjustableObjects/LGCAdjustableObjectCollection.h
	${CORE_INCLUDE_PATH}/LGCConstraintObjects/LGCFrameConstraintGroup.h
	${CORE_INCLUDE_PATH}/LGCCalculation/TLGCCalculation.h
	${CORE_INCLUDE_PATH}/LGCCalculation/TSimFrameSummary.h
	${CORE_INCLUDE_PATH}/LGCCalculation/TSimPointSummary.h
	${CORE_INCLUDE_PATH}/LGCCalculation/TVAbractAlgorithm.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TALGCObjectWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TTSTNWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TCAMWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TLEVELWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TEDMWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TFRAMEWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TSCALEWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TOtherMeasurementsWriter.h
	${CORE_INCLUDE_PATH}/LGCObjectWriters/TObservationWriter.h
	${CORE_INCLUDE_PATH}/LGCProject/TLGCApp.h
	${CORE_INCLUDE_PATH}/LGCProject/Global.h
	${CORE_INCLUDE_PATH}/LGCProject/Defaults.h
	${CORE_INCLUDE_PATH}/LGCProject/DoxygenModulesDefinitions.h
	${CORE_INCLUDE_PATH}/LGCProjectData/tree.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TTreeEntry.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TLGCStatistic.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TLGCData.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TLGCConfig.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TInstrumentData.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TDataAnalyzer.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TFileLogger.h
	${CORE_INCLUDE_PATH}/LGCProjectData/TLSCalcRelativeError.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TTransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TTransformParameters.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TDirectTransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TInverseTransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TDerivativeTransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TLOR2LOR.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TCCS2CGRFTransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TCGRF2LGTransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TILG2ILATransformation.h
	${CORE_INCLUDE_PATH}/LocalTransformations/TLA2MLATransformation.h
	${CORE_INCLUDE_PATH}/LSCalculation/TLSInputMatricesFiller.h
	${CORE_INCLUDE_PATH}/LSCalculation/TLSResultsMatricesExtractor.h
	${CORE_INCLUDE_PATH}/LSCalculation/TLSAlgorithm.h
	${CORE_INCLUDE_PATH}/LSCalculation/TLSSimulation.h
	${CORE_INCLUDE_PATH}/LSCalculation/TLSAllfixed.h
	${CORE_INCLUDE_PATH}/Measurements/TMeasurements.h
	${CORE_INCLUDE_PATH}/Measurements/TTSTN.h
	${CORE_INCLUDE_PATH}/Measurements/TAMeas.h
	${CORE_INCLUDE_PATH}/Measurements/TEDM.h
	${CORE_INCLUDE_PATH}/Measurements/TLEVEL.h
	${CORE_INCLUDE_PATH}/Measurements/TCAM.h
	${CORE_INCLUDE_PATH}/Measurements/TUVEC.h
	${CORE_INCLUDE_PATH}/Measurements/TUVD.h
	${CORE_INCLUDE_PATH}/Measurements/TLGCObsSummary.h
	${CORE_INCLUDE_PATH}/Measurements/MeasDef.h
	${CORE_INCLUDE_PATH}/Measurements/RoundOfMeasurements.h
	${CORE_INCLUDE_PATH}/Readers/AdjObjectsReader.h
	${CORE_INCLUDE_PATH}/Readers/InstrumentReaders.h
	${CORE_INCLUDE_PATH}/Readers/TReader.h
	${CORE_INCLUDE_PATH}/Readers/MeasurementReaders.h
	${CORE_INCLUDE_PATH}/Readers/OptionReaders.h
	${CORE_INCLUDE_PATH}/Readers/ReaderBase.h
	${CORE_INCLUDE_PATH}/Readers/KeywordRights.h
	${CORE_INCLUDE_PATH}/Readers/MeasurementReader_lgc1.h
	${CORE_INCLUDE_PATH}/Writers/TAFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TResultsFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TPunchFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TFautFileWriter.h
    ${CORE_INCLUDE_PATH}/Writers/TInputFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TSimulationOutputFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TSimFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TDefaFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TCovarFileWriter.h
	${CORE_INCLUDE_PATH}/Writers/TChabaFileWriter.h
)

# Create a variable containing all .cpp files:
set(Core_SOURCES
	${CORE_SOURCES_PATH}/ContributionsGenerators/TContributionsGenerator.cpp
	${CORE_SOURCES_PATH}/ContributionsGenerators/TPointTransformer.cpp
	${CORE_SOURCES_PATH}/ContributionsGenerators/TObservationGenerator.cpp
	${CORE_SOURCES_PATH}/ContributionsGenerators/TAllfixedParamGenerator.cpp
	${CORE_SOURCES_PATH}/ContributionsGenerators/TLibrCnstrGenerator.cpp
	${CORE_SOURCES_PATH}/LGCAdjustableObjects/LGCAdjustablePoint.cpp
	${CORE_SOURCES_PATH}/LGCAdjustableObjects/LGCAdjustableLine.cpp
	${CORE_SOURCES_PATH}/LGCAdjustableObjects/LGCAdjustablePlane.cpp
	${CORE_SOURCES_PATH}/LGCConstraintObjects/LGCFrameConstraintGroup.cpp
	${CORE_SOURCES_PATH}/LGCCalculation/TLGCCalculation.cpp
	${CORE_SOURCES_PATH}/LGCCalculation/TSimPointSummary.cpp
	${CORE_SOURCES_PATH}/LGCCalculation/TSimFrameSummary.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TALGCObjectWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TTSTNWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TCAMWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TLEVELWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TEDMWriter.cpp	
	${CORE_SOURCES_PATH}/LGCObjectWriters/TFRAMEWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TSCALEWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TOtherMeasurementsWriter.cpp
	${CORE_SOURCES_PATH}/LGCObjectWriters/TObservationWriter.cpp
	# ${CORE_SOURCES_PATH}/LGCProject/main.cpp
	${CORE_SOURCES_PATH}/LGCProject/TLGCApp.cpp
	${CORE_SOURCES_PATH}/LGCProjectData/TLGCStatistic.cpp
	${CORE_SOURCES_PATH}/LGCProjectData/TLGCData.cpp
	${CORE_SOURCES_PATH}/LGCProjectData/TDataAnalyzer.cpp
	${CORE_SOURCES_PATH}/LGCProjectData/TFileLogger.cpp
	${CORE_SOURCES_PATH}/LGCProjectData/TLSCalcRelativeError.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TTransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TDirectTransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TInverseTransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TDerivativeTransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TLOR2LOR.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TCCS2CGRFTransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TCGRF2LGTransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TILG2ILATransformation.cpp
	${CORE_SOURCES_PATH}/LocalTransformations/TLA2MLATransformation.cpp
	${CORE_SOURCES_PATH}/LSCalculation/TLSInputMatricesFiller.cpp
	${CORE_SOURCES_PATH}/LSCalculation/TLSResultsMatricesExtractor.cpp
	${CORE_SOURCES_PATH}/LSCalculation/TLSAlgorithm.cpp
	${CORE_SOURCES_PATH}/LSCalculation/TLSSimulation.cpp
	${CORE_SOURCES_PATH}/LSCalculation/TLSAllfixed.cpp
    ${CORE_SOURCES_PATH}/Measurements/TMeasurements.cpp
	${CORE_SOURCES_PATH}/Measurements/MeasDef.cpp
	${CORE_SOURCES_PATH}/Measurements/TLGCObsSummary.cpp
	${CORE_SOURCES_PATH}/Measurements/TTSTN.cpp
	${CORE_SOURCES_PATH}/Measurements/TCAM.cpp
	${CORE_SOURCES_PATH}/Measurements/TLEVEL.cpp
	${CORE_SOURCES_PATH}/Measurements/TEDM.cpp
	${CORE_SOURCES_PATH}/Measurements/RoundOfMeasurements.cpp
	${CORE_SOURCES_PATH}/Readers/TReader.cpp
	${CORE_SOURCES_PATH}/Readers/AdjObjectsReader.cpp
	${CORE_SOURCES_PATH}/Readers/InstrumentReaders.cpp
	${CORE_SOURCES_PATH}/Readers/MeasurementReaders.cpp
	${CORE_SOURCES_PATH}/Readers/OptionReaders.cpp
	${CORE_SOURCES_PATH}/Readers/MeasurementReader_lgc1.cpp
	${CORE_SOURCES_PATH}/Writers/TAFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TResultsFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TPunchFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TFautFileWriter.cpp
    ${CORE_SOURCES_PATH}/Writers/TInputFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TSimulationOutputFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TSimFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TDefaFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TCovarFileWriter.cpp
	${CORE_SOURCES_PATH}/Writers/TChabaFileWriter.cpp
)

# Define VS folders:
file(GLOB CONTRIBUTIONSGENERATORS_H ${CORE_INCLUDE_PATH}/ContributionsGenerators/*.*)
file(GLOB LGCADJOBJECT_H ${CORE_INCLUDE_PATH}/LGCAdjustableObjects/*.*)
file(GLOB LGCCALCULATION_H ${CORE_INCLUDE_PATH}/LGCCalculation/*.*)
file(GLOB LGCOBJECTWRITERS_H ${CORE_INCLUDE_PATH}/LGCObjectWriters/*.*)
file(GLOB LGCProject_H ${CORE_INCLUDE_PATH}/LGCProject/*.*)
file(GLOB LGCProjectData_H ${CORE_INCLUDE_PATH}/LGCProjectData/*.*)
file(GLOB LOCALTRANSFORMATIONS_H ${CORE_INCLUDE_PATH}/LocalTransformations/*.*)
file(GLOB LSCALCULATION_H ${CORE_INCLUDE_PATH}/LSCalculation/*.* )
file(GLOB MEASUREMENTS_H ${CORE_INCLUDE_PATH}/Measurements/*.*)
file(GLOB READERS_H ${CORE_INCLUDE_PATH}/Readers/*.*)
file(GLOB WRITERS_H ${CORE_INCLUDE_PATH}/Writers/*.* )

file(GLOB CONTRIBUTIONSGENERATORS_CPP ${CORE_SOURCES_PATH}/ContributionsGenerators/*.*)
file(GLOB LGCADJOBJECT_CPP ${CORE_SOURCES_PATH}/LGCAdjustableObjects/*.*)
file(GLOB LGCCALCULATION_CPP ${CORE_SOURCES_PATH}/LGCCalculation/*.*)
file(GLOB LGCOBJECTWRITERS_CPP ${CORE_SOURCES_PATH}/LGCObjectWriters/*.*)
file(GLOB LGCProject_CPP ${CORE_SOURCES_PATH}/LGCProject/*.*)
file(GLOB LGCProjectData_CPP ${CORE_SOURCES_PATH}/LGCProjectData/*.*)
file(GLOB LOCALTRANSFORMATIONS_CPP ${CORE_SOURCES_PATH}/LocalTransformations/*.*)
file(GLOB LSCALCULATION_CPP ${CORE_SOURCES_PATH}/LSCalculation/*.*)
file(GLOB MEASUREMENTS_CPP ${CORE_SOURCES_PATH}/Measurements/*.*)
file(GLOB READERS_CPP ${CORE_SOURCES_PATH}/Readers/*.*)
file(GLOB WRITERS_CPP ${CORE_SOURCES_PATH}/Writers/*.*)

source_group("ContributionsGenerators\\Headers" FILES ${CONTRIBUTIONSGENERATORS_H})
source_group("LGCAdjustableObjects\\Headers" FILES ${LGCADJOBJECT_H})
source_group("LGCCalculation\\Headers" FILES ${LGCCALCULATION_H})
source_group("LGCObjectWriters\\Headers" FILES ${LGCOBJECTWRITERS_H})
source_group("LGCProject\\Headers" FILES ${LGCProject_H})
source_group("LGCProjectData\\Headers" FILES ${LGCProjectData_H})
source_group("LocalTransformations\\Headers" FILES ${LOCALTRANSFORMATIONS_H})
source_group("LSCalculation\\Headers" FILES ${LSCALCULATION_H})
source_group("Measurements\\Headers" FILES ${MEASUREMENTS_H})
source_group("Readers\\Headers" FILES ${READERS_H})
source_group("Writers\\Headers" FILES ${WRITERS_H})

source_group("ContributionsGenerators\\Sources" FILES ${CONTRIBUTIONSGENERATORS_CPP})
source_group("LGCAdjustableObjects\\Sources" FILES ${LGCADJOBJECT_CPP})
source_group("LGCCalculation\\Sources" FILES ${LGCCALCULATION_CPP})
source_group("LGCObjectWriters\\Sources" FILES ${LGCOBJECTWRITERS_CPP})
source_group("LGCProject\\Sources" FILES ${LGCProject_CPP})
source_group("LGCProjectData\\Sources" FILES ${LGCProjectData_CPP})
source_group("LocalTransformations\\Sources" FILES ${LOCALTRANSFORMATIONS_CPP})
source_group("LSCalculation\\Sources" FILES ${LSCALCULATION_CPP})
source_group("Measurements\\Sources" FILES ${MEASUREMENTS_CPP})
source_group("Readers\\Sources" FILES ${READERS_CPP})
source_group("Writers\\Sources" FILES ${WRITERS_CPP})


# Create a static library from sources:
add_library(LGCCore STATIC 
    ${Core_HEADERS}
    ${Core_SOURCES}
)

target_link_libraries(LGCCore 
    LSAlgorithms
    ProjectFramework
    SpatialObjects
    Tools
)

