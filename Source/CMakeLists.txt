# declare a project
PROJECT(LGC2)

# Set version number and provide a header file with corresponding defines
set (LGC_VERSION_MAJOR 2)
set (LGC_VERSION_MINOR 0)
set (LGC_VERSION_PATCH 0)
configure_file (
  "${CMAKE_SOURCE_DIR}/LGCVersion.in"
  "${CMAKE_SOURCE_DIR}/../include/LGCVersion.h"
)

# minimum version of CMake
cmake_minimum_required(VERSION 2.8)

# enable languages
enable_language("CXX")

# default build type
SET(CMAKE_BUILD_TYPE RELEASE)

#######################################################################
# Whether to use Intel Compiler specific flags
SET(USE_INTEL_COMPILER FALSE)

# REQ: BOOST_ROOT VCREDIST_INSTALLER_PATH SURVEYLIB_INCLUDE_PATH SURVEYLIB_LIBRARY_DIR EIGEN_INCLUDE_PATH
INCLUDE(../../ext_libs.txt)

INCLUDE(${SURVEYLIB_ROOT}/source/SubLibraries.cmake)

FIND_PACKAGE(Boost 1.49.0 REQUIRED)

SET(LGC_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/../include")
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

IF(NOT EXISTS "${VCREDIST_INSTALLER_PATH}")
    MESSAGE(FATAL_ERROR "[ERROR] Cannot find VS redistribuable #installer at ${VCREDIST_INSTALLER_PATH}")
ELSE()
    MESSAGE(STATUS "[STATUS] Found VS redistribuable #installer at ${VCREDIST_INSTALLER_PATH}")
ENDIF()

#######################################################################

# Specific options will go here
IF(WIN32)
    IF(MSVC)
        # turn off various warnings
        # foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
        #     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${warning}")
        # endforeach(warning)

        # More MSVC specific compilation flags
        ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
	SET(CMAKE_CXX_FLAGS "/openmp /EHsc /O2 /W3")
	#SET(CMAKE_CXX_FLAGS "/EHsc /Od")
    ENDIF(MSVC)
	IF(USE_INTEL_COMPILER)
		# Support for the quadruple precision:
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qoption,cpp,--extended_float_type")
		# Maximum level (5) of warnings produces too many warnings
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
	ENDIF(USE_INTEL_COMPILER)
ENDIF(WIN32)

include_directories (
	${LGC_INCLUDE_PATH}
	${Boost_INCLUDE_DIRS}
)

INCLUDE("${SURVEYLIB_ROOT}/source/SourceSurveyLib.cmake")

SET(LGC_SUBPROJECTS
   ContributionsGenerators
   Measurements
   LGCConverters
   LGCProject
   LGCProjectData
   LSCalcObservations
   LSCalcObservationSet
   ReaderAndWriter
)

FOREACH(subproject ${LGC_SUBPROJECTS})
    ADD_SUBDIRECTORY(${subproject})
ENDFOREACH(subproject) 

# And also process the directory with the executable project:
ADD_SUBDIRECTORY(LGC++)

# Install the manual file
install(FILES "../Documentation/LGCHelp.pdf"
       DESTINATION "."
       COMPONENT helpfiles)
	

# Installer:
set(CPACK_PACKAGE_NAME "LGC")
set(CPACK_PACKAGE_VENDOR "CERN BE-ABP-SU")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LGC")

set(CPACK_PACKAGE_VERSION_MAJOR ${LGC_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${LGC_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${LGC_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.0${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_VENDOR "CERN SUSOFT")
set(CPACK_NSIS_HELP_LINK "https://espace.cern.ch/be-dep-ABP-SU/default.aspx")
set(CPACK_NSIS_URL_INFO_ABOUT "https://espace.cern.ch/be-dep-ABP-SU/default.aspx")
set(CPACK_NSIS_CONTACT "majones@cern.ch")
#set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION}")
#set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_HKLM_REGISTRY_KEY "${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_VERSION}")

# Directory name in the "Program Files"
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SUSOFT\\\\LGC\\\\${CPACK_PACKAGE_VERSION}")
#set(CPACK_NSIS_MODIFY_PATH ON)
# Disable asking for components (there are no subcomponents)
set(CPACK_MONOLITHIC_INSTALL TRUE)
# Define the application components: (Obsolete, uses monolithic install)
#set(CPACK_COMPONENTS_ALL application)
# And the component description
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "LGC")
# Elements of the menu start folder
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_PACKAGE_EXECUTABLES LGC "LGC ${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_NAME "LGC++Installer" )
# Icon in the add/remove control panel. Must be an .exe file 
set(CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_COMPONENT_APPLICATION_DISPLAY_NAME}.exe)

# For help file link
set(CPACK_COMPONENT_HELPFILES_DISPLAY_NAME "Help Files")
SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
Delete \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Helpfile.lnk\\\"
Delete \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\OnlineHelp.lnk\\\"
")

# Set installer icon (need .ico !!!)
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}\\\\..\\\\LGC.ico")
# set the package header icon for MUI
SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}\\\\..\\\\LGC.ico")


set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	SetOverwrite on
	ExecWait '\\\"$INSTDIR\\\\vcredist_x86.exe\\\"/install /quiet /norestart'
")

SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
    CreateShortcut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Helpfile.lnk' '$INSTDIR\\\\LGCHelp.pdf'
    CreateShortcut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\OnlineHelp.lnk' 'https://wikis.cern.ch/display/SUS/LGC+Guide+d+Utilisation'
    WriteRegStr HKLM 'SOFTWARE\\\\${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_HKLM_REGISTRY_KEY}' 'Path' '$INSTDIR'
    WriteRegStr HKLM 'SOFTWARE\\\\${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_HKLM_REGISTRY_KEY}' 'Product Name' '${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION}'
	WriteRegStr HKLM 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Un#install\\\\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}' 'DisplayName' '${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}'
	WriteRegStr HKLM 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Un#install\\\\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}' 'Publisher' 'SUSOFT'
")


# This must always be last!
include(CPack)
