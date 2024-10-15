/*
© Copyright CERN 2000-2024. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
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
TARG1   0.0176   0   0   0   0   0   
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
TARG1   0.0176   0   0   0   0   0   
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

char const *const DLEV_PARSE_NOK_4 = R"(*TITR
TARGET line has not enough parameters
*OLOC
*INSTR
*LEVEL LEVEL1   TARG1   0   0   0
TARG1   0.0176   0   0   0   0   0	0   
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
TARG1   0.0176   1   0.2   0.3   0.04   0.03   
TARG2   0.02   0   0   0   0   0   
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
TARG1   0.0176   1   0   0   0   0   
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
ST		5   	5	 0     
*DLEV   LEVEL1 RefPt    ST
PT1		0.5 D TH 0.5
PT2		1 
PT3		1 
PT4		1 
*DLEV   LEVEL1  RefPt    ST  IH 2 IHFIX IHSE 0.4
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
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1 
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
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
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1  IHFIX
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
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
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1  IHFIX IH 4 IHSE 0.3
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
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
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		10     10    2   
PT2		20     20    2   
PT3		10     20    2   
PT4		20     10    2
*VZ
PT5		30	   30	 2
*DLEV   LEVEL1  IH 4 IHSE 0.3
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

//-----------------------------------SPHE------------------------------//
char const *const DLEV_SPHE_1 = R"(*TITR
Test SPHE
*SPHE
*INSTR
*LEVEL LEVEL1   TARG1   1   0   0	0
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		2010     2010    330   
PT2		2020     2020    330   
PT3		2010     2020    330   
PT4		2020     2010    330
*VZ
PT5		2030	 2030	 330
*DLEV   LEVEL1 
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
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
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		2010     2010    330   
PT2		2020     2020    330   
PT3		2010     2020    330   
PT4		2020     2010    330
*VZ
PT5		2030	 2030	 330
*DLEV   LEVEL1 
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
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
TARG1   0.0176   1   0   0   0   0    
*CALA
PT1		2010     2010    330   
PT2		2020     2020    330   
PT3		2010     2020    330   
PT4		2020     2010    330
*VZ
PT5		2030	 2030	 330
*DLEV   LEVEL1 
PT1		0.5 D TH 0.5  DHOR 7.071067812  DSE   10000
PT2		1 DHOR 7.071067812  DSE   10000
PT3		1 DHOR 7.071067812  DSE   10000
PT4		1 DHOR 7.071067812  DSE   10000
PT5		2
*END
)";

} // namespace TestDLEV
#endif //_LGC_TESTS_DLEV_H
