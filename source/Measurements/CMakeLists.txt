include_directories(${CMAKE_SOURCE_DIR})

set(MEASUREMENTS_HEADERS
	TMeasurements.h
	TTSTN.h
	TAMeas.h
	TEDM.h
	TLEVEL.h
	TCAM.h
	TLGCObsSummary.h
	MeasDef.h	
)

set(MEASUREMENTS_SOURCES
	TMeasurements.cpp
	MeasDef.cpp
	TLGCObsSummary.cpp
	TTSTN.cpp
	TCAM.cpp
)

add_library(Measurements STATIC ${MEASUREMENTS_HEADERS} ${MEASUREMENTS_SOURCES} )
