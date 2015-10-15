#ifndef _LGC_TESTS_NONTSTN_H
#define _LGC_TESTS_NONTSTN_H


namespace TestNonTSTN {

	char const *const dspt_1 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*EDM EDM1 ET1 0.0 0.1 0.1\n"
		"ET1  0.7  0.0 0 0.0 0.05 0.1 0 0.1\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"STN2  0 0 200\n"

		"*VZ\n"
			"PT  0 0 50\n" /*Correct value is 0 0 100*/   /*If greater then 200 or smaller then 0 a failure case is reached, when target point has the same coordinates as the station one*/

		"*DSPT STN EDM1\n"
			"PT 100\n"
		"*DSPT STN2 EDM1\n"
			"PT 100\n"	
			;

	/*Reference point given*/
	char const *const dlev_1 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*LEVEL LEV1 DLS1 0 0.0\n" 
		"DLS1 0.8 0.1 0.0 0.0 0 0.2\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 0 200\n"
			"PTCAL1  200 10 130\n"
			"PTCAL2  250 10 800\n"
			/*"PTCAL1  0 0 130\n"
			"PTCAL2  0 0 800\n"*/

		"*VZ\n"
			"PT  0 0 20\n" /* Correct solution 0 0 50*/ 

		"*DLEV LEV1 RefPt REF\n" //Calculated distance of the reference point is 100m, measurement of a plane
			"STN 100 \n"
			"PTCAL1 -30.000000001 \n"
			"PTCAL2 -699.99999999 \n"
			"PT 50 \n"
		;

	/*Reference point given*/
	char const *const dlev_1_RS2K = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*LEVEL LEV1 DLS1 0 0.0\n" 
		"DLS1 0.8 0.1 0.0 0.0 0 0.2\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 0 200\n"
			"PTCAL1  200 10 130\n"
			"PTCAL2  250 10 800\n"
			/*"PTCAL1  0 0 130\n"
			"PTCAL2  0 0 800\n"*/

		"*VZ\n"
			"PT  0 0 20\n" /* Correct solution 0 0 50*/ 

		"*DLEV LEV1 RefPt REF\n" //Calculated distance of the reference point is 100m, measurement of a plane
			"STN 100 \n"
			"PTCAL1 -30.000000001 \n"
			"PTCAL2 -699.99999999\n"
			"PT 50 \n"
		;

	/*REFERENCE POINT NOT PROVIDED, DHOR GIVEN AND X,Y coordinates of the implicitly introduced REFPT well determined*/
	char const *const dlev_2 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*LEVEL LEV1 DLS1 0 0.0\n" 
		"DLS1 0.8 0.1 0.0 0.0 0 0.2\n"
		"*EDM EDM1 ET1 0.0 0.1 0.1\n"
		"ET1  0.7  0.0 0 0.0 0.05 0.1 0 0.1\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 20 50\n"
			"PTCAL1  100 100 200\n"
			"PTCAL2  100 0 300\n"

		"*VZ\n"
			"PT  0 20 20\n" /* Correct solution 0 20 50*/ 

		"*DLEV LEV1\n" //Calculated distance of the reference point from the plane should be 100 m
			"STN 100 DHOR 52.497618993626749 DSE 0.8\n"
			"REF 50 DHOR 42.3792402008341 DSE 0.8\n"
			"PTCAL1 -100 DHOR 89.196412483911 DSE 0.8\n"
			"PTCAL2 -200 DHOR 68.96375859826666 DSE 0.8\n"
			"PT 50 DHOR 42.379240200834 DSE 0.8\n"
		;

	/*This test needs to be adapted in order to meet the fact that RS2K is used */
	char const *const dlev_2_RS2K = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*LEVEL LEV1 DLS1 0 0.0\n" 
		"DLS1 0.8 0.1 0.0 0.0 0 0.2\n"
		"*EDM EDM1 ET1 0.0 0.1 0.1\n"
		"ET1  0.7  0.0 0 0.0 0.05 0.1 0 0.1\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 20 50\n"
			"PTCAL1  100 100 200\n"
			"PTCAL2  100 0 300\n"

		"*VZ\n"
			"PT  0 20 20\n" /* Correct solution 0 20 50*/ 

		"*DLEV LEV1\n" //Calculated distance of the reference point from the plane should be 100 m
			"STN 100 DHOR 52.497618993626749 DSE 0.8\n"
			"REF 50 DHOR 42.3792402008341 DSE 0.8\n"
			"PTCAL1 -100 DHOR 89.196412483911 DSE 0.8\n"
			"PTCAL2 -200 DHOR 68.96375859826666 DSE 0.8\n"
			"PT 50 DHOR 42.379240200834 DSE 0.8\n"
		;

	/*same as dlev_2, but some levelling stuff points defined in LOR*/
	char const *const dlev_3 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*LEVEL LEV1 DLS1 0 0.0\n" 
		"DLS1 0.8 0.1 0.0 0.0 0 0.2\n"

		"*CALA\n"
			"PTCAL1  100 100 200\n"
			"PTCAL2  100 0 300\n"

		"*VZ\n"
			"PT  0 20 20\n" /* Correct solution 0 20 50*/ 

		"*FRAME f1 0.0 0.0 100 0.0 0.0 0.0 1 TZ\n" // Correct values : TZ = 100 
		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 20 50\n"

			"PT2  50 20 70\n" 
			"PT3  90 60 80\n" 
			"PT4  0 40 50\n" 
		"*ENDFRAME\n"

		"*DLEV LEV1\n" //Calculated distance of the reference point from the plane should be 100 m
			"STN 100 DHOR 52.497618993626749 DSE 0.8\n"
			"REF 50 DHOR 42.3792402008341 DSE 0.8\n"
			"PTCAL1 -100 DHOR 89.196412483911 DSE 0.8\n"
			"PTCAL2 -200 DHOR 68.96375859826666 DSE 0.8\n"
			"PT 50 DHOR 42.379240200834 DSE 0.8\n"
		;

	    //DVER in OLOC
		char const *const dver_oloc = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*EDM EDM1 ET1 0.0 0.1 0.1\n"
		"ET1  0.7  0.0 0 0.0 0.05 0.1 0 0.1\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 10 200\n"

		"*VZ\n"
			"PT  15 0 20\n" /* Correct solution 20 0 50*/ 

		"*DVER \n"
			"STN REF 200 \n"
			"STN PT 50 \n"
			"REF PT -150\n"
			"PT REF  150 \n"
/*
		"*DSPT STN EDM1\n"
			"REF 200.24984394500785727697212 \n"
			"PT 53.851648071345 \n"*/
		;

		 //DVER in RS2K, works OK
		char const *const dver_rs2k = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 10 200\n"

		"*VZ\n"
			"PT  0 20 20\n" /* Correct solution 0 20 50*/ 

		"*DVER \n"
			"STN REF 200 \n"
			"STN PT 50 \n"
			"REF PT -150\n"
			"PT REF  150 \n"
		;

		 //DVER in RS2K + local frame fixed , works, the Z value is the value for point in ROOT which is (X Y H) = (0 0 0), works OK
		char const *const dver_rs2k_lor = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

	    "*FRAME f1 -100.0 0.0 0 0.0 0.0 0.0 1 \n" // Correct values : TX = 100
			"*CALA\n"
				"STN  100 0 1999.3459262518684\n" 
				"REF  100 10 2199.3492402545812\n"
		"*ENDFRAME \n"

		"*VZ\n"
			"PT  0 20 20\n" /* Correct solution 0 20 50*/ 

		"*DVER \n"
			"STN REF 200 \n"
			"STN PT 50 \n"
			"REF PT -150\n"
			"PT REF  150 \n"
		;

		 //DVER in RS2K + local frame free
		char const *const dver_rs2k_lor_free = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*CALA\n"
			"REF  0 10 200\n"

	    "*FRAME f1 150.0 0.0 0 0.0 0.0 0.0 1 TX\n" // Correct values : TX = 10
			"*CALA\n"
				"STN  0 0 1999.3459262518684\n"  /*For (-10 0 0 in ROOT) apparently with this value it works: 1999.3428429860537 CORRECT for (0 0 0 in ROOT): 1999.3459262518684*/
				//"REF  0 10 2199.3461571484468\n" /*2199.3461571484468   CORRECT:    2199.3492402545812*/
				"NNN  200 10 0\n"
				"NNN2  100 40 2199.3492402545812\n"
		"*ENDFRAME \n"

		"*VZ\n"
			"PT  0 20 20\n" /* Correct solution 0 20 50*/ 

		"*DVER \n"
			"STN REF 200 \n"
			"STN PT 50 \n"
			"REF PT -150\n"
			"PT REF  150 \n"
		;
}
#endif