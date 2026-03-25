
# Define a variable that contains the path to LGC modules
set(LGCCoreTests_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(LGCCore_INCLUDE_PATH ${LGCCoreTests_MODULE_PATH}/../LGC_Core/headers)

include_directories(${LGCCore_INCLUDE_PATH}
					${LGCCore_INCLUDE_PATH}/LGCAdjustableObjects
					${LGCCore_INCLUDE_PATH}/ContributionsGenerators
					${LGCCore_INCLUDE_PATH}/LGCCalculation
					${LGCCore_INCLUDE_PATH}/LGCObjectWriters
					${LGCCore_INCLUDE_PATH}/LGCProject
					${LGCCore_INCLUDE_PATH}/LGCProjectData
					${LGCCore_INCLUDE_PATH}/LocalTransformations
					${LGCCore_INCLUDE_PATH}/LSCalculation
					${LGCCore_INCLUDE_PATH}/Measurements
					${LGCCore_INCLUDE_PATH}/readers
					${LGCCore_INCLUDE_PATH}/Writers
					${LGCCoreTests_MODULE_PATH}/headers/
					)

set(tests_SOURCES
	${LGCCoreTests_MODULE_PATH}/sources/testReaders.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testGlobals.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testsTransformationClasses.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testTstnInputFiles.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testTstnInputFiles_2_Stations.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testGeodeticTransformations.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testTLOR2LOR.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testFilesGlobalRF.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testCAM.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testNonTSTNmeas.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testMixingObservation.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testAppExecution.cpp
	${LGCCoreTests_MODULE_PATH}/sources/main.cpp
	${LGCCoreTests_MODULE_PATH}/sources/tests_with_ScaleInstr.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testAllfixed.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testPointGroupConstraints.cpp
	${LGCCoreTests_MODULE_PATH}/sources/testLgc1Reader.cpp
)

set(tests_INPUT
	${LGCCoreTests_MODULE_PATH}/headers/testFilesGlobalRF.h
	${LGCCoreTests_MODULE_PATH}/headers/tests_TSTN_OLOC.h
	${LGCCoreTests_MODULE_PATH}/headers/tests_2_TSTN_OLOC.h
	${LGCCoreTests_MODULE_PATH}/headers/testTLOR2LOR.h
	${LGCCoreTests_MODULE_PATH}/headers/testNonTSTNmeas.h
	${LGCCoreTests_MODULE_PATH}/headers/testCAM.h
	${LGCCoreTests_MODULE_PATH}/headers/testMixingObservation.h
	${LGCCoreTests_MODULE_PATH}/headers/testDVER.h
	${LGCCoreTests_MODULE_PATH}/headers/tests_with_ScaleInstr.h
	${LGCCoreTests_MODULE_PATH}/headers/testAllfixed.h
	${LGCCoreTests_MODULE_PATH}/headers/testPointGroupConstraints.cpp
	${LGCCoreTests_MODULE_PATH}/headers/testLgc1Reader.h
)

add_executable(LGCCoreTests
    ${tests_INPUT}
    ${tests_SOURCES}
)

target_link_libraries(LGCCoreTests
    LGCCore
    LSAlgorithms
    ProjectFramework
    SpatialObjects
    Tools
)

# Copy test files for unit tests
set(TEST_FILE_DIR ${CMAKE_CURRENT_LIST_DIR}/testFiles)
file(GLOB_RECURSE test_files_to_copy RELATIVE ${TEST_FILE_DIR} ${TEST_FILE_DIR}/*)

foreach(test_file ${test_files_to_copy})
configure_file(${TEST_FILE_DIR}/${test_file} test_files/${test_file} COPYONLY)
endforeach()
