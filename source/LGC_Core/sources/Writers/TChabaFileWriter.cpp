#include "TChabaFileWriter.h"

#include "TLGCApp.h"
#include "TLGCData.h"

#include <TAStreamFormatter.h>
#include <TRefSystemFactory.h>
#include <TRotation.h>
#include <TRotationMatrix.h>
#include <TAdjustableHelmertTransformation.h>
#include <TXYH2CCS.h>
#include <TLOR2LOR.h>


TChabaFileWriter::TChabaFileWriter(TAStreamFormatter* stream, const TLGCData* project) : TAFileWriter(stream, project)
{//constructor
}

TChabaFileWriter::~TChabaFileWriter()
{//destructor
}

void TChabaFileWriter::writeFile(TAStreamFormatter* stream, const TLGCData & proj)
{
	std::string sep = stream->getSeparator();

	writeTitle(proj);

	//write transformation and points information for each bestfit
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		if (itTree->get()->frame.getName() != "ROOT")
		{

			// write transformation parameters
			writeHelmertTransformationDetails(itTree.node->data->frame);

			// write points
			(*stream) << sep << "COORDONNÉES POINTS REFERENCE (ACTIFS)\n";
			writePointPairs(createPair(proj, itTree));
			(*stream) << endl << endl;
			
			(*stream) << sep << "COORDONNÉES POINTS REFERENCE (PASSIF) ANCIEN SYSTÈME DANS LE NOUVEAU SYSTÈME\n" << endl;;
			// create a empty vector
			std::vector<LGCAdjustablePoint> emptyVector;
			writeTransformedPoints(createPair(proj, itTree), emptyVector, true);
			(*stream) << endl << endl;
			
			(*stream) << sep << "COORDONNÉES POINTS MODIFIES (PASSIFS) DANS LE NOUVEAU SYSTÈME\n" << endl;;
			writeTransformedSecondaryPoints(createSecPoint(proj, itTree));
		}
	}
}

void TChabaFileWriter::writeHelmertTransformationDetails(const TAdjustableHelmertTransformation & helmert)
{
	//obtain attributes stored in parent object
	TAStreamFormatter* stream = getStream();
	string sep = stream->getSeparator();
	int	coordPrecision = getCoordPrecision();
	
	//set output precision for the parameters
	int rotMatrixPrecision = coordPrecision + 7;
	int anglePrecision = coordPrecision + 5;
	int translationPrecision = coordPrecision;
	int scalePrecision = coordPrecision + 7;

	//set up a rotation matrix 
	TRotation rot(TRotationMatrix::ERotationType::kRzyx, helmert.getEstRotation(0).getRadiansValue(), helmert.getEstRotation(1).getRadiansValue(), helmert.getEstRotation(2).getRadiansValue());
	
	//set output units for different types of measurement
	stream->setPrecisionFormat(4);
	stream->setAngleUnits(TAngle::kGons);
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(17);


	
	//write the results table 
	(*stream)<<endl;
	(*stream)<<sep<<"**************** INFORMATION CONCERNANT LES PARAMETRES CALCULÉS ********************"<<endl;
	(*stream)<<sep<<"*                                                                                  *"<<endl;
	(*stream)<<sep<<"*                         TERMES DE LA MATRICE DE ROTATION:                        *"<<endl;
	(*stream)<<sep<<"*                                                                                  *"<<endl;
	(*stream)<<sep<<"* "<<sep<<"R11 = ";
	writeDouble(17,rotMatrixPrecision,rot(0,0));
	(*stream)<<sep<<"R12 = ";
	writeDouble(17,rotMatrixPrecision,rot(0,1));
	(*stream)<<sep<<"R13 = ";
	writeDouble(17,rotMatrixPrecision,rot(0,2));
	(*stream)<<sep<<"*"<<endl;
	(*stream)<<sep<<"* "<<sep<<"R21 = ";
	writeDouble(17,rotMatrixPrecision,rot(1,0));
	(*stream)<<sep<<"R22 = ";
	writeDouble(17,rotMatrixPrecision,rot(1,1));
	(*stream)<<sep<<"R23 = ";
	writeDouble(17,rotMatrixPrecision,rot(1,2));
	(*stream)<<sep<<"*"<<endl;
	(*stream)<<sep<<"* "<<sep<<"R31 = ";
	writeDouble(17,rotMatrixPrecision,rot(2,0));
	(*stream)<<sep<<"R32 = ";
	writeDouble(17,rotMatrixPrecision,rot(2,1));
	(*stream)<<sep<<"R33 = ";
	writeDouble(17,rotMatrixPrecision,rot(2,2));
	(*stream)<<sep<<"*"<<endl;

	(*stream)<<sep<<"*                                                                                  *"<<endl;
	(*stream)<<sep<<"*        ANGLES DE ROTATION (Gons):                                                *"<<endl;
	(*stream)<<sep<<"*                                                                                  *"<<endl;

	(*stream)<<sep<<"*"<<sep<<sep<<sep<<"OMEGA(x) = ";
	(writeAngle(17,anglePrecision, TAngle::kGons, rot.getAngles(TRotationMatrix::kRzyx).omega));
	(*stream)<<" ";
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;
		


	(*stream) << sep << "*" << sep << sep << sep << "PHI(y)   = ";
	(writeAngle(17,anglePrecision, TAngle::kGons, rot.getAngles(TRotationMatrix::kRzyx).phi));
	(*stream)<<" ";
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep  << "*" << endl;


	
	
	(*stream)<<sep<<"*"<<sep<<sep<<sep<<"KAPPA(z) = ";
	(writeAngle(17,anglePrecision, TAngle::kGons, rot.getAngles(TRotationMatrix::kRzyx).kappa));
	(*stream)<<" ";
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;

		





	(*stream) << sep << "*                                                                                  *" << endl;
	(*stream) << sep << "*        VECTEUR DE TRANSLATION (M):                                               *" << endl;
	(*stream) << sep << "*                                                                                  *" << endl;

	(*stream) << sep << "*" << sep << sep << sep << "TX = ";
	writeLength(16, translationPrecision, TLength::kMetres, helmert.getEstTranslation(0));
	(*stream) << " ";
	(*stream) << sep << sep << sep << sep << "   " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;


	(*stream) << sep << "*" << sep << sep << sep << "TY = ";
	writeLength(16, translationPrecision, TLength::kMetres, helmert.getEstTranslation(1));
	(*stream) << " ";
	(*stream) << sep << sep << sep << sep << "   " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;


	(*stream) << sep << "*" << sep << sep << sep << "TZ = ";
	writeLength(16, translationPrecision, TLength::kMetres, helmert.getEstTranslation(2));
	(*stream) << " ";
	(*stream) << sep << sep << sep << sep << "   " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;





	(*stream)<<sep<<"*                                                                                  *"<<endl;	
	(*stream)<<sep<<"*        FACTEUR D'ÉCHELLE (SANS UNITÉ):                                           *" << endl;
	(*stream) << sep << "*                                                                                  *" << endl;

	(*stream) << sep << "*" << sep << sep << sep <<  "ECH = ";
	writeDouble(17,scalePrecision, helmert.getEstScale());
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;


	(*stream)<<sep<<"*                                                                                  *"<<endl;	
	(*stream)<<sep<<"************************************************************************************"<<endl;
	(*stream)<<endl;
	(*stream)<<endl;
}

void TChabaFileWriter::writePointPairs(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> & pairs)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	//write column heading
	int	nameWidth = getNameWidth();
	int	coordWidth = getCoordWidth();
	int	coordPrecision = getCoordPrecision();
	string sep = stream->getSeparator();

	(*stream) << endl << endl;
	//Write group headings	
	writeStringSep(9 + nameWidth + coordWidth * 2, "NOUVEAU SYSTÈME");
	writeStringSep(4 + coordWidth * 3, "ANCIEN SYSTÈME");
	//Write Column Headings
	(*stream) << endl;
	writeStringLeftSep(nameWidth, "NOM");
	writeStringSep(coordWidth, "X(M)");
	writeStringSep(coordWidth, "Y(M)");
	if (pairs.at(0).first.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		writeStringSep(coordWidth, "H(M)");
	else
		writeStringSep(coordWidth, "Z(M)");
	writeStringSep(coordWidth, "X(M)");
	writeStringSep(coordWidth, "Y(M)");
	if (pairs.at(0).first.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		writeStringSep(coordWidth, "H(M)");
	else
		writeStringSep(coordWidth, "Z(M)");
	(*stream) << endl;

	for (auto& itPair : pairs)
	{
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(coordWidth);
		stream->setPrecisionFormat(coordPrecision);

		//transform Z in H
		TPositionVector xyhToValue = itPair.second.initialValue;
		if (itPair.first.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::CCS2XYHs(xyhToValue);
		else if (itPair.first.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::CCS2XYHg2000Machine(xyhToValue);
		else if (itPair.first.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::CCS2XYHg1985Machine(xyhToValue);

		converter.writeName(itPair.first.getName(), nameWidth);
		(*stream) << sep;

		//NEW SYSTEM
		TPositionVector xyzToValue = itPair.first.getProvisionalValue();
		converter.write3Coordinates(coordWidth,
			coordPrecision,
			sep,
			itPair.first.getProvisionalValue());
		(*stream) << sep;
		
		//OLD SYSTEM
		converter.write3Coordinates(coordWidth,
			coordPrecision,
			sep,
			xyhToValue);
		
		(*stream) << endl;

		
	}
}

void TChabaFileWriter::writeTransformedPoints(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> & pairs, const std::vector<LGCAdjustablePoint> & secondary, bool writeDeltas)
{
	TAStreamFormatter* stream = getStream();
	std::string sep = stream->getSeparator();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	//write column headings
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)"); 
	writeStringSep(getCoordWidth(), "Y(M)");
	if (pairs.size() != 0)
	{
		if (pairs.at(0).first.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
			writeStringSep(getCoordWidth(), "H(M)");
		else
			writeStringSep(getCoordWidth(), "Z(M)");
		if (writeDeltas)
		{
			(*stream) << sep << sep;
			writeStringSep(getCoordWidth() - 5, "DX(MM)");
			writeStringSep(getCoordWidth() - 5, "DY(MM)");
			if (pairs.at(0).first.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
				writeStringSep(getCoordWidth(), "DH(MM)");
			else
				writeStringSep(getCoordWidth()-5, "DZ(MM)");
			writeStringSep(getCoordWidth() - 5, "DD(MM)");
		}
	}
	else
	{
		if (secondary.at(0).getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
			writeStringSep(getCoordWidth(), "H(M)");
		else
			writeStringSep(getCoordWidth(), "Z(M)");
		if (writeDeltas)
		{
			(*stream) << sep << sep;
			writeStringSep(getCoordWidth() - 5, "DX(MM)");
			writeStringSep(getCoordWidth() - 5, "DY(MM)");
			if (secondary.at(0).getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
				writeStringSep(getCoordWidth(), "DH(MM)");
			else
				writeStringSep(getCoordWidth(), "DZ(MM)");
			writeStringSep(getCoordWidth() - 5, "DD(MM)");
		}
	}
	(*stream)<<endl;	
	
	int usablePointCount = 0;
	TReal sumdd2 = 0.0;

	for (auto& it : pairs)
	{
		++usablePointCount;

		//write the transformed coordinates of the passive point (coordinates in the root frame)
		TPositionVector stationRoot = it.second.initialValue;
		TLOR2LOR transfo = TLOR2LOR(it.second.positionInTree, fProjectData->getTree().begin(), "transfo");
		//transform coordinates in root
		transfo.transform(stationRoot);
		LGCAdjustablePoint ptInRoot(stationRoot,0, 0,0, it.first.getName(),it.first.getReferenceFrame(),it.first.getFrameTreePosition());

		delta d;
		d.dx =  stationRoot.getX() - it.first.getEstValue(0);
		d.dy =  stationRoot.getY() - it.first.getEstValue(1);
		//transform Z in H
		TPositionVector xyzToValue = it.first.getEstimatedValue();
		if (it.first.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		{
			TXYH2CCS::CCS2XYHs(xyzToValue);
			TXYH2CCS::CCS2XYHs(stationRoot);
			d.dz = stationRoot.getH() - xyzToValue.getH();
		}
		else if (it.first.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		{
			TXYH2CCS::CCS2XYHg2000Machine(xyzToValue);
			TXYH2CCS::CCS2XYHg2000Machine(stationRoot);
			d.dz = stationRoot.getH() - xyzToValue.getH();
		}
		else if (it.first.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		{
			TXYH2CCS::CCS2XYHg1985Machine(xyzToValue);
			TXYH2CCS::CCS2XYHg1985Machine(stationRoot);
			d.dz = stationRoot.getH() - xyzToValue.getH();
		}
		else
			d.dz = it.first.getEstValue(2) - stationRoot.getZ();

		sumdd2 += pow2(d.dx.getMMetresValue()) + pow2(d.dy.getMMetresValue()) + pow2(d.dz.getMMetresValue());

		writePUNPoint(ptInRoot, d, writeDeltas);
	}
	
	for(auto& itSec:secondary)
	{
		delta d;
		d.dx = d.dy = d.dz = TLength(0.0);
		writePUNPoint(itSec, d, writeDeltas);
	}
	
	//write EMQ from primary passive point information and write it at the bottom of the list
	double fEMQ = sqrt(sumdd2 / usablePointCount);
	
	(*stream)<<endl;
	if (writeDeltas)
	{
		(*stream) << sep << sep << sep << "*** EMQ DD (MM) = " << sep;
		writeDouble(7, 4, fEMQ);
		(*stream) << sep << "***" << endl;
	}
}


void TChabaFileWriter::writeTransformedSecondaryPoints(const std::vector<LGCAdjustablePoint> & secondary)
{
	delta d;
	d.dx = d.dy = d.dz = TLength(0.0);

	for (auto& it : secondary)
		writePUNPoint(it, d, false);
}

void TChabaFileWriter::writePUNPoint(const LGCAdjustablePoint & it, delta d, bool writeDeltas)
{
	TAStreamFormatter* stream = getStream();
	stream->setLengthUnits(TLength::EUnits::kMetres);
	std::string sep = stream->getSeparator();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	//write the point name
	converter.writeName(it.getName(), getNameWidth());
	(*stream) << sep;

	//write the transformed coordinates of the passive point (coordinates in the root frame)
	TPositionVector xyhToValue = it.getEstimatedValue();
	TLOR2LOR transfo = TLOR2LOR(it.getFrameTreePosition(), fProjectData->getTree().begin(), "transfo");
	//transform coordinates in root
	transfo.transform(xyhToValue);

	//transform Z in H
	if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		TXYH2CCS::CCS2XYHs(xyhToValue);
	else if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		TXYH2CCS::CCS2XYHg2000Machine(xyhToValue);
	else if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		TXYH2CCS::CCS2XYHg1985Machine(xyhToValue);

	converter.write3Coordinates(getCoordWidth(), getCoordPrecision(), sep, xyhToValue);

	if (writeDeltas)
	{
		(*stream) << sep;
		//Change output properties from metres to millimetres
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(getCoordWidth() - 5);
		stream->setPrecisionFormat(getCoordPrecision());
		//Print difference vector and total difference
		TFreeVector diff(d.dx, d.dy, d.dz, TCoordSysFactory::k3DCartesian);
		(*stream) << diff << sep;
		(*stream) << diff.length();
	}
	(*stream) << endl;
}

void TChabaFileWriter::writeTitle(const TLGCData &projet)
{
	TAStreamFormatter* stream = getStream();
	std::string sep = stream->getSeparator();

	(*stream) << endl;
	(*stream) << "**********************************************************************************************************" << endl << endl;

	(*stream) << "                                                 " << "LGC - Best-Fit " << TLGCApp::getProgId() << "                                        " << endl;
	(*stream) << "                                " << "Copyright 2003 - 2016, CERN ACE / SU.All rights reserved." << "                                        " << endl << endl;

	(*stream) << "***********************************************************************************************************" << endl;
	(*stream) << "                                         ----- FICHIER OUTPUT  -----" << endl;
	(*stream) << "***********************************************************************************************************" << endl;
	(*stream) << endl;


	time_t ltime;
	time(&ltime);
	(*stream) << sep << "Fait: " << sep << ctime(&ltime) << endl;


	(*getStream()) << sep << "project path:             " << projet.getConfig().title << "\n";

	
		(*getStream()) << sep << "File referential system:   ";
		if (projet.getConfig().referential == TRefSystemFactory::ERefFrame::kCCS)
			(*getStream()) << ", coordinate system: 3DCartesian, reference frame: kCCS";
		else
		{
			if (projet.getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
				(*getStream()) << ", coordinate system: 2DPlusH, reference frame: kCERNXYHsSphereSPS\n";
			else if (projet.getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
				(*getStream()) << ", coordinate system: 2DPlusH, reference frame: kCernXYHg00Machine\n";
			else if (projet.getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
				(*getStream()) << ", coordinate system: 2DPlusH, reference frame: kCernXYHg85Machine\n";
			else
				(*getStream()) << ", coordinate system: 3DCartesian, reference frame: kLocal\n";
		}

	

	(*getStream()) << sep << "precision: " << projet.getConfig().outPrecision.digits << "\n" << "\n";

	TReal upLimits = projet.getChiS0UpLimit();
	TReal lowLimits =projet.getChiS0LowLimit();
	TReal S0 = projet.getS0APosteriori();
	(*stream) << sep << "S0 A Posteriori: ";
	fStream->writeDouble(16, 7, S0);
	(*stream) << "\n";
	(*stream) << sep << "Fisher's Limits: ";
	fStream->writeDouble(16, 7, lowLimits);
	(*stream) << "  -  ";
	fStream->writeDouble(8, 7, upLimits);
	(*stream) << "\n";

	if (S0 > upLimits || S0 < lowLimits)
		(*stream) << sep << " S0 is applied on the sigma \n";
}

const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> TChabaFileWriter::createPair(const TLGCData &project, TDataTreeIterator itFrame)
{
	std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> commonPoints;
	std::vector<LGCAdjustablePoint> newPoint; //point corresponding to the OBSXYZ, there are defined in the ROOT
	std::vector<TOBSXYZ> oldPt;  //OBSXYZ

	//toPoint
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
		if (itTree == itFrame)
			if (!itTree.node->data->measurements.fOBSXYZ.empty())
				oldPt = { std::begin(itTree.node->data->measurements.fOBSXYZ), std::end(itTree.node->data->measurements.fOBSXYZ) };
	
	for (auto& itOBS = oldPt.begin(); itOBS != oldPt.end(); itOBS++)
	{
		const LGCAdjustablePoint& pt = project.getPoints().getObject(itOBS->station->getName());
		if(pt.getFrameTreePosition().node->data->isROOTNode())
			newPoint.emplace_back(pt);
	}

	//make pair vector, sort the 2 vectors
	for (auto& ItNewPoint : newPoint)
	{
		for (auto& ItOldPoint : oldPt)
		{
			if (ItOldPoint.station->getName() == ItNewPoint.getName())
				commonPoints.emplace_back(std::pair<LGCAdjustablePoint, TOBSXYZ>(ItNewPoint, ItOldPoint));
		}
	}
	return commonPoints;

}

const std::vector<LGCAdjustablePoint> TChabaFileWriter::createSecPoint(const TLGCData &project, TDataTreeIterator itFrame)
{
	std::vector<LGCAdjustablePoint> secondaryPoint; //other points defined in the frame
	
	for (auto& pt = project.getPoints().begin(); pt != project.getPoints().end(); pt++)
		if (pt->getFrameTreePosition() == itFrame)
			secondaryPoint.emplace_back(*pt);
	
	return secondaryPoint;
}
