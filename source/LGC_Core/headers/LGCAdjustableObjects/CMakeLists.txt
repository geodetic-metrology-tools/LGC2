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
	LGCAdjustablePoint.h
	LGCAdjustableLine.h
	LGCAdjustablePlane.h
	LGCAdjustableObjectCollection.h
)

set(ADJUSTABLE_SOURCES
	LGCAdjustablePoint.cpp
	LGCAdjustableLine.cpp
	LGCAdjustablePlane.cpp
)

add_library(LGCAdjustableObjects STATIC ${ADJUSTABLE_HEADERS} ${ADJUSTABLE_SOURCES} )
