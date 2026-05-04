/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_DLEV_H
#define _LGC_TESTS_DLEV_H

namespace TestDLEV
{
//-----------------------------------PARSE------------------------------//
char const *const DLEV_PARSE_NOK_1 = R"(*TITR
LEVEL line has too much parameters
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0	0	0
TARG1   0.0176   0   0   0   0   0   0	0	0
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2      
*DLEV   LEVEL1   
PT1		1 DHOR 7.071067812 DSE   1
PT2		1 DHOR 7.071067812 DSE   1     
PT3		1 DHOR 7.071067812  DSE   1   
PT4		1 DHOR 7.071067812  DSE   1 
*END
)";

char const *const DLEV_PARSE_NOK_2 = R"(*TITR
LEVEL line has not enough parameters
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   0   0   
TARG1   0.0176   0   0   0   0   0  0	0	0
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2      
*DLEV   LEVEL1   
PT1		1 DHOR 7.071067812 DSE   1
PT2		1 DHOR 7.071067812 DSE   1     
PT3		1 DHOR 7.071067812  DSE   1   
PT4		1 DHOR 7.071067812  DSE   1 
*END
)";

char const *const DLEV_PARSE_NOK_3 = R"(*TITR
TARGET line has too much parameters
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0
TARG1   0.0176   0   0   0   0   0	0	0 0
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2      
*DLEV   LEVEL1   
PT1		1 DHOR 7.071067812 DSE   1
PT2		1 DHOR 7.071067812 DSE   1     
PT3		1 DHOR 7.071067812  DSE   1   
PT4		1 DHOR 7.071067812  DSE   1 
*END
)";

char const *const DLEV_PARSE_NOK_4 = R"(*TITR
TARGET line has not enough parameters
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0
TARG1   0.0176   0   0   0   0   
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2      
*DLEV   LEVEL1   
PT1		1 DHOR 7.071067812 DSE   1
PT2		1 DHOR 7.071067812 DSE   1     
PT3		1 DHOR 7.071067812  DSE   1   
PT4		1 DHOR 7.071067812  DSE   1 
*END
)";

//-----------------------------------OLOC------------------------------//
char const *const DLEV_OLOC_1 = R"(*TITR
Test OLOC
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0	0
TARG1   0.0176   1   0.2   0.3   0.04   0.03   0   0   0
TARG2   0.02   0   0   0   0   0   0   0   0
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2      
*DLEV   LEVEL1   
PT1		0.5 DHOR 7.071067812 DSE   1 OBSE 3	PPM	4  TH 0.5 THSE 5  ID TEST1
PT2		1 DHOR 7.071067812 DSE   3   ID TEST2
PT3		1 DHOR 7.071067812  DSE   2   TRGT  TARG2
PT4		1 DHOR 7.071067812  DSE   1 
*END
)";

char const *const DLEV_OLOC_2 = R"(*TITR
Test OLOC
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0   
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
ST		5   	5	 0     
*DLEV   LEVEL1 RefPt    ST
PT1		0.5 TH 0.5
PT2		1 
PT3		1 
PT4		1 
*DLEV   LEVEL1  REFPT    ST  IH 2 IHFIX IHSE 0.4
PT1		0.5  TH 0.5
PT2		1 
PT3		1 
PT4		1 
*END
)";

char const *const DLEV_OLOC_3 = R"(*TITR
Test OLOC
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1 
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

char const *const DLEV_OLOC_4 = R"(*TITR
Test OLOC
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1  IHFIX
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

char const *const DLEV_OLOC_5 = R"(*TITR
Test OLOC
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1  IHFIX IH 4 IHSE 0.3
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

char const *const DLEV_OLOC_6 = R"(*TITR
Test OLOC 
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0    0   0   0   
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1  IH 4 IHSE 0.3
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

char const *const DLEV_OLOC_7 = R"(*TITR
Test Reading DHOR Flags and Default target
*OLOC
*PREC 7 
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0	0
TARG1   0.02   0   0   0   0   0   10   20   1
TARG2   0.02   0   0   0   0   0   30   40  -1
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2        
*DLEV   LEVEL1   %RefPt at 15 15
PT1		1 DHOR 6.07106781186548  
PT2		1 DHOR 8.07106781186548  TRGT TARG2
PT3		1 DHOR 7.07106781186548  DSE   50  DHPPM 60 DHDCOR 0
PT4		1 DHOR 7.07106781186548  DSE   50  DHPPM 60 DHDCOR 0	TRGT TARG2
PT1		1 DHOR 7.07106781186548  TRGT TARG2 DSE   50  DHPPM 60 DHDCOR 0
PT2		1 DHOR 8.07106781186548  TRGT TARG2 DSE   50		
PT3		1 DHOR 8.07106781186548  TRGT TARG2 DHPPM 60
PT4		1 DHOR 9.07106781186548  TRGT TARG2 DHDCOR -2
PT1		1 DHOR 7.07106781186548  DSE   50  DHDCOR 0
*END
)";

char const *const DLEV_OLOC_8 = R"(*TITR
Checking contribution with frames
*OLOC
*PREC 7 
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0	0
TARG1   0.02   0   0   0   0   0   10   20   0
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2        
*DLEV   LEVEL1   RefPt ST IHFIX IH 0
PT1		1 DHOR 7.07106781186548  
PT2		1 DHOR 7.07106781186548  
PT3		1 DHOR 7.07106781186548  
PT4		1 DHOR 7.07106781186548  
PT1		1 DHOR 7.07106781186548  
PT2		1 DHOR 7.07106781186548  
PT3		1 DHOR 7.07106781186548  
PT4		1 DHOR 7.07106781186548  
PT5		1 DHOR 7.07106781186548  
*FRAME FORTARGET 0 0 0 0 0 0 1 TZ
*CALA
PT5	10 10 3
*ENDFRAME
*FRAME FORSTATION 0 0 0 0 0 0 1 TX TY TZ
*CALA
ST	5	5	5
*ENDFRAME
*END
)";

char const *const DLEV_OLOC_9 = R"(*TITR
Checking contribution with frames, computatibility with residuals
*OLOC
*PREC 7 
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0	0
TARG1   0.02   0   0   0   0   0   10   20   0
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2        
*DLEV   LEVEL1   RefPt ST IHFIX IH 2
PT1		1.00002 DHOR 7.071
PT2		1.00006 DHOR 7.072
PT3		1.00004 DHOR 7.070
PT4		1.00003 DHOR 7.076
PT1		1.00002 DHOR 7.071
PT2		1.00005 DHOR 7.073
PT3		1.00007 DHOR 7.071
PT4		1.00003 DHOR 7.070
PT5		1.00001 DHOR 7.071
*FRAME FORTARGET 0 0 0 0 0 0 1 TZ
*CALA
PT5	10 10 3
*ENDFRAME
*FRAME FORSTATION 0 0 0 0 0 0 1 TX TY TZ
*CALA
ST	5	5	5
*ENDFRAME
*END
)";

char const *const DLEV_OLOC_10 = R"(*TITR
Checking REFPT and edge cases for DHOR FLAGS
*OLOC
*PREC 7
*HIST
*FAUT     .01     .10
*JSON
*PRES
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0	0
TARG1   0.02   0   1   0.01   0   0   10   20   0
*LEVEL LEVEL2   TARG1   0   0   0	0
TARG1   0.03   0   0   0   0   0   10   20   1
TARG2   0.04   0   0   0   0   0   30   40  -1
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2        
*DLEV   LEVEL1   RefPt ST IHFIX IH 2
PT1		1.00002 DHOR 6.071 DSE 0.03 DHPPM 30 DHDCOR 1
PT2		1.00006 DHOR 7.072
PT3		1.00004 DHOR 7.070 ID test
PT4		1.00003 DHOR 7.076 
PT1		1.00002 DHOR 7.071  ID test3
PT2		1.00005 DHOR 7.073
PT3		1.00007 
PT4		1.00003 DHOR 7.070
PT5		1.00001 DHOR 7.071
*DLEV   LEVEL1   REFPT ST IHFIX IH 2
PT1		1.00002  DSE   50
PT2		1.00006  DHDCOR 0
PT3		1.00004  DHPPM 60
PT4		1.00003  DHPPM 60 DHDCOR 0 DSE   50
PT1		1.00002   ID test1
PT2		1.001 
PT3		1.00007 
PT4		1.00003 
PT5		1.00001 
*DLEV   LEVEL2   %RefPt at 15 15
PT1		1 DHOR 6.0  
PT2		1 DHOR 8.0  TRGT TARG2
PT3		1 DHOR 7.0  DHPPM 60 DHDCOR 0 DSE   50
PT4		1 DHOR 7.0  DSE   50  DHPPM 60 DHDCOR 0	TRGT TARG2
PT1		1 DHOR 7.0  TRGT TARG2 DSE   50  DHPPM 60 DHDCOR 0
PT2		1 DHOR 8.0  TRGT TARG2 DSE   50		
PT3		1 DHOR 8.0  TRGT TARG2 DHPPM 60
PT4		1 DHOR 9.0  TRGT TARG2 DHDCOR -2
PT1		1 DHOR 7.0  DSE   50  DHDCOR 0
*FRAME FORTARGET 0 0 0 0 0 0 1 TZ
*CALA
PT5	10 10 3
*ENDFRAME
*FRAME FORSTATION 0 0 0 0 0 0 1 TX TY TZ
*CALA
ST	5	5	5
*ENDFRAME
*END
)";

//-----------------------------------SPHE------------------------------//
char const *const DLEV_SPHE_1 = R"(*TITR
Test SPHE
*SPHE
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0    
*CALA
PT1		2010     2010    330   
PT2		2020     2020    330   
PT3		2010     2020    330   
PT4		2020     2010    330
*VZ
PT5		2030	 2030	 330
*DLEV   LEVEL1 
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

//-----------------------------------LEP------------------------------//
char const *const DLEV_LEP_1 = R"(*TITR
Test LEP
*LEP
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0    
*CALA
PT1		2010     2010    330   
PT2		2020     2020    330   
PT3		2010     2020    330   
PT4		2020     2010    330
*VZ
PT5		2030	 2030	 330
*DLEV   LEVEL1 
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

//-----------------------------------RS2K------------------------------//
char const *const DLEV_RS2K_1 = R"(*TITR
Test RS2K
*RS2K
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0   0   0   0    
*CALA
PT1		2010     2010    330   
PT2		2020     2020    330   
PT3		2010     2020    330   
PT4		2020     2010    330
*VZ
PT5		2030	 2030	 330
*DLEV   LEVEL1 
PT1		0.5 TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

//-----------------------------------Staff vertical at the level of the target------------------------------//
char const *const DLEV_STAFFV_1 = R"(*TITR
Testing the staff verticality
*RS2K
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 0 0 0 0 0 0 0
*CALA
ST1 2000 2097.79265 500  %CCS point where the Z is colinear to the local vertical
PT1 6000 6000 0          %a point far far away
*POIN
PT2 2000 2000 400
*OBSXYZ
PT2 2000 2000 2000 0.1 0.1 0.1
*DLEV default RefPt ST1 IHFIX IH 0
% PT1 502.4181242 %old implementation with the staff following the station vertical
PT1	502.41831302343815 %Computed using CSGEO
*END
)";

char const *const DLEV_STAFFV_2 = R"(*TITR
Testing the staff verticality: also for TH and dCorr 
see INSTR section
*RS2K
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 250 0 250 0 0 0 0
*CALA
ST1 2000 2097.79265 500  %CCS point where the Z is colinear to the local vertical
PT1 6000 6000 0          %a point far far away
*POIN
PT2 2000 2000 400
*OBSXYZ
PT2 2000 2000 2000 0.1 0.1 0.1
*DLEV default RefPt ST1 IHFIX IH 0
% PT1 502.4181242 %old implementation with the staff following the station vertical
PT1	2.41831302343815 %Computed using CSGEO
*END
)";

char const *const DLEV_STAFFV_3 = R"(*TITR
Testing the staff verticality: IH should still follow local vertical vector at the station position 
see INSTR section
*RS2K
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 250 0 250 0 0 0 0
*CALA
ST1 2000 2097.79265 500  %CCS point where the Z is colinear to the local vertical
PT1 6000 6000 0          %a point far far away
*DLEV default RefPt ST1 IH 1000
% PT1 502.4181242 %old implementation with the staff following the station vertical
PT1	2.41831302343815 %Computed using CSGEO
*END
)";

char const *const DLEV_STAFFV_4 = R"(*TITR
Testing the staff verticality, LEP
*LEP
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 0 0 0 0 0 0 0
*CALA
ST1 2000 2097.79265 500  %CCS point where the Z is colinear to the local vertical
PT1 6000 6000 0          %a point far far away
*POIN
PT2 2000 2000 400
*OBSXYZ
PT2 2000 2000 2000 0.1 0.1 0.1
*DLEV default RefPt ST1 IHFIX IH 0
PT1	502.4193667124958 %Computed using CSGEO
*END
)";

char const *const DLEV_STAFFV_5 = R"(*TITR
Testing the staff verticality, SPHE
*SPHE
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 0 0 0 0 0 0 0
*CALA
ST1 2000 2097.79265 500  %CCS point where the Z is colinear to the local vertical
PT1 6000 6000 0          %a point far far away
*POIN
PT2 2000 2000 400
*OBSXYZ
PT2 2000 2000 2000 0.1 0.1 0.1
*DLEV default RefPt ST1 IHFIX IH 0
PT1	502.4509253775075 %Computed using CSGEO
*END
)";

char const *const DLEV_STAFFV_6 = R"(*TITR
Testing the staff verticality, SPHE, with the station not at P0
*SPHE
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 0 0 0 0 0 0 0
*CALA
ST1 1000 1000 500  
PT1 6000 6000 0          %a point far far away
*POIN
PT2 2000 2000 400
*OBSXYZ
PT2 2000 2000 2000 0.1 0.1 0.1
*DLEV default RefPt ST1 IHFIX IH 0
PT1	503.92421241858614 %Computed using CSGEO
*END
)";

char const *const DLEV_STAFFV_7 = R"(*TITR
Testing the staff verticality: also for TH, dCorr and IH, with the station not at P0
see INSTR section
*SPHE
*PREC 7 
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 250 0 250 0 0 0 0
*CALA
ST1 1000 1000 500
PT1 6000 6000 0          %a point far far away
*DLEV default RefPt ST1 IH 1000
PT1	3.92421241858614 %Computed using CSGEO
*END
)";

// DLEV+DHOR combined, IH adjustable, OLOC.
// ST1=(0,0,0), PT1=(5,0,-1), IH converges to 1m:
//   DLEV = (0+1-(-1))/1 - 0 = 2.0
//   DHOR = dist((0,0),(5,0)) = 5.0
// sigmaDHor=0.1mm (token[7]) ensures nonzero DHOR variance.
char const *const DLEV_DHOR_TH_1 = R"(*TITR
DLEV+DHOR combined, IH adjustable, exercises getDLEVContribCombined with DHOR
*OLOC
*PREC 7
*INSTR
*LEVEL default Staff 0 0 0 0
Staff 0.1 0 0 0 0 0 0.1 0 0
*CALA
ST1 0 0 0
PT1 5 0 -1
*DLEV default RefPt ST1 IH 0
PT1 2.0 DHOR 5.0
*END
)";

} // namespace TestDLEV
#endif //_LGC_TESTS_DLEV_H
