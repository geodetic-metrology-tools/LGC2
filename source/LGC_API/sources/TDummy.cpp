#include <iostream>
#include "TDummy.h"
#include <TLGCData.h>
#include <TReader.h>
#include "TLGCCalculation.h"
#include <Behavior.h>
#include "testMixingObservation.h"

using namespace std;

void HelloWorld::PrintHelloWorld()
{
	cout << "Hello World!\n";
	std::shared_ptr<TLGCData> projTest(new TLGCData);

	TReader r(projTest);
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DIST.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(MixObs::ANGL_ZEND_DIST);

	bool succesReading = r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);

}