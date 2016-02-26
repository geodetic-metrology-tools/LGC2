////////////////////////////////////////////////////////////////////
// TPunchFileWriter.cpp :Implementation file
//
// Write an LGC output file for point coordinates.
// Creates a file from the calculation results and sends the appropriate messages.
//
/////////////////////////////////////////////////////////////////////

// other forward declarations
#include "TLGCData.h"
#include "TPunchFileWriter.h"
#include "TPointConverter.h"
#include "TLGCApp.h"
#include "TAStreamFormatter.h"
#include "TRefSystemFactory.h"
#include <TLOR2LOR.h>
#include "TAGeoidModel.h"
#include "TXYH2CCS.h"

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////

TPunchFileWriter::TPunchFileWriter() :
TAFileWriter(), fData(nullptr)
{
}

TPunchFileWriter::TPunchFileWriter(TAStreamFormatter* stream, const TLGCData* project) :
TAFileWriter(stream, project), fData(project)
{
	stream->setSeparator(project->getConfig().CustomOutputSeparatorPunch.separator);
}

TPunchFileWriter::~TPunchFileWriter()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void TPunchFileWriter::writeFile(const string error)
{//write error messages from project
	writeTitle();
	writeError(error);
}

// Write the LGC results for the given project
void TPunchFileWriter::writeFile()
{	
	//write the header title of the results file
	this->writeTitle();

	this->writePoints();
}

void TPunchFileWriter::writePoints()
{
	
	switch(fProjectData->getConfig().writePunch.fmode)
	{
		case TLGCConfig::TCoordOut::kPLAIN: writeXYZHeader(); break;
		case TLGCConfig::TCoordOut::kE: 	writeXYZVarCovarDeltaHeader(); break;
		case TLGCConfig::TCoordOut::kEE: 	writeXYZErrorEllHeader(); break;
		case TLGCConfig::TCoordOut::kH: 	writeXYHHeader(); break;
		case TLGCConfig::TCoordOut::kZ: 	writeXYZHeader(); break;
		case TLGCConfig::TCoordOut::kHZ: 	writeXYZHHeader(); break;
		case TLGCConfig::TCoordOut::kHN: 	writeXYHNHeader(); break;
		case TLGCConfig::TCoordOut::kZHN: 	writeXYZHNHeader(); break;
		case TLGCConfig::TCoordOut::kT: 	writeXYZSigmaHeader(); break;
		case TLGCConfig::TCoordOut::kOUT1: 	writeCooHeader(); break;
		//case TLGCConfig::TCoordOut::kOUT3: 	writeXYZHeader(); break;
	}

	for(auto point : fProjectData->getPoints())
		writePoint(point, fProjectData->getConfig().writePunch.fmode);
}

void TPunchFileWriter::writePoint(TAdjustablePoint const& point, TLGCConfig::TCoordOut::eMode output_type)
{
	TAStreamFormatter* stream = getStream();
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(getCoordWidth());
	stream->setPrecisionFormat(getCoordPrecision());

	(*stream).setWidthFormat(20);
	auto writeName	= [&](TAdjustablePoint const& point) { (*stream) << point.getName() << " "; };
	(*stream).setWidthFormat(8);

	auto writeDxyz	= [&](TAdjustablePoint const& point) { (*stream) << point.getDXValue() << " " << point.getDYValue() << " " << point.getDZValue() << " ";};
	auto writeCovs	= [&](TAdjustablePoint const& point) { (*stream) << point.getXYCovar() << " " << point.getXZCovar() << " " << point.getYZCovar() << " ";};
	// transform the point coordinate in root
	auto point_in_root = [&](TAdjustablePoint const& point) {
		// Transformation of the point to the root
		TPositionVector estimatedValue = point.getEstimatedValue();
		TDataTreeIterator root = fProjectData->getTree().begin();

		if (root != point.getFrameTreePosition()){
			TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
			transfo.transform(estimatedValue);
		}

		// Set new coordinates to the point expressed in the root
		TAdjustablePoint point_int_root = TAdjustablePoint(estimatedValue,
			point.isCoordinateFixed(0),
			point.isCoordinateFixed(1),
			point.isCoordinateFixed(2),
			point.getName(),
			point.getReferenceFrame(),
			point.getFrameTreePosition());
		
		return point_int_root;
	};

	auto pointRoot = point_in_root(point);

	switch(output_type)
	{
		///< Nom_Pt X Y Z
		case TLGCConfig::TCoordOut::kPLAIN:
			writeXYZData(pointRoot);
			break; 
		
		///< Nom_Pt X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz
		case TLGCConfig::TCoordOut::kE:
			if (point.getFrameTreePosition()->get()->isROOTNode())
				writeXYZVarCovarDeltaData(point);
			else
				writeXYZData(pointRoot);
			break; 

		///< Nom_Pt X Y Z Gist_gd_axe Gd_axe Pt_axe Sz dx dy dz
		case TLGCConfig::TCoordOut::kEE:
			if (point.getFrameTreePosition()->get()->isROOTNode())
				writeXYZErrorEllData(point);
			else
				writeXYZData(pointRoot);
			break;

		///< Nom_Pt X Y H
		case TLGCConfig::TCoordOut::kH:			
			writeXYHData(pointRoot);
			break; 

		///< Nom_Pt X Y Z
		case TLGCConfig::TCoordOut::kZ:
			writeXYZData(pointRoot);
			break; 

		///< Nom_Pt X Y Z H
		case TLGCConfig::TCoordOut::kHZ:
			writeXYZHData(pointRoot);
			break; 

		///< Nom_Pt X Y Z H NLEP
		case TLGCConfig::TCoordOut::kHN:
			writeXYHNData(pointRoot);
			break; 

		///< Nom_Pt X Y Z H NLEP
		case TLGCConfig::TCoordOut::kZHN:		
			writeXYZHNData(pointRoot);
			break; 

		///< Nom_Pt X Y Z Sx Sy Sz
		case TLGCConfig::TCoordOut::kT:	
			if (point.getFrameTreePosition()->get()->isROOTNode())
				writeXYZSigmaData(point);
			else
				writeXYZData(pointRoot);
			break;

		///< .coo file for GEODE
		case TLGCConfig::TCoordOut::kOUT1:
			if (point.getFrameTreePosition()->get()->isROOTNode())
				writeCooData(point);
			else
				writeCooData(pointRoot);
			break;
 

		///< .coo file for GEODE
		case TLGCConfig::TCoordOut::kOUT3:
			
			// ?
			break; 
	}

	(*stream) << endl;
}

void TPunchFileWriter::writeTitle()
{
	TAStreamFormatter* stream = getStream();
	//write software id.
	(*stream)<<(TLGCApp::getProgId())<<endl;

	//write software copyright
	(*stream)<<(TLGCApp::getCopyright())<<endl;

	//last compilation (now showed in progID together with surveylib version)
	//(*stream)<<"Derniere compilation : "<<__DATE__<<endl<<endl<<endl;

	//write title
	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<(fProjectData->getConfig().title)<<endl;
	(*stream)<<endl;

	//write time
	char tmpbuf[128];
	time_t ltime;
	time(&ltime);
	struct tm *today;
#ifdef __linux__
	tzset();
#else
	_tzset();
#endif
	today = localtime( &ltime );
	string essai = ctime( &ltime );
	strftime( tmpbuf, 128,"CALCUL DU %d %B %Y %X", today );
	(*stream)<<tmpbuf<<endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl<<endl<<endl<<endl;

}


void	TPunchFileWriter::writeXYZHeader()
{//XYZ
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeXYZVarCovarDeltaHeader()
{//X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz	

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	if (fData->getConfig().useApriori.isActive())
		(*stream) << "LES VARIANCES ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)" << endl << endl;
	else
		(*stream) << "LES VARIANCES ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI" << endl << endl;


	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "VX ");
	(*stream).writeString(coordResWidth, "VY ");
	(*stream).writeString(coordResWidth, "VZ ");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "COVXY");
	(*stream).writeString(coordResWidth, "COVXZ");
	(*stream).writeString(coordResWidth, "COVYZ");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeXYZErrorEllHeader()
{//X Y Z gist_gd_axe Gd_axe Pt_axe Sz Cyz Dx Dy Dz	

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	int					obsWidth = max(getObsWidth(), 11);

	if (fData->getConfig().useApriori.isActive())
		(*stream) << "LES ELLIPSES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)" << endl << endl;
	else
		(*stream) << "LES ELLIPSES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI" << endl << endl;



	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line	
	(*stream).writeString(nameWidth, "NOM  ");
	stream->writeString(coordWidth, "X");
	stream->writeString(coordWidth, "Y");
	stream->writeString(coordWidth, "Z");
	// Direction vectors: (+0.000, -0.000, +0.000) => 24
	const int vecwidth(24);
	stream->writeString(vecwidth, "DIRECTION X");
	stream->writeString(vecwidth, "DIRECTION Y");
	stream->writeString(vecwidth, "DIRECTION Z");
	stream->writeString(coordWidth, "LONGUER X");
	stream->writeString(coordWidth, "LONGUER Y");
	stream->writeString(coordWidth, "LONGUER Z");
	stream->writeString(coordResWidth, "DX");
	stream->writeString(coordResWidth, "DY");
	stream->writeString(coordResWidth, "DZ");

	*stream << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	stream->writeString(nameWidth, "");//Nom
	stream->writeString(coordWidth, "(M)");// coordianate
	stream->writeString(coordWidth, "(M)");// coordianate
	stream->writeString(coordWidth, "(M)");// coordianate
	stream->writeString(vecwidth, "");//Direction
	stream->writeString(vecwidth, "");//Direction
	stream->writeString(vecwidth, "");//Direction
	stream->writeString(coordWidth, "(MM)");// Axis length
	stream->writeString(coordWidth, "(MM)");// Axis length
	stream->writeString(coordWidth, "(MM)");// Axis length
	stream->writeString(coordResWidth, "(MM)");// delta
	stream->writeString(coordResWidth, "(MM)");// delta
	stream->writeString(coordResWidth, "(MM)");// delta
	(*stream) << endl;

	return;
}

void	TPunchFileWriter::writeXYHHeader()
{//XYH

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeXYZHHeader()
{//XYZH

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeXYHNHeader()
{//XYHN

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeXYZHNHeader()
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream) << endl;

	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeXYZSigmaHeader()
{
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	//First line
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordResWidth, "SX ");
	(*stream).writeString(coordResWidth, "SY ");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream) << endl;

	//second line : units
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream) << endl << endl;
	return;
}

void	TPunchFileWriter::writeCooHeader()
{//XYH

	TAStreamFormatter*	stream = getStream();
	int	nameWidth = getNameWidth();
	int	coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();

	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(nameWidth, "ID");
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream).writeString(coordWidth, "DCUM ");
	(*stream).writeString(nameWidth, "OPTION ");
	(*stream) << endl<< endl;
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//DATA
///////////////////////////////////////////////////////////////////////////////////////////////////////

void TPunchFileWriter::writeXYZData(TAdjustablePoint const& point)
{
   TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());
	return;
}

void TPunchFileWriter::writeXYHData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstValue(0), point.getEstValue(1), TLength(point.getHEstValue()));
	return;
}

void TPunchFileWriter::writeXYZVarCovarDeltaData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(),TLength::kMetres, separator, point.getEstimatedValue());

	//Variance
	(*stream).width(1);
	(*stream)<<"";
	int  pre = getCoordPrecision();
	if(pre >=3)
	{pre = pre -3;}
	else
	{pre = 0;}
	converter.writeCoordinateParam( point.getSpatialStatus(),
									coordResWidth,
									pre,
									separator,
									point.getXEstPrecision().getMMetresValue()*point.getXEstPrecision().getMMetresValue(),
									point.getYEstPrecision().getMMetresValue()*point.getXEstPrecision().getMMetresValue(),
									point.getZEstPrecision().getMMetresValue()*point.getZEstPrecision().getMMetresValue(),
									" ");
	(*stream).width(1);
	(*stream)<<"";
	//Covariance 
	converter.writeCoordinateParam(point.getSpatialStatus(),
								coordResWidth,
								getCoordPrecision(),
								separator,
								point.getXYCovar(),
								point.getXZCovar(),
								point.getYZCovar(),
								" ", true);


	(*stream).width(1);
	(*stream)<<"";
	//Delta

	//transform H in Z
	TPositionVector xyzValue = point.getProvisionalValue();
	if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
	{
		TXYH2CCS::XYHs2CCS(xyzValue);
		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			" ");
	}
	else if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
	{
		TXYH2CCS::XYHg2000Machine2CCS(xyzValue);
		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			" ");
	}
	else if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
	{
		TXYH2CCS::XYHg1985Machine2CCS(xyzValue);

		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			" ");
	}
	else 
		converter.writeCoordinateParam(point.getSpatialStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								point.getDXValue(),
								point.getDYValue(),
								point.getDZValue(),
								" ");
	return;
}


void TPunchFileWriter::writeXYZErrorEllData(TAdjustablePoint const& point)
{	
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	int					obsWidth = max(getObsWidth(),11);
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());

	
	if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxyz)
	{ // Error ellipsoid
		const auto& ell(point.getErrorEllipsoid());
		
		(*stream).width(1);
		(*stream)<<"" ;
		const int vecwidth(24);
		char vecstr[32]; // format the vector output here and write as a string
		sprintf(vecstr, "(% .3f, % .3f, % .3f)", ell.vx[0], ell.vx[1], ell.vx[2]);
		stream->writeString(vecwidth, vecstr);
		sprintf(vecstr, "(% .3f, % .3f, % .3f)", ell.vy[0], ell.vy[1], ell.vy[2]);
		stream->writeString(vecwidth, vecstr);
		sprintf(vecstr, "(% .3f, % .3f, % .3f)", ell.vz[0], ell.vz[1], ell.vz[2]);
		stream->writeString(vecwidth, vecstr);
		stream->writeDouble(coordWidth, coordResWidth, ell.lx*M2MM);
		stream->writeDouble(coordWidth, coordResWidth, ell.ly*M2MM);
		stream->writeDouble(coordWidth, coordResWidth, ell.lz*M2MM);

	}
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxy
		|| point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxz
		|| point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVyz)
	{//Error ellipse

		(*stream).width(1);
		(*stream) << "";
		const int vecwidth(24);
		char vecstr[32]; // format the vector output here and write as a string
		stream->writeDouble(vecwidth, coordResWidth, point.getErrorEllGis().getGonsValue());
		stream->writeString(vecwidth, "");
		stream->writeString(vecwidth, "");
		stream->writeDouble(coordWidth, coordResWidth, point.getErrorEllMajorAxis().getMMetresValue());
		stream->writeDouble(coordWidth, coordResWidth, point.getErrorEllMinorAxis().getMMetresValue());
		stream->writeString(coordWidth, "");

	}
	else
	{
		writeXYZData(point);
		(*stream).width(obsWidth);
		(*stream) << "" << separator;
		(*stream) << "" << separator;
		(*stream) << "" << separator;
	}


	//Delta
	//transform H in Z
	TPositionVector xyzValue = point.getProvisionalValue();
	if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
	{
		TXYH2CCS::XYHs2CCS(xyzValue);
		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			"");
	}
	else if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
	{
		TXYH2CCS::XYHg2000Machine2CCS(xyzValue);
		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			"");
	}
	else if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
	{
		TXYH2CCS::XYHg1985Machine2CCS(xyzValue);

		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			"");
	}
	else
		converter.writeCoordinateParam(point.getSpatialStatus(),
		coordResWidth,
		getCoordPrecision(),
		TLength::kMillimetres,
		separator,
		point.getDXValue(),
		point.getDYValue(),
		point.getDZValue(),
		"");

	return;
}


void TPunchFileWriter::writeXYZHData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZandH(point.getEstimatedValue(), TLength(point.getHEstValue()));
	return;
}

void TPunchFileWriter::writeXYHNData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstValue(0), point.getEstValue(1), TLength(point.getHEstValue()));
	converter.writeN(coordWidth, getCoordPrecision(), getN(point));	
	(*stream) << separator;
	return;
}

void TPunchFileWriter::writeXYZHNData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZandH(point.getEstimatedValue(), TLength(point.getHEstValue()));
	converter.writeN(coordWidth, getCoordPrecision(), getN(point));
	(*stream) << stream->getSeparator();
	return;
}

void	TPunchFileWriter::writeXYZSigmaData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	//Coordinate
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());

	//Sigma
	converter.writeCoordinateParam(point.getSpatialStatus(),
		getCoordResWidth(),
		getCoordPrecision(),
		TLength::kMillimetres,
		separator,
		point.getXEstPrecision(),
		point.getYEstPrecision(),
		point.getZEstPrecision(),
		" ");/*sigma*/
	return;
}

void TPunchFileWriter::writeCooData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int	nameWidth = getNameWidth();
	int	coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstValue(0), point.getEstValue(1), TLength(point.getHEstValue()));
	stream->writeString(nameWidth, "-1");
	//Delta
	//transform H in Z
	TPositionVector xyzValue = point.getProvisionalValue();
	if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
	{
		TXYH2CCS::XYHs2CCS(xyzValue);
		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			"");
	}
	else if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
	{
		TXYH2CCS::XYHg2000Machine2CCS(xyzValue);
		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			"");
	}
	else if (point.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
	{
		TXYH2CCS::XYHg1985Machine2CCS(xyzValue);

		converter.writeCoordinateParam(point.getSpatialStatus(),
			coordResWidth,
			getCoordPrecision(),
			TLength::kMillimetres,
			separator,
			point.getDXValue(),
			point.getDYValue(),
			point.getEstValue(2) - xyzValue.getZ(),
			"");
	}
	else
		converter.writeCoordinateParam(point.getSpatialStatus(),
		coordResWidth,
		getCoordPrecision(),
		TLength::kMillimetres,
		separator,
		point.getDXValue(),
		point.getDYValue(),
		point.getDZValue(),
		"");
	
	
	stream->writeDouble(coordWidth, getCoordPrecision(), -1.0);
	
	string status;
	if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kCala)
		status = "CALA";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxyz)
		status = "POIN";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxy)
		status = "VXY";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxz)
		status = "VXZ";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVyz)
		status = "VYZ";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVz)
		status = "VZ";

	stream->writeString(nameWidth, status);
	return;
}

TReal TPunchFileWriter::getN(TAdjustablePoint const& point)
{
	TRefSystemFactory::EGeoid	fGeoidModel;
	if (fData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		fGeoidModel = TRefSystemFactory::EGeoid::kCGSphere;
	else if (fData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG2000Machine;
	else if (fData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG1985Machine;
	else
		fGeoidModel = TRefSystemFactory::EGeoid::kNoGeoid;

	//Toujours en CCS pendant le calcul TAReferenceFrame* pointRefFrame = getStreamFormatter()->getReferenceFrame();
	TAReferenceFrame* ccs = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS);
	TAGeoidModel* geoid = TRefSystemFactory::getRefSystemFactory()->getGeoid(fGeoidModel);

	TSpatialPosition pos(ccs);
	pos.setCoordinates(point.getEstimatedValue());
	return geoid->getN(pos).getMetresValue();
}