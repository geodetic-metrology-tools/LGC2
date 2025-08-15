/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef __UTILS__
#define __UTILS__

#pragma once

#include <string>

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef __linux__    
	#include <unistd.h>
    #define GetCurrentDir getcwd
	#define slash "/"
#else
    #include <direct.h>
    #define GetCurrentDir _getcwd
	#define slash "\\"
#endif


namespace
{
	std::string getCurrentDirectory() { 
	
		char cCurrentPath[FILENAME_MAX];
		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
			return "";
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		return std::string(cCurrentPath);
	}
}


#endif // __UTILS__