/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

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
	char const *const orie_SPHE =
		"*TITR												\n" 
		"Mesures de DMES, ANG.								\n" 
		"*SPHE												\n" 
		"*PREC 7											\n" 
		"*APRI										    	\n" 
		"*INSTR											\n" 
		"*POLAR TS2 T2 0.0 0.0 0.0 0						\n" 
		"T2  5.0  5.0  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0\n" 
		"*EDM EDM1 ET1 0.0 0.0 0.0							\n" 
		"ET1  0.2  0.0 0 0.0 0.0 0.0 0 0.0					\n" 
		"*CALA												\n" 
		"A_1   2000.00000   2100.00001   449.9992104		\n" 
		"*VXY												\n" 
		"B_1   2029.38926   2140.45085   449.9994201		\n" 
		"C_1   2058.77853   2180.90170   450.0000212		\n" 
		"D_1   2088.16779   2221.35255   450.0010136		\n" 
		"E_1   2117.55705   2261.80340   450.0023975		\n" 
		"F_1   2146.94631   2302.25425   450.0041728		\n" 
		"G_1   2176.33558   2342.70510   450.0063395		\n" 
		"H_1   2205.72484   2383.15595   450.0088976		\n" 
		"I_1   2235.11410   2423.60680   450.0118471		\n" 
		"*DSPT   B_1  EDM1									\n" 
		"A_1   50.00001									\n" 
		"C_1   50.00003									\n" 
		"*DSPT   D_1  EDM1									\n" 
		"C_1   50.00002									\n" 
		"E_1   50.00000									\n" 
		"*DSPT   F_1  EDM1									\n" 
		"E_1   50.00000									\n" 
		"G_1   50.00001									\n" 
		"*DSPT   H_1  EDM1									\n" 
		"G_1   50.00000									\n" 
		"I_1   50.00004									\n" 
		"*ORIE  B_1  TS2   5								\n" 
		"A_1   277.77864									\n" 
		"C_1   77.77865									\n" 
		"*ORIE  C_1  TS2   5								\n" 
		"B_1   277.77867									\n" 
		"D_1   77.77864									\n" 
		"*ORIE  D_1  TS2   5								\n" 
		"C_1   277.77868									\n" 
		"E_1   77.77864									\n" 
		"*ORIE  E_1  TS2   5								\n" 
		"D_1   277.77864									\n" 
		"F_1   77.77869									\n" 
		"*ORIE  F_1  TS2   5								\n" 
		"E_1   277.77864									\n" 
		"G_1   77.77858									\n" 
		"*ORIE  G_1  TS2   5								\n" 
		"F_1   277.77864									\n" 
		"H_1   77.77864									\n" 
		"*ORIE  H_1  TS2   5								\n" 
		"G_1   277.77864									\n" 
		"I_1   77.77861									\n" 
		"*ORIE  I_1  TS2   5								\n" 
		"H_1   277.77864									\n" 
		"*END												\n" 

		;

	char const *const orie_LEP =
		"*TITR												\n"
		"Mesures de DMES, ANG.								\n"
		"*LEP												\n"
		"*PREC 7											\n"
		"*APRI										    	\n"
		"*INSTR											\n"
		"*POLAR TS2 T2 0.0 0.0 0.0 0						\n"
		"T2  5.0  5.0  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0\n"
		"*EDM EDM1 ET1 0.0 0.0 0.0							\n"
		"ET1  0.2  0.0 0 0.0 0.0 0.0 0 0.0					\n"
		"*CALA												\n"
		"A_1   2000.00000   2100.00001   449.9992104		\n"
		"*VXY												\n"
		"B_1   2029.38926   2140.45085   449.9994201		\n"
		"C_1   2058.77853   2180.90170   450.0000212		\n"
		"D_1   2088.16779   2221.35255   450.0010136		\n"
		"E_1   2117.55705   2261.80340   450.0023975		\n"
		"F_1   2146.94631   2302.25425   450.0041728		\n"
		"G_1   2176.33558   2342.70510   450.0063395		\n"
		"H_1   2205.72484   2383.15595   450.0088976		\n"
		"I_1   2235.11410   2423.60680   450.0118471		\n"
		"*DSPT   B_1  EDM1									\n"
		"A_1   50.00001									\n"
		"C_1   50.00003									\n"
		"*DSPT   D_1  EDM1									\n"
		"C_1   50.00002									\n"
		"E_1   50.00000									\n"
		"*DSPT   F_1  EDM1									\n"
		"E_1   50.00000									\n"
		"G_1   50.00001									\n"
		"*DSPT   H_1  EDM1									\n"
		"G_1   50.00000									\n"
		"I_1   50.00004									\n"
		"*ORIE  B_1  TS2   5								\n"
		"A_1   277.77864									\n"
		"C_1   77.77865									\n"
		"*ORIE  C_1  TS2   5								\n"
		"B_1   277.77867									\n"
		"D_1   77.77864									\n"
		"*ORIE  D_1  TS2   5								\n"
		"C_1   277.77868									\n"
		"E_1   77.77864									\n"
		"*ORIE  E_1  TS2   5								\n"
		"D_1   277.77864									\n"
		"F_1   77.77869									\n"
		"*ORIE  F_1  TS2   5								\n"
		"E_1   277.77864									\n"
		"G_1   77.77858									\n"
		"*ORIE  G_1  TS2   5								\n"
		"F_1   277.77864									\n"
		"H_1   77.77864									\n"
		"*ORIE  H_1  TS2   5								\n"
		"G_1   277.77864									\n"
		"I_1   77.77861									\n"
		"*ORIE  I_1  TS2   5								\n"
		"H_1   277.77864									\n"
		"*END												\n"

		;


	char const *const orie_RS2K =
		"*TITR												\n"
		"Mesures de DMES, ANG.								\n"
		"*RS2K												\n"
		"*PREC 7											\n"
		"*APRI										    	\n"
		"*INSTR											\n"
		"*POLAR TS2 T2 0.0 0.0 0.0 0						\n"
		"T2  5.0  5.0  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0\n"
		"*EDM EDM1 ET1 0.0 0.0 0.0							\n"
		"ET1  0.2  0.0 0 0.0 0.0 0.0 0 0.0					\n"
		"*CALA												\n"
		"A_1   2000.00000   2100.00001   449.9992104		\n"
		"*VXY												\n"
		"B_1   2029.38926   2140.45085   449.9994201		\n"
		"C_1   2058.77853   2180.90170   450.0000212		\n"
		"D_1   2088.16779   2221.35255   450.0010136		\n"
		"E_1   2117.55705   2261.80340   450.0023975		\n"
		"F_1   2146.94631   2302.25425   450.0041728		\n"
		"G_1   2176.33558   2342.70510   450.0063395		\n"
		"H_1   2205.72484   2383.15595   450.0088976		\n"
		"I_1   2235.11410   2423.60680   450.0118471		\n"
		"*DSPT   B_1  EDM1									\n"
		"A_1   50.00001									\n"
		"C_1   50.00003									\n"
		"*DSPT   D_1  EDM1									\n"
		"C_1   50.00002									\n"
		"E_1   50.00000									\n"
		"*DSPT   F_1  EDM1									\n"
		"E_1   50.00000									\n"
		"G_1   50.00001									\n"
		"*DSPT   H_1  EDM1									\n"
		"G_1   50.00000									\n"
		"I_1   50.00004									\n"
		"*ORIE  B_1  TS2   5								\n"
		"A_1   277.77864									\n"
		"C_1   77.77865									\n"
		"*ORIE  C_1  TS2   5								\n"
		"B_1   277.77867									\n"
		"D_1   77.77864									\n"
		"*ORIE  D_1  TS2   5								\n"
		"C_1   277.77868									\n"
		"E_1   77.77864									\n"
		"*ORIE  E_1  TS2   5								\n"
		"D_1   277.77864									\n"
		"F_1   77.77869									\n"
		"*ORIE  F_1  TS2   5								\n"
		"E_1   277.77864									\n"
		"G_1   77.77858									\n"
		"*ORIE  G_1  TS2   5								\n"
		"F_1   277.77864									\n"
		"H_1   77.77864									\n"
		"*ORIE  H_1  TS2   5								\n"
		"G_1   277.77864									\n"
		"I_1   77.77861									\n"
		"*ORIE  I_1  TS2   5								\n"
		"H_1   277.77864									\n"
		"*END												\n"

		;

	char const *const orie_OLOC =
		"*TITR												\n"
		"Mesures de DMES, ANG.								\n"
		"*OLOC												\n"
		"*PREC 7											\n"
		"*APRI										    	\n"
		"*INSTR											\n"
		"*POLAR TS2 T2 0.0 0.0 0.0 0						\n"
		"T2  5.0  5.0  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0\n"
		"*EDM EDM1 ET1 0.0 0.0 0.0							\n"
		"ET1  0.2  0.0 0 0.0 0.0 0.0 0 0.0					\n"
		"*CALA												\n"
		"A_1   2000.00000   2100.00001   449.9992104		\n"
		"*VXY												\n"
		"B_1   2029.38926   2140.45085   449.9994201		\n"
		"C_1   2058.77853   2180.90170   450.0000212		\n"
		"D_1   2088.16779   2221.35255   450.0010136		\n"
		"E_1   2117.55705   2261.80340   450.0023975		\n"
		"F_1   2146.94631   2302.25425   450.0041728		\n"
		"G_1   2176.33558   2342.70510   450.0063395		\n"
		"H_1   2205.72484   2383.15595   450.0088976		\n"
		"I_1   2235.11410   2423.60680   450.0118471		\n"
		"*DSPT   B_1  EDM1									\n"
		"A_1   50.00001									\n"
		"C_1   50.00003									\n"
		"*DSPT   D_1  EDM1									\n"
		"C_1   50.00002									\n"
		"E_1   50.00000									\n"
		"*DSPT   F_1  EDM1									\n"
		"E_1   50.00000									\n"
		"G_1   50.00001									\n"
		"*DSPT   H_1  EDM1									\n"
		"G_1   50.00000									\n"
		"I_1   50.00004									\n"
		"*ORIE  B_1  TS2   5								\n"
		"A_1   277.77864									\n"
		"C_1   77.77865									\n"
		"*ORIE  C_1  TS2   5								\n"
		"B_1   277.77867									\n"
		"D_1   77.77864									\n"
		"*ORIE  D_1  TS2   5								\n"
		"C_1   277.77868									\n"
		"E_1   77.77864									\n"
		"*ORIE  E_1  TS2   5								\n"
		"D_1   277.77864									\n"
		"F_1   77.77869									\n"
		"*ORIE  F_1  TS2   5								\n"
		"E_1   277.77864									\n"
		"G_1   77.77858									\n"
		"*ORIE  G_1  TS2   5								\n"
		"F_1   277.77864									\n"
		"H_1   77.77864									\n"
		"*ORIE  H_1  TS2   5								\n"
		"G_1   277.77864									\n"
		"I_1   77.77861									\n"
		"*ORIE  I_1  TS2   5								\n"
		"H_1   277.77864									\n"
		"*END												\n"

		;
	
	char const *const OBSXYZ_subframe =

		"*TITR\n"
		"Test for correct OBSXYZ handling of measurements in rotated subframes\n"
		"see SUS-2095\n"
		"*OLOC\n"
		"*CONSI\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*POIN\n"
		"Zero 0 0 0 \n"
		"*OBSXYZ\n"
		"Zero 0 0.001 0  0.1 1 0.1\n"
		"*FRAME Rot_Z 0 0 0 0 0 100 1\n"
		"*OBSXYZ\n"
		"Zero	0.001 0 0 1 0.1 0.1 \n"
		"% this corresponds to an *OBSXYZ measurement\n"
		"% Zero 0 -0.001 0 0.1 1 0.1 \n"
		"% defined in the root frame (symmetric negative of the other OBSXYZ, considering the 100 gon RZ rotation)\n"
		"% so the estimation should be (0,0,0)\n"
		"*ENDFRAME\n"
		"*END\n";

	char const *const OBSXYZ_weights =

		"*TITR\n"
		"OBSXYZ weights handling\n"
		"*OLOC\n"
		"*INSTR\n"
		"*CALA\n"
		"UX  1 0 0\n"
		"UY  0 1 0\n"
		"UZ  0 0 1\n"
		"*FRAME TESTFRAME 0 0 0 0 0 0 1 RX RY RZ \n"
		"*OBSXYZ\n"
		"UX  0 -1 0  1  1  1\n"
		"UY  1 1000 0  1  1000000  1\n"
		"UZ  0 0 1  1  1  1\n"
		"*ENDFRAME\n"
		"*END\n";

	}
#endif
