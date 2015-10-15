include_directories(${CMAKE_SOURCE_DIR})

# Create a variable containing all .cpp files:
set(LGCObjectWriters_SOURCES
	TALGCObjectWriter.cpp
	TTSTNWriter.cpp
	TCAMWriter.cpp
	TLEVELWriter.cpp
	TEDMWriter.cpp	
	TFRAMEWriter.cpp
	TSCALEWriter.cpp
	TOtherMeasurementsWriter.cpp
	TObservationWriter.cpp
)


# Create a variable containing all .h files:
set(LGCObjectWriters_HEADERS 	
	${LGC2_INCLUDE_PATH}/TALGCObjectWriter.h
	${LGC2_INCLUDE_PATH}/TTSTNWriter.h
	${LGC2_INCLUDE_PATH}/TCAMWriter.h
	${LGC2_INCLUDE_PATH}/TLEVELWriter.h
	${LGC2_INCLUDE_PATH}/TEDMWriter.h
	${LGC2_INCLUDE_PATH}/TFRAMEWriter.h
	${LGC2_INCLUDE_PATH}/TSCALEWriter.h
	${LGC2_INCLUDE_PATH}/TOtherMeasurementsWriter.h
	${LGC2_INCLUDE_PATH}/TObservationWriter.h
)

# Create a static library from sources:
add_library (LGCObjectWriters STATIC ${LGCObjectWriters_SOURCES} ${LGCObjectWriters_HEADERS})


# Link static libraries to the current one
target_link_libraries(LGCObjectWriters Measurements)
