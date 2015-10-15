

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
DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !
Testing Input Matrices Filler.


CALCUL DU 13 October 2015 08:35:26
*********************************************************************************************************************************** 



             LECTURE DES POINTS DE CALAGE
+        OK :

             NOMBRE=    3


             LECTURE DES POINTS VARIABLES EN XYZ
+        OK :

             NOMBRE=    2


             LECTURE DES ANGLES HORIZONTAUX
+        OK :

             NOMBRE=    6


             LECTURE DES DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE
+        OK :

             NOMBRE=    6


             LECTURE DES DISTANCES MESUREES
+        OK :

             NOMBRE=    6


*** FIN DE FICHIER ***

             NOMBRE D'OBSERVATIONS =  18

             NOMBRE D'INCONNUES =     8

             NUMBER OF ITERATIONS =     2




SIGMA ZERO A POSTERIORI =     0.6333129 , VALEUR CRITIQUE = ( 0.56982,  1.43119)

LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)



POINTS DE CALAGE (NB. = 3,  REFERENTIEL = SPHE )

       NOM             X              Y              Z            H(G)      SX       SY       SZ       DX       DY       DZ  
                      (M)            (M)            (M)            (M)     (MM)     (MM)     (MM)     (MM)     (MM)     (MM) 

STN          2260.7773700   4493.9461700   2401.7426700    402.1977888                                                       

REF          2203.0482608   4412.2923345   2401.7757411    402.1985736                                                       

STN2         2284.7021029   4354.5632280   2401.7941098    402.1993583                                                       



POINTS VARIABLE EN XYZ (NB. = 2,  REFERENTIEL = SPHE )

       NOM             X              Y              Z            H(G)      SX       SY       SZ       DX       DY       DZ  
                      (M)            (M)            (M)            (M)     (MM)     (MM)     (MM)     (MM)     (MM)     (MM) 

PT           2342.4312653   4436.2168464   2401.7610521    402.1985869   0.6895   0.8369   0.7854   0.0532  -0.2172   0.0134 

PT2          2283.7024001   4353.5635281   2401.7945077    402.1993576   0.3537   0.3537   0.0111   0.2972   0.3000  -0.0008 





ANGLES HORIZONTAUX (NB. = 6 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU    ECART      RES      DISTANCE            V0 
                             (GRAD)     (CC)        (GRAD)     (CC)     (MM)     /SIG           (M)        (GRAD) 

STN        STN2         189.1779908    5.000   189.1780423   -0.762  -0.1694     -0.2         141.4     0.0001278 

STN        REF          239.1778383    5.000   239.1780423    0.762   0.1198      0.2         100.0     0.0001278 

STN        PT           139.1780079    5.000   139.1781357    0.000   0.0000      0.0         100.0     0.0001278 


STN2       STN          389.1781395    5.000   389.1780424   -0.646  -0.1435     -0.1         141.4   399.9999675 

STN2       REF          339.1780103    5.000   339.1780424    0.646   0.1015      0.1         100.0   399.9999675 

STN2       PT2          250.0001218    5.000   250.0000893   -0.000  -0.0000     -0.0           1.4   399.9999675 

       RESIDU MOYEN =   -0.0000  CC :  LIMITES DE CONFIANCE A 95.0 = (-0.6633, 0.6633)
       ECART-TYPE   =    0.6320  CC :  LIMITES DE CONFIANCE A 95.0 = (0.3945, 1.5501) 




DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE (NB. = 6 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU    ECART      RES       HAUTEUR      RALLONGE 
                             (GRAD)     (CC)        (GRAD)     (CC)     (MM)     /SIG       D'INSTR           (M) 
                                                                                                (M) 

STN        STN2          99.9999882    5.000   100.0000000    0.118   0.0262      0.0     0.0000000     0.0000000 

STN        REF           99.9999354    5.000   100.0000000    0.646   0.1015      0.1     0.0000000     0.0000000 

STN        PT            99.9999915    5.000    99.9999915    0.000   0.0000      0.0     0.0000000     0.0000000 


STN2       STN          100.0013861    5.000   100.0014131    0.270   0.0599      0.1     0.0000000     0.0000000 

STN2       REF          100.0009627    5.000   100.0009992    0.365   0.0573      0.1     0.0000000     0.0000000 

STN2       PT2          100.0000393    5.000   100.0000393   -0.000  -0.0000     -0.0     0.0000000     0.0000000 

       RESIDU MOYEN =    0.2331  CC :  LIMITES DE CONFIANCE A 95.0 = (-0.2618, 0.2618)
       ECART-TYPE   =    0.2494  CC :  LIMITES DE CONFIANCE A 95.0 = (0.1557, 0.6118) 




DISTANCES MESUREES (NB. = 6 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU  RES/SIG    SENSI       HAUTEUR       HAUTEUR    CONST   SCONST 
                                (M)     (MM)           (M)     (MM)            BILITE        D'INST      DE PRISM     (MM)     (MM) 
                                                                              (MM/CM)           (M)           (M) 

STN        STN2         141.4219572   0.5007   141.4213562  -0.6010    -1.20     0.00     0.0000000     0.0000000   0.0000   0.0000 

STN        REF          100.0003740   0.5005   100.0000000  -0.3740    -0.75     0.00     0.0000000     0.0000000   0.0000   0.0000 

STN        PT           100.0001688   0.5005   100.0001688   0.0000     0.00     0.00     0.0000000     0.0000000   0.0000   0.0000 

STN2       STN          141.4213366   0.5007   141.4213562   0.0196     0.04    -0.00     0.0000000     0.0000000   0.0000   0.0000 

STN2       REF           99.9993092   0.5005   100.0000000   0.6908     1.38    -0.00     0.0000000     0.0000000   0.0000   0.0000 

STN2       PT2            1.4137913   0.5000     1.4137913   0.0000     0.00     0.00     0.0000000     0.0000000   0.0000   0.0000 

       RESIDU MOYEN =  -0.04409  MM :  LIMITES DE CONFIANCE A 95.0 = (-0.46150, 0.46150)
       ECART-TYPE   =   0.43976  MM :  LIMITES DE CONFIANCE A 95.0 = (0.27450, 1.07857) 




