include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/Readers
					${LGC_MODULE_PATH}/Writers)

set(ADJUSTABLE_HEADERS
	TAdjustablePoint.h
	TVAdjustableObject.h
	TAdjustableLine.h
	TAdjustableHelmertTransformation.h
	TAdjustableAngle.h
	TAdjustablePlane.h
	TAdjustableScalar.h
	TAdjustableLength.h
	TAdjustableObjectCollection.h
)

set(ADJUSTABLE_SOURCES
	TAdjustablePoint.cpp
	TAdjustableLine.cpp
	TAdjustableHelmertTransformation.cpp
	TAdjustableAngle.cpp
	TAdjustablePlane.cpp
	TAdjustableScalar.cpp
	TAdjustableLength.cpp
)

add_library(AdjustableObjects STATIC ${ADJUSTABLE_HEADERS} ${ADJUSTABLE_SOURCES} )
