////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////
#include "TCovarFileWriter.h"
#include "TLGCData.h"
#include "LGCAdjustableObjectCollection.h"

#include "TSeparatedFormatTStream.h"
#include "TLSResultsMatrices.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TCovarFileWriter::TCovarFileWriter() : TAFileWriter()
{//default constructor
}


TCovarFileWriter::TCovarFileWriter(TAStreamFormatter* stream, const TLGCData* project) : TAFileWriter(stream, project)
{//constructor
}


TCovarFileWriter::~TCovarFileWriter()
{//destructor
}

void TCovarFileWriter::writeFile(const string)
{

}

void TCovarFileWriter::writeFile(TLGCData &project)
{
	writeTitle(project);
	LGCAdjustablePointCollection& AdjPointIter = project.getPoints();
	
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++){
		if (itTree->get()->frame.getName() != "ROOT")
		{
			
			writeFrames(itTree->get());

			for (auto& pointIt: AdjPointIter)
				if (pointIt.getFrameTreePosition().node->data.get()->ID == itTree->get()->ID)
				{
					//Write points defined in the frame
					writePoints(pointIt);
				}
		}
		else
		{
			for (auto& pointIt : AdjPointIter)
				if (pointIt.getFrameTreePosition().node->data.get()->ID == itTree->get()->ID)
				{
					//Write points defined in the frame
					writePoints(pointIt);
				}
		}

		
	}
}

void TCovarFileWriter::writeTitle(TLGCData &project)
{
	fStream->writeStringLeft(8, "FILE OF COVARIANCE MATRICES: ");
	fStream->writeStringLeft(8, project.getConfig().title);
	*fStream << endl;
	fStream->writeStringLeft(8, "S0 a posteriori = ");
	fStream->writeDouble(15, 8, project.getS0APosteriori());
	*fStream << endl;
}

void TCovarFileWriter::writePoints(LGCAdjustablePoint &point)
{
	//write point name
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	std::string			TABs = stream->getCurrSpace();

	//Writting point: name X Y Z
	fStream->writeStringLeft(nameWidth, point.getName());
	(*stream) << TABs;
	fStream->writeDouble(20, 16, point.getEstimatedValue().getX());
	(*stream) << TABs;
	fStream->writeDouble(20, 16, point.getEstimatedValue().getY());
	(*stream) << TABs;
	fStream->writeDouble(20, 16, point.getEstimatedValue().getZ());
	(*stream) << endl;


	//Write frame upper covariance matrix
	// sx  cov_xy   cov_xz
	//      sy      cov_yz
	//                sz  
	writePointUpperTriangularCovarianceMatrix(point);
	
}

void TCovarFileWriter::writeFrames(TTreeEntry *frameIt)
{
	//Write frame definition
	writeFrameDefinition(frameIt->frame);

	//Write frame upper covariance matrix
	writeFrameUpperTriangularCovarianceMatrix(frameIt->frame);

}

void TCovarFileWriter::writeFrameDefinition(TAdjustableHelmertTransformation& frameDef)
{
	//Write frame definition
	//name tx ty tz rx ry rz scale

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	string				separator = getSeparator();
	int					lengthPrecision = getLengthPrecision();
	int					anglePrecision = getAnglePrecision();
	std::string			TABs = stream->getCurrSpace();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	stream->setPrecisionFormat(anglePrecision);

	//Writting name
	(*stream) << endl;
	fStream->writeStringLeft(nameWidth, frameDef.getName());

	//Writing translations
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(0));
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(1));
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(2));

	
	//Writing rotations
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(0).getGonsValue());
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(1).getGonsValue());
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(2).getGonsValue());

	//Writing scale factor
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstScale());

	(*stream) << endl;
}

void TCovarFileWriter::writeFrameUpperTriangularCovarianceMatrix(TAdjustableHelmertTransformation& frameDef)
{
	//Write frame upper covariance matrix
	// stx cov_txty cov_txtz 0		 0		 0
	//      sty     cov_tytz 0		 0		 0
	//              stz      0       0       0
	//                       srx cov_rxry cov_ryrz
	//							    sry   cov_ryrz
	//                                       srz


	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	string				separator = getSeparator();
	int					lengthPrecision = getLengthPrecision();
	int					anglePrecision = getAnglePrecision();
	std::string			TABs = stream->getCurrSpace();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	//set length parameters
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);


	if (!frameDef.isFixed()){

		//TRANSLATIONS
		if (!frameDef.isTranslationFixed(0))
		{
			//variance
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionTransl(0).getMMetresValue());

			//covariances
			if (!frameDef.isTranslationFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXYCovarTransl()*M2MM*M2MM);
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!frameDef.isTranslationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXZCovarTransl()*M2MM*M2MM);
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}

			//rotations
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
		else
		{
			if (frameDef.hasTranslStandDev(0))
				fStream->writeDouble(20, 16, frameDef.getTranslationStandDev(0)*M2MM);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!frameDef.isTranslationFixed(1))
		{
			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionTransl(1).getMMetresValue());

			//covariances
			if (!frameDef.isTranslationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getYZCovarTransl()*M2MM*M2MM);

			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}

			//rotations
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (frameDef.hasTranslStandDev(1))
				fStream->writeDouble(20, 16, frameDef.getTranslationStandDev(1)*M2MM);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!frameDef.isTranslationFixed(2))
		{
			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionTransl(2).getMMetresValue());


			//rotations
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (frameDef.hasTranslStandDev(2))
				fStream->writeDouble(20, 16, frameDef.getTranslationStandDev(2)*M2MM);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		//ROTATIONS
		if (!frameDef.isRotationFixed(0))
		{
			//tanslation
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");


			//variance
			(*stream) << TABs;
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionRot(0).getGonsValue());

			//covariances
			if (!frameDef.isRotationFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXYCovarRot()*RAD2GON*RAD2GON);
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!frameDef.isRotationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXZCovarRot()*RAD2GON*RAD2GON);
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}

			
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (frameDef.hasRotationStandDev(0))
				fStream->writeDouble(20, 16, frameDef.getRotationStandDev(0)*RAD2GON);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!frameDef.isRotationFixed(1))
		{
			//tanslation
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;

			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionRot(1).getGonsValue());

			//covariances
			if (!frameDef.isRotationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getYZCovarRot()*RAD2GON*RAD2GON);
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}

		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (frameDef.hasRotationStandDev(1))
				fStream->writeDouble(20, 16, frameDef.getRotationStandDev(1)*RAD2GON);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}


		if (!frameDef.isRotationFixed(2))
		{
			//tanslation
			//tanslation
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;

			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionRot(2).getGonsValue());

		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (frameDef.hasRotationStandDev(2))
				fStream->writeDouble(20, 16, frameDef.getRotationStandDev(2)*RAD2GON);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

	}
	(*stream) << endl;

}

void TCovarFileWriter::writePointUpperTriangularCovarianceMatrix(LGCAdjustablePoint &point)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	string				separator = getSeparator();
	int					lengthPrecision = getLengthPrecision();
	std::string			TABs = stream->getCurrSpace();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	//set length parameters
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);


	if (!point.isFixed()){

		if (!point.isCoordinateFixed(0))
		{
			//variance
			fStream->writeDouble(20, 16, point.getXEstPrecision().getMMetresValue());

			//covariances
			if (!point.isCoordinateFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getXYCovar()*M2MM*M2MM);
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!point.isCoordinateFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getXZCovar()*M2MM*M2MM);
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}
		}
		else
		{
			if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(0)))
				fStream->writeDouble(20, 16, point.getStandDev(0)*M2MM);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!point.isCoordinateFixed(1))
		{
			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, point.getYEstPrecision().getMMetresValue());

			//covariances
			if (!point.isCoordinateFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getYZCovar()*M2MM*M2MM);
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(1)))
				fStream->writeDouble(20, 16, point.getStandDev(1)*M2MM);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!point.isCoordinateFixed(2))
		{
			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, point.getZEstPrecision().getMMetresValue());
			(*stream) << endl;
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(2)))
				fStream->writeDouble(20, 16, point.getStandDev(2)*M2MM);
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
	}
	else
	{
		(*stream) << TABs;
		if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(0)))
			fStream->writeDouble(20, 16, point.getStandDev(0)*M2MM);
		else
			fStream->writeString(20, "");
		(*stream) << TABs;
		if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(1)))
			fStream->writeDouble(20, 16, point.getStandDev(1)*M2MM);
		else
			fStream->writeString(20, "");
		(*stream) << TABs;
		if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(2)))
			fStream->writeDouble(20, 16, point.getStandDev(2)*M2MM);
		else
			fStream->writeString(20, "");
		(*stream) << endl;
	}
	
}
