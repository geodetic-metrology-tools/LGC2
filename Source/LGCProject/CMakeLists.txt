# Create a variable containing all .cpp files:
set(LGCProject_SOURCES
	TLGCApplication.cpp
	TLGCProject.cpp
)


# Create a variable containing all .h files:
set(LGCProject_HEADERS 	
	${LGC_INCLUDE_PATH}/TLGCApplication.h
	${LGC_INCLUDE_PATH}/TLGCProject.h
)

# Create a static library from sources:
add_library(LGCProject STATIC ${LGCProject_SOURCES} ${LGCProject_HEADERS})