

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



LGC++ v1.13.00 compiled against SurveyLib v3.03.07 on Sep 21 2015
Copyright 2003-2013, CERN SU. All rights reserved.
*********************************************************************************************************************************** 
Testing Input Matrices Filler.


CALCUL DU 22 September 2015 13:31:36
*********************************************************************************************************************************** 



             LECTURE DES POINTS DE CALAGE
+        OK :

             NOMBRE=    4


             LECTURE DES POINTS VARIABLES EN XYZ
+        OK :

             NOMBRE=    1


             LECTURE DES ANGLES HORIZONTAUX
+        OK :

             NOMBRE=    4


             LECTURE DES DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE
+        OK :

             NOMBRE=    4


             LECTURE DES DISTANCES MESUREES
+        OK :

             NOMBRE=    4


*** FIN DE FICHIER ***

             NOMBRE D'OBSERVATIONS =  12

             NOMBRE D'INCONNUES =     4

             NUMBER OF ITERATIONS =     5




SIGMA ZERO A POSTERIORI =   215.75264 , VALEUR CRITIQUE = (0.52198, 1.48048)

LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A POSTERIORI



POINTS DE CALAGE (NB. = 4,  REFERENTIEL = LOCAL )

       NOM           X            Y            Z      SX      SY      SZ      DX      DY      DZ  
                    (M)          (M)          (M)    (MM)    (MM)    (MM)    (MM)    (MM)    (MM) 

STN             0.00000      0.00000      0.00000                                                 

P1              0.00000    100.00000    100.00000                                                 

P3            100.00000    200.00000    200.00000                                                 

P4              0.00000    200.00000    200.00000                                                 



POINTS VARIABLE EN XYZ (NB. = 1,  REFERENTIEL = LOCAL )

       NOM           X            Y            Z      SX      SY      SZ      DX      DY      DZ  
                    (M)          (M)          (M)    (MM)    (MM)    (MM)    (MM)    (MM)    (MM) 

P2             99.79174     99.99156     99.89158  237.61  237.45  247.38 -100213.26 -50064.44 -208.42 





ANGLES HORIZONTAUX (NB. = 4 ) 

POINT 1    POINT 2        OBSERVE   SIGMA     CALCULE  RESIDU   ECART     RES    DISTANCE          V0 
                           (GRAD)    (CC)      (GRAD)    (CC)    (MM)    /SIG         (M)      (GRAD) 

STN        P1             0.04506     5.0     0.00000   177.2   27.84    35.4       100.0   399.93722 

STN        P2            49.99911     5.0    49.93633     0.0    0.00     0.0       141.3   399.93722 

STN        P3            29.51324     5.0    29.51672   662.7  232.75   132.5       223.6   399.93722 

STN        P4             0.14677     5.0     0.00000  -839.9 -263.86  -168.0       200.0   399.93722 

       RESIDU MOYEN =     0.00  CC :  LIMITES DE CONFIANCE A 95.0 = (-996.23, 996.23)
       ECART-TYPE   =   626.08  CC :  LIMITES DE CONFIANCE A 95.0 = (354.67, 2334.36) 




DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE (NB. = 4 ) 

POINT 1    POINT 2        OBSERVE   SIGMA     CALCULE  RESIDU   ECART     RES     HAUTEUR    RALLONGE 
                           (GRAD)    (CC)      (GRAD)    (CC)    (MM)    /SIG     D'INSTR         (M) 
                                                                                      (M) 

STN        P1            49.99345     5.0    50.00000    65.5   14.55    13.1     0.00000     0.00000 

STN        P2            60.81738     5.0    60.81738     0.0    0.00     0.0     0.00000     0.00000 

STN        P3            53.53918     5.0    53.54409    49.1   23.16     9.8     0.00000     0.00000 

STN        P4            50.00874     5.0    50.00000   -87.4  -38.83   -17.5     0.00000     0.00000 

       RESIDU MOYEN =     6.81  CC :  LIMITES DE CONFIANCE A 95.0 = (-109.32, 109.32)
       ECART-TYPE   =    68.70  CC :  LIMITES DE CONFIANCE A 95.0 = (38.92, 256.15) 




DISTANCES MESUREES (NB. = 4 ) 

POINT 1    POINT 2        OBSERVE   SIGMA     CALCULE  RESIDU RES/SIG   SENSI     HAUTEUR     HAUTEUR   CONST  SCONST 
                              (M)    (MM)         (M)    (MM)          BILITE      D'INST    DE PRISM    (MM)    (MM) 
                                                                      (MM/CM)         (M)         (M) 

STN        P1           141.66117    0.50   141.42136 -239.81 -478.95    7.07     0.00000     0.00000    0.00    0.00 

STN        P2           173.01743    0.50   173.01743    0.00    0.00    5.77     0.00000     0.00000    0.00    0.00 

STN        P3           299.90492    0.50   300.00000   95.08  189.59    6.67     0.00000     0.00000    0.00    0.00 

STN        P4           282.72046    0.50   282.84271  122.25  243.82    7.07     0.00000     0.00000    0.00    0.00 

       RESIDU MOYEN =   -5.620  MM :  LIMITES DE CONFIANCE A 95.0 = (-262.061, 262.061)
       ECART-TYPE   =  164.692  MM :  LIMITES DE CONFIANCE A 95.0 = (93.296, 614.061) 




