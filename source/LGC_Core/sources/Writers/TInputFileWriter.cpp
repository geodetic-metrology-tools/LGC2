////////////////////////////////////////////////////////////////////
// TInputFileWriter.cpp : implementation class
// Write an LGC "input" file from the current virtual LGC data
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TInputFileWriter.h"

#include "TSeparatedFormatTStream.h"
#include "TLGCData.h"

//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////

TInputFileWriter::TInputFileWriter() : TSimFileWriter(){}


TInputFileWriter::TInputFileWriter(TAStreamFormatter* stream, const TLGCData* project)
    : TSimFileWriter(stream, project){}


TInputFileWriter::~TInputFileWriter(){}


/////////////////////////////
// public member functions
/////////////////////////////

void TInputFileWriter::writeFile(){
    TSimFileWriter::writeFile();
}

void TInputFileWriter::writeFile(const std::string error){
    //write error messages from project
    TSimFileWriter::writeFile(error);
}

/////////////////////////////
// protected member functions
/////////////////////////////

void TInputFileWriter::writeHeader()
{
	TAStreamFormatter* stream = getStream();
    const auto &config = data->getConfig();
	
	(*stream) << "*TITR" << endl;
	(*stream) << config.title << endl;

    // Ref system:
    if(config.referential == TRefSystemFactory::kCernXYHg85Machine)
		(*stream) << "*LEP" << endl;
    else if(config.referential == TRefSystemFactory::kCERNXYHsSphereSPS)
		(*stream) << "*SPHE" << endl;
    else if(config.referential == TRefSystemFactory::kCernXYHg00Machine)
		(*stream) << "*RS2K" << endl;
	else
		(*stream) << "*OLOC" << endl;

    // Calculation options:
	if (config.allfixed.isActive())
		(*stream) << "*ALLFIXED" << endl;
	else if (config.libre.isActive())
		(*stream) << "*LIBR" << endl;

    if(config.sim.isActive() || config.sim.numSims != 0){
        if(!config.sim.isActive())
            (*stream) << DEACTIVATION_CHAR;

        (*stream) << "*SIMU " << config.sim.numSims << endl;
    }

    // Output options:
    if(config.useApriori.isActive())
        (*stream) << "*APRI" << endl;

    if(config.covar.isActive())
        (*stream) << "*COVAR" << endl;

    if(config.writeDefa.isActive())
        (*stream) << "*DEFA" << endl;

    if (!config.fRelErrors.points.empty())
	{
		(*stream) << "*EREL" << endl;
		for (const auto &erelTuple : config.fRelErrors.points)
		{
			(*stream) << erelTuple.getPoint1() << "  " << erelTuple.getPoint2() << "  " << erelTuple.getDestinationFrame() << endl;
		}
	}
    if (!config.fRelErrors.frames.empty())
	{
		(*stream) << "*ERELFRAME" << endl;
		for (const auto &erelTuple : config.fRelErrors.frames)
		{
			(*stream) << erelTuple.getFromFrame() << "  " << erelTuple.getToFrame() << endl;
		}
	}


    if(config.faut.isActive() || config.faut.alpha != FAUT_DEF_ALPHA || config.faut.beta != FAUT_DEF_BETA){
        if(!config.faut.isActive())
            (*stream) << DEACTIVATION_CHAR;

        (*stream) << "*FAUT " << config.faut.alpha << "  " << config.faut.beta << endl;
    }

    if(config.CustomOutputSeparatorPunch.isActive() || config.CustomOutputSeparatorPunch.separator != ""){
        if(!config.CustomOutputSeparatorPunch.isActive())
            (*stream) << DEACTIVATION_CHAR;

        (*stream) << "*FMTP SEP \"" << config.CustomOutputSeparatorPunch.separator << "\"" << endl;
    }

	if (config.histo.isActive())
		(*stream) << "*HIST" << endl;

	if (config.nodup.isActive())
		(*stream) << "*NODUP" << endl;

	if (config.outPrecision.digits != 5 )
		(*stream) << "*PREC " << config.outPrecision.digits <<  endl;

	if (config.errorEllipses.isActive())
		(*stream) << "*PRES" << endl;

    if(config.writePunch.isActive() || config.writePunch.fmode != TLGCConfig::TCoordOut::eMode::kPLAIN){
        if(!config.writePunch.isActive())
            (*stream) << DEACTIVATION_CHAR;

        if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kE)
            (*stream) << "*PUNC E" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kEE)
            (*stream) << "*PUNC EE" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kH)
            (*stream) << "*PUNC H" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kHN)
            (*stream) << "*PUNC HN" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kHZ)
            (*stream) << "*PUNC HZ" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kZ)
            (*stream) << "*PUNC Z" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kZHN)
            (*stream) << "*PUNC ZHN" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kT)
            (*stream) << "*PUNC T" << endl;
        else if(config.writePunch.fmode == TLGCConfig::TCoordOut::eMode::kOUT1)
            (*stream) << "*PUNC OUT1" << endl;
        else
            (*stream) << "*PUNC" << endl;
    }
    
    if(config.sim.writeLGCFile)
        (*stream) << "*SOBS" << endl;
}
