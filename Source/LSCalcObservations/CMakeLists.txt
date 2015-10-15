# Create a variable containing all .cpp files:
set(LSCalcObservations_SOURCES
	TALSCalcAngleObservation.cpp
	TALSCalcLengthObservation.cpp
	TALSCalcObservation.cpp
	TLSCalcGyroOrientationObservation.cpp
	TLSCalcHorAngleObservation.cpp
	TLSCalcOffsetToSpatialLineConstraint.cpp
	TLSCalcOffsetToTheoPlaneObservation.cpp
	TLSCalcOffsetToVerPlaneObservation.cpp
	TLSCalcOffsetToVerticalLineObservation.cpp
	TLSCalcOrientationCnstrObservation.cpp
	TLSCalcPolarObservation.cpp
	TLSCalcRadialOffsetCnstrObservation.cpp
	TLSCalcRelativeError.cpp
	TLSCalcVertDistObservation.cpp
	TLSCalcZenithDistObservation.cpp
	
)

# Create a variable containing all .h files:
set(LSCalcObservations_HEADERS
	${LGC_INCLUDE_PATH}/TALSCalcAngleObservation.h
	${LGC_INCLUDE_PATH}/TALSCalcLengthObservation.h
	${LGC_INCLUDE_PATH}/TALSCalcObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcGyroOrientationObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcHorAngleObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcHorDistObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcLevelObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToSpatialLineConstraint.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToSpatialLineObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToTheoPlaneObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToVerPlaneObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToVerticalLineObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcOffsetToVerticalPlaneConstraint.h
	${LGC_INCLUDE_PATH}/TLSCalcOrientationCnstrObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcPolarObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcRadialOffsetCnstrObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcRelativeError.h
	${LGC_INCLUDE_PATH}/TLSCalcSpatialDistObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcVertDistObservation.h
	${LGC_INCLUDE_PATH}/TLSCalcZenithDistObservation.h
	
)

# Create a static library from sources:
add_library(LSCalcObservations STATIC ${LSCalcObservations_SOURCES} ${LSCalcObservations_HEADERS})