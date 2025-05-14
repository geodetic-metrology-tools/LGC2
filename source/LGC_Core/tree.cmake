include(FetchContent)

FetchContent_Declare(
	treehh
	GIT_REPOSITORY "https://github.com/kpeeters/tree.hh"
	GIT_TAG ba91aab1a9642585d747b8d2371ab69290b36664 # Commit on July 19, 2024
	GIT_SHALLOW	OFF
)

# Only fetch/populate if not already done
if(NOT treehh_POPULATED)
    message(STATUS "Populating tree.hh...")
    FetchContent_MakeAvailable(treehh)
    message(STATUS "Done populating tree.hh")
endif()


# Create an interface target to expose include dir
add_library(treehh INTERFACE)
target_include_directories(treehh INTERFACE "${treehh_SOURCE_DIR}/src")
