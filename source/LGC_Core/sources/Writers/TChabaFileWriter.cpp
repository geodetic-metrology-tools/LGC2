#include "TChabaFileWriter.h"

#include <TAStreamFormatter.h>
#include <TAdjustableHelmertTransformation.h>
#include <TLOR2LOR.h>
#include <TPointTransformer.h>
#include <TRefSystemFactory.h>
#include <TRotation.h>
#include <TRotationMatrix.h>
#include <TXYH2CCS.h>

#include "TLGCApp.h"
#include "TLGCData.h"

TChabaFileWriter::TChabaFileWriter(TAStreamFormatter *stream, const TLGCData *project) : TAFileWriter(stream, project)
{ // constructor
	stream->setSeparator("   ");
}

TChabaFileWriter::~TChabaFileWriter()
{ // destructor
}

void TChabaFileWriter::writeFile(TAStreamFormatter *stream)
{
	std::string sep = stream->getSeparator();
	writeTitle();

	// write transformation and points information for each bestfit
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		if (itTree->get()->frame.getName() != "ROOT")
		{
			// write transformation parameters
			writeHelmertTransformationDetails(itTree.node->data->frame, itTree.node->data->ID);

			// write the input data
			(*stream) << sep << "DONNÉES D'ENTRÉE" << endl << endl;
			(*stream) << sep << "ACTIF" << endl;

			const std::vector<TOBSXYZ> obsActif = keepOBSXYZ(fProjectData->getTree().begin());
			if (!obsActif.empty())
				writeInputPoints(obsActif); // first node is root
			else
				(*stream)
					<< "Don't be able to display data. See https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=55117116 for a correct input file and output data" << endl;

			const std::vector<TOBSXYZ> obsPassif = keepOBSXYZ(itTree);
			(*stream) << endl << sep << "PASSIF" << endl;
			if (!obsPassif.empty())
				writeInputPoints(obsPassif);
			else
				(*stream)
					<< "Don't be able to display data. See https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=55117116 for a correct input file and output data" << endl;
			(*stream) << endl << endl;

			// write results
			const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> pairActif = createPair(fProjectData->getTree().begin());
			(*stream) << sep << "NOUVELLES COORDONNÉES DES POINTS REFERENCE (ACTIF)" << endl << endl;
			if (!pairActif.empty())
				writeTransformedPoints(pairActif, true, fProjectData->getTree().begin());
			else
				(*stream)
					<< "Don't be able to display data. See https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=55117116 for a correct input file and output data" << endl;
			(*stream) << endl << endl;

			const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> pairPassif = createPair(itTree);
			(*stream) << sep << "COORDONNÉES DES POINTS REFERENCE (PASSIF) DANS LE NOUVEAU SYSTÈME" << endl << endl;
			if (!pairPassif.empty())
				writeTransformedPoints(pairPassif, true, itTree);
			else
				(*stream)
					<< "Don't be able to display data. See https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=55117116 for a correct input file and output data" << endl;
			(*stream) << endl << endl;

			const std::vector<LGCAdjustablePoint> secondaryPts = createSecPoint(itTree);
			if (!secondaryPts.empty())
			{
				(*stream) << sep << "COORDONNÉES POINTS MODIFIES (PASSIFS) DANS LE NOUVEAU SYSTÈME" << endl << endl;
				writeTransformedSecondaryPoints(secondaryPts);
			}
		}
	}
}

void TChabaFileWriter::writeHelmertTransformationDetails(const TAdjustableHelmertTransformation &helmert, const std::vector<int> &ID)
{
	// obtain attributes stored in parent object
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();
	int coordPrecision = getCoordPrecision();

	// set output precision for the parameters
	int rotMatrixPrecision = coordPrecision + 7;
	int anglePrecision = coordPrecision + 5;
	int translationPrecision = coordPrecision;
	int scalePrecision = coordPrecision + 7;

	// set up a rotation matrix
	TRotation rot(TRotationMatrix::ERotationType::kRxyz, helmert.getEstRotation(0).getRadiansValue(), helmert.getEstRotation(1).getRadiansValue(),
		helmert.getEstRotation(2).getRadiansValue());

	// set output units for different types of measurement
	stream->setPrecisionFormat(4);
	stream->setAngleUnits(TAngle::kGons);
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(17);

	std::string nameID;
	for (std::vector<int>::const_iterator it = ID.begin(); it != ID.end(); ++it)
	{
		if (it == ID.begin())
			nameID += std::to_string(*it);
		else
			nameID += "_" + std::to_string(*it);
	}

	// write the results table
	(*stream) << endl;
	(*stream) << sep << "FRAME\t" << helmert.getName() << "  ID(" << nameID << ")" << endl;
	(*stream) << endl;
	(*stream) << sep << "**************** INFORMATION CONCERNANT LES PARAMETRES CALCULÉS ********************" << endl;
	(*stream) << sep << "*                                                                                  *" << endl;
	(*stream) << sep << "*                         TERMES DE LA MATRICE DE ROTATION:                        *" << endl;
	(*stream) << sep << "*                                                                                  *" << endl;
	(*stream) << sep << "* " << sep << "R11 = ";
	writeDouble(17, rotMatrixPrecision, rot(0, 0));
	(*stream) << sep << "R12 = ";
	writeDouble(17, rotMatrixPrecision, rot(0, 1));
	(*stream) << sep << "R13 = ";
	writeDouble(17, rotMatrixPrecision, rot(0, 2));
	(*stream) << sep << "*" << endl;
	(*stream) << sep << "* " << sep << "R21 = ";
	writeDouble(17, rotMatrixPrecision, rot(1, 0));
	(*stream) << sep << "R22 = ";
	writeDouble(17, rotMatrixPrecision, rot(1, 1));
	(*stream) << sep << "R23 = ";
	writeDouble(17, rotMatrixPrecision, rot(1, 2));
	(*stream) << sep << "*" << endl;
	(*stream) << sep << "* " << sep << "R31 = ";
	writeDouble(17, rotMatrixPrecision, rot(2, 0));
	(*stream) << sep << "R32 = ";
	writeDouble(17, rotMatrixPrecision, rot(2, 1));
	(*stream) << sep << "R33 = ";
	writeDouble(17, rotMatrixPrecision, rot(2, 2));
	(*stream) << sep << "*" << endl;

	(*stream) << sep << "*                                                                                  *" << endl;
	(*stream) << sep << "*        ANGLES DE ROTATION (Gons):                                                *" << endl;
	(*stream) << sep << "*                                                                                  *" << endl;

	(*stream) << sep << "*" << sep << sep << sep << "OMEGA(x) = ";
	(writeAngle(17, anglePrecision, TAngle::kGons, rot.getAngles(TRotationMatrix::kRzyx).omega));
	(*stream) << " ";
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;

	(*stream) << sep << "*" << sep << sep << sep << "PHI(y)   = ";
	(writeAngle(17, anglePrecision, TAngle::kGons, rot.getAngles(TRotationMatrix::kRzyx).phi));
	(*stream) << " ";
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;

	(*stream) << sep << "*" << sep << sep << sep << "KAPPA(z) = ";
	(writeAngle(17, anglePrecision, TAngle::kGons, rot.getAngles(TRotationMatrix::kRzyx).kappa));
	(*stream) << " ";
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

	(*stream) << sep << "*                                                                                  *" << endl;
	(*stream) << sep << "*        FACTEUR D'ÉCHELLE (SANS UNITÉ):                                           *" << endl;
	(*stream) << sep << "*                                                                                  *" << endl;

	(*stream) << sep << "*" << sep << sep << sep << "ECH = ";
	writeDouble(17, scalePrecision, helmert.getEstScale());
	(*stream) << sep << sep << sep << sep << "  " << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << sep << "*" << endl;

	(*stream) << sep << "*                                                                                  *" << endl;
	(*stream) << sep << "************************************************************************************" << endl;
	(*stream) << endl;
	(*stream) << endl;
}

void TChabaFileWriter::writeInputPoints(const std::vector<TOBSXYZ> &data)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	// Write Column Headings
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)");
	writeStringSep(getCoordWidth(), "Y(M)");
	writeStringSep(getCoordWidth(), "Z(M)");
	(*stream) << sep << " ";
	writeStringSep(getCoordResWidth(), "SX(MM)");
	writeStringSep(getCoordResWidth(), "SY(MM)");
	writeStringSep(getCoordResWidth(), "SZ(MM)");
	(*stream) << endl;

	for (auto &itPair : data)
	{
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(getCoordWidth());
		stream->setPrecisionFormat(getCoordPrecision());

		converter.writeName(itPair.station->getName(), getNameWidth());

		// xyz
		converter.write3Coordinates(getCoordWidth(), getCoordPrecision(), "", itPair.obsValue);

		// sx sy sz
		converter.writeCoordinateParam(TSpatialStatus::ESpatialStatus::kVxyz, getCoordResWidth(), getCoordPrecision(), TLength::EUnits::kMillimetres, sep,
			itPair.getXObservedStDev(), itPair.getYObservedStDev(), itPair.getZObservedStDev(), "");

		(*stream) << endl;
	}
}

void TChabaFileWriter::writeTransformedPoints(const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> &pairs, bool writeDeltas, TDataTreeIterator itTree)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	// write column headings
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)");
	writeStringSep(getCoordWidth(), "Y(M)");
	writeStringSep(getCoordWidth(), "Z(M)");
	if (pairs.at(0).first.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		writeStringSep(getCoordWidth(), "H(M)");
	(*stream) << sep << " ";
	writeStringSep(getCoordResWidth(), "SX(MM)");
	writeStringSep(getCoordResWidth(), "SY(MM)");
	writeStringSep(getCoordResWidth(), "SZ(MM)");

	if (writeDeltas)
	{
		(*stream) << sep;
		writeStringSep(getCoordResWidth(), "DX(MM)");
		(*stream) << sep;
		writeStringSep(getCoordResWidth(), "DY(MM)");
		(*stream) << sep;
		writeStringSep(getCoordResWidth(), "DZ(MM)");
		(*stream) << sep;
		writeStringSep(getCoordResWidth(), "DD(MM)");
	}
	(*stream) << endl;

	int usablePointCount = 0;
	TReal sumdd2 = 0.0;

	for (auto &it : pairs)
	{
		++usablePointCount;

		// write the transformed coordinates of the passive point (coordinates in the root frame)
		TPositionVector stationRoot = it.second.obsValue;
		TLOR2LOR transfo = TLOR2LOR(it.second.positionInTree, fProjectData->getTree().begin(), "transfo");
		// transform coordinates in root
		transfo.transform(stationRoot);
		LGCAdjustablePoint ptInRoot(stationRoot, 0, 0, 0, it.first.getName(), it.first.getReferenceFrame(), it.first.getFrameTreePosition());

		delta d;
		d.dx = stationRoot.getX() - it.first.getEstValue(0);
		d.dy = stationRoot.getY() - it.first.getEstValue(1);
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
			// need to transform sigma in the root
			sigma = transformSigmaInRoot(it.second.obsValue, itTree, it.second.getXObservedStDev(), it.second.getYObservedStDev(), it.second.getZObservedStDev());

		writePUNPoint(ptInRoot, sigma, d, writeDeltas);
	}

	// write EMQ from primary passive point information and write it at the bottom of the list
	double fEMQ = sqrt(sumdd2 / usablePointCount);
	(*stream) << endl;
	if (writeDeltas)
	{
		(*stream) << sep << sep << sep << "*** EMQ DD (MM) = " << sep;
		writeDouble(7, 4, fEMQ);
		(*stream) << sep << "***" << endl;
	}
}

void TChabaFileWriter::writeTransformedSecondaryPoints(const std::vector<LGCAdjustablePoint> &secondary)
{
	delta d;
	d.dx = d.dy = d.dz = TLength(0.0);

	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	// write column headings
	writeStringLeftSep(getNameWidth(), "NOM");
	writeStringSep(getCoordWidth(), "X(M)");
	writeStringSep(getCoordWidth(), "Y(M)");
	writeStringSep(getCoordWidth(), "Z(M)");
	if (secondary.at(0).getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		writeStringSep(getCoordWidth(), "H(M)");
	(*stream) << sep << " ";
	writeStringSep(getCoordResWidth(), "SX(MM)");
	writeStringSep(getCoordResWidth(), "SY(MM)");
	writeStringSep(getCoordResWidth(), "SZ(MM)");
	(*stream) << endl;

	for (auto &it : secondary)
		writePUNPoint(it, it.transformSigmaInRoot(it, fProjectData), d, false);
}

void TChabaFileWriter::writePUNPoint(const LGCAdjustablePoint &it, TFreeVector sigma, delta d, bool writeDeltas)
{
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	stream->setLengthUnits(TLength::EUnits::kMetres);
	TPointConverter converter(stream, fProjectData->getConfig().referential);

	// write the point name
	converter.writeName(it.getName(), getNameWidth());

	// transform coordinates in root
	TPositionVector xyhToValue = it.getEstimatedValue();
	TLOR2LOR transfo = TLOR2LOR(it.getFrameTreePosition(), fProjectData->getTree().begin(), "transfo");
	transfo.transform(xyhToValue);

	// write coordinates (X,Y,Z and possibly H when non-local reference frame)
	converter.write3Coordinates(getCoordWidth(), getCoordPrecision(), "", xyhToValue);
	if (it.getReferenceFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame)
	{
		// transform Z in H
		if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			TXYH2CCS::CCS2XYHs(xyhToValue);
		else if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			TXYH2CCS::CCS2XYHg2000Machine(xyhToValue);
		else if (it.getReferenceFrame() == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			TXYH2CCS::CCS2XYHg1985Machine(xyhToValue);

		(*stream) << xyhToValue.getH();
	}

	// Write point's estimated precision after calculation
	converter.writeCoordinateParam(TSpatialStatus::ESpatialStatus::kVxyz, getCoordResWidth(), getCoordPrecision(), TLength::EUnits::kMillimetres, "", sigma.getX(),
		sigma.getY(), sigma.getZ(), ""); /*sigma*/

	// write differences
	if (writeDeltas)
	{
		(*stream) << sep;
		// Change output properties from metres to millimetres
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
	TAStreamFormatter *stream = getStream();
	std::string sep = stream->getSeparator();

	(*stream) << endl;
	(*stream) << "**********************************************************************************************************" << endl << endl;

	(*stream) << "     "
			  << "LGC - Best-Fit " << TLGCApp::getProgId() << endl;
	(*stream) << "     "
			  << "Copyright 2003 - 2016, CERN ACE / SU.All rights reserved." << endl
			  << endl;

	(*stream) << "***********************************************************************************************************" << endl;
	(*stream) << "                                         ----- FICHIER OUTPUT  -----" << endl;
	(*stream) << "***********************************************************************************************************" << endl;
	(*stream) << endl;

	time_t ltime;
	time(&ltime);
	(*stream) << sep << "Calculation date:       " << sep << ctime(&ltime) << endl;
	(*stream) << sep << "Project path:              " << fProjectData->getConfig().title << endl;

	(*stream) << sep << "File referential system:   ";
	if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCCS)
		(*stream) << "Coordinate system: 3DCartesian, reference frame: kCCS" << endl;
	else
	{
		if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
			(*stream) << "Coordinate system: 2DPlusH, reference frame: kCERNXYHsSphereSPS" << endl;
		else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
			(*stream) << "Coordinate system: 2DPlusH, reference frame: kCernXYHg00Machine" << endl;
		else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
			(*stream) << "Coordinate system: 2DPlusH, reference frame: kCernXYHg85Machine" << endl;
		else
			(*stream) << "Coordinate system: 3DCartesian, reference frame: kLocal" << endl;
	}

	(*stream) << sep << "Precision (digits): " << fProjectData->getConfig().outPrecision.digits << endl << endl;

	TReal upLimits = fProjectData->getChiS0UpLimit();
	TReal lowLimits = fProjectData->getChiS0LowLimit();
	TReal S0 = fProjectData->getS0APosteriori();
	(*stream) << sep << "S0 A Posteriori: ";
	fStream->writeDouble(16, 7, S0);
	(*stream) << endl;
	(*stream) << sep << "Fisher's Limits: ";
	fStream->writeDouble(16, 7, lowLimits);
	(*stream) << "  -  ";
	fStream->writeDouble(8, 7, upLimits);
	(*stream) << endl;

	if (S0 > upLimits || S0 < lowLimits)
		(*stream) << sep << sep << "S0 is applied on the sigma " << endl;
}

const std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> TChabaFileWriter::createPair(TDataTreeIterator itFrame)
{
	std::vector<std::pair<LGCAdjustablePoint, TOBSXYZ>> commonPoints;
	std::vector<LGCAdjustablePoint> newPoint; // point corresponding to the OBSXYZ, there are defined in the ROOT
	std::vector<TOBSXYZ> oldPt; // OBSXYZ

	// toPoint
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
		if (itTree == itFrame)
			if (!itTree.node->data->measurements.fOBSXYZ.empty())
				oldPt = {std::begin(itTree.node->data->measurements.fOBSXYZ), std::end(itTree.node->data->measurements.fOBSXYZ)};

	for (const auto &itOBS : oldPt)
	{
		const LGCAdjustablePoint &pt = fProjectData->getPoints().getObject(itOBS.station->getName());
		if (pt.getFrameTreePosition().node->data->isROOTNode())
			newPoint.emplace_back(pt);
	}

	// make pair vector, sort the 2 vectors
	for (auto &ItNewPoint : newPoint)
	{
		for (auto &ItOldPoint : oldPt)
		{
			if (ItOldPoint.station->getName() == ItNewPoint.getName())
				commonPoints.emplace_back(std::pair<LGCAdjustablePoint, TOBSXYZ>(ItNewPoint, ItOldPoint));
		}
	}
	return commonPoints;
}

const std::vector<TOBSXYZ> TChabaFileWriter::keepOBSXYZ(TDataTreeIterator itFrame)
{
	std::vector<TOBSXYZ> obs; // OBSXYZ

	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
		if (itTree == itFrame)
			if (!itTree.node->data->measurements.fOBSXYZ.empty())
				obs = {std::begin(itTree.node->data->measurements.fOBSXYZ), std::end(itTree.node->data->measurements.fOBSXYZ)};

	return obs;
}

const std::vector<LGCAdjustablePoint> TChabaFileWriter::createSecPoint(TDataTreeIterator itFrame)
{
	std::vector<LGCAdjustablePoint> secondaryPoint; // other points defined in the frame

	for (const auto &pt : fProjectData->getPoints())
		if (pt.getFrameTreePosition() == itFrame)
			secondaryPoint.emplace_back(pt);

	return secondaryPoint;
}

TFreeVector TChabaFileWriter::transformSigmaInRoot(const TPositionVector pt, TDataTreeIterator itTree, TLength sx, TLength sy, TLength sz)
{
	TPositionVector ptInSF = pt;
	TFreeVector sigmaRoot(ptInSF.getCoordSys());
	TPointTransformer fPointTransfo(&fProjectData->getTree(), fProjectData->getConfig().referential);

	// actual frame iterator
	TDataTreeIterator frameIt = itTree;
	// actual frame
	auto frame = itTree.node->data.get()->frame;

	// transformed covariance matrix
	TSparseMatrix covRoot(3, 3);

	while (frame.getName() != "ROOT")
	{
		// transform point in the actual subframe
		const TLOR2LOR &lorTrafo = fPointTransfo.getLORTransformation(itTree, frameIt); // Transformation from "TARGET POSITION" to "node n-1"
		lorTrafo.transform(ptInSF);

		// Matrix C : partial derive restect to tx ty tz rx ry rz x y z
		TSparseMatrix C(3, 10);
		// tx
		C.insert(0, 0) = 1.0;
		// ty
		C.insert(1, 1) = 1.0;
		// tz
		C.insert(2, 2) = 1.0;
		// rx
		C.insert(0, 3) = 0.0;
		C.insert(1,
			3) = ((sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
						 * ptInSF.getX()
					 + (-cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
						 * ptInSF.getY()
					 + (cos(frame.getEstRotation(1)) * cos(frame.getEstRotation(0))) * ptInSF.getZ())
			* frame.getEstScale();
		C.insert(2,
			3) = ((sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) - cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
						 * ptInSF.getX()
					 + (-cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) - sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
						 * ptInSF.getY()
					 - (cos(frame.getEstRotation(1)) * sin(frame.getEstRotation(0))) * ptInSF.getZ())
			* frame.getEstScale();
		// ry
		C.insert(0, 4) = (-cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * ptInSF.getX()
							 - sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * ptInSF.getY() - cos(frame.getEstRotation(1)) * ptInSF.getZ())
			* frame.getEstScale();
		C.insert(1, 4) = (cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)) * ptInSF.getX()
							 + sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)) * ptInSF.getY()
							 - sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)) * ptInSF.getZ())
			* frame.getEstScale();
		C.insert(2, 4) = (cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)) * ptInSF.getX()
							 + sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)) * ptInSF.getY()
							 - sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)) * ptInSF.getZ())
			* frame.getEstScale();
		// rz
		C.insert(0,
			5) = (-sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * ptInSF.getX() + cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * ptInSF.getY())
			* frame.getEstScale();
		C.insert(1,
			5) = ((-cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) - sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
						 * ptInSF.getX()
					 + (-sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
						 * ptInSF.getY())
			* frame.getEstScale();
		C.insert(2,
			5) = ((cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) - sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
						 * ptInSF.getX()
					 + (sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
						 * ptInSF.getY())
			* frame.getEstScale();
		// scale
		C.insert(0, 6) = cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * ptInSF.getX()
			+ sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(1)) * ptInSF.getY() - sin(frame.getEstRotation(1)) * ptInSF.getZ();
		C.insert(1, 6) = (-sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
				* ptInSF.getX()
			+ (cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) + sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
				* ptInSF.getY()
			+ cos(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)) * ptInSF.getZ();
		C.insert(2, 6) = (sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
				* ptInSF.getX()
			+ (-cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
				* ptInSF.getY()
			+ cos(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)) * ptInSF.getZ();
		// x
		C.insert(0, 7) = (cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(1))) * frame.getEstScale();
		C.insert(1, 7) = (-sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
			* frame.getEstScale();
		C.insert(2, 7) = (sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
			* frame.getEstScale();
		// y
		C.insert(0, 8) = (sin(frame.getEstRotation(2)) * cos(frame.getEstRotation(1))) * frame.getEstScale();
		C.insert(1, 8) = (cos(frame.getEstRotation(2)) * cos(frame.getEstRotation(0)) + sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * sin(frame.getEstRotation(0)))
			* frame.getEstScale();
		C.insert(2, 8) = (-cos(frame.getEstRotation(2)) * sin(frame.getEstRotation(0)) + sin(frame.getEstRotation(2)) * sin(frame.getEstRotation(1)) * cos(frame.getEstRotation(0)))
			* frame.getEstScale();
		// z
		C.insert(0, 9) = (-sin(frame.getEstRotation(1))) * frame.getEstScale();
		C.insert(1, 9) = (cos(frame.getEstRotation(1)) * sin(frame.getEstRotation(0))) * frame.getEstScale();
		C.insert(2, 9) = (cos(frame.getEstRotation(1)) * cos(frame.getEstRotation(0))) * frame.getEstScale();

		// covariance matrix = (cov frame, 0 ; 0, cov point)
		TSparseMatrix covSF(10, 10);

		TDenseMatrix covSFDense(10, 10);
		covSFDense.setZero();
		TDenseMatrix fullCov = frame.getCovar();
		// convention used here: 0 entry at inactive indices
		TDenseMatrix covSubframe(7, 7);
		covSubframe.setZero();
		covSubframe(frame.getRelativeUnknIndices(), frame.getRelativeUnknIndices()) = fullCov(frame.getRelativeUnknIndices(), frame.getRelativeUnknIndices());
		covSFDense.topLeftCorner(7, 7) = covSubframe;
		covSFDense(8, 8) = pow2q(sx);
		covSFDense(9, 9) = pow2q(sy);
		covSFDense(10, 10) = pow2q(sz);
		covSF = covSFDense.sparseView();

		// calculated new point covariance in the upper frame
		covRoot = C * covSF * (C.transpose());

		// frame, go up
		frameIt = frameIt.node->parent;
		frame = frameIt.node->data.get()->frame;
	}

	// extract variance in root
	sigmaRoot.setX(TLength(sqrt(covRoot.coeff(0, 0))));
	sigmaRoot.setY(TLength(sqrt(covRoot.coeff(1, 1))));
	sigmaRoot.setZ(TLength(sqrt(covRoot.coeff(2, 2))));

	return sigmaRoot;
}