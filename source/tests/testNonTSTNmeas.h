#ifndef _LGC_TESTS_NONTSTN_H
#define _LGC_TESTS_NONTSTN_H

/*Non TSTN and non CAM tests, like levelling, DLEV, ECHO, DVER, etc.*/
namespace TestNonTSTN {
//----------------------------- DSPT --------------------------------//
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
//----------------------------- DLEV --------------------------------//
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
                     "STN 150 \n"
                     "PTCAL1 30 \n"
                     "PTCAL2 -650\n"
                     "PT 100 \n"
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

		"*FRAME f1 0.0 0.0 150 0.0 0.0 0.0 1 TZ\n" // Correct values : TZ = 100 
		"*CALA\n"
			"STN  0 0 0\n"
			"REF  0 20 50\n"

			"PT2  50 20 70\n" 
			"PT3  90 60 80\n" 
			"PT4  0 40 50\n" 
		"*ENDFRAME\n"

		"*DLEV LEV1\n" 
			"STN 0 DHOR 52.497618993626749 DSE 0.8\n"
			"REF -50 DHOR 42.3792402008341 DSE 0.8\n"
			"PTCAL1 -100 DHOR 89.196412483911 DSE 0.8\n"
			"PTCAL2 -200 DHOR 68.96375859826666 DSE 0.8\n"
			"PT 50 DHOR 42.379240200834 DSE 0.8\n"
		;





/////////////////////////OFFSET MEASUREMENTS/////////////////////////////////////////////////////////////////////////////////////////////////////////
	char const *const echo_1 = 
		"*TITR\n"
		"ECHO simple\n"
		"*OLOC\n"
		"*FAUT\n"
		"*PUNC\n"

		"*INSTR\n"
		"*SCALE SCL1 0.8 0.1 0.0 0.0 0.0\n"

		"*CALA\n"
			"REF1  100 0 0\n"
			"REF2  120 20 0\n"
			"REF3  150 50 0\n"
			"REF4  130 80 0\n"
			"REF5  100 100 0\n"


		"*ECHO SCL1\n"
			"REF1 -100 \n"
			"REF2 -120 \n"
			"REF3 -150\n"
			"REF4 -130 \n"
			"REF5 -100\n"

		;


	char const *const echo_1_plus_on_left_side = 
		"*TITR\n"
		"ECHO simple\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*SCALE SCL1 0.8 0.1 0.0 0.0 0.0\n"

		"*CALA\n"
			"REF1  -100 0 0\n"
			"REF2  -120 20 0\n"
			"REF3  -150 50 0\n"
			"REF4  -130 80 0\n"
			"REF5  -100 100 0\n"

		/*Test to determine the distance of the reference point from the plane changes (the plane is shifter for 10 in the direction of x axis)*/
		"*ECHO SCL1\n"
			"REF1 110 \n" //Correct 100
			"REF2 130 \n" // 120 
			"REF3 160\n"  // 150
			"REF4 140 \n" // 130
			"REF5 110\n" // 100

		;

	char const *const echo_2_line_exact = 
		"*TITR\n"
		"ECHO simple\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*SCALE SCL1 0.8 0.1 0.0 0.0 0.0\n"

		"*CALA\n"
			"REF1  100 0 0\n"
			"REF2  120 20 0\n"
			"REF3  150 50 0\n"
			"REF4  170 70 0\n"
			"REF5  190 90 0\n"

		"*ECHO SCL1\n"
			"REF1 -70.7106781187 \n"
			"REF2 -70.7106781187 \n"
			"REF3 -70.7106781187\n"
			"REF4 -70.7106781187 \n"
			"REF5 -70.7106781187\n"
		;

	//----------------------------- ORIE --------------------------------//
	char const *const orie1 =
		"*TITR											  \n"
		"tests ORIE										  \n"
		"*OLOC											  \n"
		"*INSTR											  \n"
		"*POLAR TS1 T1 0.0 0.0 0.0 0					  \n"
		"T1  20.0  60.0  0.5  0.0  0  0.0  0.0  0.0  0.0  \n"
		"*POLAR TS2 T2 0.0 0.0 0.0 0					  \n"
		"T2  5.0  5.0  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0\n"
		"*EDM EDM1 ET1 0.0 0.0 0.0						  \n"
		"ET1  0.5  0.0 0 0.0 0.0 0.0 0 0.0				  \n"
		"*CALA											  \n"
		"ALES2 - 6.60260 - 0.00077 - 0.00021			  \n"
		"ALES3 - 6.60361   3.57921 - 0.00006			  \n"
		"ALES4 - 2.24445   3.57893   0.00003			  \n"
		"AUX1   0.37545   -0.00013   -0.00014			  \n"
		"AUX2   -0.88338   -0.00024   -0.00013			  \n"
		"*POIN											  \n"
		"TU1 - 4.46547   0.11646   0.00051				  \n"
		"TU2 - 4.46393   3.39299   0.00171				  \n"
		"*ORIE  ALES3  TS2								  \n"
		"AUX1   130.16873								  \n"
		"*TSTN ALES3 TS1 IHFIX							  \n"
		"*V0											  \n"
		"*ANGL											  \n"
		"AUX1   130.16932								  \n"
		"ALES4   100.00340								  \n"
		"TU2   105.52484								  \n"
		"TU1   164.78303								  \n"
		"AUX1   130.16943								  \n"
		"*ZEND											  \n"
		"AUX1   101.61927								  \n"
		"ALES4   102.92120								  \n"
		"TU2   105.85580								  \n"
		"TU1   103.11448								  \n"
		"AUX1   101.61364								  \n"
		"*TSTN ALES2 TS1 IHFIX							  \n"
		"*V0											  \n"
		"*ANGL											  \n"
		"AUX2   99.99379								  \n"
		"ALES4   56.22393								  \n"
		"TU2   35.80426									  \n"
		"TU1   96.51091									  \n"
		"AUX2   99.99437								  \n"
		"*ZEND											  \n"
		"AUX2   102.22429								  \n"
		"ALES4   102.24943								  \n"
		"TU2   103.13359								  \n"
		"TU1   105.90161								  \n"
		"AUX2   102.22577								  \n"
		"*DSPT TU2 EDM1									  \n"
		"TU1      3.27666								  \n"
		"*FIN											  \n"


		;

}
#endif