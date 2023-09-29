/*
© Copyright CERN 2000-2023. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_POINTGROUPCONSTRAINT_H
#define _LGC_TESTS_POINTGROUPCONSTRAINT_H

namespace pointConstraintTest
{
char const *const group_definition = R"(*TITR
testFile
*OLOC
*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
*POIN
p0 1 2 3
*OBSXYZ 
p0 1 2 3 1 1 1
*FRAME testFrame 0 1 2 0 0 0 1 TX TY TZ
*CALA
p1 1 2 3
p2 3 4 5
p3 6 7 8
*ENDFRAME
*OBSXYZ
p1 2 3 4 1 1 1
p2 6 7 8 1 1 1
p3 9 10 11 1 1 1
)";
}
#endif
