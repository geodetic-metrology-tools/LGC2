/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_CUSTOMGEOID_H
#define _LGC_TESTS_CUSTOMGEOID_H

namespace TestCUSTOMGEOID
{
//-----------------------------------PARSE------------------------------//
char const *const RS2K_PARSE = R"(*TITR
Test H to Z

*RS2K
*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
P0                                     2000.0000     2097.79265    433.65921   
Test1                                     63.34159     5271.95242    453.57390 
Test2                                    1764.56580     283.09069    253.56984 
Test3                                    166.52069     2289.92934    453.57414 
*POIN
TestXYZ                                   759.11954     5300.00270   449.86914 

*OBSXYZ
TestXYZ                                   759.11954     5300.00270   2448.94777   0.1 0.1 0.1

*END
)";

char const *const OLOC_PARSE = R"(*TITR
Test H to Z

*OLOC
*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
P0                                     2000.0000     2097.79265    433.65921   
Test1                                     63.34159     5271.95242    453.57390 
Test2                                    1764.56580     283.09069    253.56984 
Test3                                    166.52069     2289.92934    453.57414 
*POIN
TestXYZ                                   759.11954     5300.00270   449.86914 

*OBSXYZ
TestXYZ                                   759.11954     5300.00270   2448.94777   0.1 0.1 0.1

*END
)";

char const *const CUSTOMGEOID_PARSE = R"(*TITR
Test H to Z

*CUSTOMG "C:\Users\bweyer\cernbox\Documents\Development\SurveyLib\ImplementationGeoid\CERN\FCC-G2025_V1.0.tif"

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
P0             4387932.75100    4580837.51956    667660.70413
Test1          4385256.59460    4583391.59034    667904.46665
Test2          4388922.45162    4579894.00432    665917.14381
Test3          4387241.97666    4581821.66905    665648.57092
% P0          6.0486633109    46.2321561210       483.62404
% Test1       6.0508605655    46.2655700713       503.64465
% Test2       6.0329821215    46.2198054063       303.52076
% Test3       6.0303489733    46.2428105148       503.60295

*POIN
TestXYZ        4385455.32337    4583072.23780    668749.21307
% TestXYZ     6.0585451616    46.2622789155       499.91373

*OBSXYZ
TestXYZ                                   759.11954     5300.00270   2448.94777   0.1 0.1 0.1

*END
)";

char const *const RS2K_TSTN = R"(*TITR
Fichier d'input créé le 17-JUL-2018
Opération n 14736, Alignement H4

*RS2K
*HIST
*PUNC   OUT1
*FAUT     .01     .10
*JSON

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
EHN1.ST.H4-4.                                     754.53167     5263.83883    453.58094   $670.000    776121   coordonnées au 17-JUL-2018 15:48:57
EHN1.ST.H4-5.                                     763.34159     5271.95242    453.57390   $680.000    779740   coordonnées au 17-JUL-2018 15:48:57
EHN1.ST.H4-6.                                     764.56580     5283.09069    453.56984   $690.000    779741   coordonnées au 17-JUL-2018 15:48:57
EHN1.ST.H4-7.                                     766.52069     5289.92934    453.57414   $700.000    779742   coordonnées au 17-JUL-2018 15:48:57
*POIN
H4.XBPF.22716.E                                   759.11954     5300.00270    448.94777   $715.853    780399   coordonnées théoriques au 17-JUL-2018 15:48:57
H4.XBPF.22716.S                                   759.23973     5300.36554    448.87048   $716.243    780400   coordonnées théoriques au 17-JUL-2018 15:48:57

*TSTN   EHN1.ST.H4-6.   AT402.392781    IHFIX   IH  0.189800    TRGT CCR1.5.1143L    %station n268877
*V0   
*ANGL   
EHN1.ST.H4-4.                                125.800700   $1110206 -12-JUL-2018
EHN1.ST.H4-5.                                102.179500   $1110207 -12-JUL-2018
EHN1.ST.H4-7.                                312.938700   $1110208 -12-JUL-2018
H4.XBPF.22716.S                              276.172900   $1110215 -12-JUL-2018
H4.XBPF.22716.E                              275.378300   $1110216 -12-JUL-2018
*ZEND   
EHN1.ST.H4-4.                                100.523400 TH  0.000000   $1110209 -12-JUL-2018
EHN1.ST.H4-5.                                101.054200 TH  0.000000   $1110210 -12-JUL-2018
EHN1.ST.H4-7.                                101.660300 TH  0.000000   $1110211 -12-JUL-2018
H4.XBPF.22716.S                              116.813900 TH  0.000000   $1110217 -12-JUL-2018
H4.XBPF.22716.E                              116.835500 TH  0.000000   $1110218 -12-JUL-2018
*DIST   
EHN1.ST.H4-4.                                 21.710293 TH  0.000000   $1110212 -12-JUL-2018
EHN1.ST.H4-5.                                 11.207336 TH  0.000000   $1110213 -12-JUL-2018
EHN1.ST.H4-7.                                  7.115589 TH  0.000000   $1110214 -12-JUL-2018
H4.XBPF.22716.S                               18.729277 TH  0.000000   $1110219 -12-JUL-2018
H4.XBPF.22716.E                               18.409762 TH  0.000000   $1110220 -12-JUL-2018
*END
)";

char const *const CUSTOMGEOID_TSTN = R"(*TITR
Test sample file computation with custom geoid

*CUSTOMG "C:\Users\bweyer\cernbox\Documents\Development\SurveyLib\ImplementationGeoid\CERN\FCC-G2025_V1.0.tif"
*HIST
*PUNC   OUT1
*FAUT     .01     .10
*JSON

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
EHN1.ST.H4-4.          4392998.61554    466237.46304    4585795.98967 $670.000    776121   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
EHN1.ST.H4-5.          4392996.06859    466249.10026    4585797.22791 $680.000    779740   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
EHN1.ST.H4-6.          4392989.15933    466255.65204    4585803.13519 $690.000    779741   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
EHN1.ST.H4-7.          4392985.30007    466260.71838    4585806.30174 $700.000    779742   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
%EHN1.ST.H4-4.          46.2620323022    6.0582335019    503.62526 $670.000    776121   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
%EHN1.ST.H4-5.          46.2620484817    6.0583870763    503.61800 $680.000    779740   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
%EHN1.ST.H4-6.          46.2621253815    6.0584810341    503.61403 $690.000    779741   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
%EHN1.ST.H4-7.          46.2621665424    6.0585516589    503.61834 $700.000    779742   coordonnées au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO

*POIN
H4.XBPF.22716.E        4392973.19703    466258.93012    4585811.59621 $715.853    780399   coordonnées théoriques au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
H4.XBPF.22716.S        4392972.94412    466259.20755    4585811.70187 $716.243    780400   coordonnées théoriques au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO

%H4.XBPF.22716.E        46.2622789201    6.0585451628    498.99236 $715.853    780399   coordonnées théoriques au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO
%H4.XBPF.22716.S        46.2622810215    6.0585490872    498.91507 $716.243    780400   coordonnées théoriques au 17-JUL-2018 15:48:57 transformées en ETRF93 avec CSGEO

*TSTN   EHN1.ST.H4-6.   AT402.392781    IHFIX   IH  0.189800    TRGT CCR1.5.1143L    %station n268877
*V0   
*ANGL   
EHN1.ST.H4-4.                                125.800700   $1110206 -12-JUL-2018
EHN1.ST.H4-5.                                102.179500   $1110207 -12-JUL-2018
EHN1.ST.H4-7.                                312.938700   $1110208 -12-JUL-2018
H4.XBPF.22716.S                              276.172900   $1110215 -12-JUL-2018
H4.XBPF.22716.E                              275.378300   $1110216 -12-JUL-2018
*ZEND   
EHN1.ST.H4-4.                                100.523400 TH  0.000000   $1110209 -12-JUL-2018
EHN1.ST.H4-5.                                101.054200 TH  0.000000   $1110210 -12-JUL-2018
EHN1.ST.H4-7.                                101.660300 TH  0.000000   $1110211 -12-JUL-2018
H4.XBPF.22716.S                              116.813900 TH  0.000000   $1110217 -12-JUL-2018
H4.XBPF.22716.E                              116.835500 TH  0.000000   $1110218 -12-JUL-2018
*DIST   
EHN1.ST.H4-4.                                 21.710293 TH  0.000000   $1110212 -12-JUL-2018
EHN1.ST.H4-5.                                 11.207336 TH  0.000000   $1110213 -12-JUL-2018
EHN1.ST.H4-7.                                  7.115589 TH  0.000000   $1110214 -12-JUL-2018
H4.XBPF.22716.S                               18.729277 TH  0.000000   $1110219 -12-JUL-2018
H4.XBPF.22716.E                               18.409762 TH  0.000000   $1110220 -12-JUL-2018
*END
)";

char const *const SPHE_TSTN = R"(*TITR
Fichier d'input créé le 17-JUL-2018
Opération n 14736, Alignement H4

*SPHE
*HIST
*PUNC   OUT1
*FAUT     .01     .10
*JSON

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
EHN1.ST.H4-4.          4380362.55297    464933.55198    4603442.35046 $ Geocentric SPHE
%EHN1.ST.H4-5.          4380360.00616    464945.18923    4603443.58871 $ Geocentric SPHE
EHN1.ST.H4-6.          4380353.09683    464951.74108    4603449.49584 $ Geocentric SPHE
%EHN1.ST.H4-7.          4380349.23755    464956.80745    4603452.66231 $ Geocentric SPHE

*POIN
H4.XBPF.22716.E        4380337.13434    464955.01934    4603457.95642 $ Geocentric SPHE
H4.XBPF.22716.S        4380336.88143    464955.29677    4603458.06207 $ Geocentric SPHE

*TSTN   EHN1.ST.H4-6.   AT402.392781    IHFIX   IH  0.189800    TRGT CCR1.5.1143L    %station n268877
*V0   
*ANGL   
EHN1.ST.H4-4.                                125.800700   $1110206 -12-JUL-2018
%EHN1.ST.H4-5.                                102.179500   $1110207 -12-JUL-2018
%EHN1.ST.H4-7.                                312.938700   $1110208 -12-JUL-2018
H4.XBPF.22716.S                              276.172900   $1110215 -12-JUL-2018
H4.XBPF.22716.E                              275.378300   $1110216 -12-JUL-2018
*ZEND   
EHN1.ST.H4-4.                                100.523400 TH  0.000000   $1110209 -12-JUL-2018
%EHN1.ST.H4-5.                                101.054200 TH  0.000000   $1110210 -12-JUL-2018
%EHN1.ST.H4-7.                                101.660300 TH  0.000000   $1110211 -12-JUL-2018
H4.XBPF.22716.S                              116.813900 TH  0.000000   $1110217 -12-JUL-2018
H4.XBPF.22716.E                              116.835500 TH  0.000000   $1110218 -12-JUL-2018
*DIST   
EHN1.ST.H4-4.                                 21.710293 TH  0.000000   $1110212 -12-JUL-2018
%EHN1.ST.H4-5.                                 11.207336 TH  0.000000   $1110213 -12-JUL-2018
%EHN1.ST.H4-7.                                  7.115589 TH  0.000000   $1110214 -12-JUL-2018
H4.XBPF.22716.S                               18.729277 TH  0.000000   $1110219 -12-JUL-2018
H4.XBPF.22716.E                               18.409762 TH  0.000000   $1110220 -12-JUL-2018
*END
)";

char const *const CUSTOMGEOID_TSTN_2 = R"(*TITR
Test sample file computation with custom geoid

*CUSTOMG "C:\Users\bweyer\cernbox\Documents\Development\SurveyLib\ImplementationGeoid\CERN\FCC-G2025_V1.0.tif"
*HIST
*PUNC   OUT1
*FAUT     .01     .10
*JSON

*INSTR
*POLAR AT402.392781 CCR1.5.1143L 0 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
EHN1.ST.H4-4.          4393155.402467948    466068.581471977    4585659.970881662
EHN1.ST.H4-6.          4393152.260545896    466059.911753347    4585663.839254038
*POIN
H4.XBPF.22716.E        4393145.717179410    466064.842192140    4585669.574681613

*TSTN   EHN1.ST.H4-6.   AT402.392781    IHFIX   IH  0    TRGT CCR1.5.1143L    %station n268877
*V0   
*ANGL   
EHN1.ST.H4-4.                                100.0   $1110206 -12-JUL-2018
H4.XBPF.22716.E                              0.0   $1110216 -12-JUL-2018
*ZEND   
EHN1.ST.H4-4.                                100.01273 TH  0.000000   $1110209 -12-JUL-2018
H4.XBPF.22716.E                              99.97100 TH  0.000000   $1110218 -12-JUL-2018
*DIST   
EHN1.ST.H4-4.                                 10 TH  0.000000   $1110212 -12-JUL-2018
H4.XBPF.22716.E                               10 TH  0.000000   $1110220 -12-JUL-2018
*END
)";

char const *const SPHE_SIMPLE_TSTN = R"(*TITR
Fichier d'input créé le 17-JUL-2018
Opération n 14736, Alignement H4

*SPHE
*HIST
*PUNC   OUT1
*FAUT     .01     .10
*JSON
*PREC 6

*INSTR
*POLAR AT402.392781 CCR1.5.1143L .1898 0 0 0
CCR1.5.1143L 3 3 .02 6 0 0 0 0 0 0

*CALA
EHN1.ST.H4-4.          4380362.550637    464933.551733    4603442.348017
EHN1.ST.H4-5.          4380360.003863    464945.188989    4603443.586290
EHN1.ST.H4-6.          4380353.094530    464951.740832    4603449.493431
EHN1.ST.H4-7.          4380349.235261    464956.807212    4603452.659906
*POIN
H4.XBPF.22716.E        4380337.132022    464955.019094    4603457.953983
H4.XBPF.22716.S        4380336.879112    464955.296529    4603458.059639

*TSTN   EHN1.ST.H4-6.   AT402.392781    IHFIX   IH  0.189800    TRGT CCR1.5.1143L    %station n268877
*V0   
*ANGL   
EHN1.ST.H4-4.                                125.800700   $1110206 -12-JUL-2018
EHN1.ST.H4-5.                                102.179500   $1110207 -12-JUL-2018
EHN1.ST.H4-7.                                312.938700   $1110208 -12-JUL-2018
H4.XBPF.22716.S                              276.172900   $1110215 -12-JUL-2018
H4.XBPF.22716.E                              275.378300   $1110216 -12-JUL-2018
*ZEND   
EHN1.ST.H4-4.                                100.523400 TH  0.000000   $1110209 -12-JUL-2018
EHN1.ST.H4-5.                                101.054200 TH  0.000000   $1110210 -12-JUL-2018
EHN1.ST.H4-7.                                101.660300 TH  0.000000   $1110211 -12-JUL-2018
H4.XBPF.22716.S                              116.813900 TH  0.000000   $1110217 -12-JUL-2018
H4.XBPF.22716.E                              116.835500 TH  0.000000   $1110218 -12-JUL-2018
*DIST   
EHN1.ST.H4-4.                                 21.710293 TH  0.000000   $1110212 -12-JUL-2018
EHN1.ST.H4-5.                                 11.207336 TH  0.000000   $1110213 -12-JUL-2018
EHN1.ST.H4-7.                                  7.115589 TH  0.000000   $1110214 -12-JUL-2018
H4.XBPF.22716.S                               18.729277 TH  0.000000   $1110219 -12-JUL-2018
H4.XBPF.22716.E                               18.409762 TH  0.000000   $1110220 -12-JUL-2018
*END


)";

} // namespace TestCUSTOMGEOID
#endif //_LGC_TESTS_CUSTOMGEOID_H
