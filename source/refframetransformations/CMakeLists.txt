include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(RefFrameTransformations_SOURCES 
	TCCS2CGRFTransformation.cpp
	TCGRF2ILGTransformation.cpp
	TILG2ILATransformation.cpp
	TXYH2CCS.cpp
	Cartesian2Ellipsoidal.cpp


	TARefFrameTransformation.cpp
	TGC2LGTransformation.cpp
	TGC2MLATransformation.cpp
	THelmertRefFrameTransform.cpp
	TLG2GCTransformation.cpp
	TMLA2GCTransformation.cpp

	#TLA2LGTransformation.cpp
	#TLG2LATransformation.cpp
	TMLA2XYHeTransformation.cpp
	TMLA2XYHsTransformation.cpp
	#TX0Y0He2XYHeTransformation.cpp
	TXYHe2MLATransformation.cpp
	#TXYHe2X0Y0HeTransformation.cpp
	#TXYHe2XYHgTransformation.cpp
	#TXYHg2XYHeTransformation.cpp
	TXYHs2MLATransformation.cpp
	#TRGF93CC46Transformation.cpp
	#TLambert93Transformation.cpp
)

IF(USE_SWISSTOPO)
set(RefFrameTransformations_SOURCES 
	${RefFrameTransformations_SOURCES}
	TLV03Transformation.cpp
	TLV95Transformation.cpp
	)
ENDIF(USE_SWISSTOPO)

# Create a variable containing all .h files:
set(RefFrameTransformations_HEADERS 
	TCCS2CGRFTransformation.h
	GeodeticConstants.h
	TCGRF2ILGTransformation.h
	TILG2ILATransformation.h
	TXYH2CCS.h
	Cartesian2Ellipsoidal.h


	${SURVEYLIB_INCLUDE_PATH}/TARefFrameTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TGC2LGTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TGC2MLATransformation.h
	${SURVEYLIB_INCLUDE_PATH}/THelmertRefFrameTransform.h
	${SURVEYLIB_INCLUDE_PATH}/TLG2GCTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TMLA2GCTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TVRefFrameTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TVTransformation.h
	

	#${SURVEYLIB_INCLUDE_PATH}/TLA2LGTransformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TLG2LATransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TMLA2XYHeTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TMLA2XYHsTransformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TX0Y0He2XYHeTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TXYHe2MLATransformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TXYHe2X0Y0HeTransformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TXYHe2XYHgTransformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TXYHg2XYHeTransformation.h
	${SURVEYLIB_INCLUDE_PATH}/TXYHs2MLATransformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TLV95Transformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TLV03Transformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TRGF93CC46Transformation.h
	#${SURVEYLIB_INCLUDE_PATH}/TLambert93Transformation.h
	
)

# Create a static library from sources:
add_library(RefFrameTransformations STATIC ${RefFrameTransformations_SOURCES} ${RefFrameTransformations_HEADERS})
#####


