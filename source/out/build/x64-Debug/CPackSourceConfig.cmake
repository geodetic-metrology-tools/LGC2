# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BINARY_7Z "")
set(CPACK_BINARY_BUNDLE "")
set(CPACK_BINARY_CYGWIN "")
set(CPACK_BINARY_DEB "")
set(CPACK_BINARY_DRAGNDROP "")
set(CPACK_BINARY_FREEBSD "")
set(CPACK_BINARY_IFW "")
set(CPACK_BINARY_NSIS "")
set(CPACK_BINARY_NUGET "")
set(CPACK_BINARY_OSXX11 "")
set(CPACK_BINARY_PACKAGEMAKER "")
set(CPACK_BINARY_PRODUCTBUILD "")
set(CPACK_BINARY_RPM "")
set(CPACK_BINARY_STGZ "")
set(CPACK_BINARY_TBZ2 "")
set(CPACK_BINARY_TGZ "")
set(CPACK_BINARY_TXZ "")
set(CPACK_BINARY_TZ "")
set(CPACK_BINARY_WIX "")
set(CPACK_BINARY_ZIP "")
set(CPACK_BUILD "ON")
set(CPACK_BUILD_SOURCE_DIRS "C:/Users/mcoppofr/susoft/LGC2/source;C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug")
set(CPACK_CMAKE_GENERATOR "Ninja")
set(CPACK_COMPONENTS_ALL "application;documentation;libraries")
set(CPACK_COMPONENTS_ALL_SET_BY_USER "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_CREATE_DESKTOP_LINKS "LGC")
set(CPACK_GENERATOR "7Z;ZIP")
set(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_INSTALLED_DIRECTORIES "C:/Users/mcoppofr/susoft/LGC2/source;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "")
set(CPACK_INSTALL_PREFIX "C:/Users/mcoppofr/susoft/LGC2/source/out/install/x64-Debug")
set(CPACK_MODULE_PATH "C:/Users/mcoppofr/susoft/LGC2/source/CMake/Modules")
set(CPACK_NSIS_CONTACT "susoft.support@cern.ch")
set(CPACK_NSIS_DEFINES "!define MUI_STARTMENUPAGE_DEFAULTFOLDER \"SUSoft\\LGC\\2.03.02-dev\"")
set(CPACK_NSIS_DISPLAY_NAME "LGC 2.03.02-dev")
set(CPACK_NSIS_DISPLAY_NAME_SET "TRUE")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	ExecWait '\"\$INSTDIR\\vc_redist.x64.exe\" /install /quiet /norestart'
	WriteRegStr HKLM 'SOFTWARE\\CERN SUSoft - EN-SMM-APC\\LGC\\2.03.02-dev' 'Path' '\$INSTDIR'
	WriteRegStr HKLM 'SOFTWARE\\CERN SUSoft - EN-SMM-APC\\LGC\\2.03.02-dev' 'Product Name' 'LGC 2.03.02-dev'
	WriteRegStr HKLM 'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LGC 2.03.02-dev' 'DisplayName' 'LGC 2.03.02-dev'
	WriteRegStr HKLM 'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LGC 2.03.02-dev' 'Publisher' 'CERN SUSoft - EN-SMM-APC'
;
	CreateShortcut '\$SMPROGRAMS\\\$STARTMENU_FOLDER\\OnlineHelp.lnk' 'https://readthedocs.web.cern.ch/display/SUS/LGC2+User+Guide'
	WriteRegStr HKLM 'SOFTWARE\\CERN SUSoft - EN-SMM-APC\\LGC\\2.03.02-dev' 'Path' '\$INSTDIR'
	WriteRegStr HKLM 'SOFTWARE\\CERN SUSoft - EN-SMM-APC\\LGC\\2.03.02-dev' 'Product Name' 'LGC 2.03.02-dev 2.03.02-dev'
	WriteRegStr HKLM 'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LGC 2.03.02-dev' 'DisplayName' 'LGC 2.03.02-dev'
	WriteRegStr HKLM 'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LGC 2.03.02-dev' 'Publisher' 'CERN SUSoft - EN-SMM-APC'
")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
	Delete \"\$SMPROGRAMS\\\$STARTMENU_FOLDER\\OnlineHelp.lnk\"
")
set(CPACK_NSIS_HELP_LINK "https://readthedocs.web.cern.ch/display/SUS/LGC2+User+Guide")
set(CPACK_NSIS_INSTALLED_ICON_NAME ".exe")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "\$PROGRAMFILES64")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_NSIS_MUI_ICON "C:/Users/mcoppofr/susoft/LGC2/source\\..\\LGC.ico")
set(CPACK_NSIS_MUI_UNIICON "C:/Users/mcoppofr/susoft/LGC2/source\\..\\LGC.ico")
set(CPACK_NSIS_PACKAGE_NAME "LGC 2.03.02-dev")
set(CPACK_NSIS_URL_INFO_ABOUT "https://readthedocs.web.cern.ch/display/SUS/User+Guides")
set(CPACK_OUTPUT_CONFIG_FILE "C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Users/mcoppofr/susoft/LGC2/source/../README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LGC 2.03.02-dev")
set(CPACK_PACKAGE_EXECUTABLES "LGC;LGC 2.03.02-dev")
set(CPACK_PACKAGE_FILE_NAME "LGC-2.03.02-dev-Source")
set(CPACK_PACKAGE_HKLM_REGISTRY_KEY "LGC\\2.03.02-dev")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://readthedocs.web.cern.ch/display/SUS/Home")
set(CPACK_PACKAGE_ICON "C:/Users/mcoppofr/susoft/LGC2/source\\..\\LGC.ico")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SUSoft\\LGC\\2.03.02-dev")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "LGC 2.03.02-dev")
set(CPACK_PACKAGE_NAME "LGC")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "CERN SUSoft - EN-SMM-APC")
set(CPACK_PACKAGE_VERSION "2.03.02-dev")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "03")
set(CPACK_PACKAGE_VERSION_PATCH "02-dev")
set(CPACK_RESOURCE_FILE_LICENSE "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.15/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "C:/Users/mcoppofr/susoft/LGC2/source/../README.md")
set(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.15/Templates/CPack.GenericWelcome.txt")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "ON")
set(CPACK_SOURCE_CYGWIN "")
set(CPACK_SOURCE_GENERATOR "7Z;ZIP")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "C:/Users/mcoppofr/susoft/LGC2/source;/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "LGC-2.03.02-dev-Source")
set(CPACK_SOURCE_RPM "")
set(CPACK_SOURCE_TBZ2 "")
set(CPACK_SOURCE_TGZ "")
set(CPACK_SOURCE_TOPLEVEL_TAG "win64-Source")
set(CPACK_SOURCE_TXZ "")
set(CPACK_SOURCE_TZ "")
set(CPACK_SOURCE_ZIP "ON")
set(CPACK_STRIP_FILES "")
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_TOPLEVEL_TAG "win64-Source")
set(CPACK_VERBATIM_VARIABLES "TRUE")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "C:/Users/mcoppofr/susoft/LGC2/source/out/build/x64-Debug/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
