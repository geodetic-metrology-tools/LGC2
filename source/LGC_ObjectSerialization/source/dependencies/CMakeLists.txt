project(yaml-cpp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add here options for yaml-cpp building
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_BUILD_SHARED_LIBS OFF)
set(YAML_CPP_INSTALL OFF)
set(YAML_CPP_CLANG_FORMAT_EXE OFF)

include(FetchContent)
FetchContent_Declare(
		yaml-cpp
		GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
		GIT_SHALLOW	ON
		GIT_TAG yaml-cpp-0.7.0
	)
FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
	message(STATUS "Populating yaml-cpp...")
	FetchContent_Populate(yaml-cpp)
	add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
	target_include_directories(yaml-cpp PUBLIC
		${yaml-cpp_SOURCE_DIR}
		${yaml-cpp_BINARY_DIR}/include
	)
	message(STATUS "Done.")
endif()

set_target_properties(Experimental yaml-cpp Continuous Nightly NightlyMemoryCheck PROPERTIES FOLDER "LGC_ObjectSerialization/yaml-cpp")
