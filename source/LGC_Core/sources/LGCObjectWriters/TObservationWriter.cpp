////////////////////////////////////////////////////////////////////
// TObservationConverter.cpp
////////////////////////////////////////////////////////////////////////////////////
#include <TObservationWriter.h>
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include "TLGCObsSummary.h"
#include <TLGCData.h> 


TObservationWriter::TObservationWriter(TAStreamFormatter& stream):
TALGCObjectWriter(stream)
{
}

TObservationWriter::~TObservationWriter()
{//destructor
}

// write an observation table title
void	TObservationWriter::writeObsTitle(const std::string& description, const int numObs)
{
	TAStreamFormatter& stream = getStreamRef();
	//Title
	stream<<description<<getSeparator()<<"(NB. = "<<numObs<<" )"<<getSeparator()<<endl;

	return;
}

void TObservationWriter::writeAngleResultsSummary(const TLGCObsSummary& summary, std::string TABs){
	int					obsResWidth = getObsResWidth();
	int					angleResidualPrecision = std::max(getAngleResidualPrecision()-3, 0);
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
	stream << endl <<endl;

}

void TObservationWriter::writeDistanceResultsSummary(const TLGCObsSummary& summary, std::string TABs){

	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = std::max(getLengthResidualPrecision()-2, 0);
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
	stream<<endl<<endl;;

	return;
}

void TObservationWriter::writeUnitlessResultsSummary(const TLGCObsSummary& summary, std::string TABs){

	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = std::max(getLengthResidualPrecision()-3, 0);
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
	stream<<endl<<endl;;

	//Histogram is not yet implemented, ToDo:
/*
	 write histograms for the observation residuals
	writeResidualsHistogram(summary, description);
	stream << endl;
*/
	return;
}

void TObservationWriter::writeHisto(const TLGCObsSummary& summary, std::string description)
{

	// return if histograms are not to be written out, or if there are too
	// few observations to generate a meaningful graph
	if (summary.getNumberOfObs() >= 5)
	{
		TAStreamFormatter &stream = getStreamRef();

		// write out the histogram title and scale factor
		{
			int k = summary.getHistoScale();
			std::string ech = "";

			if (description == "ANGL" ||
				description == "ZEND" ||
				description == "ORIE" ||
				description == "PLR3D: ZEND"||
				description == "PLR3D: ANGL")/* Angle*/
				ech = "(CC)";
			else if (description == "UVD: XVEC" ||
				description == "UVD: YVEC"||
				description == "UVEC: XVEC" ||
				description == "UVEC: YVEC")/*cam componant vector,  unitless*/
				ech = "(unitless)";
			else/*distance*/
			{
				switch (k)
				{
				case 1:
					ech = "(MM)";
					break;
				case 10:
					ech = "(1/10 MM)";
					break;
				case 100:
					ech = "(1/100 MM)";
					break;
				}
			}
			stream << "REPARTITION DES " << description << getSeparator() << ech;
			stream << endl << endl;
		}

		// output the histogram
		{
			//get the histogram data and set up begin and end iterators
			std::list<int> result = summary.getHistogramData();
			std::list<int>::iterator  iter, iterEnd;
			iter = result.begin();
			iterEnd = result.end();
			// get the other necessary parameters for the histogram
			TReal min = summary.getHistoLoLimit();
			// interval is taken to be 1
			int lowLim = 0;

			// loop through the results writing out the data for each interval
			while (iter != iterEnd)
			{
				stream << (min + lowLim) << endl;
				stream << " + " << getSeparator();
				int j = *iter;  // number of residuals in current interval, output counter
				int J = j;  // number of residuals in current interval
				int i = 0;  // counts multiples of ten residuals
				int k = 0;  // identifies when too many residuals for the output line, limit 79

				// output the histogram line for the current interval
				while ((j != 0) && (k<79))
				{
					i++;
					k++;
					// if residual number is a multiple of ten output a 0 instead of a *
					// otherwise output a star
					if (i == 10)
					{
						stream << 'O';
						i = 0;
					}
					else
					{
						stream << '*';
					}

					// if the number of residuals is greater than 79 write a summary
					// string for those remaining
					if (k == 79)
					{
						stream << "+... " << J;
					}

					j--;
				}

				// move on to the next interval
				iter++;
				stream << endl;
				lowLim++;
			}
			if (lowLim != 0)
			{
				stream << (min + lowLim);
				stream << endl << endl;
			}

			// write the number of pointsthat don't lie within the histogram limits
			stream << "NOMBRE DE POINTS HORS-HISTOGRAMME" << getSeparator() << summary.getNumBeyondHistoLimits();
			stream << endl << endl << endl;
		}
	}
	return;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationWriter::writeReliabilityHeader(std::string name1,
	std::string name2,
	std::string name3,
	std::string Observation,
	std::string unit1,
	std::string unit2)
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

void TObservationWriter::writeReliabilityCC(int index, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();

	// Test the computing: if it failed, write the corresponding error messages
	if (stat.getAreDetermined(index))
	{
		//get zi
		TReal z = stat.getZi().coeff(index);
		stream->setf(std::ios::fixed, std::ios::floatfield);
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
		if (stat.getGToCompute(index))
		{
			(*stream).writeString(3,"**");
			(*stream).writeDouble(obsResWidth, 2, stat.getGi().coeff(index)*RAD2CC);
		}else{
			(*stream).writeString(3,"");
			(*stream).writeString(obsResWidth,"");
		}
		
		// get nabla
		if (stat.getDeltaComputed(index))
		{ //tests that the nabla has been correctly computed
			(*stream).writeDouble(obsResWidth,2, stat.getNabla().coeff(index)*RAD2CC);
		}else{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

		// get T
		(*stream).writeDouble(obsResWidth,2, stat.getTi().coeff(index));
		
		// get DELTY
		stream->width(10);
		if (stat.getDeltaComputed(index))/*tests that the delty has been correctly computed*/
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

void TObservationWriter::writeReliabilityMM(int index, const TLGCStatistic& stat)
{
	TAStreamFormatter*	stream = getStream();
	int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();

	// Test the computing: if it failed, write the corresponding error messages
	if (stat.getAreDetermined(index))
	{
		//get zi
		TReal z = stat.getZi().coeff(index);
		stream->setf(std::ios::fixed, std::ios::floatfield);
		// checks that z is between 25% and 60%, and if not, adds some symbols
		(*stream).writeDouble(obsResWidth, 1, 100 * z);

		if (z < LITERAL(0.25))
		{
			(*stream).writeString(3, "**");
		}

		if (z > LITERAL(0.60))
		{
			(*stream).writeString(3, "!");
		}

		if (z >= LITERAL(0.25) && z <= LITERAL(0.60))
		{
			(*stream).writeString(3, "");
		}

		//get wi
		if (stat.getWToCompute())
		{
			(*stream).writeDouble(8, 2, stat.getWi().coeff(index));
		}
		else{
			(*stream).writeString(8, "");
		}

		//get gi	
		if (stat.getGToCompute(index))
		{
			(*stream).writeString(3, "**");
			(*stream).writeDouble(obsResWidth, 2, stat.getGi().coeff(index)*M2MM);
		}
		else{
			(*stream).writeString(3, "");
			(*stream).writeString(obsResWidth, "");
		}

		// get nabla
		if (stat.getDeltaComputed(index))
		{ //tests that the nabla has been correctly computed
			(*stream).writeDouble(obsResWidth, 2, stat.getNabla().coeff(index)*M2MM);
		}
		else{
			(*stream).writeString(obsResWidth, "INDTMNE");
		}

		// get T
		(*stream).writeDouble(obsResWidth, 2, stat.getTi().coeff(index));

		// get DELTY
		stream->width(10);
		if (stat.getDeltaComputed(index))/*tests that the delty has been correctly computed*/
		{
			(*stream).writeDouble(obsResWidth, 2, stat.getDelty().coeff(index));
		}
		else{
			(*stream).writeString(obsResWidth, "INDTMNE");
		}

	}
	else {
		(*stream).writeString(obsResWidth, "0.0");//z
		(*stream).writeString(3, "");
		(*stream).writeString(8, "INDETERMINE");//wi
		(*stream).writeString(0, "");
		(*stream).writeString(obsResWidth, "");//gi
		(*stream).writeString(obsResWidth, "");//nabla
		(*stream).writeString(obsResWidth, "INFINI");//T
		(*stream).writeString(obsResWidth, "");//DELTY
	}

	(*stream) << endl;
}

// Get the French version of the observation description
// returns a string with the description
std::string	TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"PLR3D",
		"Vecteur directeur (UVEC)",
		"Vecteur directeur + Distance (UVD)",
		"ANGLES HORIZONTAUX (ANGL)",
		"DISTANCES ZENITHALES (ZEND)",
		"DISTANCES MESUREES (DIST)",
		"ECART A UN PLAN VERTICAL (ECTH)",
		"ECART A UNE DIRECTION (ECDIR)",
		"DISTANCES HORIZONTALES (DHOR)",
		"DISTANCES SPATIALES (DSPT)",
		"DISTANCES VERTICALES (DLEV)",
		"DISTANCES VERTICALES (DVER)",
		"ECARTS DANS LE PLAN HORIZONTAL (ECHO)",
		"ECARTS A UNE DROITE (ECSP)",
		"ECARTS A UNE DROITE VERTICALE (ECVE)",
		"ORIENTATIONS (ORIE)",
		"POINT D'ORIENTATION (PDOR)",
		"CONTRAINTES RADIALES (RADI)",
		"CONTRAINTES 3D SUR UN POINT (OBSXYZ)",

	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCObjectWriter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}


// Get the English version of the observation description
// returns a string with the description
std::string	TObservationWriter::getObsDescriptionEN(TALGCObjectWriter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"PLR3D",
		"Unit Vector Measurment (UVEC)",
		"Unit Vector Measurement + Distance (UVD)",
		"HORIZONTAL ANGLE OBSERVATIONS (ANGL)",
		"ZENITHAL DISTANCE OBSERVATIONS (ZEND)",
		"OBSERVED SPATIAL DISTANCES (DIST)",
		"OFFSETS TO A THEODOLITE PLAN (ECTH)",
		"OFFSETS TO A THEODOLITE DIRECTION (ECDIR)",
		"HORIZONTAL DISTANCES (DHOR)",
		"SPATIAL DISTANCES (DSPT)",
		"VERTICAL DISTANCES (DLEV)",
		"VERTICAL DISTANCES (DVER)",
		"OFFSETS IN AN HORIZONTAL PLAN (ECHO)",
		"OFFSETS TO A SPATIAL LINE (ECSP)",
		"OFFSETS TO A VERTICAL LINE (ECVE)",
		"ORIENTATIONS (ORIE)",
		"POINT OF ORIENTATION (PDOR)",
		"RADIAL CONSTRAINTS (RADI)",
		"3D CONSTRAINTS FOR POINTS (OBSXYZ)"
		//"NoObs"
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCObjectWriter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}

