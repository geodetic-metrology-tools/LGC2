/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_Evaluator_H
#define _LGC_TESTS_Evaluator_H


/*Relative Error calculation tests*/
namespace Evaluator {
char const *const Evaluator_test = R"(*TITR
testTitle
*OLOC
*PREC 7
*INSTR
*POIN
A 1 2 3
B 4 5 6
*OBSXYZ
A 0 0 0 1 1 1
B -1 -2 -3 1 1 1
B 1 2 3 1 1 1
*END)";

char const *const armijo_test = R"(*TITR
demonstrating diverging behaviour of angle measurements 
with bad initial values.
*OLOC
*INSTR
*POLAR   TS1   TGT1   0.0   0.0   0.0   0.0
TGT1  1 1 1  0.0   0.0   0.0   0.0   0.0   0.0   0.0
*CALA
Zero 0 0 0
*VZ
%diverging:
%P1 1 0 2.85
%good enough values:
%7 iterations:
%P1 1 0 2.8
%actual solution (1 iteration):
P1 1 0 1
%10 iterations
%P1 1 0 -1.19
%again diverging:
%P1 1 0 -1.2
*FRAME bearingFrame 0 0 0 0 0 0 1 
*TSTN Zero TS1 IHFIX 0
*V0
*ZEND
P1 50
*ENDFRAME
*END
)";
}
#endif

