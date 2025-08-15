/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _SimuTest
#	define _SimuTest

/*Non TSTN and non CAM tests, like levelling, DLEV, ECHO, DVER, etc.*/
namespace Simu
{
char const *const OBSXYZ_1 = R"(*TITR
OBSXYZ sim test
*OLOC
*SIMU 1 0
*INSTR
*CALA
UX  1 0 0
UY  0 1 0
UZ  0 0 1
*FRAME test 1 3 7 0 0 0 1 TX TY TZ
*OBSXYZ
UX  1 2 3  0.0001  0.0001  0.0001
UY  4 5 6  0.0001  0.0001  0.0001
UZ  7 8 9  0.0001  0.0001  0.0001
*ENDFRAME
*END
)";
}
#endif
