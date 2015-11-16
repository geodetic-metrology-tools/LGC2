////////////////////////////////////////////////////////////////////
// TObservationConverter.cpp
////////////////////////////////////////////////////////////////////////////////////
#include "TObservationWriter.h"
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "TLGCObsSummary.h"
#include "TLGCData.h" 


TObservationWriter::TObservationWriter(TAStreamFormatter& stream):
TALGCObjectWriter(stream)
{
}

TObservationWriter::~TObservationWriter()
{//destructor
}

// write an observation table title
void	TObservationWriter::writeObsTitle(const string& description, const int numObs)
{
	TAStreamFormatter& stream = getStreamRef();
	//Title
	stream<<description<<getSeparator()<<"(NB. = "<<numObs<<" )"<<getSeparator()<<endl;

	return;
}

void TObservationWriter::writeAngleResultsSummary(TLGCObsSummary summary, std::string TABs){
	int					obsResWidth = getObsResWidth();
	int					angleResidualPrecision = max(getAngleResidualPrecision()-3, 0);
	TAStreamFormatter	&stream = getStreamRef();

	//Write statistic
	// mean residual
	stream << TABs;
	stream << "RESIDU MOYEN =  "; 
	stream.width( obsResWidth );
	stream.precision(angleResidualPrecision);
	stream << summary.getMean();
	stream << getSeparator();

	// mean residual statistical test limits
	stream<<" CC :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(angleResidualPrecision);
	stream << summary.getMeanLoLimit()<<", ";
	stream.precision(angleResidualPrecision);
	stream << summary.getMeanHiLimit()<<")";
	stream << endl;

	// variance of residuals
	stream << TABs;
	stream<<"ECART-TYPE   =  ";
	stream.width( obsResWidth );
	stream.precision(angleResidualPrecision);
	stream << summary.getVariance() ;
	stream << getSeparator();

	//variance of residuals statistical test limits
	stream<<" CC :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(angleResidualPrecision);
	stream << summary.getVarLoLimit()<<", ";
	stream.precision(angleResidualPrecision);
	stream << summary.getVarHiLimit()<<")";
	stream << getSeparator();
	stream << endl << endl <<endl;

	//Histogram is not yet implemented, ToDo:
/*
	 write histograms for the observation residuals
	writeResidualsHistogram(summary, description);
	stream << endl;
*/
}


void TObservationWriter::writeDistanceResultsSummary(TLGCObsSummary summary, std::string TABs){

	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = max(getLengthResidualPrecision()-2, 0);
	TAStreamFormatter	&stream = getStreamRef();
	//Write statistic
	stream << TABs;

	// mean residual
	stream<<"RESIDU MOYEN =  "; 
	stream.width( obsResWidth );
	stream.precision(lengthResidualPrecision);
	stream << summary.getMean();
	stream<<getSeparator();

	// mean residual statistical test limits
	stream<<" MM :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(lengthResidualPrecision);
	stream << summary.getMeanLoLimit()<<", ";
	stream.precision(lengthResidualPrecision);
	stream << summary.getMeanHiLimit()<<")";
	stream<<endl;

	stream << TABs;
	// variance of residuals
	stream<<"ECART-TYPE   =  ";
	stream.width( obsResWidth );
	stream.precision(lengthResidualPrecision);
	stream << summary.getVariance() ;
	stream<<getSeparator();

	//variance of residuals statistical test limits
	stream<<" MM :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(lengthResidualPrecision);
	stream << summary.getVarLoLimit()<<", ";
	stream.precision(lengthResidualPrecision);
	stream << summary.getVarHiLimit()<<")";
	stream<<getSeparator();
	stream<<endl<<endl<<endl;;

	//Histogram is not yet implemented, ToDo:
/*
	 write histograms for the observation residuals
	writeResidualsHistogram(summary, description);
	stream << endl;
*/
	return;
}


void TObservationWriter::writeUnitlessResultsSummary(TLGCObsSummary summary, std::string TABs){

	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = max(getLengthResidualPrecision()-3, 0);
	TAStreamFormatter	&stream = getStreamRef();
	//Write statistic
	stream << TABs;

	// mean residual
	stream<<"RESIDU MOYEN =  "; 
	stream.width( obsResWidth );
	stream.precision(lengthResidualPrecision);
	stream << summary.getMean();
	stream<<getSeparator();

	// mean residual statistical test limits
	stream<<" [] :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(lengthResidualPrecision);
	stream << summary.getMeanLoLimit()<<", ";
	stream.precision(lengthResidualPrecision);
	stream << summary.getMeanHiLimit()<<")";
	stream<<endl;

	stream << TABs;
	// variance of residuals
	stream<<"ECART-TYPE   =  ";
	stream.width( obsResWidth );
	stream.precision(lengthResidualPrecision);
	stream << summary.getVariance() ;
	stream<<getSeparator();

	//variance of residuals statistical test limits
	stream<<" [] :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(lengthResidualPrecision);
	stream << summary.getVarLoLimit()<<", ";
	stream.precision(lengthResidualPrecision);
	stream << summary.getVarHiLimit()<<")";
	stream<<getSeparator();
	stream<<endl<<endl<<endl;;

	//Histogram is not yet implemented, ToDo:
/*
	 write histograms for the observation residuals
	writeResidualsHistogram(summary, description);
	stream << endl;
*/
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationWriter::writeReliabilityHeader(	string name1,
														string name2,
														string name3,
														string Observation,
														string unit1,
														string unit2)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();


	////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).writeStringLeft(nameWidth,	name1); // name of the first point
	(*stream).writeStringLeft(nameWidth,	name2); // name of the second point
	(*stream).writeStringLeft(nameWidth,	name3); // name of the third point
	(*stream).writeString(obsWidth,	Observation); //mesured
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsResWidth,	"RESIDU"); //residue 
	(*stream).writeString(obsResWidth,			"ZI");//local reliability statistic zi
	(*stream).writeString(3,			"");//zi flag
	(*stream).writeString(8,			"WI");//Gross-error detection statistics wi
	(*stream).writeString(3,			"");//wi flag
	(*stream).writeString(obsResWidth,			"GI");//Estimation of the gross-error gi
	(*stream).writeString(obsResWidth,			"NABLA");// Greatest undetected error NABLA
	(*stream).writeString(obsResWidth,			"TI");//Level of ease a gross error of size NABLA can be detected: Ti
	(*stream).writeString(obsResWidth,			"DELTY");//Maximum effect of an undetected gross-error: DELTY
	(*stream)<<endl;


	////////////////////////////////////////////////////////////////////////////////////
	//Second line
	(*stream).writeString(nameWidth,	""); // name of the first point
	(*stream).writeString(nameWidth,	""); // name of the second point
	(*stream).writeString(nameWidth,	""); // name of the third point (third point's name vs third point name)
	(*stream).writeString(obsWidth,	unit1); //mesured
	(*stream).writeString(obsResWidth,	unit2); //sigma
	(*stream).writeString(obsResWidth,	unit2); //residue 
	(*stream).writeString(obsResWidth,			"(%)");//local reliability statistic zi
	(*stream).writeString(3,			"");//zi flag
	(*stream).writeString(8,			"");//Gross-error detection statistics wi
	(*stream).writeString(3,			"");//wi flag
	(*stream).writeString(obsResWidth,			unit2);//Estimation of the gross-error gi
	(*stream).writeString(obsResWidth,			unit2);// Greatest undetected error NABLA
	(*stream).writeString(obsResWidth,			"");//Maximum effect of an undetected gross-error: DELTY
	(*stream)<<endl<<endl;

	return;
}

void TObservationWriter::writeReliability(int index, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					obsResWidth = getObsResWidth();
	string				separator = getSeparator();

	// Test the computing: if it failed, write the corresponding error messages
	if (stat.getAreDetermined())
	{
		//get zi
		TReal z = stat.getZi().coeff(index);
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
		if (stat.getWToCompute())
		{
			(*stream).writeDouble(8,2, stat.getWi().coeff(index));
		}else{
			(*stream).writeString(8,"");
		}
	
		//get gi	
		if (stat.getGToCompute())
		{
			(*stream).writeString(3,"**");
			(*stream).writeDouble(obsResWidth,2, stat.getGi().coeff(index) );
		}else{
			(*stream).writeString(3,"");
			(*stream).writeString(obsResWidth,"");
		}
		
		// get nabla
		if (stat.getDeltaComputed()) 
		{ //tests that the nabla has been correctly computed
			(*stream).writeDouble(obsResWidth,2, stat.getNabla().coeff(index));
		}else{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

		// get T
		(*stream).writeDouble(obsResWidth,2, stat.getTi().coeff(index));
		
		// get DELTY
		stream->width(10);
		if (stat.getDeltaComputed())/*tests that the delty has been correctly computed*/
		{ 
			(*stream).writeDouble(obsResWidth,2, stat.getDelty().coeff(index));
		}else{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

	}else { 
			(*stream).writeString(obsResWidth,"0.0");//z
			(*stream).writeString(3,"");
			(*stream).writeString(8,"INDETERMINE");//wi
			(*stream).writeString(0,"");
			(*stream).writeString(obsResWidth,"");//gi
			(*stream).writeString(obsResWidth,"");//nabla
			(*stream).writeString(obsResWidth,"INFINI");//T
			(*stream).writeString(obsResWidth,"");//DELTY
	}

	(*stream)<<endl;
}
// Get the French version of the observation description
// returns a string with the description
string	TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"POLAR3D",
		"Unit Vector Measurment (UVEC)",
		"Unit Vector Measurment + Distance (UVD)",
		"ANGLES HORIZONTAUX",
		"DISTANCES ZENITHALES",
		"DISTANCES MESUREES (DIST)",
		"ECART A UN PLAN VERTICAL (ECTH)",
		"DISTANCES HORIZONTALES",
		"DISTANCES SPATIALES (DSPT)",
		"DISTANCES VERTICALES (DLEV)",
		"DISTANCES VERTICALES (DVER)",
		"ECARTS DANS LE PLAN HORIZONTAL (ECHO)",
		"ECARTS DANS LE PLAN (ECSP)",
		"ECARTS DANS LE PLAN VERTICAL (ECVE)",
		"ORIENTATIONS"
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCObjectWriter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}


// Get the English version of the observation description
// returns a string with the description
string	TObservationWriter::getObsDescriptionEN(TALGCObjectWriter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"POLAR3D",
		"Unit Vector Measurment (UVEC)",
		"Unit Vector Measurement + Distance (UVD)",
		"HORIZONTAL ANGLE OBSERVATIONS",
		"ZENITHAL DISTANCE OBSERVATIONS",
		"OBSERVED SPATIAL DISTANCES (DIST)",
		"OFFSETS TO A THEODOLITE PLAN (ECTH)",
		"HORIZONTAL DISTANCES",
		"SPATIAL DISTANCES (DSPT)",
		"VERTICAL DISTANCES (DLEV)",
		"VERTICAL DISTANCES (DVER)",
		"OFFSETS IN AN HORIZONTAL PLAN (ECHO)",
		"OFFSETS TO A SPATIAL PLAN (ECSP)",
		"OFFSETS TO A VERTICAL PLAN (ECVE)",
		"ORIENTATIONS"
		//"NoObs"
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCObjectWriter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}


