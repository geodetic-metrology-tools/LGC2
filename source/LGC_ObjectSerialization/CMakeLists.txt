cmake_minimum_required(VERSION 3.10)

#############################################################
#
# PROJECT NAME
#

project(LGC_ObjectSerialization)

#############################################################
#
# App specific includes

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/tests)

############################
# Final target

add_library(${PROJECT_NAME} STATIC 
	source/Serializer.hpp 
	source/Serializer_json.hpp 
	source/Serializer_json.cpp 
	source/Serializer_yaml.hpp 
	source/Serializer_yaml.cpp 
	source/CustomTypeTraits.hpp
)

add_dependencies(${PROJECT_NAME}
	yaml-cpp
)
target_link_libraries(${PROJECT_NAME}
	yaml-cpp
	#rapidjson TODO
)
add_definitions(-DRAPIDJSON_HAS_STDSTRING) # TODO

target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/source
	# TODO - REMOVE AFTER CMAKE UPDATE
	${RAPIDJSON_INCLUDE_DIR}
)
set_target_properties(${PROJECT_NAME} LGC_ObjectSerialization_Test PROPERTIES FOLDER ${PROJECT_NAME})
