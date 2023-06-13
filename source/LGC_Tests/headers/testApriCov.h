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
P1  1 1 1 apriCov mat(1,0.2,0.3,0.2,2,0.2,0.3,0.2,3)
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
P1  1 1 1 apriCov mat(0.000001,0,0,0,0.000004,0,0,0,0.000009)
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
*FRAME testFrame1 0 0 0 0 0 0 1 TX  TY TZ apriCov mat(1,0.2,0.3,0.2,2,0.2,0.3,0.2,3)
*CALA
auxPoint1 0 0 0
% useless measurements to calm down the unknowns vs equations counter
*OBSXYZ
auxPoint1 0 0 0 1 1 1
*ENDFRAME
*FRAME testFrame2 0 0 0 0 0 0 1 TX RX RZ apriCov mat(1,0.4,0.3,0.4,4,0.4,0.3,0.4,3)
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
P1  1 1 1 apriCov diag(1,4,9)
P2 1 1 1
%*FRAME testFrame 1 2 3 4 5 6 1 TX TY TZ RX RY RZ apriCov (2,0,0,0,0,0,0,3,0,0,0,0,0,0,5,0,0,0,0,0,0,7,0,0,0,0,0,0,11,0,0,0,0,0,0,13)
*FRAME testFrame 1 2 3 4 5 6 1 TX TY TZ RX RY RZ apriCov diag(2,3,5,7,11,13)
*ENDFRAME
*OBSXYZ 
P2 1 2 3 1 1 1
*END
)";

	char const *const APRICOV_slave_vs_apriCov = R"(*TITR
test apriCov together with slave constraints
*OLOC
*APRI
*CONSI
*INSTR
*POLAR Station1 Target1 0 0 0 0
Target1 12 13 14 0 0 0 0 0 0 0  
*FRAME testFrame1 1 2 3 4 5 6 1 TX RY SLAVE myGroup1 apriCov mat(2,1,1,2)
*ENDFRAME
*FRAME testFrame2  10 20 30 40 50 60 1 TX RY SLAVE myGroup1
*ENDFRAME
*FRAME testFrame3 1 2 3 4 5 6 1 TX RY   SLAVE myGroup2
*ENDFRAME
*FRAME testFrame4  10 20 30 40 50 60 1 TX RY apriCov mat(2,1,1,2) SLAVE myGroup2
*ENDFRAME
*END
)";


}
#endif

