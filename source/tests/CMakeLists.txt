include_directories(${CMAKE_SOURCE_DIR})

set(tests_SOURCES
	testReaders.cpp
	testGlobals.cpp
	testsTransformationClasses.cpp
	testTstnInputFiles.cpp
	testTstnInputFiles_2_Stations.cpp
	testGeodeticTransformations.cpp
	testTLOR2LOR.cpp
	testFilesGlobalRF.cpp
	testCAM.cpp
	testNonTSTNmeas.cpp
	testMixingObservation.cpp
	testMathis.cpp
	testAppExecution.cpp
	main.cpp
	simulationTests.cpp
)

set(tests_INPUT
	testFilesGlobalRF.h
	tests_TSTN_OLOC.h
	tests_2_TSTN_OLOC.h
	testTLOR2LOR.h
	testNonTSTNmeas.h
	testCAM.h
	testMixingObservation.h
	testDVER.h
)

add_executable(tests ${tests_INPUT} ${tests_SOURCES})

target_link_libraries(tests LGCProject LGCProjectData LGCCalculation Readers CalcElements  LocalTransformations ProjectData Converters LGCObjectWriters FiltersAndFormats StreamFormatters Writers LSCalculation MathematicalConcepts AffineTransformations ContributionsGenerators ReferenceSurfaces ShortestPath RefFrameTransformations SpatialObj SpatialSystems AdjustableObjects Measurements)