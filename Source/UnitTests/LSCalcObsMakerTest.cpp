// LSCalcObsMakerTest.cpp
// includes for LGCProject
#include "TAStreamFormatter.h"
#include "TSeparatedFormatTStream.h"
#include "TLGCFileReader.h"
#include "TLGCDataSet.h"
#include "TLGCProject.h"
#include "TLGCApplication.h"
//#include "TGeodeticRefFrame.h"

// includes for ObsMkr
#include "TObservationMaker.h"

// includes for LSCalcObsMkr
#include "TLSCalcObservationMaker.h"

// includes for input matrices filling
#include "TLSInputMatricesFiller.h"
#include "TLSLocalSysContribGenFactory.h"
#include "TLSInputMatrices.h"

// includes for calculation
#include "TLSParametricMtdComputer.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLSResultsMatrices.h"

void main() {

	const char * LGCFile;
	LGCFile = "C:\\temp\\LSAlgorithmsTest\\ALLOBS.inp";

	// settings for Data parameters
	TFileParameters fp;
	fp.setFileName(LGCFile);

	cout << fp.getPath() << "\\" << fp.getName() << endl;
	cout << fp.defined() << endl;

	// creating an (empty) application and a project
	//TLGCApplication application;
	TLGCProject project;

	// use of load function
	project.load(fp);


	// Definition of a pointer to the input data formatted in the project as a TLGCDataSet
	TLGCDataSet* data = new TLGCDataSet();
	data = project.getDataSet();
	ObservationSet obs;
	
	//Use of the maker in order to generate input data formatted as LGC++-defined observations
	TObservationMaker maker; // object used for the data conversion
	
	bool B = maker.processData(*data,obs); // processing of the data to observations

	// check of the processing validity
	if (B) 
		cout << "Observations processed" << endl;
	else
	{
		cout << "Processing failed" << endl;
		return;
	}

	// Use of the ls calc maker in order to generate input data for least-square calculation,
	// formatted as LGC++-defined LSCalc observations, and LSAlgorithms-defined parameters
//	ObservationSet obs = *(maker.getObservation()); // output of observation maker processing (LGC++-defined observations)
	LSCalcDataSet dataset; // definition of the ls calc observation processing output (LGC++-defined LSCalc observations)
	TLSCalcObservationMaker lsmkr; // object used for the data conversion
	
	bool answer = lsmkr.processData(obs,dataset); // processing of the observations to ls calc observations & parameters

	// check of the processing validity
	if ( answer ) {
		cout << "ls calc observations & parameters created" << endl;
		
		// CHECK-UP

		// Position Vector parameters container
/*		cout << "*************************" << endl;
		cout << "*Tests on Positions List*" << endl;
		cout << "*************************" << endl << endl;

		LSPosVecIter itB = dataset.beginPV();
		LSPosVecIter itE = dataset.endPV();

		while (itB!=itE) {

			cout.precision(8);
			cout << " POINT: " << itB->getName() <<  endl;
			cout << " Provisional coordinates, corresponding status and matrix indices " << endl;
			cout << " X-coord prov: " << itB->getProvisionalValue().getX().getMetresValue() << " with status : " << itB->getXStatus() << " and Index " << itB->getXIndex() << endl;
			cout << " Y-coord prov: " << itB->getProvisionalValue().getY().getMetresValue() << " with status : " << itB->getYStatus() << " and Index " << itB->getYIndex() << endl;
			cout << " Z-coord prov: " << itB->getProvisionalValue().getZ().getMetresValue() << " with status : " << itB->getZStatus() << " and Index " << itB->getZIndex() << endl << endl;
			cout << " Correction to be added on the provisional coordinates: " << endl;
			cout << " X-coord corr: " << itB->getCorrection().getX().getMetresValue() << endl;
			cout << " Y-coord corr: " << itB->getCorrection().getY().getMetresValue() << endl;
			cout << " Z-coord corr: " << itB->getCorrection().getZ().getMetresValue() << endl;
			cout << " Estimated coordinates for calculation: " << endl;
			cout << " X-coord est: " << itB->getEstimatedValue().getX().getMetresValue() << endl;
			cout << " Y-coord est: " << itB->getEstimatedValue().getY().getMetresValue() << endl;
			cout << " Z-coord est: " << itB->getEstimatedValue().getZ().getMetresValue() << endl;
			cout << " Estimated error on estimated coordinates: " << endl;	
			cout << " X-coord estSigma: " << itB->getEstimatedPrecision().getX().getMetresValue() << endl;
			cout << " Y-coord estSigma: " << itB->getEstimatedPrecision().getY().getMetresValue() << endl;
			cout << " Z-coord estSigma: " << itB->getEstimatedPrecision().getZ().getMetresValue() << endl << endl;
			itB++;
		}	*/
//	} 
		// Orientation parameters container
		cout << "****************************" << endl;
		cout << "*Tests on Orientations List*" << endl;
		cout << "****************************" << endl << endl;

		LSOrientIter itB = dataset.beginOrient();
		LSOrientIter itE = dataset.endOrient();
		int n = 0;
		while (itB!=itE) {
			n++;
			cout.precision(8);
			cout << " ORIENTATION: " << itB->getName() <<  endl;
			cout << " Provisional values, corresponding status and matrix indices " << endl;
			cout << " Omega: " << itB->getProvisionalValue().omega.getGonsValue() << " with status : " << itB->getOmegaStatus() << " and Index " << itB->getOmegaIndex() << endl;
			cout << " Phi: " << itB->getProvisionalValue().phi.getGonsValue() << " with status : " << itB->getPhiStatus() << " and Index " << itB->getPhiIndex() << endl;
			cout << " Kappa: " << itB->getProvisionalValue().kappa.getGonsValue() << " with status : " << itB->getKappaStatus() << " and Index " << itB->getKappaIndex() << endl << endl;
			cout << " Correction to be added on the provisional coordinates: " << endl;
			cout << " Omega corr: " << itB->getCorrection().omega.getGonsValue() << endl;
			cout << " Phicorr: " << itB->getCorrection().phi.getGonsValue() << endl;
			cout << " Kappa corr: " << itB->getCorrection().kappa.getGonsValue() << endl;
			cout << " Estimated coordinates for calculation: " << endl;
			cout << " Omega est: " << itB->getEstimatedValue().omega.getGonsValue() << endl;
			cout << " Phi est: " << itB->getEstimatedValue().phi.getGonsValue() << endl;
			cout << " Kappa est: " << itB->getEstimatedValue().kappa.getGonsValue() << endl;
			cout << " Estimated error on estimated coordinates: " << endl;	
			cout << " Omega estSigma: " << itB->getEstimatedPrecision().omega.getGonsValue() << endl;
			cout << " Phi estSigma: " << itB->getEstimatedPrecision().phi.getGonsValue() << endl;
			cout << " Kappa estSigma: " << itB->getEstimatedPrecision().kappa.getGonsValue() << endl << endl;
			itB++;
		} 
		cout << n << endl;
		// Length parameters container
/*		cout << "****************************" << endl;
		cout << "*Tests on Length List*" << endl;
		cout << "****************************" << endl << endl;

		LSLengthIter itB = dataset.beginLength();
		LSLengthIter itE = dataset.endLength();
		int n = 0;
		while (itB!=itE) {
			n++;
			cout.precision(8);
			cout << " Length number: " << n << " named: " << itB->getName() <<  endl;
			cout << " Provisional value, corresponding status and matrix index " << endl;
			cout << " Value : " << itB->getProvisionalValue().getMetresValue() << " with status : " << itB->getStatus() << " and Index " << itB->getIndex() << endl;
			cout << " Correction to be added on the provisional value: " << endl;
			cout << " Correction: " << itB->getCorrection().getMetresValue() << endl;
			cout << " Estimated value for calculation: " << endl;
			cout << " Estimated value: " << itB->getEstimatedValue().getMetresValue() << endl;
			cout << " Estimated error on estimated value: " << endl;	
			cout << " Estimated error: " << itB->getEstimatedPrecision().getMetresValue() << endl << endl;
			itB++;

		} 

		cout << n << endl; */
		
		// Horizontal Angle Observations

/*		cout << "***********************************************" << endl;
		cout << "*HORIZONTAL ANGLE OBSERVATIONS FOR LSALGORITHMS*" << endl;
		cout << "***********************************************" << endl;
		

		LSHorAngIter iterb = dataset.beginLSHorAng();
		LSHorAngIter itere = dataset.endLSHorAng();

		int m = 1;
		while ( iterb != itere ) {

			cout << "---------------------------------------------" << endl;
			cout << "Horizontal Angle Observation numero: " << m << endl;
			cout << "---------------------------------------------" << endl;
			cout << "Position Vector parameters" << endl << endl;
			cout.precision(8);
			LSPosVecConstIter st = iterb->getStPoint();
			LSPosVecConstIter tg = iterb->getTgPoint();
			cout << "Stationed position: " << st->getName() << endl;
			cout << "X-Coord: " << st->getProvisionalValue().getX().getMetresValue() << " with status: " << st->getXStatus() << " and Index: " << st->getXIndex() << endl;
			cout << "Y-Coord: " << st->getProvisionalValue().getY().getMetresValue() << " with status: " << st->getYStatus() << " and Index: " << st->getYIndex() <<  endl;
			cout << "Z-Coord: " << st->getProvisionalValue().getZ().getMetresValue() << " with status: " << st->getZStatus() << " and Index: " << st->getZIndex() <<  endl;
			cout << "Target position:    " << tg->getName() << endl;
			cout << "X-Coord: " << tg->getProvisionalValue().getX().getMetresValue() << " with status: " << tg->getXStatus() << " and Index: " << tg->getXIndex() << endl;
			cout << "Y-Coord: " << tg->getProvisionalValue().getY().getMetresValue() << " with status: " << tg->getYStatus() << " and Index: " << tg->getYIndex() << endl;
			cout << "Z-Coord: " << tg->getProvisionalValue().getZ().getMetresValue() << " with status: " << tg->getZStatus() << " and Index: " << tg->getZIndex() << endl << endl;
			
			cout << "Orientation parameter" << endl << endl;

			LSOrientConstIter o = iterb->getV0();
			cout << "Orientation: " << o->getName() << endl;
			cout << "Angle Value: " << o->getProvisionalValue().omega.getGonsValue() << " with status: " << o->getOmegaStatus() << " and Index: " << o->getOmegaIndex() << endl;
			cout << "Angle Value: " << o->getProvisionalValue().phi.getGonsValue() << " with status: " << o->getPhiStatus() << " and Index: " << o->getPhiIndex() << endl;
			cout << "Angle Value: " << o->getProvisionalValue().kappa.getGonsValue() << " with status: " << o->getKappaStatus() << " and Index: " << o->getKappaIndex() << endl << endl;
			
			cout << "Observation's values" << endl;
			cout << "Observed value: " << iterb->getObsAngle().getGonsValue() << endl;
			cout << "Sigma a priori: " << iterb->getSigmaAPriori().getGonsValue() << endl;
			
			iterb++;
			m++;
		}*/
		
/*		cout << "*************************************************" << endl;
		cout << "*ZENITHAL DISTANCE OBSERVATIONS FOR LSALGORITHMS*" << endl;
		cout << "*************************************************" << endl;
		

		LSZenDistIter iterb = dataset.beginLSZenDist();
		LSZenDistIter itere = dataset.endLSZenDist();

		int m = 1;
		while ( iterb != itere ) {

			cout << "-----------------------------------------------" << endl;
			cout << "Zenithal distance Observations numero: " << m << endl;
			cout << "-----------------------------------------------" << endl;
			cout << "Position Vector parameters" << endl << endl;
			cout.precision(8);
			LSPosVecConstIter st = iterb->getStPoint();
			LSPosVecConstIter tg = iterb->getTgPoint();
			cout << "Stationed position: " << st->getName() << endl;
			cout << "X-Coord: " << st->getProvisionalValue().getX().getMetresValue() << " with status: " << st->getXStatus() << " and Index: " << st->getXIndex() << endl;
			cout << "Y-Coord: " << st->getProvisionalValue().getY().getMetresValue() << " with status: " << st->getYStatus() << " and Index: " << st->getYIndex() <<  endl;
			cout << "Z-Coord: " << st->getProvisionalValue().getZ().getMetresValue() << " with status: " << st->getZStatus() << " and Index: " << st->getZIndex() <<  endl;
			cout << "Target position:    " << tg->getName() << endl;
			cout << "X-Coord: " << tg->getProvisionalValue().getX().getMetresValue() << " with status: " << tg->getXStatus() << " and Index: " << tg->getXIndex() << endl;
			cout << "Y-Coord: " << tg->getProvisionalValue().getY().getMetresValue() << " with status: " << tg->getYStatus() << " and Index: " << tg->getYIndex() << endl;
			cout << "Z-Coord: " << tg->getProvisionalValue().getZ().getMetresValue() << " with status: " << tg->getZStatus() << " and Index: " << tg->getZIndex() << endl << endl;
			
			cout << "Instrument and prism heights parameters" << endl << endl;

			LSLengthConstIter hi = iterb->getHInstrument();
			cout << "Instrument's height: " << hi->getName() << endl;
			cout << "Value: " << hi->getProvisionalValue().getMetresValue() << " with status: " << hi->getStatus() << " and Index: " << hi->getIndex() << endl;
			
			cout << "Observation's values" << endl;
			cout << "Observed value: " << iterb->getObsAngle().getGonsValue() << endl;
			cout << "Sigma a priori: " << iterb->getSigmaAPriori().getGonsValue() << endl;
			
			iterb++;
			m++;
		}*/
/*		cout << "*************************************************" << endl;
		cout << "*SPATIAL DISTANCE OBSERVATIONS FOR LSALGORITHMS*" << endl;
		cout << "*************************************************" << endl;
		

		LSSpaDistIter iterb = dataset.beginLSSpaDist();
		LSSpaDistIter itere = dataset.endLSSpaDist();

		int m = 1;
		while ( iterb != itere ) {

			cout << "-----------------------------------------------" << endl;
			cout << "spatial distance Observations numero: " << m << endl;
			cout << "-----------------------------------------------" << endl;
			cout << "Position Vector parameters" << endl << endl;
			cout.precision(8);
			LSPosVecConstIter st = iterb->getStPoint();
			LSPosVecConstIter tg = iterb->getTgPoint();
			cout << "Stationed position: " << st->getName() << endl;
			cout << "X-Coord: " << st->getProvisionalValue().getX().getMetresValue() << " with status: " << st->getXStatus() << " and Index: " << st->getXIndex() << endl;
			cout << "Y-Coord: " << st->getProvisionalValue().getY().getMetresValue() << " with status: " << st->getYStatus() << " and Index: " << st->getYIndex() <<  endl;
			cout << "Z-Coord: " << st->getProvisionalValue().getZ().getMetresValue() << " with status: " << st->getZStatus() << " and Index: " << st->getZIndex() <<  endl;
			cout << "Target position:    " << tg->getName() << endl;
			cout << "X-Coord: " << tg->getProvisionalValue().getX().getMetresValue() << " with status: " << tg->getXStatus() << " and Index: " << tg->getXIndex() << endl;
			cout << "Y-Coord: " << tg->getProvisionalValue().getY().getMetresValue() << " with status: " << tg->getYStatus() << " and Index: " << tg->getYIndex() << endl;
			cout << "Z-Coord: " << tg->getProvisionalValue().getZ().getMetresValue() << " with status: " << tg->getZStatus() << " and Index: " << tg->getZIndex() << endl << endl;
			
			cout << "Instrument and prism heights parameters" << endl << endl;

			LSLengthConstIter hi = iterb->getHInstrument();
			LSLengthConstIter hp = iterb->getHPrism();
			cout << "Instrument's height: " << hi->getName() << endl;
			cout << "Value: " << hi->getProvisionalValue().getMetresValue() << " with status: " << hi->getStatus() << " and Index: " << hi->getIndex() << endl;
			cout << "Prism's height: " << hp->getName() << endl;
			cout << "Value: " << hp->getProvisionalValue().getMetresValue() << " with status: " << hi->getStatus() << " and Index: " << hi->getIndex() << endl;

			cout << "Distance constant" << endl << endl;
			LSLengthConstIter cst = iterb->getDistConst();
			cout << "distance constant: " << cst->getName() << endl;
			cout << "Value: " << cst->getProvisionalValue().getMetresValue() << " with status: " << cst->getStatus() << " and Index: " << cst->getIndex() << endl;
			cout << "Observation's values" << endl;
			cout << "Observed value: " << iterb->getObsDist().getMetresValue() << endl;
			cout << "Sigma a priori: " << iterb->getSigmaAPriori().getMetresValue() << endl;
			
			iterb++;
			m++;
		}*/

/*		cout << "***************************************************" << endl;
		cout << "*HORIZONTAL DISTANCE OBSERVATIONS FOR LSALGORITHMS*" << endl;
		cout << "***************************************************" << endl;
		

		LSHorDistIter iterb = dataset.beginLSHorDist();
		LSHorDistIter itere = dataset.endLSHorDist();

		int m = 1;
		while ( iterb != itere ) {

			cout << "-----------------------------------------------" << endl;
			cout << "horizontal distance Observations numero: " << m << endl;
			cout << "-----------------------------------------------" << endl;
			cout << "Position Vector parameters" << endl << endl;
			cout.precision(8);
			LSPosVecConstIter st = iterb->getStPoint();
			LSPosVecConstIter tg = iterb->getTgPoint();
			cout << "Stationed position: " << st->getName() << endl;
			cout << "X-Coord: " << st->getProvisionalValue().getX().getMetresValue() << " with status: " << st->getXStatus() << " and Index: " << st->getXIndex() << endl;
			cout << "Y-Coord: " << st->getProvisionalValue().getY().getMetresValue() << " with status: " << st->getYStatus() << " and Index: " << st->getYIndex() <<  endl;
			cout << "Z-Coord: " << st->getProvisionalValue().getZ().getMetresValue() << " with status: " << st->getZStatus() << " and Index: " << st->getZIndex() <<  endl;
			cout << "Target position:    " << tg->getName() << endl;
			cout << "X-Coord: " << tg->getProvisionalValue().getX().getMetresValue() << " with status: " << tg->getXStatus() << " and Index: " << tg->getXIndex() << endl;
			cout << "Y-Coord: " << tg->getProvisionalValue().getY().getMetresValue() << " with status: " << tg->getYStatus() << " and Index: " << tg->getYIndex() << endl;
			cout << "Z-Coord: " << tg->getProvisionalValue().getZ().getMetresValue() << " with status: " << tg->getZStatus() << " and Index: " << tg->getZIndex() << endl << endl;
			
			cout << "Observation's values" << endl;
			cout << "Observed value: " << iterb->getObsDist().getMetresValue() << endl;
			cout << "Sigma a priori: " << iterb->getSigmaAPriori().getMetresValue() << endl;
			
			iterb++;
			m++;
		} */
/*		cout << "***************************************************" << endl;
		cout << "*VERTICAL DISTANCE OBSERVATIONS FOR LSALGORITHMS*" << endl;
		cout << "***************************************************" << endl;
		

		LSVertDistIter iterb = dataset.beginLSVertDist();
		LSVertDistIter itere = dataset.endLSVertDist();

		int m = 1;
		while ( iterb != itere ) {

			cout << "-----------------------------------------------" << endl;
			cout << "vertical distance Observations numero: " << m << endl;
			cout << "-----------------------------------------------" << endl;
			cout << "Position Vector parameters" << endl << endl;
			cout.precision(8);
			LSPosVecConstIter ref = iterb->getRefPoint();
			LSPosVecConstIter tg = iterb->getTgPoint();
			cout << "Reference position: " << ref->getName() << endl;
			cout << "X-Coord: " << ref->getProvisionalValue().getX().getMetresValue() << " with status: " << ref->getXStatus() << " and Index: " << ref->getXIndex() << endl;
			cout << "Y-Coord: " << ref->getProvisionalValue().getY().getMetresValue() << " with status: " << ref->getYStatus() << " and Index: " << ref->getYIndex() <<  endl;
			cout << "Z-Coord: " << ref->getProvisionalValue().getZ().getMetresValue() << " with status: " << ref->getZStatus() << " and Index: " << ref->getZIndex() <<  endl;
			cout << "Target position:    " << tg->getName() << endl;
			cout << "X-Coord: " << tg->getProvisionalValue().getX().getMetresValue() << " with status: " << tg->getXStatus() << " and Index: " << tg->getXIndex() << endl;
			cout << "Y-Coord: " << tg->getProvisionalValue().getY().getMetresValue() << " with status: " << tg->getYStatus() << " and Index: " << tg->getYIndex() << endl;
			cout << "Z-Coord: " << tg->getProvisionalValue().getZ().getMetresValue() << " with status: " << tg->getZStatus() << " and Index: " << tg->getZIndex() << endl << endl;
			
			cout << "Observation's values" << endl;
			cout << "Observed value: " << iterb->getObsDist().getMetresValue() << endl;
			cout << "Sigma a priori: " << iterb->getSigmaAPriori().getMetresValue() << endl;
			
			iterb++;
			m++;
		} */
	}

	else
		cout << "processing failed" << endl;

	return;

};


