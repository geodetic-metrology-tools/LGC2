#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testLIBR.h"
#include "TLGCCalculation.h"
#include "TLibrCnstrGenerator.h"
#include <Behavior.h>

namespace tut
{
	struct test_LIBR{};
	typedef test_group<test_LIBR> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Tests LIBR");
}

namespace tut
{
	// test cnstrIdentifier filler
	template<>
	template<>
	void object::test<1>()
	{
		bool T, F;
		T = true;
		F = false;

		struct isFreeCnstr fCnstr;

		auto initCnstrIdentifier = [&](int* pointSettings)
		{
			//Initialise elements	
			int numberOfPoints = pointSettings[0] + pointSettings[1] + pointSettings[2] + pointSettings[3] + pointSettings[4] + pointSettings[5];

			int nbCALA = pointSettings[0];
			int nbPOIN = pointSettings[1];
			int nbVXY = pointSettings[2];
			int nbVXZ = pointSettings[3];
			int nbVYZ = pointSettings[4];
			int nbVZ = pointSettings[5];

			int nbORIE = pointSettings[7];
			int nbPDOR = pointSettings[6];

			//fill fCnstr
			switch (nbCALA)
			{

			case 0:  //0 CALA point

				// Only one point type
				if (nbPOIN == numberOfPoints)
				{
					if (nbORIE == 1)
					{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = T;
						fCnstr.ry = F;
						fCnstr.rz = T;
						fCnstr.k = T;
						break;
					}
					else if (nbORIE == 2)
					{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbORIE > 2)
					{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = T;
						fCnstr.k = T;
						break;
					}
					else{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = T;
						fCnstr.ry = T;
						fCnstr.rz = T;
						fCnstr.k = T;
						break;
					}
				}
				else if (nbVXY == numberOfPoints)
				{
					fCnstr.dx = T;
					fCnstr.dy = T;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = T;
					fCnstr.k = T;
					break;
				}
				else if (nbVXZ == numberOfPoints)
				{
					fCnstr.dx = T;
					fCnstr.dy = F;
					fCnstr.dz = T;
					fCnstr.rx = F;
					fCnstr.ry = T;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if (nbVYZ == numberOfPoints)
				{
					fCnstr.dx = F;
					fCnstr.dy = T;
					fCnstr.dz = T;
					fCnstr.rx = T;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if (nbVZ == numberOfPoints)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = T;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}

				// Mixing point types
				if (nbVZ > 0)
				{
					if (nbVXY > 0) // VZ + VXY + ...
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = F;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if ((nbVZ > 1 && (nbVXZ > 0 || nbVYZ > 0 || nbPOIN > 0)) || ((nbVXZ > 1 || (nbVXZ == 1 && nbVYZ > 0))) || ((nbORIE > 1 || (nbORIE == 1 && nbPOIN > 0)))) // VZ + VZ + (VXZ|VYZ|POIN)  || VZ+ VXZ + (VXZ | VYZ)  || VZ+ORIE+(POIN | ORIE)
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbVXZ == 1 && nbVXY == 0 && nbVYZ == 0 && nbPDOR == 0 && (nbPOIN > 0 || nbORIE > 0)) //VZ+ (POIN | ORIE) +VXZ
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = T;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbVYZ > 0 && nbVXY == 0 && nbVYZ > 0 && nbPDOR == 0) //VZ+ (POIN | ORIE | VYZ) +VYZ
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = T;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbVZ == 1 && nbVXY == 0 && nbVYZ == 0 && nbPDOR == 0 && nbORIE == 0 && nbPOIN > 1) //VZ+ POIN + POIN
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = T;
						fCnstr.ry = T;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
				}
				else if (nbVXY > 0)
				{
					if (nbVXZ > 0 && nbVYZ > 0)  // VXY + VXZ + VYZ
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = F;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbVXZ > 0) //VXY + VXZ + (VXZ , POIN, ORIE, VXY)
					{
						fCnstr.dx = T;
						fCnstr.dy = F;
						fCnstr.dz = F;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbVYZ > 0) //VXY + VYZ + (VXY, VYZ, POIN, ORIE)
					{
						fCnstr.dx = F;
						fCnstr.dy = T;
						fCnstr.dz = F;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if ((nbVXY == 2 && nbORIE > 0) | (nbVXY == 1 && nbORIE > 0)) //VXY + VXY + ORIE | VXY +ORIE +(POIN|ORIE)
					{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = F;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if ((nbVXY == 2 || nbVXY == 1) && nbPOIN > 0) //VXY + (POIN|VXY) +POIN
					{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = F;
						fCnstr.rx = T;
						fCnstr.ry = T;
						fCnstr.rz = T;
						fCnstr.k = T;
						break;
					}
					else
					{
						fCnstr.dx = T;
						fCnstr.dy = T;
						fCnstr.dz = F;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = T;
						fCnstr.k = T;
						break;
					}
				}
				else if (nbVXZ > 0)
				{
					if (nbVYZ > 0) // VXZ + VYZ + (VXZ, VYZ, POIN, ORIE)
					{
						fCnstr.dx = F;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else if (nbVXZ == 1 && nbPOIN == (numberOfPoints - 1) && nbORIE > 1)
					{
						fCnstr.dx = T;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					else // other case wich are not yet tested with VXZ : VXZ + VXZ + (POIN, ORIE) ou VXZ + POIN + (POIN, ORIE)
					{
						fCnstr.dx = T;
						fCnstr.dy = F;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = T;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
				}
				else if (nbVYZ > 0) // last combinations with VYZ
				{
					if (nbVYZ == 1 && nbPOIN == (numberOfPoints - 1) && nbORIE > 1) // VYZ + ORIE +ORIE
					{
						fCnstr.dx = F;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = F;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
					if (nbVYZ == 1 && nbPOIN == (numberOfPoints - 1) && nbORIE == 0) // VYZ + POIN +POIN
					{
						fCnstr.dx = F;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = T;
						fCnstr.ry = T;
						fCnstr.rz = T;
						fCnstr.k = T;
						break;
					}
					else // VYZ + VYZ + (POIN , ORIE) | VYZ +POIN + ORIE
					{
						fCnstr.dx = F;
						fCnstr.dy = T;
						fCnstr.dz = T;
						fCnstr.rx = T;
						fCnstr.ry = F;
						fCnstr.rz = F;
						fCnstr.k = T;
						break;
					}
				}




			case 1: // only 1 CALA is used
				if ((nbORIE > 0 || nbPDOR > 0) && (nbVXY > 0 || nbVXZ > 0 || nbVYZ > 0 || nbVZ > 0)) // CALA + (ORIE | PDOR) + (VXY | VXZ | VYZ | VZ)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if ((nbORIE == 1 || nbPDOR == 1) && nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && nbVZ == 0) // CALA + (ORIE | PDOR) + POIN 
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = T;
					fCnstr.ry = T;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if ((nbORIE > 0 || nbPDOR > 0) && nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && nbVZ == 0) // CALA + (ORIE | PDOR) + ORIE 
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if (nbORIE == 0 && nbPDOR == 0 && nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && (nbVZ == 0 || nbVZ == 1))  //CALA + POIN + POIN  ou CALA + POIN +VZ
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = T;
					fCnstr.ry = T;
					fCnstr.rz = T;
					fCnstr.k = T;
					break;
				}
				else if ((nbVXY > 0 || nbVXZ > 0 || nbVYZ > 0) && nbVZ > 0) // CALA + VZ + (VXY | VXZ | VYZ)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if ((nbVXY == 0 && nbVXZ == 0 && nbVYZ == 0 && nbPDOR ==0 && nbORIE==0) && nbVZ > 0) // CALA + VZ + VZ
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = T;
					fCnstr.ry = T;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if ((nbVZ == 0 && nbVXZ == 0 && nbVYZ == 0) && nbVXY > 0) // CALA + VXY + (VXY |POIN)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = T;
					fCnstr.k = T;
					break;
				}
				else if (nbVZ == 0 && (nbVXZ > 0 || nbVYZ > 0) && nbVXY > 0) // CALA + VXY + (VXZ | VYZ )
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if (nbVZ == 0 && nbVXY == 0 && (nbVYZ > 0 || nbORIE>0 || nbPDOR>0 )&& nbVXZ > 0)// CALA + VXZ + (VYZ |PDOR|ORIE)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if (nbVZ == 0 && nbVXY == 0 && nbVYZ == 0 && nbVXZ > 1)// CALA + VXZ + VXZ 
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = T;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else if (nbVZ == 0 && nbVXY == 0 && nbVYZ == 0 && nbORIE==0 && nbPDOR==0 && nbVXZ == 1)// CALA + VXZ + POIN 
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = T;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}
				else //CALA + VYZ +(VYZ | POIN)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = T;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = T;
					break;
				}



			case 2: // only 2 CALA are used
			{
				if (nbVXY > 0 || nbVXZ > 0 || nbVYZ > 0 || nbVZ > 0 || nbPDOR>0 || nbORIE>0) // CALA + CALA + (VXY | VXZ | VYZ | VZ| PDOR| ORIE)
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = F;
					fCnstr.ry = F;
					fCnstr.rz = F;
					fCnstr.k = F;
					break;
				}
				else // CALA + CALA + POIN
				{
					fCnstr.dx = F;
					fCnstr.dy = F;
					fCnstr.dz = F;
					fCnstr.rx = T;
					fCnstr.ry = T;
					fCnstr.rz = T;
					fCnstr.k = F;
					break;
				}
			}

			default: // more than 3 CALA are used
			{
				fCnstr.dx = F;
				fCnstr.dy = F;
				fCnstr.dz = F;
				fCnstr.rx = F;
				fCnstr.ry = F;
				fCnstr.rz = F;
				fCnstr.k = F;
				break;
			}

			}

			return ;
		};


		const unsigned int testnum(83);
		int pointSettings[testnum][8] = {
		//CALA POIN VXY VXZ VYZ VZ PDOR ORIE
			{ 3, 3, 0, 0, 0, 0, 0, 0 },	// CALA	CALA	CALA
			{ 0, 3, 0, 0, 0, 0, 0, 0 },	// POIN	POIN	POIN
			{ 0, 3, 3, 0, 0, 0, 0, 0 },	// VXY	VXY	VXY
			{ 0, 3, 0, 3, 0, 0, 0, 0 },	// VXZ	VXZ	VXZ
			{ 0, 3, 0, 0, 3, 0, 0, 0 },	// VYZ	VYZ	VYZ
			{ 0, 3, 0, 0, 0, 3, 0, 0 },	// VZ	VZ	VZ
			{ 2, 3, 0, 0, 0, 0, 0, 0 },	// CALA	CALA	POIN
			{ 2, 3, 1, 0, 0, 0, 0, 0 },	// CALA	CALA	VXY
			{ 2, 3, 0, 1, 0, 0, 0, 0 },	// CALA	CALA	VXZ
			{ 2, 3, 0, 0, 1, 0, 0, 0 },	// CALA	CALA	VYZ
			{ 2, 3, 0, 0, 0, 1, 0, 0 },	// CALA	CALA	VZ
			{ 2, 3, 0, 0, 0, 0, 1, 1 },	// CALA	CALA	PDOR/ORIE
			{ 1, 3, 0, 0, 0, 2, 0, 0 },	// CALA	VZ	VZ
			{ 1, 3, 1, 0, 0, 1, 0, 0 },	// CALA	VZ	VXY
			{ 1, 3, 0, 1, 0, 1, 0, 0 },	// CALA	VZ	VXZ
			{ 1, 3, 0, 0, 1, 1, 0, 0 },	// CALA	VZ	VYZ
			{ 1, 3, 0, 0, 0, 1, 0, 0 },	// CALA	VZ	POIN
			{ 1, 3, 0, 0, 0, 1, 1, 1 },	// CALA	VZ	PDOR/ORIE
			{ 1, 3, 2, 0, 0, 0, 0, 0 },	// CALA	VXY	VXY
			{ 1, 3, 1, 1, 0, 0, 0, 0 },	// CALA	VXY	VXZ
			{ 1, 3, 1, 0, 1, 0, 0, 0 },	// CALA	VXY	VYZ
			{ 1, 3, 1, 0, 0, 0, 0, 0 },	// CALA	VXY	POIN
			{ 1, 3, 1, 0, 0, 0, 1, 1 },	// CALA	VXY	PDOR/ORIE
			{ 1, 3, 0, 2, 0, 0, 0, 0 },	// CALA	VXZ	VXZ
			{ 1, 3, 0, 1, 1, 0, 0, 0 },	// CALA	VXZ	VYZ
			{ 1, 3, 0, 1, 0, 0, 0, 0 },	// CALA	VXZ	POIN
			{ 1, 3, 0, 1, 0, 0, 1, 1 },	// CALA	VXZ	PDOR/ORIE
			{ 1, 3, 0, 0, 3, 0, 0, 0 },	// CALA	VYZ	VYZ
			{ 1, 3, 0, 0, 1, 0, 0, 0 },	// CALA	VYZ	POIN
			{ 1, 3, 0, 0, 1, 0, 1, 1 },	// CALA	VYZ	PDOR/ORIE
			{ 1, 3, 0, 0, 0, 0, 2, 2 },	// CALA	PDOR/ORIE	ORIE
			{ 1, 3, 0, 0, 0, 0, 0, 1 },	// CALA	PDOR/ORIE	POIN
			{ 1, 3, 0, 0, 0, 0, 0, 0 },	// CALA	POIN	POIN
			{ 0, 3, 1, 0, 0, 2, 0, 0 },	// VZ	VZ	VXY
			{ 0, 3, 0, 1, 0, 2, 0, 0 },	// VZ	VZ	VXZ
			{ 0, 3, 0, 0, 1, 2, 0, 0 },	// VZ	VZ	VYZ
			{ 0, 3, 0, 0, 0, 2, 0, 0 },	// VZ	VZ	POIN
			{ 0, 3, 2, 0, 0, 1, 0, 0 },	// VZ	VXY	VXY
			{ 0, 3, 1, 1, 0, 1, 0, 0 },	// VZ	VXY	VXZ
			{ 0, 3, 1, 0, 1, 1, 0, 0 },	// VZ	VXY	VYZ
			{ 0, 3, 1, 0, 0, 1, 0, 0 },	// VZ	VXY	POIN
			{ 0, 3, 0, 2, 0, 1, 0, 0 },	// VZ	VXZ	VXZ
			{ 0, 3, 0, 1, 1, 1, 0, 0 },	// VZ	VXZ	VYZ
			{ 0, 3, 0, 1, 0, 1, 0, 0 },	// VZ	VXZ	POIN
			{ 0, 3, 0, 0, 2, 1, 0, 0 },	// VZ	VYZ	VYZ
			{ 0, 3, 0, 0, 1, 1, 0, 0 },	// VZ	VYZ	POIN
			{ 0, 3, 0, 0, 0, 1, 0, 0 },	// VZ	POIN	POIN
			{ 0, 3, 0, 0, 0, 1, 0, 2 },	// VZ	ORIE	ORIE
			{ 0, 3, 1, 0, 0, 1, 0, 1 },	// VZ	ORIE	VXY
			{ 0, 3, 0, 1, 0, 1, 0, 1 },	// VZ	ORIE	VXZ
			{ 0, 3, 0, 0, 1, 1, 0, 1 },	// VZ	ORIE	VYZ
			{ 0, 3, 0, 0, 0, 1, 0, 1 },	// VZ	ORIE	POIN
			{ 0, 3, 2, 1, 0, 0, 0, 0 },	// VXY	VXY	VXZ
			{ 0, 3, 2, 0, 1, 0, 0, 0 },	// VXY	VXY	VYZ
			{ 0, 3, 2, 0, 0, 0, 0, 0 },	// VXY	VXY	POIN
			{ 0, 3, 2, 0, 0, 0, 0, 1 },	// VXY	VXY	ORIE
			{ 0, 3, 1, 2, 0, 0, 0, 0 },	// VXY	VXZ	VXZ
			{ 0, 3, 1, 1, 1, 0, 0, 0 },	// VXY	VXZ	VYZ
			{ 0, 3, 1, 1, 0, 0, 0, 0 },	// VXY	VXZ	POIN
			{ 0, 3, 1, 1, 0, 0, 0, 2 },	// VXY	VXZ	ORIE
			{ 0, 3, 1, 0, 2, 0, 0, 0 },	// VXY	VYZ	VYZ
			{ 0, 3, 1, 0, 1, 0, 0, 0 },	// VXY	VYZ	POIN
			{ 0, 3, 1, 0, 1, 0, 0, 1 },	// VXY	VYZ	ORIE
			{ 0, 3, 1, 0, 0, 0, 0, 0 },	// VXY	POIN	POIN
			{ 0, 3, 1, 0, 0, 0, 0, 1 },	// VXY	POIN	ORIE
			{ 0, 3, 1, 0, 0, 0, 0, 2 },	// VXY	ORIE	ORIE
			{ 0, 3, 0, 2, 1, 0, 0, 0 },	// VXZ	VXZ	VYZ
			{ 0, 3, 0, 2, 0, 0, 0, 0 },	// VXZ	VXZ	POIN
			{ 0, 3, 0, 2, 0, 0, 0, 1 },	// VXZ	VXZ	ORIE
			{ 0, 3, 0, 1, 0, 2, 0, 0 },	// VXZ	VYZ	VYZ
			{ 0, 3, 0, 1, 1, 0, 0, 0 },	// VXZ	VYZ	POIN
			{ 0, 3, 0, 1, 1, 0, 0, 1 },	// VXZ	VYZ	ORIE
			{ 0, 3, 0, 1, 0, 0, 0, 0 },	// VXZ	POIN	POIN
			{ 0, 3, 0, 1, 0, 0, 0, 1 },	// VXZ	POIN	ORIE
			{ 0, 3, 0, 1, 0, 0, 0, 2 },	// VXZ	ORIE	ORIE
			{ 0, 3, 0, 0, 2, 0, 0, 0 },	// VYZ	VYZ	POIN
			{ 0, 3, 0, 0, 2, 0, 0, 1 },	// VYZ	VYZ	ORIE
			{ 0, 3, 0, 0, 1, 0, 0, 0 },	// VYZ	POIN	POIN
			{ 0, 3, 0, 0, 1, 0, 0, 1 },	// VYZ	POIN	ORIE
			{ 0, 3, 0, 0, 1, 0, 0, 2 },	// VYZ	ORIE	ORIE
			{ 0, 3, 0, 0, 0, 0, 0, 1 },	// POIN	POIN	ORIE
			{ 0, 3, 0, 0, 0, 0, 0, 2 },	// POIN	ORIE	ORIE
			{ 0, 3, 0, 0, 0, 0, 0, 3 } };	// ORIE	ORIE	ORIE


		bool cnstrSettings[testnum][7] = {
			//	 Tx  Tz  Tz  Rx  Ry  Rz  S
			{ F, F, F, F, F, F, F },  // CALA	CALA	CALA
			{ T, T, T, T, T, T, T },  // POIN	POIN	POIN
			{ T, T, F, F, F, T, T },  // VXY	VXY	VXY
			{ T, F, T, F, T, F, T },  // VXZ	VXZ	VXZ
			{ F, T, T, T, F, F, T },  // VYZ	VYZ	VYZ
			{ F, F, T, F, F, F, T },  // VZ	VZ	VZ
			{ F, F, F, T, T, T, F },  // CALA	CALA	POIN
			{ F, F, F, F, F, F, F },  // CALA	CALA	VXY
			{ F, F, F, F, F, F, F },  // CALA	CALA	VXZ
			{ F, F, F, F, F, F, F },  // CALA	CALA	VYZ
			{ F, F, F, F, F, F, F },  // CALA	CALA	VZ
			{ F, F, F, F, F, F, F },  // CALA	CALA	PDOR/ORIE
			{ F, F, F, T, T, F, T },  // CALA	VZ	VZ
			{ F, F, F, F, F, F, T },  // CALA	VZ	VXY
			{ F, F, F, F, F, F, T },  // CALA	VZ	VXZ
			{ F, F, F, F, F, F, T },  // CALA	VZ	VYZ
			{ F, F, F, T, T, T, T },  // CALA	VZ	POIN
			{ F, F, F, F, F, F, T },  // CALA	VZ	PDOR/ORIE
			{ F, F, F, F, F, T, T },  // CALA	VXY	VXY
			{ F, F, F, F, F, F, T },  // CALA	VXY	VXZ
			{ F, F, F, F, F, F, T },  // CALA	VXY	VYZ
			{ F, F, F, F, F, T, T },  // CALA	VXY	POIN
			{ F, F, F, F, F, F, T },  // CALA	VXY	PDOR/ORIE
			{ F, F, F, F, T, F, T },  // CALA	VXZ	VXZ
			{ F, F, F, F, F, F, T },  // CALA	VXZ	VYZ
			{ F, F, F, F, T, F, T },  // CALA	VXZ	POIN
			{ F, F, F, F, F, F, T },  // CALA	VXZ	PDOR/ORIE
			{ F, F, F, T, F, F, T },  // CALA	VYZ	VYZ
			{ F, F, F, T, F, F, T },  // CALA	VYZ	POIN
			{ F, F, F, F, F, F, T },  // CALA	VYZ	PDOR/ORIE
			{ F, F, F, F, F, F, T },  // CALA	PDOR/ORIE	ORIE
			{ F, F, F, T, T, F, T },  // CALA	PDOR/ORIE	POIN
			{ F, F, F, T, T, T, T },  // CALA	POIN	POIN
			{ F, F, F, F, F, F, T },  // VZ	VZ	VXY
			{ F, F, T, F, F, F, T },  // VZ	VZ	VXZ
			{ F, F, T, F, F, F, T },  // VZ	VZ	VYZ
			{ F, F, T, F, F, F, T },  // VZ	VZ	POIN
			{ F, F, F, F, F, F, T },  // VZ	VXY	VXY
			{ F, F, F, F, F, F, T },  // VZ	VXY	VXZ
			{ F, F, F, F, F, F, T },  // VZ	VXY	VYZ
			{ F, F, F, F, F, F, T },  // VZ	VXY	POIN
			{ F, F, T, F, F, F, T },  // VZ	VXZ	VXZ
			{ F, F, T, F, F, F, T },  // VZ	VXZ	VYZ
			{ F, F, T, F, T, F, T },  // VZ	VXZ	POIN
			{ F, F, T, T, F, F, T },  // VZ	VYZ	VYZ
			{ F, F, T, T, F, F, T },  // VZ	VYZ	POIN
			{ F, F, T, T, T, F, T },  // VZ	POIN	POIN
			{ F, F, T, F, F, F, T },  // VZ	ORIE	ORIE
			{ F, F, F, F, F, F, T },  // VZ	ORIE	VXY
			{ F, F, T, F, F, F, T },  // VZ	ORIE	VXZ
			{ F, F, T, F, F, F, T },  // VZ	ORIE	VYZ
			{ F, F, T, F, F, F, T },  // VZ	ORIE	POIN
			{ T, F, F, F, F, F, T },  // VXY	VXY	VXZ
			{ F, T, F, F, F, F, T },  // VXY	VXY	VYZ
			{ T, T, F, T, T, T, T },  // VXY	VXY	POIN
			{ T, T, F, F, F, F, T },  // VXY	VXY	ORIE
			{ T, F, F, F, F, F, T },  // VXY	VXZ	VXZ
			{ F, F, F, F, F, F, T },  // VXY	VXZ	VYZ
			{ T, F, F, F, F, F, T },  // VXY	VXZ	POIN
			{ T, F, F, F, F, F, T },  // VXY	VXZ	ORIE
			{ F, T, F, F, F, F, T },  // VXY	VYZ	VYZ
			{ F, T, F, F, F, F, T },  // VXY	VYZ	POIN
			{ F, T, F, F, F, F, T },  // VXY	VYZ	ORIE
			{ T, T, F, T, T, T, T },  // VXY	POIN	POIN
			{ T, T, F, F, F, F, T },  // VXY	POIN	ORIE
			{ T, T, F, F, F, F, T },  // VXY	ORIE	ORIE
			{ F, F, T, F, F, F, T },  // VXZ	VXZ	VYZ
			{ T, F, T, F, T, F, T },  // VXZ	VXZ	POIN
			{ T, F, T, F, T, F, T },  // VXZ	VXZ	ORIE
			{ F, F, T, F, F, F, T },  // VXZ	VYZ	VYZ
			{ F, F, T, F, F, F, T },  // VXZ	VYZ	POIN
			{ F, F, T, F, F, F, T },  // VXZ	VYZ	ORIE
			{ T, F, T, F, T, F, T },  // VXZ	POIN	POIN
			{ T, F, T, F, T, F, T },  // VXZ	POIN	ORIE
			{ T, F, T, F, F, F, T },  // VXZ	ORIE	ORIE
			{ F, T, T, T, F, F, T },  // VYZ	VYZ	POIN
			{ F, T, T, T, F, F, T },  // VYZ	VYZ	ORIE
			{ F, T, T, T, T, T, T },  // VYZ	POIN	POIN
			{ F, T, T, T, F, F, T },  // VYZ	POIN	ORIE
			{ F, T, T, F, F, F, T },  // VYZ	ORIE	ORIE
			{ T, T, T, T, F, T, T },  // POIN	POIN	ORIE
			{ T, T, T, F, F, F, T },  // POIN	ORIE	ORIE
			{ T, T, T, F, F, T, T } };  // ORIE	ORIE	ORIE

		for (int i = 0; i < testnum; i++)
		{
			initCnstrIdentifier(pointSettings[i]);

			ensure_equals("dx", fCnstr.dx, cnstrSettings[i][0]);
			ensure_equals("dy", fCnstr.dy, cnstrSettings[i][1]);
			ensure_equals("dz", fCnstr.dz, cnstrSettings[i][2]);
			ensure_equals("rx", fCnstr.rx, cnstrSettings[i][3]);
			ensure_equals("ry", fCnstr.ry, cnstrSettings[i][4]);
			ensure_equals("rz", fCnstr.rz, cnstrSettings[i][5]);
			ensure_equals("k", fCnstr.k, cnstrSettings[i][6]);
		}


		};

	
	// test ZEND, DIST and ANGL in SPHE
	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
	
		set_test_name("Testing AZD measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
	
		stringstream infiler(TestLIBR::LIBR_AZDist);
		stringstream cpinfiler(TestLIBR::LIBR_AZDist);
	
		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);
	
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	
		const TLGCData& data = calcul.getData();
	
		/* lgc1 results
		STN          2260.7774073   4493.9463772   2401.7426406   0.0000   0.0000   0.0000   0.0373   0.2072  -0.0294
		REF          2203.0484484   4412.2919770   2401.7757707   0.0000   0.0000   0.0000   0.1876  -0.3574   0.0295
		STN2         2284.7017749   4354.5631251   2401.7940957   0.0000   0.0000   0.0000  -0.3280  -0.1029  -0.0142
		PT           2342.4313479   4436.2171177   2401.7610678   0.0000   0.0000   0.0000   0.1358   0.0541   0.0291
		PT2          2283.7020702   4353.5634271   2401.7944936   0.0000   0.0000   0.0000  -0.0327   0.1991  -0.0150
		*/
	
		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4313479, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2171177, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2401.7610678, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7774073, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9463772, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 2401.7426406, 1e-7);
	}
	
	// test ZEND, DHOR and ANGL in OLOC
	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
	
		set_test_name("Testing AZDhor measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
	
		stringstream infiler(TestLIBR::LIBR_AZDhor);
		stringstream cpinfiler(TestLIBR::LIBR_AZDhor);
	
		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);
	
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	
		const TLGCData& data = calcul.getData();
	
		/* lgc1 results
		STN          2260.7773786   4493.9462033    402.1981709   0.0000   0.0000   0.0000   0.0086   0.0333   0.3821 
		REF          2203.0482892   4412.2923046    402.1981910   0.0000   0.0000   0.0000   0.0284  -0.0299  -0.3826 
		STN2         2284.7021160   4354.5632592    402.1995487   0.0000   0.0000   0.0000   0.0131   0.0311   0.1904 
		PT           2342.4311500   4436.2169968    402.1981941   0.0000   0.0000   0.0000  -0.0621  -0.0667  -0.3795 
		PT2          2283.7021149   4353.5632603    402.1995478   0.0000   0.0000   0.0000   0.0119   0.0323   0.1895
		*/
	
		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4311500, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2169968, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 402.1981941, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7773786, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9462033, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 402.1981709, 1e-7);
	}
	
	// test PLR in SPHE
	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
	
		set_test_name("Testing PLR measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
	
		stringstream infiler(TestLIBR::LIBR_PLR);
		stringstream cpinfiler(TestLIBR::LIBR_PLR);
	
		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);
	
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	
		const TLGCData& data = calcul.getData();
	
		/* lgc1 results
		STN          2260.7774073   4493.9463772   2401.7426406   0.0000   0.0000   0.0000   0.0373   0.2072  -0.0294
		REF          2203.0484484   4412.2919770   2401.7757707   0.0000   0.0000   0.0000   0.1876  -0.3574   0.0295
		STN2         2284.7017749   4354.5631251   2401.7940957   0.0000   0.0000   0.0000  -0.3280  -0.1029  -0.0142
		PT           2342.4313479   4436.2171177   2401.7610678   0.0000   0.0000   0.0000   0.1358   0.0541   0.0291
		PT2          2283.7020702   4353.5634271   2401.7944936   0.0000   0.0000   0.0000  -0.0327   0.1991  -0.0150
		*/
	
		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4313479, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2171177, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2401.7610678, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7774073, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9463772, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 2401.7426406, 1e-7);
	}

	// test DVER in OLOC
	template<>
	template<>
	void object::test<5>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing DVER measurement in oloc");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLIBR::LIBR_DVER);
		stringstream cpinfiler(TestLIBR::LIBR_DVER);

		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	
		const TLGCData& data = calcul.getData();

		/* lgc1 results
		STN          2260.7773969   4493.9464474    402.1977888   0.0000   0.0000            0.0269   0.2774
		REF          2203.0485258   4412.2919785    402.1985736   0.0000   0.0000            0.2650  -0.3560
		STN2         2284.7019144   4354.5632116    402.1993583   0.0000   0.0000           -0.1885  -0.0165
		PT           2342.4313995   4436.2172730    402.1495648   0.0000   0.0000   0.0000   0.1874   0.2094 -49.0088
		PT2          2283.7018122   4353.5631138    402.1987314   0.0000   0.0000   0.0000  -0.2907  -0.1142  -0.6269
		*/

		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4313995, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2172730, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 402.1495648, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7773969, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9464474, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 402.1977888, 1e-7);
	}
	


};
