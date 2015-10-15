#ifndef _LGC_TESTS_MIXOBS_H
#define _LGC_TESTS_MIXOBS_H

/*Mixing TSTN observation types*/
namespace MixObs {
	/* reference measurement, with only a tstn*/
	char const *const ANGL_ZEND_DIST = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*OLOC\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T2 0.0 0.0 0.0 0\n"
	"T2  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  0 0 0\n"
		"P1  0 100 100\n"			
		"P3  100 200 200\n"		
		"P4  0 200 200\n"

	"*POIN\n"
		"P2 200.005 150.056 100.1\n" /* Correct values: 100 100 100*/
		


	"*TSTN STN TS1 IHFIX\n"
		"*V0\n"
			"*ANGL\n"
				"P1  0.04506\n"
				"P2 49.99911\n"
				"P3 29.51324\n"
				"P4  0.14677\n"
			"*ZEND\n"
				"P1 49.99345\n"
				"P2 60.81738\n"
				"P3 53.53918\n"
				"P4 50.00874\n"
			"*DIST\n"
				"P1 141.66117\n"
				"P2 173.01743\n"
				"P3 299.90492\n"
				"P4 282.72046\n"
	;

	/*TSTN ANGL and ZEND, we replace DIST by DHOR*/
	char const *const ANGL_ZEND_DHOR = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*OLOC\n"
	"*FAUT\n"
	"*PREC 7\n"
	"*INSTR\n"
	"*POLAR TS1 T2 0.0 0.0 0.0 0\n"
	"T2  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  0 0 0\n"
		"P1  0 100 100\n"			
		"P3  100 200 200\n"		
		"P4  0 200 200\n"

	"*POIN\n"
		"P2 200.005 150.056 100.1\n" /* Correct values: 100 100 100*/
		//"P4  0 200 200\n"


	"*TSTN STN TS1 IHFIX\n"
		"*V0\n"
			"*ANGL\n"
				"P1  0.04506\n"
				"P2 49.99911\n"
				"P3 29.51324\n"
				"P4  0.14677\n"
			"*ZEND\n"
				"P1 49.99746\n"
				"P2 60.82070\n"
				"P3 53.55021\n"
				"P4 49.99884\n"
			"*DHOR\n"
				"P1 100.00001\n"
				"P2 141.42139\n"
				"P3 223.60680\n"
				"P4 199.99999\n"
	;

	/*TSTN ANGL and ZEND, we replace DIST by DSPT*/
	char const *const ANGL_ZEND_DSPT = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*OLOC\n"
	"*FAUT\n"
	"*PREC 7\n"
	"*INSTR\n"
	"*CAMD BCAM1 T1\n"
	"T1  5  5  0.5  0.5 \n"
	"*POLAR TS1 T2 0.0 0.1 0.1 0\n"
	"T2  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"
	"*EDM ED1 T3 0.0 0.001 0.001\n"
	"T3 0.5 0.005 0 0 0.001 0.001 0 0.001\n"
	"*LEVEL LEV1 DLS1 0 0.0\n" 
	"DLS1 0.8 0.1 0.0 0.0 0 0.2\n"

	"*CALA\n"
		"STN  0 0 0\n"
		"STN2 100 0 0\n"
		"STN3 100 300 0\n"

		"P1  0 100 100\n"			
		"P3  100 200 200\n"		
		"P4  0 200 200\n"

	"*POIN\n"
		"P2 200.005 150.056 100.1\n" /* Correct values: 100 100 100*/
		//"P4  0 200 200\n"


	"*TSTN STN TS1 IHFIX\n"
		"*V0\n"
			"*ANGL\n"
				"P1 399.99974\n"
				"P2  49.99771\n"
				"P3  29.51673\n"
				"P4 399.99998\n"
			"*ZEND\n"
				"P1 49.99746\n"
				"P2 60.82070\n"
				"P3 53.55021\n"
				"P4 49.99884\n"

	"*DSPT STN ED1\n"
		"P1 141.49784\n"
		"P2 173.09161\n"
		"P3 299.90877\n"
		"P4 282.63717\n"
	;

	char const *const ANGL_DHOR_DLEV = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*OLOC\n"
	"*FAUT\n"
	"*PREC 7\n"
	"*INSTR\n"
	"*CAMD BCAM1 T1\n"
	"T1  5  5  0.5  0.5 \n"
	"*POLAR TS1 T2 0.0 0.1 0.1 0\n"
	"T2  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"
	"*EDM ED1 T3 0.0 0.001 0.001\n"
	"T3 0.5 0.005 0 0 0.0 0.0 0 0.0\n"
	"*LEVEL LEV1 DLS1 0 0.0\n" 
	"DLS1 0.0000001 0.0 0.0 0.0 0 0.1\n"

	"*CALA\n"
		"STN  0 0 0\n"
		"P1  0 100 100\n"			
		"P3  100 200 200\n"		
		"P4  0 200 200\n"

	"*POIN\n"
		"P2 200.005 150.056 100.1\n" /* Correct values: 100 100 100*/
		//"P4  0 200 200\n"


	"*TSTN STN TS1 IHFIX\n"
		"*V0\n"
			"*ANGL\n"
				"P1 399.99974\n"
				"P2  49.99977\n"
				"P3  29.51672\n"
				"P4 399.99998\n"
			"*DHOR\n"
				"P1  99.99949\n"
				"P2 141.42191\n"
				"P3 223.60794\n"
				"P4 199.99961\n"
	"*DLEV LEV1 RefPt STN\n"
		"P1 -100.00027\n"
		"P2  -99.99967\n"
		"P3 -199.99985\n"
		"P4 -199.99954\n"
	;
}
#endif