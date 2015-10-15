# Create a variable containing all .cpp files:
set(LSCalcObservations_SOURCES
	TALSCalcAngleObservation.cpp
	TALSCalcLengthObservation.cpp
	TALSCalcObservation.cpp
	TLSCalc2DOffsetObservation.cpp
	TLSCalcGyroOrientationObservation.cpp
	TLSCalcHorAngleObservation.cpp
	TLSCalcHorDistObservation.cpp
	TLSCalcOffsetToSpaLineObservation.cpp
	TLSCalcOffsetToTheoPlaneObservation.cpp
	TLSCalcOffsetToVerLineObservation.cpp
	TLSCalcOffsetToVerPlaneObservation.cpp
	TLSCalcOrientationCnstrObservation.cpp
	TLSCalcRadialOffsetCnstrObservation.cpp
	TLSCalcRelativeError.cpp
	TLSCalcSpatialDistObservation.cpp
	TLSCalcVertDistObservation.cpp
	TLSCalcWPSObservation.cpp
	TLSCalcWTWObservation.cpp
	TLSCalcZenithDistObservation.cpp
	
)

# Create a variable containing all .h files:
set(LSCalcObservations_HEADERS 	
	${LGC_INCLUDE_PATH}/TALSCalcAngleObservation.h
	${LGC_INCLUDE_PATH}/TALSCalcLengthObservation.h
	${LGC_INCLUDE_PATH}/TALSCalcObservation.h
	${LGC_INCLUDE_PATH}/TLSCalc2DOffsetObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcGyroOrientationObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcHorAngleObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcHorDistObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToSpaLineObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToTheoPlaneObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToVerLineObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToVerPlaneObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOrientationCnstrObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcRadialOffsetCnstrObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcRelativeError.h
	${LGC_INCLUDE_PATH}/TLSCalcSpatialDistObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcVertDistObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWPSObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcWTWObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcZenithDistObservation.h
	
	
)

# Create a static library from sources:
add_library(LSCalcObservations STATIC ${LSCalcObservations_SOURCES} ${LSCalcObservations_HEADERS})