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

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////

TPunchFileWriter::TPunchFileWriter() :
TAFileWriter()
{
}

TPunchFileWriter::TPunchFileWriter(TAStreamFormatter* stream, const TLGCData* project) :
TAFileWriter(stream, project)
{
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
		//case TLGCConfig::TCoordOut::kE: 	writeXYZHeader(); break;
		//case TLGCConfig::TCoordOut::kEE: 	writeXYZHeader(); break;
		//case TLGCConfig::TCoordOut::kH: 	writeXYZHeader(); break;
		//case TLGCConfig::TCoordOut::kZ: 	writeXYZHeader(); break;
		case TLGCConfig::TCoordOut::kHZ: 	writeXYZHHeader(); break;
		case TLGCConfig::TCoordOut::kHN: 	writeXYHNHeader(); break;
		case TLGCConfig::TCoordOut::kZHN: 	writeXYZHNHeader(); break;
		//case TLGCConfig::TCoordOut::kT: 	writeXYZHeader(); break;
		//case TLGCConfig::TCoordOut::kOUT1: 	writeXYZHeader(); break;
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
	auto writeXY	= [&](TAdjustablePoint const& point) { (*stream) << point.getEstimatedValue()[0] << " " << point.getEstimatedValue()[1] << " "; };
	auto writeZ		= [&](TAdjustablePoint const& point) { (*stream) << point.getEstimatedValue()[2] << " "; };
	auto writeH		= [&](TAdjustablePoint const& point) { (*stream) << point.getHEstValue(); };
	auto writeDxyz	= [&](TAdjustablePoint const& point) { (*stream) << point.getDXValue() << " " << point.getDYValue() << " " << point.getDZValue() << " ";};
	auto writeCovs	= [&](TAdjustablePoint const& point) { (*stream) << point.getXYCovar() << " " << point.getXZCovar() << " " << point.getYZCovar() << " ";};
	
	switch(output_type)
	{
		///< Nom_Pt X Y Z
		case TLGCConfig::TCoordOut::kPLAIN:
			writeXYZData(point);
			break; 
		
		///< Nom_Pt X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz
		case TLGCConfig::TCoordOut::kE:
			//writeXYZVarCovarDeltaData(point);
			writeName(point);
			writeXY(point);
			writeZ(point);
			// Vx Vy Vz
			writeCovs(point);
			writeDxyz(point);
			break; 

		///< Nom_Pt X Y Z Gist_gd_axe Gd_axe Pt_axe Sz dx dy dz
		case TLGCConfig::TCoordOut::kEE:
			
			writeName(point);
			writeXY(point);
			writeZ(point);
			// Gist_gd_axe Gd_axe Pt_axe Sz
			writeDxyz(point);
			break; 

		///< Nom_Pt X Y H
		case TLGCConfig::TCoordOut::kH:			
			writeXYHData(point);
			break; 

		///< Nom_Pt X Y Z
		case TLGCConfig::TCoordOut::kZ:
			writeXYZData(point);
			break; 

		///< Nom_Pt X Y Z H
		case TLGCConfig::TCoordOut::kHZ:
			writeXYZHData(point);
			break; 

		///< Nom_Pt X Y Z H NLEP
		case TLGCConfig::TCoordOut::kHN:
			writeXYZHData(point);
			// NLEP
			break; 

		///< Nom_Pt X Y H N NLEP
		case TLGCConfig::TCoordOut::kZHN:
			
			writeName(point);
			writeXY(point);
			writeH(point);
			// NLEP
			break; 

		///< Nom_Pt X Y Z Sx Sy Sz
		case TLGCConfig::TCoordOut::kT:
			
			writeName(point);
			writeXY(point);
			writeZ(point);
			//  Sx Sy Sz
			break; 

		///< .coo file for GEODE
		case TLGCConfig::TCoordOut::kOUT1:
			
			// ?
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

void TPunchFileWriter::writeXYZHeader()
{//XYZ
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}

void TPunchFileWriter::writeXYZVarCovarDeltaHeader()
{//X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz	

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	//if(isSAPrioriUsed())
	//{
	//	(*stream)<<"LES VARIANCES ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)"<<endl<<endl;
	//}
	//else
	//{
	//	(*stream)<<"LES VARIANCES ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI"<<endl<<endl;
	//}	


	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "VX ");
	(*stream).writeString(coordResWidth, "VY ");
	(*stream).writeString(coordResWidth, "VZ ");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "COVXY");
	(*stream).writeString(coordResWidth, "COVXZ");
	(*stream).writeString(coordResWidth, "COVYZ");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream)<<endl<<endl;
	return;
}

void TPunchFileWriter::writeXYZErrorEllSigZDeltaHeader()
{//X Y Z gist_gd_axe Gd_axe Pt_axe Sz Cyz Dx Dy Dz	

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	int					obsWidth = max(getObsWidth(),11);

	//if(isSAPrioriUsed())
	//{
	//	(*stream)<<"LES ELLIPSES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)"<<endl<<endl;
	//}
	//else
	//{
	//	(*stream)<<"LES ELLIPSES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI"<<endl<<endl;
	//}



	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line	
	(*stream).writeString(nameWidth, "NOM  ");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(obsWidth, "GIS Gd_AXE");
	(*stream).writeString(11, "1/2 Gd_AXE");
	(*stream).writeString(11, "1/2 Pt_AXE");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(obsWidth, "(GON)");
	(*stream).writeString(11, "(MM)");
	(*stream).writeString(11, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream)<<endl;

	return;
}

void TPunchFileWriter::writeXYHHeader()
{//XYH

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}

void TPunchFileWriter::writeXYZHHeader()
{//XYZH

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}

void TPunchFileWriter::writeXYHNHeader()
{//XYHN

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}

void TPunchFileWriter::writeXYZHNHeader()
{
	TAStreamFormatter* stream =	getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream)<<endl;

	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}

void TPunchFileWriter::writeXYZSigmaHeader()
{
	TAStreamFormatter* stream =	getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordResWidth, "SX ");
	(*stream).writeString(coordResWidth, "SY ");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream)<<endl;

	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream)<<endl<<endl;
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//DATA
///////////////////////////////////////////////////////////////////////////////////////////////////////

void TPunchFileWriter::writeXYZData(TAdjustablePoint const& point)
{
	// Transformation of the point to the root
	TPositionVector estimatedValue = point.getEstimatedValue();
	TDataTreeIterator root = fProjectData->getTree().begin();
		
	if(root != point.getFrameTreePosition()){
		TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
		transfo.transform(estimatedValue);
	}

   TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, estimatedValue);
	(*stream)<<endl;
	return;
}

void TPunchFileWriter::writeXYHData(TAdjustablePoint const& point)
{
	// Transformation of the point to the root
	TPositionVector estimatedValue = point.getEstimatedValue();
	TDataTreeIterator root = fProjectData->getTree().begin();
		
	if(root != point.getFrameTreePosition()){
		TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
		transfo.transform(estimatedValue);
	}

	// Set new coordinates to the point expressed in the root
	TAdjustablePoint point_int_root = TAdjustablePoint( estimatedValue,
														point.isCoordinateFixed(0), 
														point.isCoordinateFixed(1), 
														point.isCoordinateFixed(2),
														point.getName(),
														point.getReferenceFrame(), 
														point.getFrameTreePosition());

	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(point_int_root.getName(), nameWidth);
   converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point_int_root.getXEstPrecision(), point_int_root.getYEstPrecision(), point_int_root.getHEstValue());
	(*stream)<<endl;
	return;
}

#if 0

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
	//Name
	converter.writeName(point.getName(), nameWidth);

	//Coordinate
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
									double(powq(point.getXEstPrecision()*M2MM,2)),
									point.getYEstPrecision(),
									posVecIterator->getZVar(),
									"XXX");
	(*stream).width(1);
	(*stream)<<"";
	//Covariance 
	/*converter.writeTwoCoordParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getXYCovar(),
								posVecIterator->getXZCovar(),
								posVecIterator->getYZCovar(),
								"XXX");*/
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getXYCovar(),
								posVecIterator->getXZCovar(),
								posVecIterator->getYZCovar(),
								"XXX", true);


	(*stream).width(1);
	(*stream)<<"";
	//Delta
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getDXValue(),
								posVecIterator->getDYValue(),
								posVecIterator->getDZValue(),
								"XXX");
	(*stream)<<endl;
	return;
}


void TPunchFileWriter::writeXYZErrorEllSigZDeltaData(TAdjustablePoint const& point)
{	
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	int					obsWidth = max(getObsWidth(),11);
	string separator = stream->getSeparator();

	//Name
	converter.writeName(posVecIterator, nameWidth);

	//Coordinate
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, posVecIterator);

	
	if (posVecIterator->getPosVecStatus() == "VXYZ") 
	{ // Error ellipsoid
		const auto& ell(posVecIterator->getErrorEllipsoid());
		
		(*stream).width(obsWidth);
		(*stream)<<"" << separator;
		writeDouble(11, getLengthResidualPrecision(), ell.lx);
		(*stream)<<separator;
		writeDouble(11, getLengthResidualPrecision(), ell.ly);
		(*stream)<<separator;
		writeDouble(11, getLengthResidualPrecision(), ell.lz);
		(*stream)<<separator;
	}
	else {
		//Error ellipse
		if( (posVecIterator->getXStatus() == TALSCalcParameter::kVariable) && (posVecIterator->getYStatus() == TALSCalcParameter::kVariable) )
		{
			writeAngle(obsWidth, getAngleResidualPrecision(), TAngle::kGons, posVecIterator->getErrorEllGis());
			(*stream)<<separator;
			writeLength(11, getLengthResidualPrecision(), TLength::kMillimetres, posVecIterator->getErrorEllMajorAxis());
			(*stream)<<separator;
			writeLength(11, getLengthResidualPrecision(), TLength::kMillimetres, posVecIterator->getErrorEllMinorAxis());
			(*stream)<<separator;
 		}
		else
		{
			(*stream).width(obsWidth);
			(*stream)<<"" << separator;
			(*stream).width(11);
			(*stream)<<"" << separator;
			(*stream).width(11);
			(*stream)<<"" << separator;
		}	
		//sigma Z
		if(posVecIterator->getZStatus() == TALSCalcParameter::kVariable)
			writeLength(coordResWidth, getCoordPrecision(),  TLength::kMillimetres, posVecIterator->getZSigma());
		else
		{
			(*stream).width(coordResWidth);
			(*stream)<<"";
		}
	}

	(*stream)<<separator;

	//delta
	
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getDXValue(),
								posVecIterator->getDYValue(),
								posVecIterator->getDZValue(),
								"");

	(*stream)<<endl;
	return;
}

#endif

void TPunchFileWriter::writeXYZHData(TAdjustablePoint const& point)
{
	// Transformation of the point to the root
	TPositionVector estimatedValue = point.getEstimatedValue();
	TDataTreeIterator root = fProjectData->getTree().begin();
		
	if(root != point.getFrameTreePosition()){
		TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
		transfo.transform(estimatedValue);
	}

	// Set new coordinates to the point expressed in the root
	TAdjustablePoint point_int_root = TAdjustablePoint( estimatedValue,
														point.isCoordinateFixed(0), 
														point.isCoordinateFixed(1), 
														point.isCoordinateFixed(2),
														point.getName(),
														point.getReferenceFrame(), 
														point.getFrameTreePosition());

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";

	(*stream).writeString(nameWidth, (point_int_root.getName()));

	writeDouble(coordWidth, getCoordPrecision(), point_int_root.getEstimatedValue().getX().getMetresValue());
	(*stream)<<separator;
   writeDouble(coordWidth, getCoordPrecision(), point_int_root.getEstimatedValue().getY().getMetresValue());
	(*stream)<<separator;
   writeDouble(coordWidth, getCoordPrecision(), point_int_root.getEstimatedValue().getZ().getMetresValue());
	(*stream)<<separator;
	writeDouble(coordWidth, getCoordPrecision(), point_int_root.getHEstValue());
	(*stream)<<separator;
	(*stream)<<endl;
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
	(*stream)<<"";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getXEstPrecision(), point.getYEstPrecision(), point.getHEstValue());
	//converter.writeN( coordWidth, getCoordPrecision(), TLength::kMetres, point);
	(*stream)<<endl;
	
	return;
}

void TPunchFileWriter::writeXYZHNData(TAdjustablePoint const& point)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, fProjectData->getConfig().referential);
	int					nameWidth = getNameWidth();
	//int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(point.getName(), nameWidth);
	//converter.writeXYZandH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point);
	//converter.writeN( coordWidth, getCoordPrecision(), TLength::kMetres, point);
	(*stream)<<endl;

	return;
}