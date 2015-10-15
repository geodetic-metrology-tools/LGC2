include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(ReferenceSurfaces_SOURCES 
	T3DLocalRefFrame.cpp
	TA2DPlus1MapProjection.cpp
	TA3DEuclideanRefFrame.cpp
	TAGeoidModel.cpp
	TAModifiedLocalAstronomicalRF.cpp
	TAReferenceFrame.cpp
	TCernGridGeoid.cpp
	TCernParabolicGeoid.cpp
	TCernSphereGeoid.cpp
	TGeodeticRefFrame.cpp
	TGeoidValues.cpp
	TGraphLocalAstronomicalRF.cpp
	TGraphMLARF.cpp
	TModifiedLocalAstronomicalRF.cpp
	TModifiedLocalGeodeticRF.cpp
	TReferenceEllipsoid.cpp
	TX0Y0HeProjection.cpp
	TXYHeProjection.cpp
	TXYHgProjection.cpp
	
	# TA2DXYRefFrame.cpp
)

# Create a variable containing all .h files:
set(ReferenceSurfaces_HEADERS 
	${SURVEYLIB_INCLUDE_PATH}/TNotInLepGridException.h

	${SURVEYLIB_INCLUDE_PATH}/T3DLocalRefFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TA2DPlus1MapProjection.h
	${SURVEYLIB_INCLUDE_PATH}/TA2DXYRefFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TA3DEuclideanRefFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TAGeoidModel.h
	${SURVEYLIB_INCLUDE_PATH}/TAModifiedLocalAstronomicalRF.h
	${SURVEYLIB_INCLUDE_PATH}/TAReferenceFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TCernGridGeoid.h
	${SURVEYLIB_INCLUDE_PATH}/TCernParabolicGeoid.h
	${SURVEYLIB_INCLUDE_PATH}/TCernSphereGeoid.h
	${SURVEYLIB_INCLUDE_PATH}/TEuclidianReferenceFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TGeodeticRefFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TGeoidValues.h
	${SURVEYLIB_INCLUDE_PATH}/TGraphLocalAstronomicalRF.h
	${SURVEYLIB_INCLUDE_PATH}/TGraphMLARF.h
	${SURVEYLIB_INCLUDE_PATH}/TModifiedLocalAstronomicalRF.h
	${SURVEYLIB_INCLUDE_PATH}/TModifiedLocalGeodeticRF.h
	${SURVEYLIB_INCLUDE_PATH}/TReferenceEllipsoid.h
	${SURVEYLIB_INCLUDE_PATH}/TVGeoidModel.h
	${SURVEYLIB_INCLUDE_PATH}/TVReferenceFrame.h
	${SURVEYLIB_INCLUDE_PATH}/TX0Y0HeProjection.h
	${SURVEYLIB_INCLUDE_PATH}/TXYHeProjection.h
	${SURVEYLIB_INCLUDE_PATH}/TXYHgProjection.h
	${SURVEYLIB_INCLUDE_PATH}/TLV95Projection.h
	${SURVEYLIB_INCLUDE_PATH}/TLV03Projection.h
	${SURVEYLIB_INCLUDE_PATH}/TRGF93CC46Projection.h
	${SURVEYLIB_INCLUDE_PATH}/TLambert93Projection.h
	
	# ${SURVEYLIB_INCLUDE_PATH}/TA2DXYRefFrame.h
)

# Create a static library from sources:
add_library(ReferenceSurfaces STATIC ${ReferenceSurfaces_SOURCES} ${ReferenceSurfaces_HEADERS})

# Link static libraries to the current one
target_link_libraries(ReferenceSurfaces AffineTransformations SpatialSystems RefFrameTransformations)
