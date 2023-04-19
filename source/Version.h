#ifndef LGC_VERSION_H
#define LGC_VERSION_H

/* !! THIS FILE IS GENERATED, DO NOT EDIT !! */
/* !! CHANGE THE VARIABLES IN THE TOP-LEVEL CMAKE FILE INSTEAD !! */

#include <string>
#include <sstream>

#define LGC_VERSION_MAJOR "2"
#define LGC_VERSION_MINOR "6"
#define LGC_VERSION_PATCH "beta_5-SLAVE_Frames"

static std::string getLGCVersion(void) {
	std::stringstream ss;
	ss << "v" << LGC_VERSION_MAJOR  << "." << LGC_VERSION_MINOR << "." << LGC_VERSION_PATCH;
	
	return ss.str();
}

#endif
