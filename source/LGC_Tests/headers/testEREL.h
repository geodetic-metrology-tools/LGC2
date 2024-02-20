/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_EREL_H
#define _LGC_TESTS_EREL_H


/*Relative Error calculation tests*/
namespace Erel {
char const *const EREL_test= R"(*TITR
Test if sigmas from polar station target can be recovered by erel.
Test if OBSXYZ from 'loose' frame gives same erel as from fixed frame.
*OLOC
%*CONSI
*APRI
*EREL 
zeroInRoot P2
station2Pos P3 dummyFrame
testPointReference zeroInRoot 
testPointTr uncertainFrameTrZero uncertainFrameTr
testPointRot uncertainFrameRotZero uncertainFrameRot
*INSTR
*POLAR Station1 Target1 0 0 0 0
Target1 12 13 14 0 0 0 0 0 0 0  
*POIN
P2  1 1 1
P3 1 2 3
testPointTr 4 5 6 
testPointRot 4 5 6 
testPointReference 4 5 6
*CALA
zeroInRoot 0 0 0
*FRAME fixBearingFrame 0 0 0 0 0 0 1 
*TSTN  zeroInRoot    Station1    IHFIX     TRGT Target1  
*V0 
*ANGL
P2 10
station2Pos 10
*ZEND
P2 10
station2Pos 10
*DIST
P2 1
station2Pos 1
*ENDFRAME
*OBSXYZ 
uncertainFrameTrZero 1 2 3 7 8 9 
uncertainFrameTrX 1 0 0 1.0 1.0 1.0 
uncertainFrameTrY 0 1 0 1.0 1.0 1.0 
uncertainFrameTrZ 0 0 1 1.0 1.0 1.0 
uncertainFrameRotZero 1 2 3 7 8 9 
uncertainFrameRotX 1 0 0 1.0 1.0 1.0 
uncertainFrameRotY 0 1 0 1.0 1.0 1.0 
uncertainFrameRotZ 0 0 1 1.0 1.0 1.0 
testPointReference 4 5 6 10 11 12
*FRAME uncertainFrameTr 1 2 3 0 0 0 1 TX TY TZ 
*CALA
uncertainFrameTrZero 0 0 0
uncertainFrameTrX 1 0 0
uncertainFrameTrY 0 1 0
uncertainFrameTrZ 0 0 1
*OBSXYZ
testPointTr 4 5 6 10 11 12
*ENDFRAME
*FRAME uncertainFrameRot 0 0 0 10 20 30 1 RX RY RZ 
*CALA
uncertainFrameRotZero 0 0 0
uncertainFrameRotX 1 0 0
uncertainFrameRotY 0 1 0
uncertainFrameRotZ 0 0 1
*OBSXYZ
testPointRot 4 5 6 10 11 12
*ENDFRAME
*FRAME dummyFrame 1 2 3 0 0 0 1  TX TY TZ
*CALA
station2Pos 0 -1 -2
*TSTN  station2Pos    Station1    IHFIX     TRGT Target1  
*V0 
*ANGL
P3 10
*ZEND
P3 10
*DIST
P3 1
*ENDFRAME
*END)";


char const *const ERELFRAME_test_1 = R"(*TITR
Test frame erel
*OLOC
*EREL
PA PB
*ERELFRAME
A B
fixedFrame ROOT
*INSTR
*OBSXYZ
PA 0 0 0 1 1 1
PB 1 2 3 1 1 1
*FRAME A 1 2 3 4 5 6 1 TX TY TZ
*CALA 
PA 0 0 0 
*ENDFRAME
*FRAME B 1 2 3 4 5 6 1 RY RZ
*CALA
PB 1 2 3 
*ENDFRAME
*FRAME fixedFrame 1 2 3 4 5 6 7
*CALA
deadPoint 1 2 3
*ENDFRAME
*END)";

char const *const ERELFRAME_test_2 = R"(*TITR
test for ERELFRAME
Here the relative trafo from F3 to ROOT is determined completely by the obsxyz inside F3.
These obsxyz are duplicsates of the F1 frame with its obsxyz.
Consequently the relative Trafo F3->ROOT should be equivalent to F1->ROOT (=F1).
To check after computation: are the helmert parameters equal, are the covariances equal?
*OLOC
*PREC 6
*ERELFRAME
F3 ROOT
*INSTR
*CALA
ex 1 0 0
ey 0 1 0
ez 0 0 1
*FRAME F1 1 2 3 4 5 6 1 TX TY TZ RX RY RZ SCL
%equivalent to obsxyz in F3
*OBSXYZ
ex 0.879  1.011 3.086   1 1 1
ey 1.284  2.049 3.957   1 1 1
ez 0.048  2.140 3.275   1 1 1
*ENDFRAME
% this frame does not influence the relative trafo between F6 and ROOT
*FRAME F2 3 4 5 6 7 8 1 TX TY TZ RX RY RZ SCL
*OBSXYZ
ex  0.813 -0.440  0.378 1 1 1 
ey  0.469  0.882  0.018 1 1 1 
ez -0.342  0.163  0.925 1 1 1 
*FRAME F3 10 11 12 13 14 15 1 TX TY TZ RX RY RZ SCL
%equivalent to obsxyz in F1
*OBSXYZ
ex 0.879  1.011 3.086   1 1 1
ey 1.284  2.049 3.957   1 1 1
ez 0.048  2.140 3.275   1 1 1
*ENDFRAME
*ENDFRAME
*END)";

}
#endif

