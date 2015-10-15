# Create a variable containing all .cpp files:
set(LGCProjectData_SOURCES
	InstrumentsCalcRecord.cpp
	LSCalcDataSet.cpp
	TLGCCalcParams.cpp
	TLGCCalcRecord.cpp
	TLGCDataSet.cpp
	TLGCObsSummary.cpp
	TLGCOutputOptions.cpp
	TLGCSimPointSummary.cpp
	TLGCSimResults.cpp
	TLSCalculation.cpp
	TLSConstraintIdentifier.cpp
	TLSInputMatricesFiller.cpp
	TLSMatricesAdapter.cpp
	TLSResultsMatricesExtractor.cpp
	TPointNamePair.cpp
	QuantileFunctions.cpp
)

# Create a variable containing all .h files:
set(LGCProjectData_HEADERS
	${LGC_INCLUDE_PATH}/InstrumentsCalcRecord.h 	
	${LGC_INCLUDE_PATH}/LSCalcDataSet.h
	${LGC_INCLUDE_PATH}/TLGCCalcParams.h
	${LGC_INCLUDE_PATH}/TLGCCalcRecord.h
	${LGC_INCLUDE_PATH}/TLGCDataSet.h
	${LGC_INCLUDE_PATH}/TLGCObsSummary.h
	${LGC_INCLUDE_PATH}/TLGCOutputOptions.h
	${LGC_INCLUDE_PATH}/TLGCSimPointSummary.h
	${LGC_INCLUDE_PATH}/TLGCSimResults.h
	${LGC_INCLUDE_PATH}/TLSCalculation.h
	${LGC_INCLUDE_PATH}/TLSConstraintIdentifier.h
	${LGC_INCLUDE_PATH}/TLSMatricesAdapter.h
	${LGC_INCLUDE_PATH}/TLSInputMatricesFiller.h
	${LGC_INCLUDE_PATH}/TLSResultsMatricesExtractor.h
	${LGC_INCLUDE_PATH}/TPointNamePair.h
	${LGC_INCLUDE_PATH}/QuantileFunctions.h
)

# Create a static library from sources:
add_library(LGCProjectData STATIC ${LGCProjectData_SOURCES} ${LGCProjectData_HEADERS})