include_directories(${LGC_MODULE_PATH}
					${LGC_MODULE_PATH}/AdjustableObjects
					${LGC_MODULE_PATH}/ContributionsGenerators
					${LGC_MODULE_PATH}/LGCProject
					${LGC_MODULE_PATH}/LGCProjectData
					${LGC_MODULE_PATH}/Measurements
					${LGC_MODULE_PATH}/LocalTransformations
					${LGC_MODULE_PATH}/readers
					${LGC_MODULE_PATH}/Writers)

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
