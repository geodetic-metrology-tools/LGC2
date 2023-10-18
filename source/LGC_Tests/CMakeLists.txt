include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCAdjustableObjects
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCConstraintObjects
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/ContributionsGenerators
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCCalculation
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCObjectWriters
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCProject
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LGCProjectData
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LocalTransformations
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/LSCalculation
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/Measurements
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/readers
					${CMAKE_SOURCE_DIR}/LGC_Core/headers/Writers
					${CMAKE_SOURCE_DIR}/LGC_Tests/headers/
					)

set(tests_SOURCES
 	sources/testReaders.cpp
 	sources/testGlobals.cpp
 	sources/testsTransformationClasses.cpp
 	sources/testTstnInputFiles.cpp
 	sources/testTstnInputFiles_2_Stations.cpp
 	sources/testGeodeticTransformations.cpp
 	sources/testTLOR2LOR.cpp
 	sources/testFilesGlobalRF.cpp
 	sources/testCAM.cpp
 	sources/testNonTSTNmeas.cpp
 	sources/testMixingObservation.cpp
 	sources/testAppExecution.cpp
sources/main.cpp
 	sources/tests_with_ScaleInstr.cpp
 	sources/testAllfixed.cpp
 	sources/testLIBR.cpp
 	sources/testLgc1Reader.cpp
 	sources/testINCL.cpp
 	sources/testHLSR.cpp
sources/testCONSI.cpp
 	sources/testWPSR.cpp
 	sources/testJSON.cpp
 	sources/testSimulation.cpp
 	sources/testSlaveConstraints.cpp
	sources/testPointGroupConstraints.cpp
 	sources/testEREL.cpp
)

set(tests_INPUT
	headers/testFilesGlobalRF.h
	headers/tests_TSTN_OLOC.h
	headers/tests_2_TSTN_OLOC.h
	headers/testTLOR2LOR.h
	headers/testNonTSTNmeas.h
	headers/testCAM.h
	headers/testMixingObservation.h
	headers/testDVER.h
	headers/tests_with_ScaleInstr.h
	headers/testAllfixed.h
	headers/testLIBR.h
	headers/testLgc1Reader.h
	headers/testINCL.h
	headers/testHLSR.h
	headers/testCONSI.h
	headers/testWPSR.h
	headers/testSimulation.h
	headers/testSlaveConstraints.h
	headers/testPointGroupConstraints.h
	headers/testEREL.h
)

add_executable(LGC_Tests ${tests_INPUT} ${tests_SOURCES})

target_link_libraries(LGC_Tests  LGC_Core LSAlgorithms ProjectFramework SpatialObjects Tools Plugins)

add_test(
	NAME LGC_Tests
	COMMAND $<TARGET_FILE_NAME:LGC_Tests>
	WORKING_DIRECTORY $<TARGET_FILE_DIR:LGC_Tests>
)

# Copy test files for unit tests
set(TEST_FILE_DIR ${CMAKE_CURRENT_LIST_DIR}/testFiles)
file(GLOB COPY_TESTFILES LIST_DIRECTORIES false ${TEST_FILE_DIR}/*)

foreach(test_file ${COPY_TESTFILES})
	post_build_copy_dlls(LGC_Tests "${test_file}" "${test_file}" "test_files")
	file(COPY "${test_file}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test_files")
endforeach()


