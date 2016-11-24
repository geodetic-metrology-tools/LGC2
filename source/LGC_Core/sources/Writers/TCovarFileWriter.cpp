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
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(0).getRadiansValue());
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(1).getRadiansValue());
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(2).getRadiansValue());

	//Writing scale factor
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstScale());

	(*stream) << endl;
}

void TCovarFileWriter::writeFrameUpperTriangularCovarianceMatrix(TAdjustableHelmertTransformation& frameDef)
{
	//Write frame upper covariance matrix in radian and meter
	// vartx	cov_txty	cov_txtz	cov_txrx	cov_txry	cov_txrz	cov_txl
	//            varty     cov_tytz	cov_tyrx	cov_tyry	cov_tyrz	cov_tyl
	//                       vartz      cov_tzrx	cov_tzry	cov_tzrz	cov_tzl
	//                                    varrx     cov_rxry    cov_rxrz	cov_rxl
	//												  varry     cov_ryrz	cov_ryl
	//                                                             varrz	cov_rzl
	//                                                                        varl


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
			fStream->writeDouble(20, 16, pow2(frameDef.getEstimatedPrecisionTransl(0)));

			//covariances
			if (!frameDef.isTranslationFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXYCovarTransl());
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!frameDef.isTranslationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXZCovarTransl());
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}

			//rotations
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(0))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(0));
			else
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(1))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(1));
			else
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(2))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(2));
			else
			fStream->writeDouble(20, 16, 0.0);

			//scale
			(*stream) << TABs;
			if (!frameDef.isScaleFixed())
				fStream->writeDouble(20, 16, frameDef.getScaleCovar(0));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
		else
		{
			if (frameDef.hasTranslStandDev(0))
				fStream->writeDouble(20, 16, pow2(frameDef.getTranslationStandDev(0)));
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
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!frameDef.isTranslationFixed(1))
		{
			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, pow2(frameDef.getEstimatedPrecisionTransl(1)));

			//covariances
			if (!frameDef.isTranslationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getYZCovarTransl());

			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}

			//rotations
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(0))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(3));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(1))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(4));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(2))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(5));
			else
				fStream->writeDouble(20, 16, 0.0);

			//scale
			(*stream) << TABs;
			if (!frameDef.isScaleFixed())
				fStream->writeDouble(20, 16, frameDef.getScaleCovar(1));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (frameDef.hasTranslStandDev(1))
				fStream->writeDouble(20, 16, pow2(frameDef.getTranslationStandDev(1)));
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

		if (!frameDef.isTranslationFixed(2))
		{
			//variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, pow2(frameDef.getEstimatedPrecisionTransl(2)));


			//rotations
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(0))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(6));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(1))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(7));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			if (!frameDef.isRotationFixed(2))
				fStream->writeDouble(20, 16, frameDef.getTrRotCovar(8));
			else
				fStream->writeDouble(20, 16, 0.0);

			//scale
			(*stream) << TABs;
			if (!frameDef.isScaleFixed())
				fStream->writeDouble(20, 16, frameDef.getScaleCovar(2));
			else
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
				fStream->writeDouble(20, 16, pow2(frameDef.getTranslationStandDev(2)));
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
			fStream->writeDouble(20, 16, pow2(frameDef.getEstimatedPrecisionRot(0)));

			//covariances
			if (!frameDef.isRotationFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXYCovarRot());
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!frameDef.isRotationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getXZCovarRot());
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}

			//scale
			(*stream) << TABs;
			if (!frameDef.isScaleFixed())
				fStream->writeDouble(20, 16, frameDef.getScaleCovar(3));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;

			
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
				fStream->writeDouble(20, 16, pow2(frameDef.getRotationStandDev(0)));
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
			fStream->writeDouble(20, 16, pow2(frameDef.getEstimatedPrecisionRot(1).getGonsValue()));

			//covariances
			if (!frameDef.isRotationFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, frameDef.getYZCovarRot());
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}
			//scale
			(*stream) << TABs;
			if (!frameDef.isScaleFixed())
				fStream->writeDouble(20, 16, frameDef.getScaleCovar(4));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
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
				fStream->writeDouble(20, 16, pow2(frameDef.getRotationStandDev(1)));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
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
			fStream->writeDouble(20, 16, pow2(frameDef.getEstimatedPrecisionRot(2)));
			//scale
			(*stream) << TABs;
			if (!frameDef.isScaleFixed())
				fStream->writeDouble(20, 16, frameDef.getScaleCovar(5));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
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
				fStream->writeDouble(20, 16, pow2(frameDef.getRotationStandDev(2)));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		//SCALE
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
		fStream->writeString(20, "");
		(*stream) << TABs;
		if (!frameDef.isScaleFixed())
			fStream->writeDouble(20, 16, frameDef.getEstimatedPrecisionScale());
		else
			fStream->writeDouble(20, 16, 0.0);
		(*stream) << endl;

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
			fStream->writeDouble(20, 16, pow2(point.getXEstPrecision()));

			//covariances
			if (!point.isCoordinateFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getXYCovar());
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!point.isCoordinateFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getXZCovar());
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
				fStream->writeDouble(20, 16, pow2(point.getStandDev(0)));
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
			fStream->writeDouble(20, 16, pow2(point.getYEstPrecision()));

			//covariances
			if (!point.isCoordinateFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getYZCovar());
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
				fStream->writeDouble(20, 16, pow2(point.getStandDev(1)));
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
			fStream->writeDouble(20, 16, pow2(point.getZEstPrecision()));
			(*stream) << endl;
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(2)))
				fStream->writeDouble(20, 16, pow2(point.getStandDev(2)));
			else
				fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
	}
	else
	{
		(*stream) << TABs;
		if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(0)))
			fStream->writeDouble(20, 16, pow2(point.getStandDev(0)));
		else
			fStream->writeString(20, "");
		(*stream) << TABs;
		if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(1)))
			fStream->writeDouble(20, 16, pow2(point.getStandDev(1)));
		else
			fStream->writeString(20, "");
		(*stream) << TABs;
		if (point.hasStandDeviations() && !isnotanumber(point.getStandDev(2)))
			fStream->writeDouble(20, 16, pow2(point.getStandDev(2)));
		else
			fStream->writeString(20, "");
		(*stream) << endl;
	}
	
}
