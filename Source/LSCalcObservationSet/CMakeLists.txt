# Create a variable containing all .cpp files:
set(LSCalcObservationSet_SOURCES
	TLSCalcObservationMaker.cpp
	TLSCalcWorkingRelativeErrors.cpp
	TLSCalcWorkingSpaDistObs.cpp
	TLSCalcWorkingZenDistObs.cpp
	
)

# Create a variable containing all .h files:
set(LSCalcObservationSet_HEADERS 	
	${LGC_INCLUDE_PATH}/TLSCalcObservationMaker.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingAngleObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingDistanceObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingRelativeErrors.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingSpaDistObs.h
	${LGC_INCLUDE_PATH}/TLSCalcWorkingZenDistObs.h
)

# Create a static library from sources:
add_library(LSCalcObservationSet STATIC ${LSCalcObservationSet_SOURCES} ${LSCalcObservationSet_HEADERS})
