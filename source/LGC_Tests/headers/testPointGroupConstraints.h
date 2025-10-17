/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_POINTGROUPCONSTRAINT_H
#define _LGC_TESTS_POINTGROUPCONSTRAINT_H

namespace pointConstraintTest
{
char const *const testfile1 = R"(*TITR
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

char const *const testfile2 = R"(*TITR
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

char const *const rotation_testfile = R"(*TITR
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

char const *const translation_testfile = R"(*TITR
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

char const *const constraintDetection_testfile = R"(*TITR
testFile
find necessary libr constraint groups
*OLOC
*CONSI LIBR
*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
% dummy observations to deactivate the unknown vs obs counter check
*CALA 
zero 0 0 0
*OBSXYZ
zero 0 0 0 1 1 1 
zero 0 0 0 1 1 1 
zero 0 0 0 1 1 1 
%%  % this frame should give rise to TX TY and TZ helmert movements in root
%%  *FRAME testFrameTranslation 0 1 2 0 0 0 1 TX TY TZ
%%  *CALA
%%  p1 1 2 3
%%  p2 3 4 5
%%  p3 6 7 8
%%  *ENDFRAME
%%  % this frame should give rise to RX RY and RZ helmert movements in root
%%  *FRAME testFrameRotation 0 1 2 0 0 0 1 RX RY RZ
%%  *CALA
%%  p4 1 2 3
%%  p5 3 4 5
%%  p6 6 7 8
%%  *ENDFRAME
% this frame should give rise to RX RY and RZ helmert movements in root
*FRAME testFrameTranslationAndRotation 0 0 0 10 20 0 1  RY 
% Ry is free but RZ is determined to be blocked because from root the movement is a combination of RY and RZ
% note: order of rotations is first RX, then RY then RZ. So if there is a RZ freedom predecessed by fixed RX, the resulting linearized direction will contain a RX component
%*FRAME testFrameTranslationAndRotation 0 0 0 10 0 0 1 RY 
*CALA
p7 1 2 3
p8 3 4 5
p9 6 7.01 8
%p10 9 10 11
%p12 12 13 14
%p13 15 16 17
%p14 1 1 1
*ENDFRAME
*END

)";

char const *const constraintDetection_pyramidFull = R"(*TITR
testing libr constraint choice. points form a rigid pyramid.
Depending on the degrees of freedom of the points, the pyramid is "floating" in space or fully/partially determined
*OLOC
*CONSI LIBR
*INSTR
*EDM EDM TGT 0.0 0.0 0.0
TGT   0.5 5.0 0  0  0 0.4  0.0  0.6
*POIN
P1 0 0 0
P2 0 1.000 0 
P3 0.9976 1.00032 0 
P4 1.00017 0 0
Top 0.5001 0.499978 1.00013
*DSPT P1 EDM  IH 0.0     
P2 1
P3 1.414
P4 1
Top 1.224
*DSPT P2 EDM  IH 0.0     
P1 1
P4 1.414
P3 1
Top 1.224
*DSPT P3 EDM  IH 0.0     
P2 1
P1 1.414
P4 1
Top 1.224
*DSPT P4 EDM  IH 0.0     
P1 1
P2 1.414
P3 1
Top 1.224
*END
)";

char const *const constraintDetection_pyramidRotationXYZ = R"(*TITR
testing libr constraint choice. points form a rigid pyramid.
Depending on the degrees of freedom of the points, the pyramid is "floating" in space or fully/partially determined
*OLOC
*CONSI LIBR
*INSTR
*EDM EDM TGT 0.0 0.0 0.0
TGT   0.5 5.0 0  0  0 0.4  0.0  0.6
*CALA
P1 0 0 0
*POIN
P2 0 1.000 0 
P3 0.9976 1.00032 0 
P4 1.00017 0 0
Top 0.5001 0.499978 1.00013
*DSPT P1 EDM  IH 0.0     
P2 1
P3 1.414
P4 1
Top 1.224
*DSPT P2 EDM  IH 0.0     
P1 1
P4 1.414
P3 1
Top 1.224
*DSPT P3 EDM  IH 0.0     
P2 1
P1 1.414
P4 1
Top 1.224
*DSPT P4 EDM  IH 0.0     
P1 1
P2 1.414
P3 1
Top 1.224
*END
)";

char const *const constraintDetection_emptyConstraint = R"(*TITR
testing libr constraint choice. manually added constraint can lead to "empty" constraints. This should abort the calculation.
*OLOC
*CONSI LIBR TZ
*INSTR
*VXY
A 1 2 3
B 4 5 6
*OBSXYZ
A 1 2 3 1 1 1
B 4 5 6 1 1 1
*END
)";

} // namespace pointConstraintTest
#endif
