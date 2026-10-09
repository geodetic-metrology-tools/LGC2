/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_Statistics_H
#define _LGC_TESTS_Statistics_H

namespace Statistics
{
char const *const alpha_beta_default = R"(*TITR
test for correct alpha beta handling (at default values alpha=0.01, beta=0.1)
alpha = 0.01 sets the wimax thershold at 2.575.. from that on a normalized residual should be flagged as blunder
with alpha = 0.01 and beta = 0.1 the Nabla should be 1mm * (z_{1-alpha/2}+z_{1-beta}) = 1mm * (2.57+1.28) = 3.857...
*OLOC
*FAUT
*APRI
*PREC 6
*INSTR
*CALA
refPt 0 0 0
*POIN
origin 0 0 0
*OBSXYZ
refPt 0.00257 0.00258 0 1 1 1 % x should not be flagged y should be flagged
origin 0 0 0 1 1 1
*END
)";
char const *const alpha_beta_non_default = R"(*TITR
test for correct alpha beta handling (at values alpha=0.02, beta=0.2)
alpha = 0.02 sets the wimax thershold at 2.326.. from that on a normalized residual should be flagged as blunder
with alpha = 0.02 and beta = 0.2 the Nabla should be 1mm * (z_{1-alpha/2}+z_{1-beta}) = 1mm * (2.326+0.842) = 3.168
*OLOC
*FAUT 0.02 0.2
*APRI
*PREC 6
*INSTR
*CALA
refPt 0 0 0
*POIN
origin 0 0 0
*OBSXYZ
refPt 0.002325 0.002327 0 1 1 1 % x should not be flagged y should be flagged
origin 0 0 0 1 1 1
*END
)";

} // namespace Statistics
#endif
