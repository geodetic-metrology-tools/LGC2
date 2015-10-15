include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(LGCConverters_SOURCES
	TALGCConverter.cpp
	TTstnConverter.cpp
	TCamConverter.cpp
	TLevelConverter.cpp
	TEDMConverter.cpp	
	TFRAMEConverter.cpp
#	TAngleObsConverter.cpp
#	TGyroOrientationConverter.cpp
#	THorAngleConverter.cpp
#	THorDistConverter.cpp
#	TLengthObsConverter.cpp
	TObservationConverter.cpp
#	TOffsetToSpaLineConverter.cpp
#	TOffsetToTheoPlaneConverter.cpp
#	TOffsetToVerLineConverter.cpp
#	TOffsetToVerPlaneConverter.cpp
#	TRadialOffsetCnstrConverter.cpp
#	TRelativeErrorConverter.cpp
#	TSpatialDistConverter.cpp
#	TVertDistConverter.cpp
#	TZenithDistConverter.cpp
)


# Create a variable containing all .h files:
set(LGCConverters_HEADERS 	
	TALGCConverter.h
	TTstnConverter.h
	TCamConverter.h
	TLevelConverter.h
	TEDMConverter.h
	TFRAMEConverter.h
#	TAngleObsConverter.h
#	TGyroOrientationConverter.h
#	THorAngleConverter.h
#	THorDistConverter.h
#	TLengthObsConverter.h
	TObservationConverter.h
#	TOffsetToSpaLineConverter.h
#	TOffsetToTheoPlaneConverter.h
#	TOffsetToVerLineConverter.h
#	TOffsetToVerPlaneConverter.h
#	TRadialOffsetCnstrConverter.h
#	TRelativeErrorConverter.h
#	TSpatialDistConverter.h
#	TVertDistConverter.h
#	TZenithDistConverter.h
)

# Create a static library from sources:
add_library(LGCConverters STATIC ${LGCConverters_SOURCES} ${LGCConverters_HEADERS})