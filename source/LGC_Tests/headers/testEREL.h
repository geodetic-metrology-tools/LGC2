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
}
#endif

