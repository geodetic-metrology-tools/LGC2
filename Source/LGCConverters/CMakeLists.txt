# Create a variable containing all .cpp files:
set(LGCConverters_SOURCES
	EDMConverter.cpp
	EDMSpatialDistConverter.cpp
	GyroscopeConverter.cpp
	GyroscopeStationConverter.cpp
	InstrumentsConverter.cpp
	LevelConverter.cpp
	LevelStationConverter.cpp
	OffsetToSpatialLineConverter.cpp
	OffsetToVerticalLineConverter.cpp
	OffsetToVerticalPlaneConverter.cpp
	PolarConverter.cpp
	ScaleConverter.cpp
	TALGCConverter.cpp
	TAngleObsConverter.cpp
	TheodoliteConverter.cpp
	TheodoliteStationConverter.cpp
	THorAngleConverter.cpp
	THorDistConverter.cpp
	TLengthObsConverter.cpp
	TObservationConverter.cpp
	TOffsetToTheoPlaneConverter.cpp
	TRadialOffsetCnstrConverter.cpp
	TRelativeErrorConverter.cpp
	TSpatialDistConverter.cpp
	TVertDistConverter.cpp
	TZenithDistConverter.cpp
)


# Create a variable containing all .h files:
set(LGCConverters_HEADERS 	
	${LGC_INCLUDE_PATH}/EDMConverter.h
	${LGC_INCLUDE_PATH}/EDMSpatialDistConverter.h
	${LGC_INCLUDE_PATH}/GyroscopeConverter.h
	${LGC_INCLUDE_PATH}/GyroscopeStationConverter.h
	${LGC_INCLUDE_PATH}/InstrumentsConverter.h
	${LGC_INCLUDE_PATH}/LevelConverter.h
	${LGC_INCLUDE_PATH}/LevelStationConverter.h
	${LGC_INCLUDE_PATH}/OffsetToSpatialLineConverter.h
	${LGC_INCLUDE_PATH}/OffsetToVerticalLineConverter.h
	${LGC_INCLUDE_PATH}/OffsetToVerticalPlaneConverter.h
	${LGC_INCLUDE_PATH}/PolarConverter.h
	${LGC_INCLUDE_PATH}/ScaleConverter.h
	${LGC_INCLUDE_PATH}/TALGCConverter.h
	${LGC_INCLUDE_PATH}/TAngleObsConverter.h
	${LGC_INCLUDE_PATH}/TheodoliteConverter.h
	${LGC_INCLUDE_PATH}/TheodoliteStationConverter.h
	${LGC_INCLUDE_PATH}/THorAngleConverter.h
	${LGC_INCLUDE_PATH}/THorDistConverter.h
	${LGC_INCLUDE_PATH}/TLengthObsConverter.h
	${LGC_INCLUDE_PATH}/TObservationConverter.h
	${LGC_INCLUDE_PATH}/TOffsetToTheoPlaneConverter.h
	${LGC_INCLUDE_PATH}/TRadialOffsetCnstrConverter.h
	${LGC_INCLUDE_PATH}/TRelativeErrorConverter.h
	${LGC_INCLUDE_PATH}/TSpatialDistConverter.h
	${LGC_INCLUDE_PATH}/TVertDistConverter.h
	${LGC_INCLUDE_PATH}/TZenithDistConverter.h
)

# Create a static library from sources:
add_library(LGCConverters STATIC ${LGCConverters_SOURCES} ${LGCConverters_HEADERS})