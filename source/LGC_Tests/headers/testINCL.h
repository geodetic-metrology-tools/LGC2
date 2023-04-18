/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_INCL_H
#define _LGC_TESTS_INCL_H


/*Tests of INCL measurements.*/
namespace TestINCL {
	/*----------INCLY----------*/
	/*One INCLY in root, should throw an error*/
	char const* const INCLY_ROOT_1 =
		"*TITR\n"
		"T1\n"
		"*OLOC\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0 \n"
		"*INCLY test\n"
		"PT1	 10\n"
		"*FRAME	RSTR_SPS.MBB.33130	20	0	0	0	0	0	1 RY\n"
		"*CALA\n"
		"PT1 0.01 0.01 0.01\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy
	check done with the DB and the modelisation of the frame from early 2019 using this frame structure
	*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522	-113.494599963348	1
	*FRAME	RSTRI_XT03.BDB.0900	0.00039	-0.00041	0.00012	0.0228	0	399.98986	1
	*FRAME	RSTRV_XT03.BDB.0900	0	0	0	0	-0.000552583822135043	0	1
	*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	-0.0070028174960434	0	1
	*/
	char const* const INCLY_SUBF_1 =
		"*TITR\n"
		"T2\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.E	0	0	0\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.E -0.0070028174960434\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy, Station fixed declared in anoter frame*/
	char const* const INCLY_SUBF_2 =
		"*TITR\n"
		"T3\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.E	1863.68622	2293.21081	437.49248\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.E -0.0070028174960434\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy, Station moving declared in anoter frame*/
	char const* const INCLY_SUBF_3 =
		"*TITR\n"
		"T4\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*POIN\n"
		"BEAM_XT03.BDB.0900.E	4839.56107	6589.57768	437.49248\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*OBSXYZ\n"
		"BEAM_XT03.BDB.0900.E 0 0 0 0.0001 0.0001 0.0001\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.E -0.0070028174960434\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy, Station moving declared in anoter frame, more complex structure*/
	char const* const INCLY_SUBF_4 =
		"*TITR\n"
		"T5\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTB2_XT03.BDB.0900	-1865	-2292	-2433	-0.00	-0.001 113.4	1\n"
		"*POIN\n"
		"BEAM_XT03.BDB.0900.E	0	0	0\n"
		"*ENDFRAME\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*OBSXYZ\n"
		"BEAM_XT03.BDB.0900.E 0 0 0 0.0001 0.0001 0.0001\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.E -0.0070028174960434\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy. marginal changes if station not not far	*/
	char const* const INCLY_SUBF_5 =
		"*TITR\n"
		"T6\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.E	0	0	0\n"
		"BEAM_XT03.BDB.0900.T	0.5	0.5  0.5\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.T -0.0070028174960434\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy. Test INCLY observation ID reading	*/
	char const* const INCLY_SUBF_5_id =
		"*TITR\n"
		"T6\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.E	0	0	0\n"
		"BEAM_XT03.BDB.0900.T	0.5	0.5  0.5\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.T -0.0070028174960434 ID test.BEAM_XT03.BDB.0900.T\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*One INCLY in subframe, point S dummy, Station moving declared in anoter frame, more complex structure. Test OBSXYZ observation ID reading */
	char const* const INCLY_SUBF_4_xyzID =
		"*TITR\n"
		"T5\n"
		"*RS2K\n"
		"*PREC 7\n"
		"*INSTR\n"
		"*INCL test 35 0 0 0 0\n"
		"*FRAME	RSTB_XT03.BDB.0900	1863.6859	2293.21034	2437.48859	0.00128521258824983	0.00177433336363522 -113.494599963348	1\n"
		"*FRAME	RSTB2_XT03.BDB.0900	-1865	-2292	-2433	-0.00	-0.001 113.4	1\n"
		"*POIN\n"
		"BEAM_XT03.BDB.0900.E	0	0	0\n"
		"*ENDFRAME\n"
		"*FRAME	RSTRI_XT03.BDB.0900	0.00039 -0.00041	0.00012	0.0228	0	399.98986	1\n"
		"*FRAME	RSTR_XT03.BDB.0900	0	0	0	0	0	0	1  RY\n"
		"*CALA\n"
		"BEAM_XT03.BDB.0900.S	100	100	100\n"
		"*OBSXYZ\n"
		"BEAM_XT03.BDB.0900.E 0 0 0 0.0001 0.0001 0.0001 ID XYZ.BEAM_XT03.BDB.0900.E\n"
		"*INCLY test\n"
		"BEAM_XT03.BDB.0900.E -0.0070028174960434\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*ENDFRAME\n"
		"*END\n"
		;

	/*Testing the INCLY contributions and the angle and scale partial derivatives with TFreeVector */
	char const *const INCLY_CONTRIB = 
		"*TITR \n"
		"test Contrib Incl \n"
		"*OLOC \n"
		"*INSTR \n"
		"*INCL I   5    0   0  0  0 \n"
		"*CALA \n"
		"PT1 0 1000 100 \n"
		"PT2 0 1000 101 \n"
		"PT3 1 1001 101 \n"
		"*FRAME TEST 500 500 500 10 10 80 0.8 TX TY TZ RX RY RZ SCL \n"
		"*OBSXYZ \n"
		"PT1 1 1001 101 0.1 0.1 0.1 \n"
		"PT2 1 1001 102 0.1 0.1 0.1 \n"
		"PT3 2 1002 102 0.1 0.1 0.1 \n"
		"*INCLY I \n"
		"PT3 0.0000 \n"
		"*FRAME TEST2 20 30 10 0 0 100 1 \n"
		"*INCLY I \n"
		"PT1 0.0000 \n"
		"*ENDFRAME \n"
		"*ENDFRAME \n"
		"*END \n";
	}
#endif
