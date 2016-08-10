include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGCAdjustableObjects
					${CMAKE_SOURCE_DIR}/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGCProject
					${CMAKE_SOURCE_DIR}/LGCProjectData
					${CMAKE_SOURCE_DIR}/Measurements
					${CMAKE_SOURCE_DIR}/LocalTransformations
					${CMAKE_SOURCE_DIR}/readers
					${CMAKE_SOURCE_DIR}/Writers)

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
	TALGCObjectWriter.h
	TTSTNWriter.h
	TCAMWriter.h
	TLEVELWriter.h
	TEDMWriter.h
	TFRAMEWriter.h
	TSCALEWriter.h
	TOtherMeasurementsWriter.h
	TObservationWriter.h
)

# Create a static library from sources:
add_library (LGCObjectWriters STATIC ${LGCObjectWriters_SOURCES} ${LGCObjectWriters_HEADERS})

# Link static libraries to the current one
target_link_libraries(LGCObjectWriters Measurements)
