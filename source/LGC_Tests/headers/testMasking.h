/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_Masking_H
#define _LGC_TESTS_Masking_H


/*Relative Error calculation tests*/
namespace Masking {
char const *const Masking_test_1= R"(*TITR
%Test file for deactivation of measurements
*OLOC
*HIST
*PREC 7
%*CONSI
%*PUNC   OUT1
%*FAUT     .01     .10
*INSTR
*CALA
point1 1 2 3 
*POIN
point2 0 0 0
*OBSXYZ 
% if both observations are active they cancel other and the result should be zero.
point2 1 2 3 1 1 1 ID firstObs
point2 -1 -2 -3 1 1 1 ID secondObs
*END
)";
char const *const Masking_test_2= R"(*TITR
%Test file for deactivation of parameters
*OLOC
*HIST
*PREC 7
%*CONSI
%*PUNC   OUT1
%*FAUT     .01     .10
*INSTR
*CALA
point1 1 2 3 
*POIN
point2 0 0 0
point3 1 2 3
*OBSXYZ 
% if both observations are active they cancel other and the result should be zero.
point2 1 2 3 1 1 1 ID firstObs
point3 1 2 3 1 1 1
*END
)";

}
#endif

