include_directories(${CMAKE_SOURCE_DIR})

set(ADJUSTABLE_HEADERS
	TAdjustablePoint.h
	TVAdjustableObject.h
	TAdjustableLine.h
	TAdjustableHelmertTransformation.h
	TAdjustableAngle.h
	TAdjustablePlane.h
	TAdjustableScalar.h
	TAdjustableObjectCollection.h
)

set(ADJUSTABLE_SOURCES
	TAdjustablePoint.cpp
	TAdjustableLine.cpp
	TAdjustableHelmertTransformation.cpp
	TAdjustableAngle.cpp
	TAdjustablePlane.cpp
	TAdjustableScalar.cpp
)

add_library(AdjustableObjects STATIC ${ADJUSTABLE_HEADERS} ${ADJUSTABLE_SOURCES} )
