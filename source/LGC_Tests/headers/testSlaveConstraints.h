/*
© Copyright CERN 2000-2023. All rights reserved. This software is released under a CERN proprietary software license.
Any permission to use it shall be granted in writing. Request shall be addressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_SLAVE_H
#define _LGC_TESTS_SLAVE_H

/* Testing the slave constraints */
namespace Slave
{
char const *const SLAVE_1 = R"(*TITR
test with several groups of frames that get identified.
*OLOC
*APRI
*PREC 6
*INSTR
*CALA
Zero 0 0 0
X 1 0 0
Y 0 1 0
Z 0 0 1
*FRAME frame_1 1 2 3 0 0 0 1 TX TY TZ SLAVE Group1
*FRAME frame_2 0 0 0 10 20 30 1 RX RY RZ SLAVE Group2
*OBSXYZ 
Zero 0 0 0 1.00 1.00 1.00
X 1.001 0 0 1.00 1.00 1.00
Y 0 -0.999 0 1.00 1.00 1.00
Z 0 0 -1.001 1.00 1.00 1.00
*ENDFRAME
*ENDFRAME
*FRAME frame_3 0 0 0 40 50 60 1 RX RY RZ SLAVE Group2
*FRAME frame_4 4 5 6 0 0 0 1 TX TY TZ SLAVE Group1
*OBSXYZ
Zero 0 0 0 1.00 1.00 1.00
X 0.999 0 0 1.00 1.00 1.00
Y 0 -1.001 0 1.00 1.00 1.00
Z 0 0 -0.999 1.00 1.00 1.00
*ENDFRAME
*ENDFRAME
*END
)";

char const *const SLAVE_2 = R"(*TITR
test with a frame that is otherwise totally free and not determinable
*OLOC
*CONSI
*APRI
*PREC 6
*INSTR
*CALA
Zero 0 0 0
X 1 0 0
Y 0 1 0
Z 0 0 1
*FRAME frame_1 1 2 3 0 0 0 1 TX TY TZ RX RY RZ SCL SLAVE Group1
*OBSXYZ 
Zero 1 2 3 1.00 1.00 1.00
X 2 2 3 1.00 1.00 1.00
Y 1 1 3 1.00 1.00 1.00
Z 1 2 4 1.00 1.00 1.00
*ENDFRAME
*FRAME frame_2 4 5 6 0 0 0 1 TX TY TZ RX RY RZ SCL SLAVE Group1
*ENDFRAME
*END
)";

char const *const SLAVE_incompatible = R"(*TITR
test with an incompatible slave group (degrees of freedom inconsistent).
*OLOC
*APRI
*PREC 6
*INSTR
*CALA
Zero 0 0 0
X 1 0 0
Y 0 1 0
Z 0 0 1
*FRAME frame_1 1 2 3 0 0 0 1 TX TY TZ SLAVE Group1
*OBSXYZ 
Zero 0 0 0 1.00 1.00 1.00
X 1.001 0 0 1.00 1.00 1.00
Y 0 -0.999 0 1.00 1.00 1.00
Z 0 0 -1.001 1.00 1.00 1.00
*ENDFRAME
*FRAME frame_4 4 5 6 0 0 0 1 TX TY RZ SLAVE Group1
*OBSXYZ
Zero 0 0 0 1.00 1.00 1.00
X 0.999 0 0 1.00 1.00 1.00
Y 0 -1.001 0 1.00 1.00 1.00
Z 0 0 -0.999 1.00 1.00 1.00
*ENDFRAME
*END
)";

} // namespace Slave
#endif
