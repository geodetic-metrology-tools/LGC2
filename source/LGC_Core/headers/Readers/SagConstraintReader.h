/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/


#ifndef _READERS_SAGCONSTRAINTREADER_H_
#define _READERS_SAGCONSTRAINTREADER_H_


//LGC
//#include <TInstrumentData.h>
#include <ReaderBase.h>
#include <KeywordRights.h>

/*!
 *  \addtogroup MeasurementReaders
 *  @{
 */

/*!
	\brief This is a class for handling asg constraint objects in the input file
*/
class TASagConstraintPairKey : public TAKeyWord
{
public:
	TASagConstraintPairKey(TLGCData &project, int nb_allowed_keywords = nb_allowed_sagConnections, const char **keywords = allowed_SAGCONNECT) : TAKeyWord(SAGCONNECT, project), fSagAccess(project.getSags())
	{
		for (int i(0); i < nb_allowed_keywords; i++)
			allowed_keywords.emplace_back(keywords[i]);
	}
	virtual void parse(const std::vector<std::string> &tokens, bool activeLine, int line);
	std::string fSagElementName;

private:
	TASagConstraintPairKey &operator=(const TASagConstraintPairKey &);	
	// access to sag element storage
	LGCAdjustableSagCollection &fSagAccess;

};
#endif