cmake_minimum_required(VERSION 3.10)

# Create a variable containing all .hpp files:
set(tests_HEADERS
)
# Create a variable containing all .cpp files:
set(tests_SOURCES
	main.cpp
	test_Serializer_yaml.cpp
)

# Create an executable from sources:
add_executable(LGC_ObjectSerialization_Test
	${tests_HEADERS}
	${tests_SOURCES}
)
target_include_directories(LGC_ObjectSerialization_Test PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_SOURCE_DIR}
	# TUT
	${TUT_INCLUDE_PATH}
)
target_link_libraries(LGC_ObjectSerialization_Test
	LGC_ObjectSerialization
)

set(TEST_DIR "$<TARGET_FILE_DIR:LGC_ObjectSerialization_Test>")
if (MSVC)
	set(TEST_DIR "$<TARGET_FILE_DIR:LGC_ObjectSerialization_Test>/..")
endif()
add_test(
	NAME LGC_ObjectSerialization_Test
	COMMAND LGC_ObjectSerialization_Test
	WORKING_DIRECTORY "${TEST_DIR}"
)

source_group("LGC_ObjectSerialization_Test" FILES ${tests_HEADERS} ${tests_SOURCES})
