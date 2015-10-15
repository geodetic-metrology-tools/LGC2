# Create a variable containing all .cpp files:
set(ObservationSet_SOURCES
	ObservationSet.cpp
	TObservationMaker.cpp
	TWorkingPointNamePairs.cpp
)

# Create a variable containing all .h files:
set(ObservationSet_HEADERS 	
	${LGC_INCLUDE_PATH}/ObservationSet.h
	${LGC_INCLUDE_PATH}/TObservationMaker.h
	${LGC_INCLUDE_PATH}/TWorkingObservation.h
	${LGC_INCLUDE_PATH}/TWorkingPointNamePairs.h
)

# Create a static library from sources:
add_library(ObservationSet STATIC ${ObservationSet_SOURCES} ${ObservationSet_HEADERS})