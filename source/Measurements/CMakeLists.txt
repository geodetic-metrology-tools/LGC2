include_directories(${CMAKE_SOURCE_DIR})

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
