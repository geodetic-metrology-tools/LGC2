# Install script for directory: C:/Users/mcoppofr/susoft/LGC2/source

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/mcoppofr/susoft/LGC2/source/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xapplicationx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE EXECUTABLE FILES "C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/LGC.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibrariesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE PROGRAM FILES "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Redist/MSVC/14.24.28127/vc_redist.x64.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdocumentationx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./doc/" TYPE DIRECTORY FILES "C:/Users/mcoppofr/susoft/LGC2/source/../doc/")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlLogs/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlLSAlgorithms/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlPlugins/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlProjectFramework/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlSpatialObjects/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlSpatialObjDLL/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/svlTools/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/LGC_Core/cmake_install.cmake")
  include("C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/LGC_Tests/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
