include(FetchContent)

FetchContent_Declare(
	rapidjson
	GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
	GIT_TAG v1.1.0
	GIT_SHALLOW	ON
)

if(NOT rapidjson_POPULATED)
	message(STATUS "Populating rapidjson...")
	set(RAPIDJSON_BUILD_CXX17 ON CACHE BOOL "" FORCE)
	set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
	set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "" FORCE)
	set(RAPIDJSON_ENABLE_INSTRUMENTATION_OPT OFF CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(rapidjson)
	message(STATUS "Done populating rapidjson")
endif()

# TODO - ENABLE WHEN CMAKE 3.19 >= IS SUPPORTED
#add_library(rapidjson INTERFACE) # adding dummy target for folders and to propagate dirs
#target_include_directories(rapidjson INTERFACE "${rapidjson_SOURCE_DIR}/include")
#set_target_properties(rapidjson
#	travis_doc
#	PROPERTIES FOLDER "LGC_ObjectSerialization/rapidjson"
#)
# TODO - REMOVE AFTER CMAKE UPDATE
set_target_properties(travis_doc
	travis_doc
	PROPERTIES FOLDER "LGC_ObjectSerialization/rapidjson"
)
set(RAPIDJSON_INCLUDE_DIR "${rapidjson_SOURCE_DIR}/include" PARENT_SCOPE)
