#ifndef LGC_VERSION_H
#define LGC_VERSION_H

/* !! THIS FILE IS GENERATED, DO NOT EDIT !! */
/* !! CHANGE THE VARIABLES IN THE TOP-LEVEL CMAKE FILE INSTEAD !! */

#define LGC_VERSION_MAJOR 2
#define LGC_VERSION_MINOR 0
#define LGC_VERSION_PATCH 0

static std::string getLGCVersion(void) {
	static char str[128];
	_snprintf(str, 128, "v%d.%02d.%02d", LGC_VERSION_MAJOR, LGC_VERSION_MINOR, LGC_VERSION_PATCH);
	return std::string(str);
}

#endif
