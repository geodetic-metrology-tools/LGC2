# Installer:
set(CPACK_PACKAGE_NAME "LGC")
set(CPACK_PACKAGE_VENDOR "CERN EN-MEF-SU")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LGC")

set(CPACK_PACKAGE_VERSION_MAJOR ${LGC_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${LGC_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${LGC_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_VENDOR "CERN SUSOFT")
set(CPACK_NSIS_HELP_LINK "https://espace.cern.ch/be-dep-ABP-SU/default.aspx")
set(CPACK_NSIS_URL_INFO_ABOUT "https://espace.cern.ch/be-dep-ABP-SU/default.aspx")
set(CPACK_NSIS_CONTACT "Mark.Jones@cern.ch")
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
set(CPACK_COMPONENTS_ALL application)
# And the component description
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "LGC")
# Elements of the menu start folder
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_PACKAGE_EXECUTABLES LGC "LGC ${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_NAME "LGC2Installer" )
# Icon in the add/remove control panel. Must be an .exe file 
set(CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_COMPONENT_APPLICATION_DISPLAY_NAME}.exe)
# Set installer icon (need .ico !!!)
set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}\\\\..\\\\LGC.ico")
# set the package header icon for MUI
SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}\\\\..\\\\LGC.ico")
#SET(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_SOURCE_DIR}\\\\RabotGUI\\\\resources\\\\su_icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME ${CPACK_COMPONENT_APPLICATION_DISPLAY_NAME}.exe)

SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
Delete \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Helpfile.lnk\\\"
Delete \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\OnlineHelp.lnk\\\"
")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	SetOverwrite on
	ExecWait '\\\"$INSTDIR\\\\vcredist_x86.exe\\\"/install /quiet /norestart'
")

SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
    CreateShortcut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Helpfile.lnk' '$INSTDIR\\\\LGCHelp.pdf'
    CreateShortcut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\OnlineHelp.lnk' 'https://wikis.cern.ch/display/SUS/LGC+Guide+d+Utilisation'
    WriteRegStr HKLM 'SOFTWARE\\\\${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_HKLM_REGISTRY_KEY}' 'Path' '$INSTDIR'
    WriteRegStr HKLM 'SOFTWARE\\\\${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_HKLM_REGISTRY_KEY}' 'Product Name' '${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION}'
	WriteRegStr HKLM 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}' 'DisplayName' '${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}'
	WriteRegStr HKLM 'SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}' 'Publisher' 'SUSOFT'
")

# This must always be last!
include(CPack)