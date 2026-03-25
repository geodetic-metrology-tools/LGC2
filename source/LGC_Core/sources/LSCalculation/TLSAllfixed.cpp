// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLSAllfixed.h"

#include <TLGCData.h>

TLSAllfixed::TLSAllfixed(TLGCData &data, int /*maxIter*/) :
	TLSAlgorithm(data), fPointTransformer(&data.getTree(), data.getConfig().referential), fAllfixedGenerator(fPointTransformer)
{
}

Behavior TLSAllfixed::run(TLGCData &data, int fMaxIterations)
{
	TLSAlgorithm normalLSProcess(data);
	Behavior computationOK = normalLSProcess.run(data, fMaxIterations);

	if (computationOK)
	{
		try
		{
			// Iteration through nodes of the LOR tree
			for (TDataTreeIterator itTree = data.getTree().begin(); itTree != data.getTree().end(); itTree++)
			{
				// Iterate through the Total station measurements (TSTN) in root
				if (itTree->get()->isROOTNode())
				{
					for (auto &itTSTN : itTree.node->data->measurements.fTSTN)
					{
						// Iterate through every ROM of TSTN
						for (auto &itROM : itTSTN->roms)
						{
							for (auto &itANGL : itROM->measANGL)
								itANGL.fAllFixedV0 = fAllfixedGenerator.getV0AllfixedANGL(*itTSTN, *itROM, itANGL);

							for (auto &itZEND : itROM->measZEND)
								if (!itTSTN->instrumentHeightAdjustable->isFixed())
									itZEND.fAllFixedHi = fAllfixedGenerator.getHiAllfixedZEND(*itTSTN, itZEND);

							for (auto &itDIST : itROM->measDIST)
							{
								if (!itDIST.target.distCorrectionAdjustable->isFixed())
									itDIST.fAllFixedCs = fAllfixedGenerator.getCsAllfixedDIST(*itTSTN, itDIST);
								if (!itTSTN->instrumentHeightAdjustable->isFixed())
									itDIST.fAllFixedHi = fAllfixedGenerator.getHiAllfixedDIST(*itTSTN, itDIST);
							}

							for (auto &itDHOR : itROM->measDHOR)
								if (!itDHOR.target.distCorrectionAdjustable->isFixed())
									itDHOR.fAllFixedCs = fAllfixedGenerator.getCsAllfixedDIST(*itTSTN, itDHOR);

							for (auto &itPLR3D : itROM->measPLR3D)
							{
								// V0
								TAngle *sol = fAllfixedGenerator.getV0AllfixedPLR(*itTSTN, *itROM, itPLR3D);
								itPLR3D.fAllFixedV0[0] = *sol;
								itPLR3D.fAllFixedV0[1] = *(sol + 1);

								// Cs
								if (!itPLR3D.target.distCorrectionAdjustable->isFixed())
									itPLR3D.fAllFixedCs = fAllfixedGenerator.getCsAllfixedPLR(*itTSTN, *itROM, itPLR3D);

								// Rx & Ry if ROT3D used
								if (itTSTN->rot3D)
								{
									TAngle *solrx = fAllfixedGenerator.getRxAllfixedPLR(*itTSTN, *itROM, itPLR3D);
									itPLR3D.fAllFixedRx[0] = *solrx;
									itPLR3D.fAllFixedRx[1] = *(solrx + 1);

									TAngle *solry = fAllfixedGenerator.getRyAllfixedPLR(*itTSTN, *itROM, itPLR3D);
									itPLR3D.fAllFixedRy[0] = *solry;
									itPLR3D.fAllFixedRy[1] = *(solry + 1);
								}
								// Hi
								else if (!itTSTN->instrumentHeightAdjustable->isFixed())
									itPLR3D.fAllFixedHi = fAllfixedGenerator.getHiAllfixedPLR(*itTSTN, *itROM, itPLR3D);
							}

							for (auto &itECTH : itROM->measECTH)
								itECTH.fAllFixedV0 = fAllfixedGenerator.getV0AllfixedECTH(*itTSTN, itECTH);
						}
					}
				}
				else
					for (auto &itTSTN : itTree.node->data->measurements.fTSTN)
					{
						// Iterate through every ROM of TSTN
						for (auto &itROM : itTSTN->roms)
							for (auto &itDIST : itROM->measDIST)
								if (!itDIST.target.distCorrectionAdjustable->isFixed())
									itDIST.fAllFixedCs = fAllfixedGenerator.getCsAllfixedDISTinFrame(*itTSTN, itDIST);
					}

				// In every node iterate through the EDM's measurements
				for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
					// Iterate through DPST measurements
					for (auto &itDPST : itEDM->measDSPT)
						if (!itDPST.target.distCorrectionAdjustable->isFixed())
							itDPST.fAllFixedCs = fAllfixedGenerator.getCsAllfixedDSPT(*itEDM, itDPST);

				// In every node iterate through the LEVEL's measurements
				for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
					// Iterate through DLEV measurements
					for (auto &itDLEV : itLEVEL.measDLEV)
						if (!itLEVEL.instrument.collAngleAdjustable->isFixed())
							itDLEV.fAllFixedCollimation = fAllfixedGenerator.getCollimationAllfixedDLEV(itLEVEL, itDLEV);
			}
		}
		catch (std::exception const &excp)
		{
			data.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
			return Behavior(Behavior::BehaviorCode::ERR_results, L"Error extraction parameter in mode ALLFIXED");
		}
	}

	return computationOK;
}
