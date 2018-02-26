#ifndef LGC_VERSION_H
#define LGC_VERSION_H

/* !! THIS FILE IS GENERATED, DO NOT EDIT !! */
/* !! CHANGE THE VARIABLES IN THE TOP-LEVEL CMAKE FILE INSTEAD !! */

#include <string>
#include <sstream>

#define LGC_VERSION_MAJOR 2
#define LGC_VERSION_MINOR 02
#define LGC_VERSION_PATCH 00

static std::string getLGCVersion(void) {
	stringstream ss;
	ss << "v" << LGC_VERSION_MAJOR  << "." << LGC_VERSION_MINOR << "." << LGC_VERSION_PATCH;
	
	return ss.str();
}

#endif
