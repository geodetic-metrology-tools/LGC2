# Create a variable containing all .cpp files:
set(LSCalcObservationSet_SOURCES
	TLSCalcObservationMaker.cpp
	TLSCalcWorkingRelativeErrors.cpp
	TWorkingPointNamePairs.cpp	
)

# Create a variable containing all .h files:
set(LSCalcObservationSet_HEADERS 	
	${LGC_INCLUDE_PATH}/TLSCalcObservationMaker.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingDistanceObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingRelativeErrors.h
	${LGC_INCLUDE_PATH}/TWorkingPointNamePairs.h
)

# Create a static library from sources:
add_library(LSCalcObservationSet STATIC ${LSCalcObservationSet_SOURCES} ${LSCalcObservationSet_HEADERS})