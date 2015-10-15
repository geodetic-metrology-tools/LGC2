# Create a variable containing all .cpp files:
set(LGCXX_SOURCES
	../lgc++.cpp
)

# Create a variable containing all .h files:
set(LGCXX_HEADERS 	
	${LGC_INCLUDE_PATH}/TLGCApplication.h
)

# Create a static library from sources:
add_executable(LGC ${LGCXX_SOURCES} ${LGCXX_HEADERS} ../CmakeSetup.rc)
target_link_libraries(LGC
	${LGC_SUBPROJECTS}
	${PROJECTFRAMEWORK_LIBRARIES}
	${LSALGO_LIBRARIES}
	${SPATIAL_LIBRARIES}
)


install(TARGETS LGC DESTINATION "." COMPONENT application)
install(FILES
		"${VCREDIST_INSTALLER_PATH}"
		DESTINATION "."
		COMPONENT application
	)
#####

#####