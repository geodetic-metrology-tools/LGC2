

                          ****               ******              *****                                                              
                          ****            ************        ***********                                                           
                          ****           *****     ****      *****    ****                                                          
                          ****          ****        ****    ****       ****       ****           ****                               
                          ****          ****                ****                  ****           ****                               
                          ****          ****                ****                  ****           ****                               
 ***********************  ****          ****      *******   ****              ************   ************   *********************** 
 ***********************  ****          ****      *******   ****              ************   ************   *********************** 
                          ****          ****       ******   ****       ****       ****           ****                               
                          ***********    ****     **** **    ****     ****        ****           ****                               
                          ***********     ***********         ***********         ****           ****                               
                          ***********        *****               *****                                                              



*********************************************************************************************************************************** 



LGC++ v1.13.00 compiled against SurveyLib v3.03.07 on Oct 12 2015
Copyright 2003-2013, CERN SU. All rights reserved.
*********************************************************************************************************************************** 
Testing Input Matrices Filler.


CALCUL DU 12 October 2015 17:04:43
*********************************************************************************************************************************** 



             LECTURE DES POINTS DE CALAGE
+        OK :

             NOMBRE=    3


             LECTURE DES POINTS VARIABLES EN XYZ
+        OK :

             NOMBRE=    1


             LECTURE DES ANGLES HORIZONTAUX
+        OK :

             NOMBRE=    3


             LECTURE DES DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE
+        OK :

             NOMBRE=    3


             LECTURE DES DISTANCES MESUREES
+        OK :

             NOMBRE=    3


*** FIN DE FICHIER ***

             NOMBRE D'OBSERVATIONS =  9

             NOMBRE D'INCONNUES =     4

             NUMBER OF ITERATIONS =     2




SIGMA ZERO A POSTERIORI =     0.1266491 , VALEUR CRITIQUE = ( 0.40773,  1.60203)

LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A POSTERIORI



POINTS DE CALAGE (NB. = 3,  REFERENTIEL = SPHE )

       NOM             X              Y              Z            H(G)      SX       SY       SZ       DX       DY       DZ  
                      (M)            (M)            (M)            (M)     (MM)     (MM)     (MM)     (MM)     (MM)     (MM) 

STN          2260.7773700   4493.9461700   2401.7426700    402.1977888                                                       

REF          2203.0482608   4412.2923345   2401.7757411    402.1985736                                                       

STN2         2284.7021029   4354.5632280   2401.7941098    402.1993583                                                       



POINTS VARIABLE EN XYZ (NB. = 1,  REFERENTIEL = SPHE )

       NOM             X              Y              Z            H(G)      SX       SY       SZ       DX       DY       DZ  
                      (M)            (M)            (M)            (M)     (MM)     (MM)     (MM)     (MM)     (MM)     (MM) 

PT           2342.4312257   4436.2170829   2401.7613528    402.1988877   0.0873   0.1060   0.0995   0.0136   0.0194   0.3142 





ANGLES HORIZONTAUX (NB. = 3 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU    ECART      RES      DISTANCE            V0 
                             (GRAD)     (CC)        (GRAD)     (CC)     (MM)     /SIG           (M)        (GRAD) 

STN        STN2          60.0000100    5.000   189.1780423   -0.050  -0.0111     -0.0         141.4   129.1780373 

STN        REF          110.0000000    5.000   239.1780423    0.050   0.0079      0.0         100.0   129.1780373 

STN        PT             9.9999900    5.000   139.1780273    0.000   0.0000      0.0         100.0   129.1780373 

       RESIDU MOYEN =    0.0000  CC :  LIMITES DE CONFIANCE A 95.0 = (-0.1242, 0.1242)
       ECART-TYPE   =    0.0500  CC :  LIMITES DE CONFIANCE A 95.0 = (0.0260, 0.3142) 




DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE (NB. = 3 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU    ECART      RES       HAUTEUR      RALLONGE 
                             (GRAD)     (CC)        (GRAD)     (CC)     (MM)     /SIG       D'INSTR           (M) 
                                                                                                (M) 

STN        STN2         100.0001000    5.000   100.0000000   -1.000  -0.2221     -0.2     0.0000000     0.0000000 

STN        REF          100.0001000    5.000   100.0000000   -1.000  -0.1571     -0.2     0.0000000     0.0000000 

STN        PT            99.9998000    5.000    99.9998000   -0.000  -0.0000     -0.0     0.0000000     0.0000000 

       RESIDU MOYEN =   -0.6667  CC :  LIMITES DE CONFIANCE A 95.0 = (-1.4342, 1.4342)
       ECART-TYPE   =    0.5774  CC :  LIMITES DE CONFIANCE A 95.0 = (0.3006, 3.6285) 




DISTANCES MESUREES (NB. = 3 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU  RES/SIG    SENSI       HAUTEUR       HAUTEUR    CONST   SCONST 
                                (M)     (MM)           (M)     (MM)            BILITE        D'INST      DE PRISM     (MM)     (MM) 
                                                                              (MM/CM)           (M)           (M) 

STN        STN2         141.4213562   0.5007   141.4213562  -0.0000    -0.00     0.00     0.0000000     0.0000000   0.0000   0.0000 

STN        REF          100.0000000   0.5005   100.0000000   0.0000     0.00     0.00     0.0000000     0.0000000   0.0000   0.0000 

STN        PT           100.0000000   0.5005   100.0000000   0.0000     0.00     0.00     0.0000000     0.0000000   0.0000   0.0000 

       RESIDU MOYEN =  -0.00000  MM :  LIMITES DE CONFIANCE A 95.0 = (-0.00000, 0.00000)
       ECART-TYPE   =   0.00000  MM :  LIMITES DE CONFIANCE A 95.0 = (0.00000, 0.00000) 




