/*
© Copyright CERN 2000-2023. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_POINTGROUPCONSTRAINT_H
#define _LGC_TESTS_POINTGROUPCONSTRAINT_H

namespace pointConstraintTest
{
char const *const testfile1= R"(*TITR
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

char const *const testfile2= R"(*TITR
testFile
*OLOC
*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
*POIN
p0 1 2 3
*OBSXYZ 
p0 1 2 3 1 1 1
p4 12 13 14 1 1 1
*FRAME testFrame 1 2 3 4 5 6 7 TX TY TZ RX RY RZ SCL
*CALA
p1 1 2 3
p2 3 4 5
p3 6 7 8
*POIN
p4 9 10 11
*ENDFRAME
*OBSXYZ
p1 2 3 4 1 1 1
p2 6 7 8 1 1 1
p3 9 10 11 1 1 1
)";

char const *const rotation_testfile= R"(*TITR
testFile
*OLOC
*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
*CALA
Zero 0 0 0
*OBSXYZ
Zero 0 0 0 1 1 1
Zero 0 0 0 1 1 1
Zero 0 0 0 1 1 1
% trying to make the frame rotate.. this should be "forbidden" via a rotation momentum constraint
px 1 0 0 1 1 1
py 0 1 0 1 1 1
pz 0 0 1 1 1 1
*FRAME rotatingFrame 0 0 0 10 20 30 1 RX 
*CALA
px 1 0 0
py 0 1 0
pz 0 0 1
*ENDFRAME
)";

char const *const translation_testfile= R"(*TITR
testFile
*OLOC
*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
*CALA
Zero 0 0 0
*OBSXYZ
Zero 0 0 0 1 1 1
Zero 0 0 0 1 1 1
Zero 0 0 0 1 1 1
% trying to make the frame translate.. this should be "forbidden" via a x-center of gravity constraint
px 1 0 0 1 1 1
py 0 1 0 1 1 1
pz 0 0 1 1 1 1
*FRAME translatingFrame 1 0 0 0 0 0 1 TX 
*CALA
px 1 0 0
py 0 1 0
pz 0 0 1
*ENDFRAME
)";

}
#endif
