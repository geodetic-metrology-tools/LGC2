include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/readers
					${CMAKE_SOURCE_DIR}/Writers)

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
