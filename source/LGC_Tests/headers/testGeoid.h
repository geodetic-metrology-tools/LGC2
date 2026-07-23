/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_GEOID
#define _LGC_TESTS_GEOID


/*Test input files which are in global RF (RS2K, LEP, SPHE)*/
namespace TestROOT {

char const *const transform_h2z_rs2k = R"(*TITR
Test H to Z

*RS2K

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
P0                                     2000.0000     2097.79265    433.65921   
Test1                                     63.34159     5271.95242    453.57390 
Test2                                    1764.56580     283.09069    253.56984 
Test3                                    166.52069     2289.92934    453.57414 
*POIN
TestXYZ                                   759.11954     5300.00270    449.86914 

*OBSXYZ
TestXYZ                                   759.11954     5300.00270    2448.94777   0.1 0.1 0.1

*END)";

char const *const transform_h2z_customG = R"(*TITR
Test H to Z

*CUSTOMG C:\Users\bweyer\Downloads\Geoid_OGD\chgeo2004_ETRS.agr

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
P0                                     2000.0000     2097.79265    433.65921   
Test1                                     63.34159     5271.95242    453.57390 
Test2                                    1764.56580     283.09069    253.56984 
Test3                                    166.52069     2289.92934    453.57414 
*POIN
TestXYZ                                   759.11954     5300.00270    449.86914 

*OBSXYZ
TestXYZ                                   759.11954     5300.00270    2448.94777   0.1 0.1 0.1

*END)";

}


	#endif



