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

	char const *const dspt_id = 
		"*TITR\n"
		"Reading ID for DSPT observation.\n"
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
			"PT 100 ID EDM1_STN_PT\n"
		"*DSPT STN2 EDM1\n"
			"PT 100 ID EDM1_STN2_PT\n"	
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
		
	char const *const dlev_id = 
		"*TITR\n"
		"Testing ID for DLEV observation.\n"
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

		"*VZ\n"
			"PT  0 0 20\n" /* Correct solution 0 0 50*/ 

		"*DLEV LEV1 RefPt REF\n" //Calculated distance of the reference point is 100m, measurement of a plane
			"STN 100 ID LEV1_STN \n"
			"PTCAL1 -30.000000001 ID LEV1_PTCAL1 \n"
			"PTCAL2 -699.99999999 ID LEV1_PTCAL2 \n"
			"PT 50 ID LEV1_PT \n"
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
		
		/*Test the reading of corrections and target heights*/
		char const *const dlev_4 = R"(*TITR
Test DLEV TH
*OLOC
*INSTR
*LEVEL L1 T1 0 0
T1 0.1 0 0 0 0 0
T2 0.1 0 0.1 0.1 0.2 0.2
*CALA
PT1 0 0 0
PT2 10 10 0
PT3 0 10 0
PT4 10 0 0
REF 5 5 2
*DLEV   L1 RefPt REF
PT1 1  THSE 0.4
PT2 0.5	TH 0.5
PT3 1.2 TH -0.2	
PT4 0.7 TRGT T2
*END
)";

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

	char const *const echo_id = 
		"*TITR\n"
		"Testing ID for ECHO observation.\n"
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
			"REF1 -100 ID SCL1_REF1\n"
			"REF2 -120 ID SCL1_REF1\n"
			"REF3 -150 ID SCL1_REF3\n"
			"REF4 -130 ID SCL1_REF4 \n"
			"REF5 -100 ID SCL1_REF5\n"

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

	/*Testing the ECHO SCALE keywords */
	char const *const ECHO_SCALE_READ = R"(*TITR 
test readers ECHO SCALE
*OLOC 
*INSTR 
*SCALE RS_1   0.8 0.1 0 0 0
*SCALE RS_2   0.9 0.2 0 0 0
*CALA
P1   10.00000   10.00000   1.00000   
P2   11.00000   9.00000   2.00000   
P3   12.00000   13.00000   10.00000   
*ECHO   RS_1
P1   0.46232   
P2   -0.92455   SCALE RS_2
P3   0.46220
*END 
)";

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

	/*Testing the ORIE TRGT keywords */
	char const *const ORIE_TRGT_READ = R"(*TITR 
test readers ORIE TRGT
*OLOC 
*INSTR 
*POLAR AT   CCR   0.1898   0   0   0   
CCR   3   3   0.02   6   0   0   0   0   0   0  
CCR2   4   4   0.04   3   0   0   0   0   0   0  
*CALA
P0   0.00000   0.00000   0.00000   
P1   10.00000   10.00000   1.00000 
*ORIE   P0   AT   TRGT CCR2
P1   50.00030 
P1   50.00030 TRGT CCR
P1   50.00030 
*ORIE   P0   AT   
P1   50.00030 
P1   50.00030 TRGT CCR2
P1   50.00030 
*END 
)";

	char const *const OBSXYZ_subframe = R"(*TITR
Test for correct OBSXYZ handling of measurements in rotated subframes
see SUS-2095
*OLOC
*CONSI
*PREC 7
*INSTR
*POIN
Zero 0 0 0 
*OBSXYZ
Zero 0 0.001 0  0.1 1 0.1
*FRAME Rot_Z 0 0 0 0 0 100 1
*OBSXYZ
Zero	0.001 0 0 1 0.1 0.1 
% this corresponds to an *OBSXYZ measurement
% Zero 0 -0.001 0 0.1 1 0.1 
% defined in the root frame (symmetric negative of the other OBSXYZ, considering the 100 gon RZ rotation)
% so the estimation should be (0,0,0)
*ENDFRAME
*END
)";

	char const *const OBSXYZ_weights = R"(*TITR
OBSXYZ weights handling
*OLOC
*INSTR
*CALA
UX  1 0 0
UY  0 1 0
UZ  0 0 1
*FRAME TESTFRAME 0 0 0 0 0 0 1 RX RY RZ 
*OBSXYZ
UX  0 -1 0  1  1  1
UY  1 1000 0  1  1000000  1
UZ  0 0 1  1  1  1
*ENDFRAME
*END
)";

	char const *const OBSXYZ_sigma2root = R"(*TITR
Test Sigma2Root function through subframes
Points Test1 and Test2 have the same observations, but Test 2 is a cala in a subframe
As the points have the same observations when viewed from the root frame they should hav ethe same covariances
*OLOC
*APRI
*PREC 7
*CONSI
*INSTR
*POIN
Test1 0 0 0
*OBSXYZ
Test1 1 2 3 4 5 6
Test2 1 2 3 4 5 6
*FRAME Testframe1 0 0 0 30 0 0 1 TX 
*FRAME Testframe2 0 0 0 0 40 0 1 TY
*FRAME Testframe3 0 0 0 0 0 50 1 TZ
*CALA
Test2 3 2 1
*ENDFRAME
*ENDFRAME
*ENDFRAME
*FRAME RotZ200 0 0 0 0 0 200 1 
*CALA
Zero200 0 0 0
*ENDFRAME
*FRAME RotZ100 0 0 0 0 0 100 1 
*CALA
Zero100 0 0 0
*ENDFRAME
*END
)";

		char const *const orie_id =
		"*TITR												\n"
		"Testing ID for ORIE observation.					\n"
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
		"*VXY \n "
		"B_1   2029.38926   2140.45085   449.9994201	\n"
		"C_1   2058.77853   2180.90170   450.0000212	\n"
		"D_1   2088.16779   2221.35255   450.0010136	\n"
		"E_1   2117.55705   2261.80340   450.0023975	\n"
		"*DSPT   B_1  EDM1		\n"
		"A_1   50.00001	\n"
		"C_1   50.00003	\n"
		"*DSPT   D_1  EDM1		\n"
		"C_1   50.00002 \n"
		"E_1   50.00000	\n"
		"*ORIE  B_1  TS2   5	\n"
		"A_1   277.77864 ID ORIE_B_1-A_1		\n"
		"C_1   77.77865	ID ORIE_B_1-C_1	\n"
		"*ORIE  C_1  TS2   5	\n"
		"B_1   277.77867 ID ORIE_C_1-B_1		\n"
		"D_1   77.77864	 ID ORIE_C_1-D_1       \n"
		"*END					\n"

		;

		//----------------------------- RADI --------------------------------//
		// the inputfile has a extreme dependency on the provisional coordinates because the deprecated LIBR chooses constraints that doe not make sense
		// just change some of the xy provisional coordinates to see the effect
		char const *const radi_id =
		"*TITR                                                                                                                           \n"
		"Test RADI with observation ID                                                                                                   \n"
		"*RS2K                                                                                                                           \n"
		"%*LIBR                                                                                                                           \n"
		"*CONSI LIBR                                                                                                                           \n"
		"*FAUT 0.01  0.1                                                                                                                 \n"
		"*HIST                                                                                                                           \n"
		"*PUNC OUT1                                                                                                                      \n"
		"*INSTR                                                                                                                          \n"
		"*EDM ME5000.357015   KE.3   0   0   0                                                                                           \n"
		"KE.3   0.2   0.2   0   0   0   0   0   0                                                                                        \n"
		"KE.3b   0.2   0.2   0   0   0   0   0   0                                                                                       \n"
		"KE.3t   0.2   0.2   0   0   0   0   0   0                                                                                       \n"
		"*VXY                                                                                                                            \n"
		"SURFACE.PBASE.2.      478.9000000        1000.0000000      452.3330000   $2.000    185534   coordonnées au 30-JAN-2018 16:05:15 \n"
		"SURFACE.PBASE.3.      528.9000000        1000.0000000      452.3320000   $3.000    185535   coordonnées au 30-JAN-2018 16:05:15 \n"
		"SURFACE.PBASE.4.      578.9000000        1000.0000000      452.3300000   $4.000    185536   coordonnées au 30-JAN-2018 16:05:15 \n"
		"SURFACE.PBASE.5.      628.9000000        1000.0000000      452.3350000   $5.000    185537   coordonnées au 30-JAN-2018 16:05:15 \n"
		"SURFACE.PBASE.7.      728.9000000        1000.0000000      452.3310000   $7.000    185539   coordonnées au 30-JAN-2018 16:05:15 \n"
		"*DSPT   SURFACE.PBASE.2.   ME5000.357015    IH  0.000000    TRGT KE.3    %station n265127                                       \n"
		"SURFACE.PBASE.3.                              49.992280 TH  0.000000   $1093916 -25-JAN-0018                                    \n"
		"SURFACE.PBASE.4.                             100.005580 TH  0.000000   $1093917 -25-JAN-0018                                    \n"
		"SURFACE.PBASE.5.                             150.002230 TH  0.000000   $1093918 -25-JAN-0018                                    \n"
		"*DSPT   SURFACE.PBASE.7.   ME5000.357015    IH  0.000000    TRGT KE.3b    %station n265138	                                     \n"
		"SURFACE.PBASE.2.                             250.002340 TH  0.000000   $1093927 -25-JAN-0018                                    \n"
		"SURFACE.PBASE.3.                             200.010190 TH  0.000000   $1093928 -25-JAN-0018                                    \n"
		"SURFACE.PBASE.4.                             149.996870 TH  0.000000   $1093929 -25-JAN-0018                                    \n"
		"SURFACE.PBASE.5.                             100.000020 TH  0.000000   $1093930 -25-JAN-0018                                    \n"
		"*RADI  0.01                                                                                                                     \n"
		"SURFACE.PBASE.3.   		100.00000    ID RADI.0.01_PBASE.3                                                                     \n"
		"SURFACE.PBASE.4.   		100.00000    ID RADI.0.01_PBASE.4                                                                     \n"
		"SURFACE.PBASE.5.   		100.00000    ID RADI.0.01_PBASE.5                                                                     \n"
		;

	char const *const OBSXYZ_INSTR = R"(*TITR
keyword issue test
*OLOC
*PREC 7
*INSTR
*OBSXYZ 
Point 0 1 0 1 1 1
*FRAME Subframe 0 0 0 100 1 0 1 RY
*CALA
Point 0 0 1
*ENDFRAME
*END
)";

		char const *const RADI_INSTR = R"(*TITR
keyword issue test
*OLOC
*PREC 7
*INSTR
*RADI 0.02
Point 0.01
*FRAME Subframe 0 0 0 100 1 0 1
*CALA
Point 0 0 1
*ENDFRAME
*END
)";

		char const *const DVER_INSTR = R"(*TITR
keyword issue test
*OLOC
*PREC 7
*INSTR
*DVER
Point Point1 0.1
*FRAME Subframe 0 0 0 0 1 0 1
*CALA
Point 0 0 1
*FRAME Subframe1 0 0 0 0 0 0 1 TZ
*CALA
Point1 0 0 1
*ENDFRAME
*ENDFRAME
*END
)";
		}
#endif
