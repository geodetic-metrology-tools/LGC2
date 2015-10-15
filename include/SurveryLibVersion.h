#ifndef SURVEYLIB_VERSION_H
#define SURVEYLIB_VERSION_H

#include <string>
#include <cstdio>

#define SurveyLib_VERSION_MAJOR 3
#define SurveyLib_VERSION_MINOR 3
#define SurveyLib_VERSION_PATCH 6

static std::string getSurveyLibVersion(void) {
	static char str[128];
	_snprintf(str, 128, "v%d.%02d.%02d", SurveyLib_VERSION_MAJOR, SurveyLib_VERSION_MINOR, SurveyLib_VERSION_PATCH);
	return std::string(str);
}

#endif
