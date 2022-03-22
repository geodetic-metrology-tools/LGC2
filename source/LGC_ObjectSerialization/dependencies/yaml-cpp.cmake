include(FetchContent)

FetchContent_Declare(
	yaml-cpp
	GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
	GIT_TAG yaml-cpp-0.7.0
	GIT_SHALLOW	ON
)

if(NOT yaml-cpp_POPULATED)
	message(STATUS "Populating yaml-cpp...")
	set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
	set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
	set(YAML_CPP_CLANG_FORMAT_EXE OFF CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(yaml-cpp)
	message(STATUS "Done populating yaml-cpp")
endif()


set_target_properties(yaml-cpp
	Continuous
	Experimental
	Nightly
	NightlyMemoryCheck 
	PROPERTIES FOLDER "LGC_ObjectSerialization/yaml-cpp"
)
