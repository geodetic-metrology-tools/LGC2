#include "TLSAllfixed.h"
#include "TLGCData.h"

TLSAllfixed::TLSAllfixed(TLGCData& data, int maxIter):fPointTransformer(&data.getTree(), data.getConfig().referential),
fAllfixedGenerator(fPointTransformer)
{}

bool TLSAllfixed::run(TLGCData& data, int fMaxIterations)
{
	bool computationOK;
	TLSAlgorithm normalLSProcess;
	computationOK = normalLSProcess.run(data, fMaxIterations);

	if (computationOK)
	{
		bool successfullExtraction = true;

		try{
			// Iteration through nodes of the LOR tree
			for (TDataTreeIterator itTree = data.getTree().begin(); itTree != data.getTree().end(); itTree++)
			{
				// Iterate through the Total station measurements (TSTN)
				for (auto itTSTN(itTree.node->data->measurements.fTSTN.begin()); itTSTN != itTree.node->data->measurements.fTSTN.end(); ++itTSTN)
				{
					// Iterate through every ROM of TSTN 
					for (auto itROM(itTSTN->roms.begin()); itROM != itTSTN->roms.end(); ++itROM)
					{
						for (auto& itANGL : itROM->measANGL)
							fAllfixedGenerator.getV0AllfixedANGL(*itTSTN, *itROM, itANGL);

						for (auto& itZEND : itROM->measZEND)
							if (!itTSTN->instrumentHeightAdjustable->isFixed())
								fAllfixedGenerator.getHiAllfixedZEND(*itTSTN, itZEND);

						for (auto& itDIST : itROM->measDIST)
						{
							if (!itDIST.target.distCorrectionAdjustable->isFixed())
								fAllfixedGenerator.getCsAllfixedDIST(*itTSTN, itDIST);
							if (!itTSTN->instrumentHeightAdjustable->isFixed())
								fAllfixedGenerator.getHiAllfixedDIST(*itTSTN, itDIST);
						}

						for (auto& itDHOR : itROM->measDHOR)
							if (!itDHOR.target.distCorrectionAdjustable->isFixed())
								fAllfixedGenerator.getCsAllfixedDIST(*itTSTN, itDHOR);

						//for (auto& itPLR3D : itROM->measPLR3D)
						//	fAllfixedGenerator(rm, itPLR3D);

						//for (auto& itECTH : itROM->measECTH)
						//	fAllfixedGenerator.getV0AllfixedECTH(rm, itECTH);

						//for (auto& itECSP : itROM->measECSP)
						//	fAllfixedGenerator.getV0AllfixedECSP(rm, itECSP);

					}
				}

				//In every node iterate through the EDM's measurements
				for (auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
					//Iterate through DPST measurements
					for (auto& itDPST : itEDM->measDSPT)
						if (!itDPST.target.distCorrectionAdjustable->isFixed())
							fAllfixedGenerator.getCsAllfixedDSPT(*itEDM, itDPST);


				//In every node iterate through the LEVEL's measurements
				for (auto& itLEVEL : itTree.node->data->measurements.fLEVEL)
					//Iterate through DLEV measurements
					for (auto& itDLEV : itLEVEL.measDLEV)
						if (!itLEVEL.instrument.collAngleAdjustable->isFixed())
							fAllfixedGenerator.getCollimationAllfixedDLEV(itLEVEL, itDLEV);
					
			}
		}
		catch (std::exception const & excp) {
			data.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
			successfullExtraction = false;
		}

		return successfullExtraction;
	}
	else
		return computationOK;
}