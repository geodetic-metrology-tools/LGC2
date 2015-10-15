#include <fstream>
#include <tut/tut.hpp>

#include <TRefFrameInfo.h>
#include <TCoordSysFactory.h>
#include <TAReferenceFrame.h>

namespace tut
{
    struct test_leveling{};
    typedef test_group<test_leveling> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test Leveling Algorithm");

	static const char* header = 
		"*TITR\n"
		"Simulation	of	height	observations	for	leveling	\n"
		"algorithm	testing	like	in	thesis	of	JZ\n"
		"*LEP\n"
		"*CALA\n"
		"1	5000	5097.79265	445.072051435999\n"
		"2	5010	5107.79265	445.121374177567\n"
		"3	5010	5117.79265	445.076128322966\n"
		"6	5004	5117.79265	445.363375735650\n"
		"7	5008	5117.79265	445.305210157353\n"
		"*VZ\n"
		"5	5020	5107.79265	445.105973795617\n"
		"*POIN\n"
		"4	5005	5127.79265	445.5686039318\n"
		"*DVER 0.2\n";
}

namespace tut
{
	template<>
    template<>
    void object::test<1>()
    {
		static const int STATION(3);
		enum {kCCS, kLAV, kXYh};

		static const char* filenames[] = {
			"C:/TEMP/TEST_LEVELING/dzCCS",
			"C:/TEMP/TEST_LEVELING/dzLAV",
			"C:/TEMP/TEST_LEVELING/dHorth",
		};
		
		std::ofstream infiles[3];
		std::ofstream cmdfiles[6];
		std::ofstream metacmd("C:/TEMP/TEST_LEVELING/run_tests.bat");

		for (int i = 0; i < 3; i++) {
			infiles[i].open(std::string(filenames[i]) + std::string(".in"));
			infiles[i] << header;
		}

		for (int i = 0, j = 0; i < 5, j < 3; i += 2, j++) {
			cmdfiles[i  ].open(std::string(filenames[j]) + std::string("1.cmd"));
			cmdfiles[i+1].open(std::string(filenames[j]) + std::string("2.cmd"));
			
			cmdfiles[i  ] << filenames[j] << "\nin\n" << filenames[j] << "1\nout";
			cmdfiles[i+1] << filenames[j] << "\nin\n" << filenames[j] << "2\nout";
			
			metacmd << "%1 -f " << filenames[j] << "1.cmd > " << filenames[j] << "1_dbg.txt\n";
			metacmd << "%2 -f " << filenames[j] << "2.cmd > " << filenames[j] << "2_dbg.txt\n";
		}

		TRefSystemFactory *sysfact(TRefSystemFactory::getRefSystemFactory());
		TAReferenceFrame  *cernXYH(sysfact->getRefFrame(TRefSystemFactory::kCernXYHe));
		TAReferenceFrame  *CCS(sysfact->getRefFrame(TRefSystemFactory::kCCS));
		TAReferenceFrame  *XYh00(sysfact->getRefFrame(TRefSystemFactory::kCernXYHg00Machine));
		TAReferenceFrame  *LAV(0);
		TCoordSysFactory::ECoordSys k3D(TCoordSysFactory::k3DCartesian);
		
		TSpatialPosition pointsCCS[] = {
			TSpatialPosition(CCS, 5000,	5097.79265,	2443.68, k3D),
			TSpatialPosition(CCS, 5010,	5107.79265,	2443.72, k3D),
			TSpatialPosition(CCS, 5010,	5117.79265,	2443.67, k3D),
			TSpatialPosition(CCS, 5005,	5127.79265,	2444.16, k3D),
			TSpatialPosition(CCS, 5020,	5107.79265,	2443.7, k3D),
			TSpatialPosition(CCS, 5004,	5117.79265,	2443.96, k3D),
			TSpatialPosition(CCS, 5008,	5117.79265,	2443.9, k3D)
		};
		
		// for the calculation of orthometric height differences
		TSpatialPosition pointsXYh[] = {
			TSpatialPosition(pointsCCS[0]), TSpatialPosition(pointsCCS[1]), TSpatialPosition(pointsCCS[2]), 
			TSpatialPosition(pointsCCS[3]), TSpatialPosition(pointsCCS[4]), TSpatialPosition(pointsCCS[5]), TSpatialPosition(pointsCCS[6])
		};
		// all points as they are seen from the station
		TSpatialPosition pointsLAVstation[] = {
			TSpatialPosition(pointsCCS[0]), TSpatialPosition(pointsCCS[1]), TSpatialPosition(pointsCCS[2]), 
			TSpatialPosition(pointsCCS[3]), TSpatialPosition(pointsCCS[4]), TSpatialPosition(pointsCCS[5]), TSpatialPosition(pointsCCS[6])
		};

		TLocalSystemOrigin lsoStation(pointsCCS[STATION], TAngle(0), TAngle(0), "STATION");
		LAV = sysfact->getNewLocalRefFrame(lsoStation, TRefSystemFactory::kCG2000Machine, TRefSystemFactory::kLA2000Machine);
		
		for (int i = 0; i < 7; i++)
			pointsLAVstation[i].transform(LAV);

		for (int i = 0; i < 7; i++)
			pointsXYh[i].transform(XYh00);

		// observed height/z-differences
		const TReal instH(0);
		TReal zCCSFrom = pointsCCS[STATION].getCoordinates(k3D).getZ().getMetresValue()+instH;
		TReal zLAVFrom = pointsLAVstation[STATION].getCoordinates(k3D).getZ().getMetresValue()+instH;
		TReal hFrom = pointsXYh[STATION].getCoordinates(TCoordSysFactory::k2DPlusH).getH().getMetresValue()+instH;
		for (int i = 0; i < 7; i++) {
			if (i == STATION) continue;
			TReal hTo = pointsXYh[i].getCoordinates(TCoordSysFactory::k2DPlusH).getH().getMetresValue();
			TReal zLAVTo = pointsLAVstation[i].getCoordinates(k3D).getZ().getMetresValue();
			TReal zCCSTo = pointsCCS[i].getCoordinates(k3D).getZ().getMetresValue();
			
			infiles[kCCS] << STATION+1 <<  "\t" << i+1 << "\t" << zCCSTo - zCCSFrom << "\n";
			infiles[kLAV] << STATION+1 <<  "\t" << i+1 << "\t" << zLAVTo - zLAVFrom << "\n";
			infiles[kXYh] << STATION+1 <<  "\t" << i+1 << "\t" << hTo - hFrom << "\n";
		}
		
		for (int i = 0; i< 3; i++)
			infiles[i] << "*DMES 100\n";

		// observed distances
		for (int i = 0; i < 3; i++) {
			infiles[kCCS] << STATION+1 << "\t" << i+1 << "\t" << 
				pointsCCS[STATION].getCoordinates(k3D).dist(pointsCCS[i].getCoordinates(k3D)).getMetresValue() << "\n";
			infiles[kLAV] << STATION+1 << "\t" << i+1 << "\t" << 
				pointsCCS[STATION].getCoordinates(k3D).dist(pointsCCS[i].getCoordinates(k3D)).getMetresValue() << "\n";
			infiles[kXYh] << STATION+1 << "\t" << i+1 << "\t" << 
				pointsCCS[STATION].getCoordinates(k3D).dist(pointsCCS[i].getCoordinates(k3D)).getMetresValue() << "\n";
		}
		
		for (int i = 0; i< 3; i++)
			infiles[i] << "*FIN\n";		
	}

	
}
