cmake_minimum_required(VERSION 3.10)

#############################################################
#
# PROJECT NAME
#

project(LGC_ObjectSerialization)

#############################################################
#
# App specific includes

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/dependencies)
add_subdirectory(source/tests)

############################
# Final target

add_library(${PROJECT_NAME} STATIC source/Serializer.hpp source/Serializer_yaml.hpp source/Serializer_yaml.cpp source/CustomTypeTraits.hpp)
target_link_libraries(${PROJECT_NAME}
	yaml-cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)

set_target_properties(${PROJECT_NAME} LGC_ObjectSerialization_Test PROPERTIES FOLDER ${PROJECT_NAME})
