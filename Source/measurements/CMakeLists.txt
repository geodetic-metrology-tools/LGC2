set(LGCMeasurements_SOURCES
    EDM.cpp
    Level.cpp
    Scale.cpp
    TAFreeInstrument.cpp
    TAMeasurement.cpp
    TAInstrument.cpp
    TAngleConstants.cpp
    TDistConstants.cpp
    TOrientationCnstr.cpp
    TRadialOffsetCnstr.cpp
    TTheodolite.cpp
    TVerticalDistMeasurement.cpp
    TWorkingConstants.cpp
    TWorkingConstraints.cpp
)

set(LGCMeasurements_HEADERS
	${LGC_INCLUDE_PATH}/EDM.h
	${LGC_INCLUDE_PATH}/Level.h
	${LGC_INCLUDE_PATH}/Scale.h
	${LGC_INCLUDE_PATH}/TAFreeInstrument.h
	${LGC_INCLUDE_PATH}/TAInstrument.h
	${LGC_INCLUDE_PATH}/TAMeasurement.h
	${LGC_INCLUDE_PATH}/TAngleConstants.h
	${LGC_INCLUDE_PATH}/TDistConstants.h
	${LGC_INCLUDE_PATH}/TOrientationCnstr.h
	${LGC_INCLUDE_PATH}/TRadialOffsetCnstr.h
	${LGC_INCLUDE_PATH}/TTheodolite.h
	${LGC_INCLUDE_PATH}/TVerticalDistMeasurement.h
	${LGC_INCLUDE_PATH}/TWorkingConstants.h
	${LGC_INCLUDE_PATH}/TWorkingConstraints.h
)

add_library(Measurements STATIC ${LGCMeasurements_HEADERS} ${LGCMeasurements_SOURCES})
