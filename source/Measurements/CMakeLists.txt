include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/LGCObjectWriters
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/readers
					${LGC_MODULE_PATH}/Writers)

set(MEASUREMENTS_HEADERS
	TMeasurements.h
	TTSTN.h
	TAMeas.h
	TEDM.h
	TLEVEL.h
	TCAM.h
	TUVEC.h
	TUVD.h
	TLGCObsSummary.h
	MeasDef.h
	RoundOfMeasurements.h
)

set(MEASUREMENTS_SOURCES
	MeasDef.cpp
	TLGCObsSummary.cpp
	TTSTN.cpp
	TCAM.cpp
	TLEVEL.cpp
	TEDM.cpp
	RoundOfMeasurements.cpp
)

add_library(Measurements STATIC ${MEASUREMENTS_HEADERS} ${MEASUREMENTS_SOURCES} )
