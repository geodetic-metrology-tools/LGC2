# Installer:

set(CPACK_NSIS_HELP_LINK "https://readthedocs.web.cern.ch/display/SUS/LGC2+User+Guide")

# set the package header icon for MUI
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_LIST_DIR}\\..\\LGC.ico")
# Set installer icon (need .ico !!!)
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_LIST_DIR}\\..\\LGC.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_LIST_DIR}\\..\\LGC.ico")

SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
	CreateShortcut '$SMPROGRAMS\\$STARTMENU_FOLDER\\OnlineHelp.lnk' 'https://readthedocs.web.cern.ch/display/SUS/LGC2+User+Guide'
	WriteRegStr HKLM 'SOFTWARE\\${CPACK_PACKAGE_VENDOR}\\${CPACK_PACKAGE_HKLM_REGISTRY_KEY}' 'Path' '$INSTDIR'
	WriteRegStr HKLM 'SOFTWARE\\${CPACK_PACKAGE_VENDOR}\\${CPACK_PACKAGE_HKLM_REGISTRY_KEY}' 'Product Name' '${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION}'
	WriteRegStr HKLM 'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}' 'DisplayName' '${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}'
	WriteRegStr HKLM 'SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}' 'Publisher' '${CPACK_PACKAGE_VENDOR}'
")

SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
	Delete \"$SMPROGRAMS\\$STARTMENU_FOLDER\\OnlineHelp.lnk\"
")
