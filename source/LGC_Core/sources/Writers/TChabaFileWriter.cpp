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
#include <TPointTransformer.h>


TChabaFileWriter::TChabaFileWriter(TAStreamFormatter* stream, const TLGCData* project) : TAFileWriter(stream, project)
{//constructor
}

TChabaFileWriter::~TChabaFileWriter()
{//destructor
}

void TChabaFileWriter::writeFile(TAStreamFormatter* stream)
{
	std::string sep = stream->getSeparator();
	writeTitle();

	//write transformation and points information for each bestfit
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		if (itTree->get()->frame.getName() != "ROOT")
		{
			// write transformation parameters
			writeHelmertTransformationDetails(itTree.node->data->frame);

			// write the input data
			(*stream) << sep << "DONNÉES D'ENTRÉE\n";
			(*stream) << sep << "ACTIF\n";
			writeInputPoints(keepOBSXYZ(fProjectData->getTree().begin())); //first node is root

			(*stream) << sep << "PASSIF\n";
			writeInputPoints(keepOBSXYZ(itTree));
			(*stream) << endl << endl;

			//write results
			(*stream) << sep << "NOUVELLES COORDONNÉES DES POINTS REFERENCE (ACTIF) \n" << endl;;
			writeTransformedPoints(createPair(fProjectData->getTree().begin()), true, fProjectData->getTree().begin());
			(*stream) << endl << endl;
			
			(*stream) << sep << "COORDONNÉES DES POINTS REFERENCE (PASSIF) DANS LE NOUVEAU SYSTÈME\n" << endl;;
			writeTransformedPoints(createPair(itTree), true, itTree);
			(*stream) << endl << endl;
			
			(*stream) << sep << "COORDONNÉES POINTS MODIFIES (PASSIFS) DANS LE NOUVEAU SYSTÈME\n" << endl;;
			writeTransformedSecondaryPoints(createSecPoint(itTree));
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

void TChabaFileWriter::writeInputPoints(const std::vector< TOBSXYZ> & data)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	//Write Column Headings
	(*stream) << endl;
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)");
	writeStringSep(getCoordWidth(), "Y(M)");
	writeStringSep(getCoordWidth(), "Z(M)");
	(*stream) << getSeparator();
	writeStringSep(getCoordResWidth(), "SX(MM)");
	writeStringSep(getCoordResWidth(), "SY(MM)");
	writeStringSep(getCoordResWidth(), "SZ(MM)");
	(*stream) << endl;

	for (auto& itPair : data)
	{
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(getCoordWidth());
		stream->setPrecisionFormat(getCoordPrecision());

		converter.writeName(itPair.station->getName(), getNameWidth());
		(*stream) << getSeparator();

		//xyz
		converter.write3Coordinates(getCoordWidth(), getCoordPrecision(), getSeparator(), itPair.initialValue);
		
		//sx sy sz
		converter.writeCoordinateParam(TSpatialStatus::ESpatialStatus::kVxyz,
			getCoordResWidth(),
			getCoordPrecision(),
			TLength::EUnits::kMillimetres,
			getSeparator(),
			itPair.getXObservedStDev(),
			itPair.getYObservedStDev(),
			itPair.getZObservedStDev(),
			"");
		
		(*stream) << endl;
	}
}

void TChabaFileWriter::writeTransformedPoints(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> & pairs, bool writeDeltas, TDataTreeIterator itTree)
{
	TAStreamFormatter* stream = getStream();

	//write column headings
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)"); 
	writeStringSep(getCoordWidth(), "Y(M)");
	writeStringSep(getCoordWidth(), "Z(M)");
	if (pairs.at(0).first.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		writeStringSep(getCoordWidth(), "H(M)");
	(*stream) << getSeparator();
	writeStringSep(getCoordResWidth(), "SX(MM)");
	writeStringSep(getCoordResWidth(), "SY(MM)");
	writeStringSep(getCoordResWidth(), "SZ(MM)");
		
	if (writeDeltas)
	{
		(*stream) << getSeparator();
		writeStringSep(getCoordResWidth(), "DX(MM)");
		writeStringSep(getCoordResWidth(), "DY(MM)");
		writeStringSep(getCoordResWidth(), "DZ(MM)");
		writeStringSep(getCoordResWidth(), "DD(MM)");
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
		d.dz = stationRoot.getZ() - it.first.getEstValue(2);

		sumdd2 += pow2(d.dx.getMMetresValue()) + pow2(d.dy.getMMetresValue()) + pow2(d.dz.getMMetresValue());

		TFreeVector sigma(ptInRoot.getEstimatedValue().getCoordSys());
		if (itTree.node->data->isROOTNode())
		{
			sigma.setX(it.first.getXEstPrecision());
			sigma.setY(it.first.getYEstPrecision());
			sigma.setZ(it.first.getZEstPrecision());
		}
		else
			//need to transform sigma in the root
			sigma = transformSigmaInRoot(it.second.initialValue, itTree, it.second.getXObservedStDev(), it.second.getYObservedStDev(), it.second.getZObservedStDev());

		writePUNPoint(ptInRoot,sigma, d, writeDeltas);
	}
	
	
	//write EMQ from primary passive point information and write it at the bottom of the list
	double fEMQ = sqrt(sumdd2 / usablePointCount);
	(*stream)<<endl;
	if (writeDeltas)
	{
		(*stream) << getSeparator() << getSeparator() << getSeparator() << "*** EMQ DD (MM) = " << getSeparator();
		writeDouble(7, 4, fEMQ);
		(*stream) << getSeparator() << "***" << endl;
	}
}

void TChabaFileWriter::writeTransformedSecondaryPoints(const std::vector<LGCAdjustablePoint> & secondary)
{
	delta d;
	d.dx = d.dy = d.dz = TLength(0.0);

	TAStreamFormatter* stream = getStream();

	//write column headings
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)");
	writeStringSep(getCoordWidth(), "Y(M)");
	writeStringSep(getCoordWidth(), "Z(M)");
	if (secondary.at(0).getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		writeStringSep(getCoordWidth(), "H(M)");
	(*stream) << getSeparator();
	writeStringSep(getCoordResWidth(), "SX(MM)");
	writeStringSep(getCoordResWidth(), "SY(MM)");
	writeStringSep(getCoordResWidth(), "SZ(MM)");
	(*stream) << endl;

	for (auto& it : secondary)
		writePUNPoint(it, it.transformSigmaInRoot(it,fProjectData), d, false);
}

void TChabaFileWriter::writePUNPoint(const LGCAdjustablePoint & it, TFreeVector sigma, delta d, bool writeDeltas)
{
	TAStreamFormatter* stream = getStream();
	stream->setLengthUnits(TLength::EUnits::kMetres);
	std::string sep = stream->getSeparator();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	
	//write the point name
	converter.writeName(it.getName(), getNameWidth());
	(*stream) << sep;

	//transform coordinates in root
	TPositionVector xyhToValue = it.getEstimatedValue();
	TLOR2LOR transfo = TLOR2LOR(it.getFrameTreePosition(), fProjectData->getTree().begin(), "transfo");
	transfo.transform(xyhToValue);

	//transform Z in H
	if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		TXYH2CCS::CCS2XYHs(xyhToValue);
	else if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		TXYH2CCS::CCS2XYHg2000Machine(xyhToValue);
	else if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		TXYH2CCS::CCS2XYHg1985Machine(xyhToValue);

	//write coordinates
	converter.write3Coordinates(getCoordWidth(), getCoordPrecision(), sep, it.getEstimatedValue());
	if (it.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		(*stream) << xyhToValue.getH() << sep;

	//Write point's estimated precision after calculation
	converter.writeCoordinateParam(TSpatialStatus::ESpatialStatus::kVxyz,
		getCoordResWidth(),
		getCoordPrecision(),
		TLength::EUnits::kMillimetres,
		sep,
		sigma.getX(),
		sigma.getY(),
		sigma.getZ(),
		"");/*sigma*/


	//write differences
	if (writeDeltas)
	{
		(*stream) << sep;
		//Change output properties from metres to millimetres
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(getCoordResWidth());
		stream->setPrecisionFormat(getCoordPrecision());
		TFreeVector diff(d.dx, d.dy, d.dz, TCoordSysFactory::k3DCartesian);
		(*stream) << diff << sep;
		(*stream) << diff.length();
	}
	(*stream) << endl;
}

void TChabaFileWriter::writeTitle()
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


	(*getStream()) << sep << "project path:             " << fProjectData->getConfig().title << "\n";

	
		(*getStream()) << sep << "File referential system:   ";
		if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCCS)
			(*getStream()) << ", coordinate system: 3DCartesian, reference frame: kCCS";
		else
		{
			if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
				(*getStream()) << ", coordinate system: 2DPlusH, reference frame: kCERNXYHsSphereSPS\n";
			else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
				(*getStream()) << ", coordinate system: 2DPlusH, reference frame: kCernXYHg00Machine\n";
			else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
				(*getStream()) << ", coordinate system: 2DPlusH, reference frame: kCernXYHg85Machine\n";
			else
				(*getStream()) << ", coordinate system: 3DCartesian, reference frame: kLocal\n";
		}

	

	(*getStream()) << sep << "precision: " << fProjectData->getConfig().outPrecision.digits << "\n" << "\n";

	TReal upLimits = fProjectData->getChiS0UpLimit();
	TReal lowLimits = fProjectData->getChiS0LowLimit();
	TReal S0 = fProjectData->getS0APosteriori();
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

const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> TChabaFileWriter::createPair(TDataTreeIterator itFrame)
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
		const LGCAdjustablePoint& pt = fProjectData->getPoints().getObject(itOBS->station->getName());
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

const std::vector<TOBSXYZ> TChabaFileWriter::keepOBSXYZ(TDataTreeIterator itFrame)
{
	std::vector<TOBSXYZ> obs;  //OBSXYZ

	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
		if (itTree == itFrame)
			if (!itTree.node->data->measurements.fOBSXYZ.empty())
				obs = { std::begin(itTree.node->data->measurements.fOBSXYZ), std::end(itTree.node->data->measurements.fOBSXYZ) };

	return obs;
}

const std::vector<LGCAdjustablePoint> TChabaFileWriter::createSecPoint(TDataTreeIterator itFrame)
{
	std::vector<LGCAdjustablePoint> secondaryPoint; //other points defined in the frame

	for (const auto& pt : fProjectData->getPoints())
		if (pt.getFrameTreePosition() == itFrame)
			secondaryPoint.emplace_back(pt);
	
	return secondaryPoint;
}

TFreeVector TChabaFileWriter::transformSigmaInRoot(const TPositionVector pt, TDataTreeIterator itTree, TLength sx, TLength sy, TLength sz)
{
	TPositionVector ptInSF = pt;
	TFreeVector sigmaRoot(ptInSF.getCoordSys());
	TPointTransformer fPointTransfo(&fProjectData->getTree(), fProjectData->getConfig().referential);

	//actual frame iterator
	TDataTreeIterator frameIt = itTree;
	// actual frame
	auto frame = itTree.node->data.get()->frame;


	//transformed covariance matrix
	TSparseMatrix covRoot(3, 3);
	//at the fist iteration , point covariance are:
	covRoot.insert(0, 0) = pow2q(sx);
	covRoot.insert(1, 1) = pow2q(sy);
	covRoot.insert(2, 2) = pow2q(sz);
	covRoot.insert(0, 1) = 0.0;
	covRoot.insert(1, 0) = 0.0;
	covRoot.insert(0, 2) = 0.0;
	covRoot.insert(2, 0) = 0.0;
	covRoot.insert(1, 2) = 0.0;
	covRoot.insert(2, 1) = 0.0;

	while (frame.getName() != "ROOT")
	{
		// transform point in the actual subframe
		const TLOR2LOR& lorTrafo = fPointTransfo.getLORTransformation(itTree, frameIt); //Transformation from "TARGET POSITION" to "node n-1"
		lorTrafo.transform(ptInSF);


		//Matrix C : partial derive restect to tx ty tz rx ry rz x y z
		TSparseMatrix C(3, 10);
		//tx
		C.insert(0, 0) = 1.0;
		//ty
		C.insert(1, 1) = 1.0;
		//tz
		C.insert(2, 2) = 1.0;
		//rx
		C.insert(0, 3) = 0.0;
		C.insert(1, 3) = ((sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getY()
			+ (cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getZ())*frame.getEstScale();
		C.insert(2, 3) = ((sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) - cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) - sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getY()
			- (cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getZ())*frame.getEstScale();
		//ry
		C.insert(0, 4) = (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))* ptInSF.getX()
			- sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))* ptInSF.getY()
			- cos(frame.getEstRotation(1))* ptInSF.getZ())*frame.getEstScale();
		C.insert(1, 4) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getX()
			+ sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getY()
			- sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getZ())*frame.getEstScale();
		C.insert(2, 4) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getX()
			+ sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getY()
			- sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getZ())*frame.getEstScale();
		//rz
		C.insert(0, 5) = (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getX()
			+ cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getY())*frame.getEstScale();
		C.insert(1, 5) = ((-cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) - sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getY())*frame.getEstScale();
		C.insert(2, 5) = ((cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) - sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getX()
			+ (sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getY())*frame.getEstScale();
		//scale
		C.insert(0, 6) = cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getX()
			+ sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1))* ptInSF.getY()
			- sin(frame.getEstRotation(1))* ptInSF.getZ();
		C.insert(1, 6) = (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getX()
			+ (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))* ptInSF.getY()
			+ cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0))* ptInSF.getZ();
		C.insert(2, 6) = (sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))* ptInSF.getY()
			+ cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0))* ptInSF.getZ();
		//x
		C.insert(0, 7) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(1)))*frame.getEstScale();
		C.insert(1, 7) = (-sin(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))*frame.getEstScale();
		C.insert(2, 7) = (sin(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))*frame.getEstScale();
		//y
		C.insert(0, 8) = (sin(frame.getEstRotation(2))*cos(frame.getEstRotation(1)))*frame.getEstScale();
		C.insert(1, 8) = (cos(frame.getEstRotation(2))*cos(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))*frame.getEstScale();
		C.insert(2, 8) = (-cos(frame.getEstRotation(2))*sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2))*sin(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))*frame.getEstScale();
		//z
		C.insert(0, 9) = (-sin(frame.getEstRotation(1)))*frame.getEstScale();
		C.insert(1, 9) = (cos(frame.getEstRotation(1))*sin(frame.getEstRotation(0)))*frame.getEstScale();
		C.insert(2, 9) = (cos(frame.getEstRotation(1))*cos(frame.getEstRotation(0)))*frame.getEstScale();

		//covariance matrix = (cov frame, 0 ; 0, cov point)
		TSparseMatrix covSF(10, 10);

		//Fill covariance matrix
		//point covariance are:
		covSF.insert(7, 7) = covRoot.coeff(0, 0);
		covSF.insert(8, 8) = covRoot.coeff(1, 1);
		covSF.insert(9, 9) = covRoot.coeff(2, 2);
		covSF.insert(7, 8) = covRoot.coeff(0, 1);
		covSF.insert(8, 7) = covRoot.coeff(0, 1);
		covSF.insert(7, 9) = covRoot.coeff(0, 2);
		covSF.insert(9, 7) = covRoot.coeff(0, 2);
		covSF.insert(8, 9) = covRoot.coeff(1, 2);
		covSF.insert(9, 8) = covRoot.coeff(1, 2);

		//and fill the frame covariances covSF
		if (!frame.isTranslationFixed(0))
		{
			covSF.insert(0, 0) = pow2q(frame.getEstimatedPrecisionTransl(0));
			if (!frame.isTranslationFixed(1))
			{
				covSF.insert(0, 1) = frame.getXYCovarTransl();
				covSF.insert(1, 0) = frame.getXYCovarTransl();
			}
			if (!frame.isTranslationFixed(2))
			{
				covSF.insert(0, 2) = frame.getXZCovarTransl();
				covSF.insert(2, 0) = frame.getXZCovarTransl();
			}
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(0, 3) = frame.getTrRotCovar(0);
				covSF.insert(3, 0) = frame.getTrRotCovar(0);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(0, 4) = frame.getTrRotCovar(1);
				covSF.insert(4, 0) = frame.getTrRotCovar(1);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(0, 5) = frame.getTrRotCovar(2);
				covSF.insert(5, 0) = frame.getTrRotCovar(2);
			}
			if (!frame.isScaleFixed())
			{
				covSF.insert(0, 6) = frame.getScaleCovar(0);
				covSF.insert(6, 0) = frame.getScaleCovar(0);
			}
			//
		}
		if (!frame.isTranslationFixed(1))
		{
			covSF.insert(1, 1) = pow2q(frame.getEstimatedPrecisionTransl(1));
			if (!frame.isTranslationFixed(2))
			{
				covSF.insert(1, 2) = frame.getYZCovarTransl();
				covSF.insert(2, 1) = frame.getYZCovarTransl();
			}
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(1, 3) = frame.getTrRotCovar(3);
				covSF.insert(3, 1) = frame.getTrRotCovar(3);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(1, 4) = frame.getTrRotCovar(4);
				covSF.insert(4, 1) = frame.getTrRotCovar(4);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(1, 5) = frame.getTrRotCovar(5);
				covSF.insert(5, 1) = frame.getTrRotCovar(5);
			}
			if (!frame.isScaleFixed())
			{
				covSF.insert(1, 6) = frame.getScaleCovar(1);
				covSF.insert(6, 1) = frame.getScaleCovar(1);
			}
			//
		}
		if (!frame.isTranslationFixed(2))
		{
			covSF.insert(2, 2) = pow2q(frame.getEstimatedPrecisionTransl(2));
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(2, 3) = frame.getTrRotCovar(6);
				covSF.insert(3, 2) = frame.getTrRotCovar(6);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(2, 4) = frame.getTrRotCovar(7);
				covSF.insert(4, 2) = frame.getTrRotCovar(7);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(2, 5) = frame.getTrRotCovar(8);
				covSF.insert(5, 2) = frame.getTrRotCovar(8);
			}
			if (!frame.isScaleFixed())
			{
				covSF.insert(2, 6) = frame.getScaleCovar(2);
				covSF.insert(6, 2) = frame.getScaleCovar(2);
			}
			//
		}

		if (!frame.isRotationFixed(0))
		{
			covSF.insert(3, 3) = pow2q(frame.getEstimatedPrecisionRot(0).getRadiansValue());
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(3, 4) = frame.getXYCovarRot();
				covSF.insert(4, 3) = frame.getXYCovarRot();
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(3, 5) = frame.getXZCovarRot();
				covSF.insert(5, 3) = frame.getXZCovarRot();
			}
		}
		if (!frame.isRotationFixed(1))
		{
			covSF.insert(4, 4) = pow2q(frame.getEstimatedPrecisionRot(1).getRadiansValue());
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(4, 5) = frame.getYZCovarRot();
				covSF.insert(5, 4) = frame.getYZCovarRot();
			}
		}
		if (!frame.isRotationFixed(2))
		{
			covSF.insert(5, 5) = pow2q(frame.getEstimatedPrecisionRot(2).getRadiansValue());
		}


		if (!frame.isScaleFixed())
		{
			covSF.insert(6, 6) = pow2q(frame.getEstimatedPrecisionScale());
			//
			if (!frame.isRotationFixed(0))
			{
				covSF.insert(6, 3) = frame.getScaleCovar(3);
				covSF.insert(3, 6) = frame.getScaleCovar(3);
			}
			if (!frame.isRotationFixed(1))
			{
				covSF.insert(6, 4) = frame.getScaleCovar(4);
				covSF.insert(4, 6) = frame.getScaleCovar(4);
			}
			if (!frame.isRotationFixed(2))
			{
				covSF.insert(6, 5) = frame.getScaleCovar(5);
				covSF.insert(5, 6) = frame.getScaleCovar(5);
			}
		}

		// calculated new point covariance in the upper frame
		covRoot = C*covSF*(C.transpose());

		//frame, go up
		frameIt = frameIt.node->parent;
		frame = frameIt.node->data.get()->frame;
	}

	//extract variance in root
	sigmaRoot.setX(TLength(sqrt(covRoot.coeff(0, 0))));
	sigmaRoot.setY(TLength(sqrt(covRoot.coeff(1, 1))));
	sigmaRoot.setZ(TLength(sqrt(covRoot.coeff(2, 2))));

	return sigmaRoot;
}