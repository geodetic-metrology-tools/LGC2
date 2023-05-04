/*
© Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_APRICOV_H
#define _LGC_TESTS_APRICOV_H


namespace ApriCov{
	char const *const APRICOV_recovering_point_covariance = R"(*TITR
test apriCov flag for points
*OLOC
*APRI
%*CONSI
*INSTR
*POIN
P1  1 1 1 apriCov (1,2,3,2,2,2,3,2,3)
P2 1 1 1
*OBSXYZ 
P2 1 2 3 1 1 1
*END
)";

	char const *const APRICOV_comparison_with_obsxyz = R"(*TITR
test apriCov vs obsxyz, obsxyz and apricov should cancel out in this example
*OLOC
*APRI
%*CONSI
*INSTR
*POIN
% this should be equivalent to an OBSXYZ measurement with precisions 1mm,2mm,3mm for P1
P1  1 1 1 apriCov (0.000001,0,0,0,0.000004,0,0,0,0.000009)
*OBSXYZ 
P1 -1 -1 -1 1 2 3
*END
)";


	char const *const APRICOV_recovering_frame_covariance = R"(*TITR
test apriCov flag for frames
*OLOC
*APRI
%*CONSI
*INSTR
*POIN
*FRAME testFrame1 0 0 0 0 0 0 1 TX  TY TZ apriCov (1,2,3,2,2,2,3,2,3)
*CALA
auxPoint1 0 0 0
% useless measurements to calm down the unknowns vs equations counter
*OBSXYZ
auxPoint1 0 0 0 1 1 1
*ENDFRAME
*FRAME testFrame2 0 0 0 0 0 0 1 TX RX RZ apriCov (1,4,3,4,4,4,3,4,3)
*CALA
auxPoint2 0 0 0
% useless measurements to calm down the unknowns vs equations counter
*OBSXYZ
auxPoint2 0 0 0 1 1 1
*ENDFRAME
*END
)";

	char const *const APRICOV_recovering_frame_covariance_2 = R"(*TITR
test apriCov flag for frames
*OLOC
*APRI
%*CONSI
*INSTR
*POLAR Station1 Target1 0 0 0 0
Target1 12 13 14 0 0 0 0 0 0 0  
*POIN
P1  1 1 1 apriCov (1,0,0,0,4,0,0,0,9)
P2 1 1 1
*FRAME testFrame 1 2 3 4 5 6 1 TX TY TZ RX RY RZ apriCov (2,0,0,0,0,0,0,3,0,0,0,0,0,0,5,0,0,0,0,0,0,7,0,0,0,0,0,0,11,0,0,0,0,0,0,13)
*ENDFRAME
*OBSXYZ 
P2 1 2 3 1 1 1
*END
)";



}
#endif

