

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



LGC++ v1.13.00 compiled against SurveyLib v3.03.07 on Sep  4 2015
Copyright 2003-2013, CERN SU. All rights reserved.
*********************************************************************************************************************************** 
DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !
LGC1


CALCUL DU 18 September 2015 10:01:10
*********************************************************************************************************************************** 



             LECTURE DES POINTS DE CALAGE
+        OK :

             NOMBRE=    2


             LECTURE DES POINTS VARIABLES EN XYZ
+        OK :

             NOMBRE=    2


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

             NOMBRE D'INCONNUES =     7

             NUMBER OF ITERATIONS =     2




SIGMA ZERO A POSTERIORI =     0.2868806 , VALEUR CRITIQUE = ( 0.15912,  1.92065)

LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)



POINTS DE CALAGE (NB. = 2,  REFERENTIEL = LOCAL )

       NOM             X              Y              Z       SX       SY       SZ       DX       DY       DZ  
                      (M)            (M)            (M)     (MM)     (MM)     (MM)     (MM)     (MM)     (MM) 

STN             0.0000000      0.0000000      0.0000000                                                       

REF           100.0000000      0.0000000      0.0000000                                                       



POINTS VARIABLE EN XYZ (NB. = 2,  REFERENTIEL = LOCAL )

       NOM             X              Y              Z       SX       SY       SZ       DX       DY       DZ  
                      (M)            (M)            (M)     (MM)     (MM)     (MM)     (MM)     (MM)     (MM) 

PT2           100.0002037     99.9999800     -0.0010243   1.1818   1.1818   1.1107   0.2037  -0.0200  -1.0243 

PT             -0.0010521     99.9995687      0.0015705   1.1107   0.5500   0.7854  -1.0521  -0.4313   1.5705 





ANGLES HORIZONTAUX (NB. = 3 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU    ECART      RES      DISTANCE            V0 
                             (GRAD)     (CC)        (GRAD)     (CC)     (MM)     /SIG           (M)        (GRAD) 

STN        PT2           50.0001917    5.000    50.0000712   -0.000  -0.0000     -0.0         141.4   399.9998795 

STN        REF          100.0001205    5.000   100.0000000    0.000   0.0000      0.0         100.0   399.9998795 

STN        PT           399.9994507    5.000   399.9993302    0.000   0.0000      0.0         100.0   399.9998795 

       RESIDU MOYEN =    0.0000  CC :  LIMITES DE CONFIANCE A 95.0 = (-0.0000, 0.0000)
       ECART-TYPE   =    0.0000  CC :  LIMITES DE CONFIANCE A 95.0 = (0.0000, 0.0000) 




DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE (NB. = 3 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU    ECART      RES       HAUTEUR      RALLONGE 
                             (GRAD)     (CC)        (GRAD)     (CC)     (MM)     /SIG       D'INSTR           (M) 
                                                                                                (M) 

STN        PT2          100.0004611    5.000   100.0004611   -0.000  -0.0000     -0.0     0.0000000     0.0000000 

STN        REF          100.0002025    5.000   100.0000000   -2.025  -0.3181     -0.4     0.0000000     0.0000000 

STN        PT            99.9990002    5.000    99.9990002    0.000   0.0000      0.0     0.0000000     0.0000000 

       RESIDU MOYEN =   -0.6750  CC :  LIMITES DE CONFIANCE A 95.0 = (-2.9043, 2.9043)
       ECART-TYPE   =    1.1691  CC :  LIMITES DE CONFIANCE A 95.0 = (0.6087, 7.3477) 




DISTANCES MESUREES (NB. = 3 ) 

POINT 1    POINT 2          OBSERVE    SIGMA       CALCULE   RESIDU  RES/SIG    SENSI       HAUTEUR       HAUTEUR    CONST   SCONST 
                                (M)     (MM)           (M)     (MM)            BILITE        D'INST      DE PRISM     (MM)     (MM) 
                                                                              (MM/CM)           (M)           (M) 

STN        PT2          141.4214861   0.5707   141.4214861   0.0000     0.00    -0.00     0.0000000     0.0000000   0.0000   0.0000 

STN        REF          100.0000132   0.5500   100.0000000  -0.0132    -0.02     0.00     0.0000000     0.0000000   0.0000   0.0000 

STN        PT            99.9995687   0.5500    99.9995687  -0.0000    -0.00     0.00     0.0000000     0.0000000   0.0000   0.0000 

       RESIDU MOYEN =  -0.00440  MM :  LIMITES DE CONFIANCE A 95.0 = (-0.01893, 0.01893)
       ECART-TYPE   =   0.00762  MM :  LIMITES DE CONFIANCE A 95.0 = (0.00397, 0.04790) 




