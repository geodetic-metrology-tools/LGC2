#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <readers/TReader.h>
#include "testSimpleROOTForDIR3D.h"
#include "TLGCCalculation.h"
#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "LSCalculation/TLSInputMatricesFiller.h"
#include "LSCalculation/TLSResultsMatricesExtractor.h"
#include "lsalgo/TLSInputMatrices.h"

#include "lsalgo/TLSParametricMtdComputer.h"
#include "lsalgo/TLSCombinedMtdComputer.h"
#include "lsalgo/TLSWeightedUnkMtdComputer.h"

namespace tut
{
    struct test_DIR_Input{};
    typedef test_group<test_DIR_Input> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("DIR Input testing");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{
#if 0
		TLGCData projTest;
		
		set_test_name("Testing dir3D case in ROOT");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLRqqq.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestDIR_ROOT::DIR);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);

		calcul.computeLSResults();
        const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		/*ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);*/

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);
#endif
        /*
        //------------------------------------ComputeLSResults()---------------------------
        bool successCalculation = false;

	    // Check whether the data are consistent, assign unknown indices and initialize uninitialized planes in DLEV measurements
	    TDataAnalyzer analyzer(projTest);
	    if(!analyzer.dataConsistent())
	    	throw std::runtime_error("Data are not consistent, see the output file: " + projTest.fOutputFileWriter.getOutputFileLocation() + " for more information.");

	    //Identify and set calculation type based on configuration options and measurement types
	    TCalcType fCalculationType = analyzer.getCalculationType();

	    TLSInputMatricesFiller matrFiller(&projTest.tree, fCalculationType);
        std::unique_ptr<TLSResultsMatrices>		fResultsMatrices;
	    fResultsMatrices.reset(new TLSResultsMatrices(projTest.fUEOIndices));

	    TLSInputMatrices inputMtr;
	    TLSResultsMatricesExtractor extractor(&projTest);

	    std::unique_ptr<TALSComputer> fComputer;

	    if(fCalculationType.fCombinedCase)
	    	fComputer.reset(new TLSCombinedMtdComputer());
	    else if(fCalculationType.fStandDevUsed)
	    	fComputer.reset(new TLSWeightedUnkMtdComputer());
	    else
	    	fComputer.reset(new TLSParametricMtdComputer());
	    }

    //------------------------------------ComputeLSResults()---------------------------
    //------------------------------------first step of itere2Sol()---------------------------
    bool mtrFilled = inpMtrFiller.fillMatrices(&fData, inputMtr);
	inputMtr.saveMatricesToFile(fNumberOfMadeIterations);

		if (mtrFilled)
		{
			if (!fData.cfg.useApriori.isActive())
			{
				fS0APosterioriVariances = true;
			}
			else 
			{
				fS0APosterioriVariances = false;
			}

			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, fResultsMatrices.get());
		}*/
	}


}