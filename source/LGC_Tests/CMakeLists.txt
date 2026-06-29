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
	sources/testPOLAR.cpp
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
	sources/testLgc1Reader.cpp
	sources/testINCL.cpp
	sources/testHLSR.cpp
	sources/testCONSI.cpp
	sources/testEvaluator.cpp
	sources/testWPSR.cpp
	sources/testJSON.cpp
	sources/testSimulation.cpp
	sources/testSlaveConstraints.cpp
	sources/testStatistics.cpp
	sources/testPointGroupConstraints.cpp
	sources/testPointWithSigma.cpp
	sources/testEREL.cpp
	sources/testDLEV.cpp
	sources/testSAG.cpp
	sources/testAPI.cpp
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
	headers/testLgc1Reader.h
	headers/testINCL.h
	headers/testHLSR.h
	headers/testCONSI.h
	headers/testEvaluator.h
	headers/testWPSR.h
	headers/testSimulation.h
	headers/testSlaveConstraints.h
	headers/testStatistics.h
	headers/testPointGroupConstraints.h
	headers/testPointWithSigma.h
	headers/testEREL.h
	headers/testDLEV.h
	headers/testPOLAR.h
	headers/testSAG.h
	headers/testAPI.h
)

add_executable(LGC_Tests ${tests_INPUT} ${tests_SOURCES})

# Ensure LGC_API (Monitoring) is built first, and copy its DLL next to the test executable
add_dependencies(LGC_Tests LGC_API)

add_custom_command(TARGET LGC_Tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:LGC_API>"
    "$<TARGET_FILE_DIR:LGC_Tests>/$<TARGET_FILE_NAME:LGC_API>"
)

target_link_libraries(LGC_Tests  LGC_API LGC_Core LSAlgorithms ProjectFramework SpatialObjects Tools Plugins)

add_test(
	NAME LGC_Tests
	COMMAND $<TARGET_FILE_NAME:LGC_Tests>
	WORKING_DIRECTORY $<TARGET_FILE_DIR:LGC_Tests>
)

# Copy test files for unit tests
set(TEST_FILE_DIR ${CMAKE_CURRENT_LIST_DIR}/testFiles)
file(GLOB COPY_TESTFILES LIST_DIRECTORIES false ${TEST_FILE_DIR}/*)

# Also stage the Monitoring API test input files (testFilesAPI/) into test_files/
set(TEST_FILE_DIR_API ${CMAKE_CURRENT_LIST_DIR}/testFilesAPI)
file(GLOB ADDITIONAL_TESTFILES LIST_DIRECTORIES false ${TEST_FILE_DIR_API}/*)
list(APPEND COPY_TESTFILES ${ADDITIONAL_TESTFILES})

# NOTE: the file(COPY) below stages the test inputs at configure time into the
# target's output dir (build/LGC_Tests/test_files). The previous
# post_build_copy_dlls() call here produced a broken "cmake -E copy <dest>"
# command under single-config generators with CMAKE_BUILD_TYPE=RelWithDebInfo
# (its $<CONFIG:debug>/$<CONFIG:release> gen-expressions expand to nothing,
# leaving no source argument), so it is intentionally dropped — file(COPY)
# already places the files where the test executable expects them.
foreach(test_file ${COPY_TESTFILES})
	file(COPY "${test_file}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/test_files")
endforeach()
