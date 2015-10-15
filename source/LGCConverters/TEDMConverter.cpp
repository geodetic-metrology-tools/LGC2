#include "TEDMConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"


TEDMConverter::TEDMConverter(TAStreamFormatter& stream):TObservationConverter(stream)
{}

TEDMConverter::~TEDMConverter(){}

//Result
void	TEDMConverter::writeResultsHeader(const int)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT 1"); //first point's Name
	(*stream).writeStringLeft(nameWidth,	"POINT 2"); //second point's Name
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured distance
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated distance
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	//(*stream).writeString(obsResWidth,	"RES/SIG"); //offset/sigma
	//(*stream).writeString(obsResWidth,	"SENSI"); //sensitivity
	(*stream).writeString(obsWidth,	"HAUTEUR"); //instrument's height 
	(*stream).writeString(obsWidth,	"HAUTEUR"); //prism's height 
	(*stream).writeString(obsResWidth,	"CONST"); //distance's constant 
	(*stream).writeString(obsResWidth,	"SCONST"); //sigma distance's constant 
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	"(M)"); //mesured distance
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsWidth,	"(M)"); //estimated distance
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	//(*stream).writeString(obsResWidth,	""); //offset/sigma
	//(*stream).writeString(obsResWidth,	"BILITE"); //sensitivity
	(*stream).writeString(obsWidth,	"D'INST"); //instrument's height 
	(*stream).writeString(obsWidth,	"DE PRISM"); //prism's height 
	(*stream).writeString(obsResWidth,	"(MM)"); //distance's constant 
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance's constant 
	(*stream)<<endl;
	
	///////////////////////////////////////////////////////////////////////////////////
	//Third line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(nameWidth,	""); //second point's Name
	(*stream).writeString(obsWidth,	""); //mesured distance
	(*stream).writeString(obsResWidth,	""); //sigma
	(*stream).writeString(obsWidth,	""); //estimated distance
	(*stream).writeString(obsResWidth,	""); //offset (mm)
	//(*stream).writeString(obsResWidth,	""); //offset/sigma
	//(*stream).writeString(obsResWidth,	"(MM/CM)"); //sensitivity
	(*stream).writeString(obsWidth,	"(M)"); //instrument's height 
	(*stream).writeString(obsWidth,	"(M)"); //prism's height 
	(*stream)<<endl<<endl;
	
	return;
}



void	TEDMConverter::writeResults(const TEDM& fEdm)
{
		TAStreamFormatter*	stream = getStream();
		int					nameWidth = getNameWidth();
		int					obsWidth = getObsWidth();
		int					obsResWidth = getObsResWidth();
		int					lengthResidualPrecision = getLengthResidualPrecision();
		int					lengthPrecision =	getLengthPrecision();
		string				separator = getSeparator();

	//For each DSPT measurement of the station
	for(auto const& ItDspt : fEdm.measDSPT)
	{
	//if(obsIt->getHInstStatus() == fStatus)
	//{

		//write Point 1
		(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());

		//write Point 2
		(*stream).writeStringLeft(nameWidth, ItDspt.target.ID);

		//write the observed distance
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<ItDspt.getScalar()<<(separator);

		//write the sigma
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(lengthResidualPrecision);
		(*stream)<<(separator)<<(separator);

		//write the estimated distance
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<(ItDspt.getScalar()+ItDspt.getScalarResidual())<<(separator);

		//write the offset (mm) after calculation
		stream->setWidthFormat(obsResWidth);
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setPrecisionFormat(lengthResidualPrecision);
		(*stream)<<(ItDspt.getScalarResidual()*0.001)<<(separator);

		//write the offset / sigma (TDouble (MM))
		//stream->setPrecisionFormat(2);
		//(*stream)<<(obsIt->getResDivSig())<<separator;

		//write the sensitivity (TReal (MM/CM))
		//(*stream)<<(obsIt->getSensitivity())<<separator;

		//write the instrument's height (TLength (M))
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<</*ItDspt.targetPos-><<*/separator;

		//write the target's height (TLength (M))
		(*stream)<<(ItDspt.target.targetHt)<<separator;

		//write the distance cste (TLength (MM))
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(obsResWidth);
		(*stream)<<(ItDspt.target.distCorrectionValue)*0.001<<separator;

		//write the distance cste sigma (TLength (MM))
		(*stream)<<(ItDspt.target.sigmaDCorr)*0.001<<separator<<endl;
		(*stream).setDataSpacing();
	}

	return;
}

//Reliability
void TEDMConverter::writeReliabilityData(const TEDM& fEdm)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();


	// get Stn point
	(*stream).writeStringLeft(nameWidth, fEdm.instrumentPos->getName());

	//For each DSPT measurement of the station
	for(auto const& ItDspt : fEdm.measDSPT)
	{
		//get Tg point
		//(*stream).writeStringLeft(nameWidth, fEdm.instrument.targets);

		//get the observed distance
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream) << ItDspt.getScalar() << separator;

		//get the residual
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(lengthResPrecision);
		(*stream) << ItDspt.getScalarResidual() << separator;

#if 0
	// Test the computing: if it failed, write the corresponding error messages
	if (obs.paramsCanBeDetermined()) 
	{
		//get zi
		TReal z = obs.getZ();
		stream->setf(ios::fixed,ios::floatfield);
		// checks that z is between 25% and 60%, and if not, adds some symbols
		(*stream).writeDouble(obsResWidth,1,100*z);
				
		if (z < LITERAL(0.25)) 
		{	(*stream).writeString(3,"**");}	

		if (z > LITERAL(0.60)) 
		{	(*stream).writeString(3,"!");}	

		if (z>=LITERAL(0.25) && z<=LITERAL(0.60))
		{	(*stream).writeString(3,"");}
		
		//get wi
		if (obs.wToCompute())
		{
			(*stream).writeDouble(8,2, obs.getW());
		}
		else
		{
			(*stream).writeString(8,"");
		}
	
		//get gi	
		if (obs.gToCompute())
		{
			(*stream).writeString(3,"**");
			(*stream) << right << obs.getG() << separator;
		}
		else
		{
			(*stream).writeString(3,"");
			(*stream).writeString(obsResWidth,"");
		}
		
		// get nabla
		if (obs.deltaComputed()) 
		{ //tests that the nabla has been correctly computed
			(*stream) << right << obs.getNabla() << separator;
		}
		else
		{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

		// get T
		(*stream).writeDouble(obsResWidth,2, obs.getT());
		
		// get DELTY
		stream->width(10);
		if (obs.deltaComputed())/*tests that the delty has been correctly computed*/
		{ 
			(*stream).writeDouble(obsResWidth,2, obs.getDelty());
		}
		else
		{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

	}

	else 
	{ 
#endif
		// write "ind" (indetermined) instead of values
		// Is Zi = LITERAL(0.0)?
//		if ((obs.getResVariance().getValue() == LITERAL(0.0)) || (obs.getZ() < LITERAL(0.0001)))
//		{
			(*stream).writeString(obsResWidth,"0.0");//z
			(*stream).writeString(3,"");
			(*stream).writeString(8,"INDETERMINE");//wi
			(*stream).writeString(0,"");
			(*stream).writeString(obsResWidth,"");//gi
			(*stream).writeString(obsResWidth,"");//nabla
			(*stream).writeString(obsResWidth,"INFINI");//T
			(*stream).writeString(obsResWidth,"");//DELTY
//		}
//		else 
//		{ // Zi is undetermined
//			(*stream).writeString(obsResWidth,"INFINI");//z
//			(*stream).writeString(3,"");
//			(*stream).writeString(8,"***");//wi
//			(*stream).writeString(3,"");
//			(*stream).writeString(obsResWidth,"***");//gi
//			(*stream).writeString(obsResWidth,"***");//nabla
//			(*stream).writeString(obsResWidth,"***");//T
//			(*stream).writeString(obsResWidth,"***");//DELTY
//		}
//	}
	(*stream)<<endl;
//	(*stream).setDataSpacing();
	}
	return;

}

void	TEDMConverter::writeReliabilityHeader()
{this->TObservationConverter::writeReliabilityHeader("STATION",
														"TARGET",
														"OBSERVE",
														"M",
														"RESIDUAL",
														"MM");
	return;
}