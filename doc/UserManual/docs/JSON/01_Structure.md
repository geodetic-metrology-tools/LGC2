# Structure

The structure of the JSON file is presented and described in sections. For each section, an example is provided, accompanied with the description of the field and the unit (where relevant).

## Main structure

The first level of the JSON structure contains information relevant to the software version and the processing time, and the main sub-structure with all the essential data concerning the calculation.

```json
{
    "fCopyright": "Copyright 2022, CERN SU. All rights reserved."       LGC copyright declaration.                         
    "LGCVersion": "v2.8.0",                                             Version of LGC that produced the current file.     
    "startProcessingTimestamp": "2023-10-25T23:23:56+0200",             The exact timestamp that the process started.       format: ISO 8601
    "processingElapsedSeconds": 4.3585863,                              The duration that the process lasted.               unit: s
    "LGC_DATA": {}                                                      Structure of the LGC data. 
}
```

## LGC data structure

The second level of the JSON structure contains various fields that store information relevant to the calculation, such as observations, uncertainties, the frame tree, the point coordinates, the statistics, user-defined configuration options, etc.

```json
"LGC_DATA":{
    "angles": [],                           List of the adjustable angles.
    "config": {},                           Structure of the configuration parameters of the input file.
    "fLSRelatedInfo": {},                   Structure of the information related to the LS adjustment.
    "fMeasInfo": {},                        Structure of the information about the number of the measurements for each observation type.
    "fPointInfo": {},                       Structure of the information about the number of points for each constraint type.
    "fRelErrors": {},                       structure holding lists of relative errors between points (*EREL) and between frames (*ERELFRAME)
    "fUEOIndices": {},                      Structure of the information about the number of unknowns, equations, observations and constraints.
    "instruments": {},                      Structure of the information concerning the instruments.
    "islgc1": false,                        Indicates whether the input file structure is according to LGC1 (true) or LGC2 (false) standards.
    "lengths": [],                          List of the adjustable lengths.
    "lines": [],                            List of the adjustable lines.
    "planes": [],                           List of the adjustable planes.
    "points": [],                           List of the adjustable points.
    "stat": {},                             Structure of information related to the statistics of the LS adjustment.
    "tree": [],                             List of the frames. This structure includes the information related to the transformation parameters and the observations of the frames.
    "slaveGroups": []                       List of the slave groups.
    "pointConstraintGroups": []             List of point constraint groups.
}
```

### Adjustable angles

List of the adjustable angles, e.g., the horizontal orientation (V0) of a total station.

```json
"angles": [{
    "active_": true,                                    Indicates the activation status of the adjustable angle.                           
    "fName": "ROOTV00",                                 Name of the adjustable angle (automatically assigned).                             
    "fProvisionalValue": 0.0,                           Angle provisional value.                                                            unit: rad
    "fEstimatedValue": -1.3225917804971787,             Angle estimated value.                                                              unit: rad
    "fCorrection": 2.773066535069805e-12,               Angle correction during the last iteration.                                         unit: rad          
    "fEstimatedPrecision": 2.2865017292931177e-06,      Angle estimated precision.                                                          unit: rad
    "ifFixed": false,                                   Indicates whether angle is fixed (true) or adjustable (false).
    "uidx": 83                                          Index of the adjustable angle parameter in the LS design matrix (-1 if fixed).                         
    },
    {...}
]
```

### Configuration information

Please see the [configuration structure](https://confluence.cern.ch/display/SUS/JSON#JSON-JSON-configStructure) section. <<<----- TDOOOOOOO

### Least-squares related information structure

Structure of information related to the LS adjustment.

```json
"fLSRelatedInfo": {
    "fNumberOfLSIterations": 9,                 Number of iterations of the LS adjustment.
    "fS0APosteriori": 0.9228902883366865,       Sigma zero a posteriori.
    "fChiLoLimit": 0.9313835412572457,          Percentile of the chi squared distribution to be used as lower limit for the statistical test of sigma zero a posteriori.
    "fChiUpLimit": 1.0685538047783332           Percentile of the chi squared distribution to be used as upper limit for the statistical test of sigma zero a posteriori.
}
```

### Measurement information

Structure of the information about the number of the measurements for each observation type. Please see the [measurements section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-Measurements) of the LGC2 User Guide. <<<----- TDOOOOOOO

```json
"fMeasInfo": {
    "fNumANGL": 73,             Number of observations of the type ANGL that participate in the LS adjustment.
    "fNumZEND": 71,             Number of observations of the type ZEND that participate in the LS adjustment.
    "fNumDIST": 74,             Number of observations of the type DIST that participate in the LS adjustment.
    "fNumPLR3D": 0,             Number of observations of the type PLR3D that participate in the LS adjustment.
    "fNumDLEV": 0,              Number of observations of the type DLEV that participate in the LS adjustment.
    "fNumDHOR": 0,              Number of observations of the type DHOR that participate in the LS adjustment.
    "fNumECTH": 0,              Number of observations of the type ECTH that participate in the LS adjustment.
    "fNumECDIR": 0,             Number of observations of the type ECDIR that participate in the LS adjustment.
    "fNumDSPT": 0,              Number of observations of the type DSPT that participate in the LS adjustment.
    "fNumDVER": 1584,           Number of observations of the type DVER that participate in the LS adjustment.
    "fNumUVEC": 0,              Number of observations of the type UVEC that participate in the LS adjustment.
    "fNumUVD": 0,               Number of observations of the type UVD  that participate in the LS adjustment.
    "fNumECHO": 0,              Number of observations of the type ECHO that participate in the LS adjustment.
    "fNumECSP": 0,              Number of observations of the type ECSP that participate in the LS adjustment.
    "fNumECVE": 0,              Number of observations of the type ECVE that participate in the LS adjustment.
    "fNumORIE": 0,              Number of observations of the type ORIE that participate in the LS adjustment.
    "fNumPDOR": 0,              Number of observations of the type PDOR that participate in the LS adjustment.
    "fNumRADI": 0,              Number of observations of the type RADI that participate in the LS adjustment.
    "fNumOBSXYZ": 21,           Number of observations of the type OBSXYZ that participate in the LS adjustment.
    "fNumINCLY": 1,             Number of observations of the type INCLY that participate in the LS adjustment.
    "fNumECWS": 0,              Number of observations of the type ECWS that participate in the LS adjustment.
    "fNumECWI": 0               Number of observations of the type ECWI that participate in the LS adjustment.
}
```

### Point information

Structure of the information about the number of points for each constraint type. Please see the [points section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-Pointssection) of the LGC2 User Guide. <<<----- TDOOOOOOO

```json
"fPointInfo": {
    "fNumCala": 1,              Number of points declared as CALA.
    "fNumVx": 0,                Number of points declared as VX.
    "fNumVy": 0,                Number of points declared as VY.
    "fNumVz": 0,                Number of points declared as VZ.
    "fNumVxy": 0,               Number of points declared as VXY.
    "fNumVxz": 0,               Number of points declared as VXZ.
    "fNumVyz": 1,               Number of points declared as VYZ.
    "fNumVxyz": 27              Number of points declared as VXYZ.
}
```

### Relative errors

List of user-defined relative error ellipses. Please see the [relative error (EREL) section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-EREL) of the LGC2 User Guide.

```json
"fRelErrorPoints": [{
    "fPoint1": "Q1EW",                                      Name of the first point.
    "fPoint2": "D1EW",                                      Name of the second point.
    "fSigmaL": {"fValue": 1.528602119059809e-05},           Longitudinal error.                                 unit: m
    "fSigmaG": {"fValue": 3.1245839069304453e-06},          Horizontal orientation error.                       unit: rad
    "fSigmaR": {"fValue": 2.195768320885386e-05},           Radial (transversal) error.                         unit: m
    "fSigmaZ": {"fValue": 1.790589484306688e-05},           Height difference error.                            unit: m
    "fSigmaV": {"fValue": 2.546867160062765e-06},           Vertical angle error.                               unit: rad
    "fDestinationFrame": "ROOT"                             Frame name that the error values are valid.
    },
    {...}
]
```

List of user-defined relative frame transformations and their precisions. Please see the [(ERELFRAME) section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-ERELFRAME) of the LGC2 User Guide.

```json
"fRelErrorFrames": [{
    "fFromFrame": "startFrame",                             Name of the start frame.
    "fToFrame": "destinationFrame",                         Name of the destination frame.
    "fIsComputed": true,                                    boolean indicating if error is computed.
    "fRelativeTransformation": {},                          relative Frame transformation from startFrame to destinationFRame.
    },
    {...}
]
```

### Number of unknowns, equations, observations and constraints

Structure of the information about the number of unknowns, equations, observations and constraints.

```json
"fUEOIndices": {
    "UIndex": 89,               Number of unknown parameters in the LS adjustment.
    "EIndex": 218,              Number of equations in the LS adjustment.
    "OIndex": 218,              Number of observations in the LS adjustment.
    "CIndex": 0                 Number of constraints in the LS adjustment.
}
```

### Instrument information

Structure of the information concerning the instruments. Please see the [instruments section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-Instruments) of the LGC2 User Guide.

Please see also the [instrument structure](https://confluence.cern.ch/display/SUS/JSON#JSON-JSON-instrStructure) section.

```json
"instruments": {
      "fCAMD": [],                    List of camera instruments.
      "fEDM": [],                     List of electronic distance measuring (EDM) instruments.
      "fHLSR": [],                    List of hydrostatic leveling sensor (HLS) instruments.
      "fINCL": [],                    List of inclination measuring (e.g., inclinometer) instruments.
      "fLEVEL": [],                   List of leveling (e.g., optical and digital level) instruments.
      "fPOLAR": [],                   List of polar (e.g., total station, laser tracker) instruments.
      "fSCALE": [],                   List of scale instruments.
      "fWPSR": []                     List of wire positioning sensor (WPS) instruments.
  }
```

### Adjustable lengths

List of the adjustable lengths, e.g., the systematic error of an electronic distance measuring (EDM) instrument.

```json
"lengths": [{
    "active_": true,                                    Indicates the activation status of the adjustable length.                          
    "fName": "TSTNROOTTS160",                           Name of the adjustable length (automatically assigned).                            
    "fProvisionalValue": 0.0,                           Length provisional value.                                                           unit: m
    "fEstimatedValue": 0.0,                             Length estimated value.                                                             unit: m
    "fCorrection": 0.0,                                 Length correction during the last iteration.                                        unit: m            
    "fEstimatedPrecision": 0.0,                         Length estimated precision.                                                         unit: m
    "ifFixed": true,                                    Indicates whether the length is fixed (true) or adjustable (false).
    "uidx": -1                                          Index of the adjustable length parameter in the LS design matrix (-1 if fixed).                        
    },
    {...}
]
```

### Adjustable lines

List of the adjustable lines.

```json
"lines": [{                                    
        "active_": true,                            Indicates the activation status of the adjustable line.
        "fInit": true,                              Indicates whether the adjustable line is initialized.
        "fixedStateLineVectorX": true,              Indicates whether the X coefficient of the vector is fixed (true) or adjustable (false).
        "fixedStateLineVectorY": true,              Indicates whether the Y coefficient of the vector is fixed (true) or adjustable (false).
        "fixedStateLineVectorZ": true,              Indicates whether the Z coefficient of the vector is fixed (true) or adjustable (false).
        "fLinePoint": {},                           The reference point of the line (see the adjustable point section).
        "fLineVectorCorrection": {                 
            "fVector": [0.0, 0.0, 0.0],             Vector coefficient correction during the last iteration [dX, dY, dZ].                                               unit: [m, m, m]
            "fCoordSys": {"fCoordSysId": 1}         Indicates the coordinate system of the vector coefficient corrections (see Table 2).                                     
        },                                         
        "fLineVectorCovariance": {                 
            "fVector": [0.0, 0.0, 0.0],             Vector coefficient covariance elements [Cxy, Cyz, Cxz].                                                             unit: [m*m, m*m, m*m]
            "fCoordSys": {"fCoordSysId": 1}         Indicates the coordinate system of the vector coefficient covariance elements (see Table 2).                                    
        },                                         
        "fLineVectorEstimatedPrecision": {          Estimated precision of the vector coefficient in the FRAME that the line is defined.
            "fVector": [0.0, 0.0, 0.0],             Square root of the diagonal of the 3x3 variance-covariance matrix in the FRAME that the line is defined.    
            "fCoordSys": {"fCoordSysId": 1}         Indicates the coordinate system of the vector coefficient corrections (see Table 2).                                     
        },                                         
        "fLineVectorEstimatedValue": {              Estimated vector coefficients in the FRAME that the line is defined.                        
            "fVector": [0.0, 0.0, 1.0],             [Vx FRAME, Vy FRAME, Vz FRAME]                                                                                      unit: [m, m, m]
            "fCoordSys": {"fCoordSysId": 1}         Indicates the coordinate system of the vector coefficient corrections (see Table 2).                                     
        },                                         
        "fLineVectorProvisionalValue": {            Provisional vector coefficients in the FRAME that the line is defined.                      
            "fVector": [0.0, 0.0, 1.0],             [Vx FRAME, Vy FRAME, Vz FRAME]                                                                                      unit: [m, m, m]
            "fCoordSys": {"fCoordSysId": 1}         Indicates the coordinate system of the vector coefficient corrections (see Table 2).                               
        },                                         
        "fName": "ECVELINE0",                       Name of the adjustable line (automatically assigned).
        "fReferencePointPosition": {                Estimated coordinates of the reference point in the FRAME that the line is defined. 
            "fVector": [0.0, 0.0, 50.0],            [X FRAME, Y FRAME, Z FRAME]                                                                                         unit: [m, m, m]
            "fCoordSys": {"fCoordSysId": 1}         Indicates the coordinate system of the vector coefficient corrections (see Table 2).                                   
        },                                         
        "uidx_lineVector": [-1, -1, -1]             Indices of the vector coefficients for the LS adjustment (-1 when the coordinate is fixed).     
    },
    {...}
    ]
```

### Adjustable planes

List of the adjustable planes.

```json
"planes": [{
        "active_": true,                                        Indicates the activation status of the adjustable plane.
        "fCorrectionPhi": 0.0,                                  Phi angle correction during the last iteration.                                                     unit: rad
        "fCorrectionRefPtDist": -9.33152386156877e-08,          Plane reference point distance correction during the last iteration.                                unit: m
        "fCorrectionTheta": 7.541031894720562e-10,              Theta angle correction during the last iteration.                                                   unit: rad
        "fEstPrecisionPhi": 0.0,                                Phi angle estimated precision (1 sigma).                                                            unit: rad
        "fEstPrecisionRefPtDist": 1.587900820924855e-05,        Plane reference point distance estimated precision (1 sigma).                                       unit: m
        "fEstPrecisionTheta": 1.112504941780281e-06,            Theta angle estimated precision (1 sigma).                                                          unit: rad
        "fEstValPhi": 1.5707963267948966,                       Phi angle estimated value.                                                                          unit: rad
        "fEstValRefPointDist": 0.33643156851691103,             Plane reference point distance estimated value.                                                     unit: m
        "fEstValTheta": 1.512088166516771,                      Theta angle estimated value.                                                                        unit: rad
        "fInitialized": true,                                   Indicates whether the adjustable plane is initialized.
        "fName": "ECHOPLANE0",                                  Name of the adjustable plane (automatically assigned).
        "fPhiFixed": true,                                      Indicates whether the phi angle is fixed (true) or adjustable (false).
        "fProvValPhi": 1.5707963267948966,                      Phi angle provisional value.                                                                        unit: rad
        "fProvValRefPtDist": 0.33637500000000004,               Plane reference point distance provisional value.                                                   unit: m
        "fProvValTheta": 1.5101532743271875,                    Theta angle provisional value.                                                                      unit: rad
        "fReferencePoint": {},                                  The reference point of the plane (see the adjustable point section).
        "fRefPtDistFixed": false,                               Indicates whether the reference point distance is fixed (true) or adjustable (false).
        "fThetaFixed": false,                                   Indicates whether the theta angle is fixed (true) or adjustable (false).
        "uidx_Phi": -1,                                         Index of the phi angle for the LS adjustment (-1 when the coordinate is fixed).
        "uidx_rpDistance": 17,                                  Index of the reference point distance for the LS adjustment (-1 when the coordinate is fixed).
        "uidx_Theta": 16                                        Index of the theta angle for the LS adjustment (-1 when the coordinate is fixed).
    },
    {...}
    ]
```

### Adjustable points

Please see the [point structure](https://confluence.cern.ch/display/SUS/JSON#JSON-JSON-pointStructure) section.

### Statistics

Structure of information related to the statistics of the LS adjustment.

```json
"stat": {
    "fAreDetermined": [0.0, 0.0, 1.0],                                                                  Indicates whether the error detection parameters are computed.
    "fDegreesOfFreedom": 2.0,                                                                           Network degrees of freedom.
    "fDeltaComputed": [0.0, 0.0, 1.0],                                                                  Indicates whether the parameters for statistic tests have been rightly chosen.
    "fDelty": [7.421631801369068e+97, 1.487445319933428e-76, 0.0],                                      Maximum effect of an undetected gross error.
    "fGToCompute": [0.0, 0.0, 0.0],                                                                     Indicates whether G needs to be computed.
    "fGValue": [1.1126068713746256e-306, 2.1138744912903637e-307, 1.3796218457330131e-306],             Value for estimation of gross error made with statistic wi.
    "fNablaValue": [6.9514731894593e-310, 6.95147318896603e-310, 4.3861811998827625e-05],               Value for greatest undetected error.
    "fOverall": null,                                                                                   Global network reliability.
    "fT": [2.121995791e-314, 6.365987374e-314, 1.0],                                                    Level of ease a gross error of size NABLA can be detected.
    "fW": [5e-324, 2.5e-323, -0.3491826456202583],                                                      Gross error detection statistic.
    "fWToCompute": true,                                                                                Indicates whether the wi parameter is computed. False in case of a simulation.
    "fZ": [4.440892098500675e-16, null, 0.9999999999999999]                                             Local reliability statistic.
}
```

### Tree

List of the frames. This structure includes the information related to the transformation parameters and the observations of the frames.

**The order of the parameters is always TX, TY , TZ, RX , RY, RZ, SCALE.**

Please see also the [measurements structure](https://confluence.cern.ch/display/SUS/JSON#JSON-JSON-measStructure) section.

```json
"tree": [{
        "ID": [1],                                                                                                 
        "branch": ["ROOT"],                                                                  ID of the current FRAME.                                                                                           
        "frame": {                                                                           List of the nodes of the branch from ROOT to the current FRAME.                                                    
            "active_": true,                                                                 Indicates the activation status of the current FRAME.                                                              
            "fCovar": [                                                                      7x7 variance-covariance matrix of the transformation parameters between the current and its parent FRAME.          
                [0, 0, 0, 0, 0, 0, 0],                                                       [CTxTx, CTxTy, CTxTz, CTxRx, CTxRy, CTxRz, CTxSc]                                                                   unit: [m*m, m*m, m*m, m, m, m, m]
                [0, 0, 0, 0, 0, 0, 0],                                                       [CTyTx, CTyTy, CTyTz, CTyRx, CTyRy, CTyRz, CTySc]                                                                   unit: [m*m, m*m, m*m, m, m, m, m]
                [0, 0, 0, 0, 0, 0, 0],                                                       [CTzTx, CTzTy, CTzTz, CTzRx, CTzRy, CTzRz, CTzSc]                                                                   unit: [m*m, m*m, m*m, m, m, m, m]
                [0, 0, 0, 0, 0, 0, 0],                                                       [CRxTx, CRxTy, CRxTz, CRxRx, CRxRy, CRxRz, CRxSc]                                                                   unit: [  m,   m,   m, 1, 1, 1, 1]
                [0, 0, 0, 0, 0, 0, 0],                                                       [CRyTx, CRyTy, CRyTz, CRyRx, CRyRy, CRyRz, CRySc]                                                                   unit: [  m,   m,   m, 1, 1, 1, 1]
                [0, 0, 0, 0, 0, 0, 0],                                                       [CRzTx, CRzTy, CRzTz, CRzRx, CRzRy, CRzRz, CRzSc]                                                                   unit: [  m,   m,   m, 1, 1, 1, 1]
                [0, 0, 0, 0, 0, 0, 0]                                                        [CScTx, CScTy, CScTz, CScRx, CScRy, CScRz, CScSc]                                                                   unit: [  m,   m,   m, 1, 1, 1, 1]
            ],                                                                                                                                                                                                      
            "fCovarianceMatrixIsSet": true,                                                  Indicates whether the covariance matrix is set.
            "fEstParameter": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0],                            Estimated transformation parameter values between the current and its parent FRAME.                                 unit: [m, m, m, rad, rad, rad, 1]
            "fixedTranfParam": [true, true, true, true, true, true, true],                   Indicates whether a parameter is fixed (true) or adjustable (false).
            "fProvParameter": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0],                           Provisional transformation parameter values between the current and its parent FRAME.                               unit: [m, m, m, rad, rad, rad, 1]                                                                                                                          
            "line": -1,                                                                      Line number of the input file where the FRAME is defined.                                                          
            "name": "ROOT",                                                                  Name of the FRAME that the point is defined.                                                                       
            "uidx": [-1, -1, -1, -1, -1, -1, -1]                                             Indices of the transformation parameters for the LS adjustment (-1 when the parameter is fixed).                   
        },                                                                                                                                                                                                      
        "measurements": {}                                                                   Structure of the measurements defined in the current FRAME.                                                        
    },
    {...}
]
```

**Example:**

In the following example the case of a branch with three nodes is presented.

The transformation parameter values and the variance-covariance matrix concern the transformation from "RSTRI_LHC.MBXW.D4L5" frame to "RSTB_LHC.MBXW.D4L5" frame.

```json
{
    "ID": [1, 3, 1],
    "branch": ["ROOT", "RSTB_LHC.MBXW.D4L5", "RSTRI_LHC.MBXW.D4L5"],
    "frame": {
        "active_": true,
        "fCovar": [
            [2.825882933467584e-09, 1.651959978639598e-09, 3.311170786343555e-10, 2.801501519829372e-11, 0, -7.5803614288400e-10, 0],
            [1.651959978639592e-09, 6.042634044648914e-08, 1.402686058023258e-09, -4.93178213837522e-10, 0, 3.50156328240508e-10, 0],
            [3.311170786343555e-10, 1.402686058023257e-09, 3.689798988365857e-09, 1.168375803741239e-09, 0, -4.3956867431468e-11, 0],
            [2.801501519829372e-11, -4.93178213837522e-10, 1.168375803741240e-09, 7.429953771938197e-10, 0, -9.7280412565392e-12, 0],
            [0,                     0,                     0,                     0,                     0, 0,                    0],
            [-7.58036142884007e-10, 3.501563282405079e-10, -4.39568674314685e-11, -9.72804125653923e-12, 0, 4.45403116476682e-10, 0],
            [0,                     0,                     0,                     0,                     0, 0,                    0],
        ],
        "fCovarianceMatrixIsSet": true,
        "fEstParameter": [9.931096774009635e-05, -0.004910809739865271, 0.00890996692590327, 4.3337179696113867e-05, 0.0, -2.783426602517863e-05, 1.0],
        "fixedTranfParam": [false, false, false, false, true, false, true],
        "fProvParameter": [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
        "line": 421,
        "name": "RSTRI_LHC.MBXW.D4L5",
        "uidx": [10, 11, 12, 13, -1, 14, -1]
    },
    "measurements": {
    }
}
```

#### Slave groups

List of the slave groups.

```json
"slaveGroups": [{
    "groupName": "Group1",                          User-defined group name.
    "constraintDim": 3,                             Number of constrains in the group. Depends on the number of frames and the number of unknown parameters in the group of the slave frames.
    "firstCIndex": 0,                               Index of the first constraint in the constraint matrix of the group.
    "slaves": ["frame_1", "frame_4"]                List of the slave frame names.
},
{...}
]
```

#### Point Constraint groups

List of the point constraint groups.

```json
"pointConstraintGroups": [{
    "affectedPoints": ["point_1", "point_4"],                                           List of points affected by the constraint
    "constraints": ["true", "false", "false", "true", "false", "false", "false"]        Imposed constraints on the group, following the order TX, TY, TZ, RX, RY, RZ, SCL.
},
{...}
]
```

## Instrument structure

The information in this structure represents the input data of the [instruments section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-Instruments) (\*INSTR) of the LGC input file.

These values are subjected to changes according to the user input in the [measurements section](https://confluence.cern.ch/display/SUS/LGC2+User+Guide#LGC2UserGuide-Measurements) of the LGC input file.

The actual values that are finally used for the least-squares adjustment are stored in [measurement structure](https://confluence.cern.ch/display/SUS/JSON#JSON-JSON-measStructure).

### CAMD: Camera instrument

List of camera instruments.

```json
"fCAMD": [{                                                                
    "<BCAM-HIE>": {                                     Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "defTarget": "TARG1",                           Name of the instrument default target.
        "ID": "BCAM-HIE",                               Name of the instrument as defined in the instrument section.
        "sigmaInstrCentering": 0.0,                     Instrument centering standard error (1 sigma).                                      unit: m
        "line": 9,                                      Line number of the input file where the instrument is defined.
        "targets": []                                   List of targets for the CAMD instruments.
        }
    },                                                 
    {...}                                              
]
```

#### CAMD target

List of targets for the CAMD instruments.

```json
"targets": [{                                          
    "<TARG1>": {                                        Name of the target as defined in the instrument section. The key in angle brackets '<>' is variable.
        "ID": "TARG1",                                  Name of the target as defined in the instrument section.
        "sigmaCombinedDist": null,                      Combined Z-component standard error (1 sigma).                                      unit: m
        "sigmaCombinedX": null,                         Combined X-component standard error (1 sigma).                                      unit: mm/m
        "sigmaCombinedY": null,                         Combined Y-component standard error (1 sigma).                                      unit: mm/m
        "sigmaDist": 0.0005,                            Z-component standard error (1 sigma).                                               unit: m
        "sigmaTargetCentering": 2e-05,                  Target centering standard error (1 sigma).                                          unit: m
        "sigmaX": 5e-06,                                X-component standard error (1 sigma).                                               unit: mm/m
        "sigmaY": 5e-06,                                Y-component standard error (1 sigma).                                               unit: mm/m
        "line": 10                                      Line number of the input file where the target is defined.                     
        }                                              
    },                                                 
    {...}                                              
]    
```

### EDM: Electronic distance measuring instrument

List of electronic distance measuring (EDM) instruments.

```json
"fEDM": [{                                             
    "<ME5000.357015>": {                                Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "defTarget": "KE.3",                            Name of the instrument default target.
        "ID": "ME5000.357015",                          Name of the instrument as defined in the instrument section.
        "instrHeight": 0.0,                             Instrument height.                                                                  unit: m
        "sigmaInstrCentering": 0.0,                     Instrument centering standard error (1 sigma).                                      unit: m
        "sigmaInstrHeight": 0.0,                        Instrument height standard error (1 sigma).                                         unit: m
        "line": 10,                                     Line number of the input file where the instrument is defined.
        "targets": []                                   List of targets for the EDM instruments.
        }
    },                                                 
    {...}                                              
]
```

#### EDM target

List of targets for the EDM instruments.

```json
"targets": [{                                          
    "<KE.3>": {                                         Name of the target as defined in the instrument section. The key in angle brackets '<>' is variable.
        "distCorrectionUnknown": false,                 Indicates whether the distance correction is an unknown (true) or a given (false) value.
        "distCorrectionValue": 0.0,                     Distance correction value.                                                          unit: m
        "ID": "KE.3",                                   Name of the target as defined in the instrument section.
        "ppmDSpt": 0.0002,                              Distance PPM error.                                                                 unit: mm/km    
        "sigmaCombinedDist": null,                      Combined distance standard error (1 sigma).                                         unit: m
        "sigmaDCorr": 0.0,                              Distance correction standard error (1 sigma).                                       unit: m
        "sigmaDSpt": 0.0002,                            Distance standard error (1 sigma).                                                  unit: m
        "sigmaTargetCentering": 0.0,                    Target centering standard error (1 sigma).                                          unit: m
        "sigmaTargetHt": 0.0,                           Target height standard error (1 sigma).                                             unit: m
        "targetHt": 0.0,                                Target height.                                                                      unit: m
        "line": 11                                      Line number of the input file where the target is defined.                     
        }                                              
    },                                                 
    {...}                                              
]
```

### HLSR: hydrostatic leveling system instrument

List of hydrostatic leveling sensor (HLS) instruments.

```json
"fHLSR": [{                                           
    "<HL_SNSR>": {                                      Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "ID": "HL_SNSR",                                Name of the instrument as defined in the instrument section.
        "sigmaCombinedDist": null,                      Combined distance standard error (1 sigma).                                             unit: m
        "sigmaDist": 3e-05,                             Distance standard error (1 sigma).                                                      unit: m
        "sigmaInstrCentering": 0.0,                     Instrument centering standard error (1 sigma).                                          unit: m
        "sigmaInstrHeight": 0.0,                        Instrument height standard error (1 sigma).                                             unit: m
        "sigmaWS": 0.0,                                 Water surface standard error (1 sigma). Indicates the quality of the water surface.     unit: m
        "line": 28                                      Line number of the input file where the instrument is defined.
        }
    },                                                 
    {...}                                              
]
```

### INCL: Inclination measuring instrument

List of inclination measuring (e.g., inclinometer) instruments.

```json
"fINCL": [{                                            
    "<RollM>": {                                        Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "angleCorrectionValue": 0.0,                    Angle correction value.                                                             unit: rad
        "ID": "RollM",                                  Name of the instrument as defined in the instrument section.
        "refAngleCorrectionValue": 0.0,                 Reference angle correction value.                                                   unit: rad
        "refSigmaCorrectionValue": 0.0,                 Reference angle correction standard error (1 sigma).                                unit: rad
        "sigmaAngl": 2.356194490192345e-05,             Distance standard error (1 sigma) given in the input file in cc unit.               unit: rad
        "sigmaPpm": 1.571e-05,                          Distance standard error (1 sigma) given in the input file in urad (ppm) unit.       unit: rad
        "sigmaCombinedAngle": null,                     Combined angle standard error (1 sigma).                                            unit: rad
        "sigmaCorrectionValue": 0.0,                    Angle correction standard error (1 sigma).                                          unit: rad
        "line": 26                                      Line number of the input file where the instrument is defined.
        }
    },                                                 
    {...}                                              
]
```

### LEVEL: Leveling instrument

List of leveling (e.g., optical and digital level) instruments.

```json
"fLEVEL": [{                                           
    "<DNA03.331089>": {                                 Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "collAngleUnknown": false,                      Indicates whether the collimation angle is an unknown (true) or a given (false) value.
        "collAngleValue": 0.015707963267948967,         Collimation angle value.                                                            unit: rad
        "instrHeight":  0                               Default Instrument Height value.                                                    unit: m
        "sigmaInstrHeight": 0                           Default standard error (1 sigma) of the Instrument Height                           unit: m
        "defStaffID": "LCBL110_3",                      Name of the instrument default target.                                             
        "ID": "DNA03.331089",                           Name of the instrument as defined in the instrument section.
        "line": 6,                                      Line number of the input file where the instrument is defined.!
        "targets": []                                   List of targets for the LEVEL instruments.
        }
    },                                                 
    {...}                                              
]  
```

#### LEVEL target

List of targets for the LEVEL instruments.

```json
"targets": [{                                          
    "<LCBL110_3>": {                                    Name of the target as defined in the instrument section. The key in angle brackets '<>' is variable.
        "distCorrectionValue": 0.0,                     Distance correction value.                                                          unit: m
        "ID": "LCBL110_3",                              Name of the target as defined in the instrument section.
        "ppmD": 0.0,                                    Distance PPM error.                                                                 unit: mm/km
        "sigmaCombinedDist": null,                      Combined distance standard error (1 sigma).                                         unit: m
        "sigmaD": 0.0001,                               Distance standard error (1 sigma).                                                  unit: m
        "sigmaDCorr": 0.0,                              Distance correction standard error (1 sigma).                                       unit: m
        "sigmaStaffHt": 0.0,                            Target (staff) height standard error (1 sigma).                                     unit: m
        "staffHt": 0.0,                                 Target (staff) height.                                                              unit: m
        "line": 7                                       Line number of the input file where the target is defined.                     
        }                                              
    },                                                 
    {...}                                              
]
```

### POLAR: Polar instrument

List of polar (e.g., total station, laser tracker) instruments.

```json
"fPOLAR": [{                                           
    "<TS1>": {                                          Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "constAngle": 0.0,                              Constant angle value.                                                               unit: rad
        "defTarget": "TGT1",                            Name of the instrument default target.
        "ID": "TS1",                                    Name of the instrument as defined in the instrument section.
        "instrHeight": 0.0,                             Instrument height.                                                                  unit: m
        "sigmaInstrCentering": 0.0,                     Instrument centering standard error (1 sigma).                                      unit: m
        "sigmaInstrHeight": 0.0,                        Instrument height standard error (1 sigma).                                         unit: m
        "line": 14,                                     Line number of the input file where the instrument is defined.
        "targets": []                                   List of targets for the POLAR instruments.
        }
    },                                                 
    {...}                                              
]
```

#### POLAR target

List of targets for the POLAR instruments.

**Note:** The "sigmaCombinedAngle" field stores either the combined horizontal angle standard error (1 sigma) or the combined zenith angle standard error (1 sigma), depending on the observation type.

```json
"targets": [{                                          
    "<TGT1>": {                                         Name of the target as defined in the instrument section. The key in angle brackets '<>' is variable.
        "distCorrectionUnknown": false,                 Indicates whether the distance correction is an unknown (true) or a given (false) value.
        "distCorrectionValue": 0.0,                     Distance correction value.                                                          unit: m
        "ID": "TGT1",                                   Name of the target as defined in the instrument section.
        "ppmDist": 0.0,                                 Distance PPM error.                                                                 unit: mm/km
        "sigmaAngl": 3.000220984178253e-06,             Horizontal angle standard error (1 sigma).                                          unit: rad
        "sigmaCombinedAngle": null,                     Combined (horizontal or zenith) angle standard error (1 sigma).                     unit: rad
        "sigmaCombinedDist": null,                      Combined distance standard error (1 sigma).                                         unit: m
        "sigmaCombinedPLRAngl": null,                   Combined PLR3D horizontal angle standard error (1 sigma).                           unit: rad
        "sigmaCombinedPLRDist": null,                   Combined PLR3D distance standard error (1 sigma).                                   unit: m
        "sigmaCombinedPLRZenD": null,                   Combined PLR3D zenith angle standard error (1 sigma).                               unit: rad
        "sigmaDCorr": 0.0,                              Distance correction standard error (1 sigma).                                       unit: m
        "sigmaDist": 2e-05,                             Distance standard error (1 sigma).                                                  unit: m
        "sigmaTargetCentering": 0.0,                    Target centering standard error (1 sigma).                                          unit: m
        "sigmaTargetHt": 0.0,                           Target height standard error (1 sigma).                                             unit: m
        "sigmaZenD": 3.5971235883603137e-06,            Zenith angle standard error (1 sigma).                                              unit: rad
        "targetHt": 0.0,                                Target height.                                                                      unit: m
        "line": 15                                      Line number of the input file where the target is defined.                     
        }                                              
    },                                                 
    {...}                                              
]
```

### SCALE: Scale instrument

List of scale instruments.

```json
"fSCALE": [{                                           
    "<RS_1000.2>": {                                    Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "distCorrectionValue": 0.0,                     Distance correction value.                                                          unit: m
        "ID": "RS_1000.2",                              Name of the instrument as defined in the instrument section.
        "ppmD": 0.0,                                    Distance PPM error.                                                                 unit: mm/km
        "sigmaCombinedDist": null,                      Combined distance standard error (1 sigma).                                         unit: m
        "sigmaD": 0.0001,                               Distance standard error (1 sigma).                                                  unit: m
        "sigmaDCorr": 0.0,                              Distance correction standard error (1 sigma).                                       unit: m
        "sigmaInstrCentering": 0.0,                     Instrument centering standard error (1 sigma).                                      unit: m
        "line": 10                                      Line number of the input file where the instrument is defined.
        }
    },                                                 
    {...}                                              
]
```

### WPSR: Wire positioning system instrument

List of wire positioning sensor (WPS) instruments.

```json
"fWPSR": [{                                           
    "<WP_SNSR>": {                                      Name of the instrument as defined in the instrument section. The key in angle brackets '<>' is variable.
        "ID": "WP_SNSR",                                Name of the instrument as defined in the instrument section.
        "sigmaX": 3e-05,                                X-axis distance standard error (1 sigma).                                           unit: m
        "sigmaZ": 3e-05,                                Z-axis distance standard error (1 sigma).                                           unit: m
        "sigmaInstrCenteringX": 0.0,                    X-axis instrument centering standard error (1 sigma).                               unit: m
        "sigmaInstrCenteringZ": 0.0,                    Z-axis instrument centering standard error (1 sigma).                               unit: m
        "sagWire": 0.0,                                 Wire sagitta                                                                        unit: m
        "sigmaSagWire": 0.0,                            Wire sagitta standard error (1 sigma).                                              unit: m
        "sigmaWire": 0.0,                               Wire standard error (1 sigma). Indicates the quality of the wire shape.             unit: m
        "sigmaCombinedX": 0.0,                          Combined X-axis instrument centering standard error (1 sigma).                      unit: m
        "sigmaCombinedZ": 0.0,                          Combined Z-axis instrument centering standard error (1 sigma).                      unit: m
        "line": 29                                      Line number of the input file where the instrument is defined.
        }
    },                                                 
    {...}                                              
]
```

## Configuration structure

Structure of the configuration parameters of the input file. The information in this section is mainly necessary in order to be able to reconstruct the LGC input file by reading the corresponding JSON file.

```json
"config": {                               
    "allfixed": {                         
        "active_": false                                    Indicates whether the keyword *ALLFIXED is included in the input file.
    },                                                     
    "chaba": {                                              
        "active_": false                                    Indicates whether the keyword *CHABA is included in the input file.
    },                                                     
    "consCheck": {                                         
        "active_": false                                    Indicates whether the keyword *CONSI is included in the input file.
    },                                                     
    "covar": {                                             
        "active_": false                                    Indicates whether the keyword *COVAR is included in the input file.
    },                                                     
    "CustomOutputSeparator": {                             
        "active_": false,                                   Indicates whether the keyword *FMTO is included in the input file. Accepted only in an LGC v1 input file.
        "separator": ""                                     User-define column separator. By default the separator is a column.
    },                                                     
    "CustomOutputSeparatorPunch": {                        
        "active_": false,                                   Indicates whether the keyword *FMTP is included in the input file.
        "separator": ""                                     User-define column separator. By default the separator is a column.
    },                                                     
    "errorEllipses": {                                     
        "active_": false                                    Indicates whether the keyword *PRES is included in the input file.
    },                                                     
    "faut": {                                              
        "active_": true,                                    Indicates whether the keyword *FAUT is included in the input file.
        "alpha": 0.01,                                      Alpha is the probability of rejecting a right measure. By default alpha is set to 1%.
        "beta": 0.1                                         Beta is the probability of accepting a false measure. By default beta is set to 10%.
    },                                                     
    "histo": {                                             
        "active_": false                                    Indicates whether the keyword *HIST is included in the input file.
    },                                                     
    "libre": {                                             
        "active_": false                                    Indicates whether the keyword *LIBR is included in the input file.
    },                                                     
    "nodup": {                                             
        "active_": false                                    Indicates whether the keyword *NODUP is included in the input file.
    },                                                     
    "obsIDwidth": 0,                                        Maximum width of the observation IDs.
    "outPrecision": {                                      
        "digits": 7,                                        Number of decimal digits in the output files. Set with the keyword *PREC
        "convCrit": 5.0000000000000024e-08                  Least-squares convergence criterion.
    },                                                     
    "pdor": {                                              
        "active_": false,                                   Indicates whether the keyword *PDOR is included in the input file.
        "fptname": "",                                      Name of the point of orientation
        "fgis": {                                          
            "fValue": 0.0                                   Constant bearing (gisement)                                                             unit: rad
        },                                                 
        "hasBearing": false                                 Indicates whether a bearing is set by the user.
    },                                                     
    "pointNameWidth": 9,                                    Maximum width of the point names.
    "referential": 104,                                     User-defined reference frame.
    "sim": {                                               
        "active_": false,                                   Indicates whether the keyword *SIMU is included in the input file.
        "numSims": 0,                                       the number of simulations to perform.
        "numSeed": 0,                                       the seed number for the pseudo-random number generator.
        "writeLGCFile": false                               Indicates whether the keyword *SOBS is included in the input file.
    },                                                     
    "title": "Fichier MATHIS LGC2 20170615-160606",         Title of the input file.
    "useApriori": {                                        
        "active_": false                                    Indicates whether the keyword *APRI is included in the input file.
    },                                                     
    "writeDefa": {                                         
        "active_": false                                    Indicates whether the keyword *DEFA is included in the input file.
    },                                                     
    "writePlot": {                                         
        "active_": false,                                   Indicates whether the keyword *PLOT is included in the input file. Accepted only in an LGC v1 input file.
        "fmode": 0                                          User-defined output mode (see Table 1) for the plot file.
    },                                                     
    "writePunch": {                                        
        "active_": false,                                   Indicates whether the keyword *PUNC is included in the input file.
        "fmode": 0                                          User-defined output mode (see Table 1) for the punch file (.coo).
    },                                                     
    "json": {                                              
        "active_": true                                     Indicates whether the keyword *JSON is included in the input file.
    }
}
```

**Table 1. Correspondence between the `"fmode"` value, the `*PUNC` parameter, and the file format of the punch/plot file (available in LGC v1).**

| fmode | \*PUNC Value | Format Description |
| --- | --- | --- |
| 0   |     | `Nom_Pt X Y Z` |
| 1   | E   | `Nom_Pt X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz` |
| 2   | EE  | `Nom_Pt X Y Z Gist_gd_axe Gd_axe Pt_axe Sz dx dy dz` |
| 3   | H   | `Nom_Pt X Y H` |
| 4   | Z   | `Nom_Pt X Y Z` |
| 5   | HZ  | `Nom_Pt X Y Z H` |
| 6   | HN  | `Nom_Pt X Y Z H NLEP` |
| 7   | ZHN | `Nom_Pt X Y Z H NLEP` |
| 8   | T   | `Nom_Pt X Y Z Sx Sy Sz` |
| 9   | OUT1 | `.coo file for GEODE` |
| 10  | OUT3 | `.coo file for GEODE` |

## Point structure

List of adjustable points. The main description corresponds to a point defined as \*POIN (variable in X, Y and Z coordinate).

Examples of various point types (e.g., \*CALA, \*VZ, etc.) in various reference frames (e.g., \*OLOC, RS2K, etc.) are also provided,

```json
"points": [{                                                                                                            
    "active_": true,                                                                                    Indicates the activation status of the adjustable point.
    "eolcomment": "",                                                                                   End of line comment, as given in the LGC input file.
    "fCorrection": [3.5225178605997177e-17, -1.9447396997544393e-17, -1.0655704452043324e-16],          Coordinate correction during the last iteration.                                                               unit: [m, m, m]
    "fCovarianceMatrix": [                                                                              3x3 variance-covariance matrix in the FRAME that the point is defined.         
        [1.075949315623386e-08, 4.605565016526907e-09, -1.277459872129724e-08],                         [Cxx, Cxy, Cxz]                                                                                                unit: [m*m, m*m, m*m]
        [4.605565016526906e-09, 3.247819339209181e-09, -6.497159237294365e-09],                         [Cyx, Cyy, Cyz]                                                                                                unit: [m*m, m*m, m*m]
        [-1.27745987212972e-08, -6.49715923729436e-09,  3.734034622166096e-08]                          [Czx, Czy, Czz]                                                                                                unit: [m*m, m*m, m*m]
    ],                                                                                                                         
    "fEstimatedPrecision": [0.00010372797672871994, 5.6989642385342104e-05, 0.00019323650333635453],    Square root of the diagonal of the 3x3 variance-covariance matrix   in the FRAME that the point is defined.            
    "fEstimatedValue": {                                                                                Estimated coordinates in the FRAME that the point is defined.                      
        "fVector": [0.5698714575684534, 0.4876742988312188, -0.1766213422143951],                       [X FRAME, Y FRAME, Z FRAME]                                                                                    unit: [m, m, m]
        "fCoordSys": {"fCoordSysId": 1}                                                                 Indicates the coordinate system of the estimated coordinates (see Table 2).                                                                                               
    },                                                                                                 
    "fHfixed": false,                                                                                   Indicates whether the height of the point is fixed. Used only with reference frames that support height information.
    "fixedState": [false, false, false],                                                                Indicates whether a coordinate is fixed (true) or adjustable (false).
    "fName": "130",                                                                                     Point name.
    "fProvisionalValue": {                                                                              Provisional coordinates in the FRAME that the point is defined.
        "fVector": [0.5698893, 0.4876589, -0.1766419],                                                  [X FRAME, Y FRAME, Z FRAME]                                                                                    unit: [m, m, m]
        "fCoordSys": {"fCoordSysId": 1}                                                                 Indicates the type of the coordinate system of the provisional coordinates (see Table 2).
    },                                                                                                 
    "fReferential": -2,                                                                                 Indicates the reference frame of the network (see Table 3).
    "fSpatialStatus": 9,                                                                                Indicates the status of the point depending on the adjustability of the coordinates (see Table 4).
    "fStandardDeviations": [null, null, null],                                                          Initial standard deviation of the coordinates in the FRAME that the point is defined.                          unit: [m, m, m]
    "fXValueSet": true,                                                                                
    "fYValueSet": true,                                                                                
    "hdrcomment": "",                                                                                   Header comment, as given in the LGC input file.
    "line": 66,                                                                                         Line number of the input file where the point is defined.
    "uidx": [153, 154, 155],                                                                            Indices of the coordinate parameters for the LS adjustment (-1 when the coordinate is fixed).
    "allfixedParam": false,                                                                             Indicates whether the keyword *ALLFIXED is included in the input file.
    "fFramePosition_ID": [1],                                                                           ID of the FRAME that the point is defined.
    "fFramePosition_Name": "ROOT",                                                                      Name of the FRAME that the point is defined.
    "fProvisionalValueInRoot": {                                                                        Provisional coordinates in the ROOT frame.
        "fVector": [0.5698893, 0.4876589, -0.1766419],                                                  [X ROOT, Y ROOT, Z ROOT]                                                                                       unit: [m, m, m]
        "fCoordSys": {"fCoordSysId": 1}                                                                 Indicates the type of the coordinate system for the provisional coordinates (see Table 2).                       
    },                                                                                                     
    "fProvisionalHeightInRoot": {                                                                       Provisional height in the ROOT frame.  
        "fValue": 0.0                                                                                   [H ROOT]                                                                                                       unit: m                 
    },                                                                                                     
    "fEstimatedValueInRoot": {                                                                          Estimated coordinates in the ROOT frame.                       
        "fVector": [0.5698714575684534, 0.4876742988312188, -0.1766213422143951],                       [X ROOT, Y ROOT, Z ROOT]                                                                                       unit: [m, m, m]
        "fCoordSys": {"fCoordSysId": 1}                                                                 Indicates the coordinate system of the estimated coordinates (see Table 2).  
    },                                                                                                     
    "fEstimatedHeightInRoot": {                                                                         Estimated height in the ROOT frame.
        "fValue": 0.0                                                                                   [H ROOT]                                                                                                       unit: m 
    },                                                                                                     
    "fCovarianceMatrixInRoot": [                                                                        3x3 variance-covariance matrix in the ROOT frame.  
        [1.075949315623386e-08, 4.605565016526907e-09, -1.277459872129724e-08],                         [Cxx, Cxy, Cxz]                                                                                                unit: [m*m, m*m, m*m]
        [4.605565016526906e-09, 3.247819339209181e-09, -6.497159237294365e-09],                         [Cyx, Cyy, Cyz]                                                                                                unit: [m*m, m*m, m*m]        
        [-1.27745987212972e-08, -6.49715923729436e-09,  3.734034622166096e-08]                          [Czx, Czy, Czz]                                                                                                unit: [m*m, m*m, m*m]
    ]                                                                                                        
    },
    "fHasPointSigma": true,                                                                             Indicates if the point has associated a-priori data (either sigmas or an a-priori covariance matrix)
    "fPointSigma": {}                                                                                   Structure described in the 'Point Sigma Data' section below.
,                                                 
    {...}
]
```

### Point Sigma Data

In case the point is associated with a-priori sigmas or an a-priori covariance matrix, the relevant data can be found in "fPointSigma"

```json
"fPointSigma":{
    "fAngles":[{"fValue":0.0},{"fValue":0.0},{"fValue":0.0},{"fValue":0.0}],                        If specified, angles describing the rotation local->RST                                                              unit: [rad, rad, rad]
    "fAngleNames":["BEAR","SLOPE","ROLL","HDEFL"],
    "fHasAngle":false,                                                                              Indicates if angles were specified or not
    "fApriCovMat":[[null,null,null],[null,null,null],[null,null,null]],                             If specified, the 3x3 a-priori covariance matrix                                                                    unit: all m*m
    "fHasApriCovMat":false,                                                                         Indicates if a a-priori covariance matrix is specified
    "fSigmas":[0.001,0.002,null],                                                                   Values of the a-priori point precisions (null if not specified)                                                     unit: [m, m, m]
    "firstObsIdx":3,                                                                                Associated first index in the first design matrix
    "firstCIdx":-1,                                                                                 If constraints are introduced, first index in the constraint first design matrix
    "fRotRes":[0.0,0.0,0.0],                                                                        Residual (=offset) of the estimated point coordinates with respect to provisional coordinates, in rotated system    unit: [m, m, m]
    "fRotResNormalized":[0.0,0.0,null],                                                             Residual (=offset) divided by sigma, in rotated system                                                              unitless
    "fRotCovar":[[5e-7,0.0,0.0],[0.0,8e-7,0.0],[0.0,0.0,0.000001]],                                 A-posteriori point covariance matrix in rotated system                                                              unit: all m*m          
    "fSigmasPost":[0.0007071067811865475,0.0008944271909999159,0.001]}                              A-posteriori point precisions in rotated system                                                                     unit: [m, m, m]
    }
```

**Table 2. Correspondence between the `"fCoordSysId"` value and the coordinate system, according to the LGC software.**  
The point coordinates in the LGC JSON file are only expressed in the 3D Cartesian coordinate system (`"fCoordSysId": 1`).

| fCoordSysId | Coordinate System |
| --- | --- |
| 1   | k3DCartesian |
| 2   | kGeodetic |
| 4   | k2DPlusH |
| 8   | k2DCartesian |
| 16  | kGeodeticSphere |

**Table 3. Correspondence between the `"fReferential"` value, the LGC datum keyword, and the reference frame of the network, according to the LGC software.**

| fReferential | LGC Datum Keyword | Reference Frame |
| --- | --- | --- |
| \-2 | \*OLOC | kLocalRefFrame |
| 104 | \*RS2K | kCernXYHg00Machine |
| 106 | \*LEP | kCernXYHg85Machine |
| 107 | \*SPHE | kCERNXYHsSphereSPS |

**Table 4. Correspondence between the `"fSpatialStatus"` value, the LGC point keyword, and the point status, according to the LGC software.**

| fSpatialStatus | LGC Point Keyword | Point Status |
| --- | --- | --- |
| 0   |     | kPosNull |
| 1   |     | kUnknown |
| 2   | \*CALA | kCala |
| 3   |     | kVx |
| 4   |     | kVy |
| 5   | \*VZ | kVz |
| 6   | \*VXY | kVxy |
| 7   | \*VXZ | kVxz |
| 8   | \*VYZ | kVyz |
| 9   | \*POIN | kVxyz |

**Examples:**

In the following examples only the most interesting fields of the point structure are presented and explained.

```json
"fCovarianceMatrix": [                                                                                   
        [0.0, 0.0, 0.0],                                                                                     
        [0.0, 0.0, 0.0],                                                                                     
        [0.0, 0.0, 0.0]                                                                                      
    ],                                                                                                       
    "fHfixed": true,                                                                                         
    "fixedState": [true, true, true],                                                                        
    "fReferential": 104,
    "fSpatialStatus": 2,
    "uidx": [-1, -1, -1],
    "fFramePosition_ID": [1],
    "fFramePosition_Name": "ROOT",
    "fEstimatedValueInRoot": {
        "fVector": [1872.16276, 2271.41545, 2435.787328809202],
        "fCoordSys": {
            "fCoordSysId": 1
        }
    },
    "fEstimatedHeightInRoot": {
        "fValue": 435.79028999999974
    },
    "fCovarianceMatrixInRoot": [
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0]
]
```

*Point defined:*

- as \*CALA ("fSpatialStatus": 2) with X, Y and Z fixed,
- in the \*RS2K geodetic reference frame ("fReferential": 104), and
- in the ROOT frame ("fFramePosition_ID": \[1\]).

*Notice:*

- that the covariance matrix is a zero matrix, and
- that the height is also fixed ("fHfixed": true) because the point is defined in the ROOT frame.

\-------

```JSON
"fCovarianceMatrix": [
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0]
    ],
    "fHfixed": false,
    "fixedState": [true, true, true],
    "fReferential": 104,
    "fSpatialStatus": 2,
    "uidx": [-1, -1, -1],
    "fFramePosition_ID": [1, 45, 1, 1, 1],
    "fFramePosition_Name": "RSTR_XT03.MQ.0700",
    "fEstimatedValueInRoot": {
        "fVector": [1864.59994244414, 2293.407244889488, 2437.4885217560472],
        "fCoordSys": {
            "fCoordSysId": 1
        }
    },
    "fEstimatedHeightInRoot": {
        "fValue": 437.49227812905394
    },
    "fCovarianceMatrixInRoot": [
        [4.3390779746186e-09, 1.2528299520283e-09, 4.398278555406036e-10],
        [1.2528299520283e-09, 7.3854084578721e-10, 9.103511089895193e-11],
        [4.3982785554060e-10, 9.1035110898951e-11, 3.693303774091795e-10]
]
```

*Point defined:*

- as \*CALA ("fSpatialStatus": 2) with X, Y and Z fixed,
- in the \*RS2K geodetic reference frame ("fReferential": 104), and
- in a subframe ("fFramePosition_ID": \[1, 45, 1, 1, 1\]).

*Notice:*

- that the covariance matrix is a zero matrix in the subframe but it is non-zero matrix in the ROOT frame, and
- that the height is not fixed ("fHfixed": false) because the point is fixed in a subframe.

\-------

```JSON
"fCovarianceMatrix": [
        [0,         0,      0                     ],
        [0,         0,      0                     ],
        [0,         0,      2.3911031714631924e-07]
    ],
    "fHfixed": false,
    "fixedState": [true, true, false],
    "fReferential": -2,
    "fSpatialStatus": 5,
    "uidx": [-1, -1, 5710],
    "fFramePosition_ID": [1],
    "fFramePosition_Name": "ROOT",
    "fEstimatedValueInRoot": {
        "fVector": [0.0, 0.0, 329.5841653189536],
        "fCoordSys": {
            "fCoordSysId": 1
        }
    },
    "fEstimatedHeightInRoot": {
        "fValue": 0.0
    },
    "fCovarianceMatrixInRoot": [
        [0,         0,      0                     ],
        [0,         0,      0                     ],
        [0,         0,      2.3911031714631924e-07]
]
```

*Point defined:*

- as \*VZ ("fSpatialStatus": 5) with X, Y fixed and Z variable,
- in the \*OLOC (local) reference frame ("fReferential": -2), and
- in the ROOT frame ("fFramePosition_ID": \[1\]).

*Notice:*

- that the covariance matrix has only one non-zero value; the Czz, and
- that the height is not estimated because of the local reference frame.

\-------

```json
"fCovarianceMatrix": [
        [6.91566682000678e-05,  -7.710610858817354e-05,     0],
        [-7.7106108588173e-05,  0.00011936478685685381,     0],
        [0,                     0,                          0]
    ],
    "fHfixed": false,
    "fixedState": [false, false, true],
    "fReferential": 104,
    "fSpatialStatus": 6,
    "uidx": [829, 830, -1],
    "fFramePosition_ID": [1],
    "fFramePosition_Name": "ROOT",
    "fEstimatedValueInRoot": {
        "fVector": [3273.33599807848, 3295.9263277694417, 2345.126552267488],
        "fCoordSys": {
            "fCoordSysId": 1
        }
    },
    "fEstimatedHeightInRoot": {
        "fValue": 345.3608025325694
    },
    "fCovarianceMatrixInRoot": [
        [6.91566682000678e-05,  -7.710610858817354e-05,     0],
        [-7.7106108588173e-05,  0.00011936478685685381,     0],
        [0,                     0,                          0]
]
```

*Point defined:*

- as \*VXY ("fSpatialStatus": 6) with X, Y variable and Z fixed,
- in the \*RS2K geodetic reference frame ("fReferential": 104), and
- in the ROOT frame ("fFramePosition_ID": \[1\]).

*Notice:*

- that the covariance matrix has a non-zero block corresponding to the variable coordinates X and Y, and
- that the height is estimated in the defined geodetic reference frame.

## Measurements structure

The content of the "measurements" field depends on the type and the amount of measurements that are defined in a FRAME.

In the hypothetical scenario that all type of measurements are defined in a FRAME, the structure would be as shown below.

Please see the internal structure of the summaries in the [observation summary structure](https://confluence.cern.ch/display/SUS/JSON#JSON-JSON-obsSumStructure) section.

```json
"measurements": {
    "anglGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ANGL observation type.
    "dhorGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the DHOR observation type.
    "distGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the DIST observation type.
    "dlevDHORGlobalSummary_": {},                   Structure of the statistics concerning the residuals of the horizontal distance (DHOR) of the DLEV observation type.
    "dlevGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the DLEV observation type.
    "dsptGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the DSPT observation type.
    "dverSummary_": {},                             Structure of the statistics concerning the residuals of the DVER observation type.
    "ecdirGlobalSummary_": {},                      Structure of the statistics concerning the residuals of the ECDIR observation type.
    "echoGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ECHO observation type.
    "ecspGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ECSP observation type.
    "ecthGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ECTH observation type.
    "ecveGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ECVE observation type.
    "ecwsGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ECWS observation type.
    "inclyGlobalSummary_": {},                      Structure of the statistics concerning the residuals of the INCLY observation type.
    "orieGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ORIE observation type.
    "radiSummary_": {},                             Structure of the statistics concerning the residuals of the RADI observation type.
    "zendGlobalSummary_": {},                       Structure of the statistics concerning the residuals of the ZEND observation type.
    "obsxyzSummary_": {                             Structure of the statistics concerning the residuals of the OBSXYZ observation type.
        "obsXObsSum": {},                           Structure of the statistics concerning the residuals of the X-axis observation of the OBSXYZ observation type.
        "obsYObsSum": {},                           Structure of the statistics concerning the residuals of the Y-axis observation of the OBSXYZ observation type.
        "obsZObsSum": {}                            Structure of the statistics concerning the residuals of the Z-axis observation of the OBSXYZ observation type.
    },                                                                         
    "plrGlobalSummary_": {                          Structure of the compound summaries of the PLR3D observation type.
        "distObsSum": {},                           Structure of the statistics concerning the residuals of the spatial distance (DIST) of the PLR3D observation type.
        "anglObsSum": {},                           Structure of the statistics concerning the residuals of the horizontal angle (ANGL) of the PLR3D observation type.
        "zendObsSum": {}                            Structure of the statistics concerning the residuals of the zenith distance (ZEND) of the PLR3D observation type.
    },   
    "uvdGlobalSummary_": {                          Structure of the compound summaries of the UVD observation type.
        "distObsSum": {},                           Structure of the statistics concerning the residuals of the Z-axis component of the unit vector of the UVD observation type.
        "xVectorCompObsSum": {},                    Structure of the statistics concerning the residuals of the X-axis component of the unit vector of the UVD observation type.
        "yVectorCompObsSum": {}                     Structure of the statistics concerning the residuals of the Y-axis component of the unit vector of the UVD observation type.
    },                                                                         
    "uvecGlobalSummary_": {                         Structure of the compound summaries of the UVEC observation type.
        "xVectorCompObsSum": {},                    Structure of the statistics concerning the residuals of the X-axis component of the unit vector of the UVEC observation type.
        "yVectorCompObsSum": {}                     Structure of the statistics concerning the residuals of the Y-axis component of the unit vector of the UVEC observation type.
    },                                                                         
    "ecwiGlobalSummary_": {                         Structure of the compound summaries of the ECWI observation type.
        "xObsSum": {},                              Structure of the statistics concerning the residuals of the X-axis component of the unit vector of the ECWI observation type.
        "zObsSum": {}                               Structure of the statistics concerning the residuals of the Z-axis component of the unit vector of the ECWI observation type.
    },                                             
    "dverActive": true,                             Indicates the activation status of the DVER observation type.
    "obsxyzActive": true,                           Indicates the activation status of the OBSXYZ observation type.
    "radiActive": true,                             Indicates the activation status of the RADI observation type.
    "fCAM": [],                                     List of the measurements performed by camera instruments.
    "fDVER": [],                                    List of the DVER measurements.
    "fECHO": [],                                    List of the ECHO measurements.
    "fECSP": [],                                    List of the ECSP measurements.
    "fECVE": [],                                    List of the ECVE measurements.
    "fECWS": [],                                    List of the ECWS measurements.
    "fEDM": [],                                     List of the measurements performed by electronic distance measuring instruments.
    "fINCLY": [],                                   List of the INCLY measurements.
    "fLEVEL": [],                                   List of the measurements performed by leveling instruments.
    "fOBSXYZ": [],                                  List of the OBSXYZ measurements.
    "fORIE": [],                                    List of the ORIE measurements.
    "fPDOR": {},                                    The PDOR constraint.
    "fRADI": [],                                    List of the RADI measurements.
    "fTSTN": [],                                    List of the measurements performed by polar instruments.
    "fECWI": []                                     List of the ECWI measurements.
}
```

### fCAM measurements

```json
"fCAM": [{
    "instrument": {},                                           Structure described in the 'Instrument structure' section.
    "instrumentPos": "PIV1",                                    Point name of the instrument position.
    "line": 22,                                                 Line number of the input file where the point is defined.
    "measUVD": [],                                              UVD type of measurement. The structure is similar to the UVEC type of measurement.
    "measUVEC": [{                                              List of structures containing information about UVEC type of measurements.
        *1*,                                                    Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        "vector": {                                             Measurement of the spatial direction vector.
            "fVector": [0.129931, -0.742782, 0.656804],         [X, Y, Z] Components of the spatial direction vector.                                   unit: [m, m, m]
            "fCoordSys": {"fCoordSysId": 1}                     Indicates the coordinate system of the estimated coordinates (see Table 2).            
        },                                                                                                                                             
        "XcompResidual": 1.4274681090775676e-06,                Residual of the X component.                                                            unit: m
        "YcompResidual": 1.1471555179110027e-06                 Residual of the Y component.                                                            unit: m
    },                                                         
    {...}],                                                         
    "stnCounter_": 6,                                           Total number of instrument stations.
    "stnId": 0,                                                 Identification number of the present instrument station.
    "uvdActive": true,                                          Indicates the activation status of the UVD observation type.
    "uvdSummary_": {},                                          Structure of the compound summaries of the UVD observation type.
    "uvecActive": true,                                         Indicates the activation status of the UVEC observation type.
    "uvecSummary_": {                                           Structure of the compound summaries of the UVEC observation type.
        "xVectorCompObsSum": {},                                Structure described in the 'Observation summary structure' section.
        "yVectorCompObsSum": {}                                 Structure described in the 'Observation summary structure' section.
    }
}]
```

### fDVER measurements

```json
"fDVER": [{
    *1*,                            Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
    *2*,                            Observation values for length and angle measurements, described in 'Observation Values' section.                                                       
    "fDistanceCorrection": 0.0,     Distance correction value for the measurements.    
    "fSigmaObsVal": 0.0001,         Combined observation standard error (1 sigma).                                                                      unit: m
    "station": {}                   Structure described in the 'Point structure' section.
},
{...}]
```

### fECHO measurements

```json
"fECHO": [{                        
    "echoSummary_": {},             Structure described in the 'Observation summary structure' section.
    "fMeasuredPlane": {},           Measured plane (see the adjustable plane section).
    "fReferencePoint": {},          Reference point (see the adjustable point section).
    "line": 292,                    Line number of the input file where the observation is given.
    "measECHO": [{                  List of structures containing information about ECHO type of measurements.
        *1*,                        Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
        *2*                         Observation values for length and angle measurements, described in 'Observation Values' section.                                       
    },                             
    {...}],                            
    "romId": 0                      Identification number of the present round of measurements (ROM).
},
{...}]
```

### fECSP measurements

```json
"fECSP": [{                        
    "ecspSummary_": {},             Structure described in the Observation summary structure section. 
    "line": 54,                     Line number of the input file where the observation is given.
    "measECSP": [{                  List of structures containing information about ECSP type of measurements.
        *1*,                        Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                         Observation values for length and angle measurements, described in 'Observation Values' section.
    },                             
    {...}],
    "p1",                           Point name of the first point that defines the measured line.
    "p2",                           Point name of the second point that defines the measured line.
    "romId": 0,                     Identification number of the present round of measurements (ROM).
    "romName": "WATER42",           Identification name of the present round of measurements (ROM).
},
{...}]
```

### fECVE measurements

```json
"fECVE": [{                        
    "ecveSummary_": {},             Structure described in the 'Observation summary structure' section.
    "fMeasuredLine": {},            Measured line (see the adjustable line section).
    "fPtLine": {},                  Reference point (see the adjustable point section).
    "line": 17,                     Line number of the input file where the observation is given.
    "measECVE": [{                  List of structures containing information about ECVE type of measurements.
        *1*,                        Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                         Observation values for length and angle measurements, described in 'Observation Values' section.                                         
    },                             
    {...}],                                
    "romId": 0                      Identification number of the present round of measurements (ROM).
},
{...}]
```

### fECWS measurements

```json
"fECWS": [{                        
    "ecwsSummary_": {},             Structure described in the Observation summary structure section.
    "fMeasuredWSHeight": {},        Structure described in the 'LGC data structure' section, in the 'Adjustable lengths' subsection. 
    "instrument": {},               Structure described in the 'Instrument structure' section, in the 'HLSR: hydrostatic leveling system instrument' subsection.      
    "line": 44,                     Line number of the input file where the observation is given.
    "measECWS": [{                  List of structures containing information about ECWS type of measurements.
        *1*,                        Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                         Observation values for length and angle measurements, described in 'Observation Values' section.
    },                             
    {...}],                        
    "romId": 0,                     Identification number of the present round of measurements (ROM).
    "romName": "WATER42",           Identification name of the present round of measurements (ROM).
    "sigmaWS": 0.0                  Water surface observation standard error (1 sigma).                                                                   unit: m
},
{...}]
```

### fEDM measurements

```json
"fEDM": [{
    "dsptSummary_": {},                     Structure described in the Observation summary structure section.
    "instrument": {},                       Structure described in the 'Instrument structure' section, in the 'EDM: Electronic distance measuring instrument' subsection.
    "instrumentPos": "LHC.MQML.8L5.E",      Point name of the instrument position.
    "line": 150,                            Line number of the input file where the observation is given.
    "measDSPT": [{                          List of structures containing information about DSPT type of measurements.
        *1*,                                Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                                 Observation values for length and angle measurements, described in 'Observation Values' section.                                                   
    },                                                     
    {...}],                                                
    "stnId": 0                              Identification number of the present instrument station.                                     
},
{...}]
```

### fINCLY measurements

```json
"fINCLY": [{                                       
    "inclySummary_": {},        Structure described in the Observation summary structure section.
    "instrument": {},           Structure described in the 'Instrument structure' section, in the 'INCL: Inclination measuring instrument' subsection.
    "line": 29,                 Line number of the input file where the observation is given.
    "measINCLY": [{             List of structures containing information about INCLY type of measurements.
        *1*,                    Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                     Observation values for length and angle measurements, described in 'Observation Values' section.
    }],                        
    "romId": 0                  Identification number of the present round of measurements (ROM).
}]
```

### fLEVEL measurements

```json
"fLEVEL": [{                                               
    "dhorSummary_": {},                     Structure described in the Observation summary structure section.
    "dlevSummary_": {},                     Structure described in the Observation summary structure section.
    "fMeasuredPlane": {},                   Measured plane (see the adjustable plane section).
    "fRefPt": {},                           Reference point (see the adjustable point section).
    "hasDHOR": true,                        Indicates whether DHOR observations are present.
    "instrument": {},                       Structure described in the 'Instrument structure' section, in the 'LEVEL: Leveling instrument' subsection.
    "line": 806,                            Line number of the input file where the observation is given.
    "measDLEV": [{                          List of structures containing information about DLEV type of measurements.
        *1*,                                Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*,                                Observation values for length and angle measurements, described in 'Observation Values' section.
        "dhor": {                           Structure containing information about DHOR type of measurements.
            *1*,                            Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
            *2*,                            Observation values for length and angle measurements, described in 'Observation Values' section.
            "dhorSigma": 0.01               Horizontal distance observation standard error (1 sigma).                                                                    unit: m
        },                                 
        "fAllFixedCollimation": null        Fixed value for all collimation angle parameters. (null when a value is not provided by the used).                          unit: rad
        },                                                 
        {...}                                              
    ],                                                     
    "stnId": 0                              Identification number of the present instrument station.
}]
```

### fOBSXYZ measurements

```json
"fOBSXYZ": [{
    *1*,                                                        Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
    "fXResidual": -4.566036981292713e-22,                       Residual of the X-component observation.                                                                unit: m
    "fXSigmaObsVal": 1e-05,                                     X-component observation standard error (1 sigma).                                                       unit: m
    "fYResidual": -3.790141566717973e-21,                       Residual of the Y-component observation.                                                                unit: m
    "fYSigmaObsVal": 1e-05,                                     Y-component observation standard error (1 sigma).                                                       unit: m
    "fZResidual": 3.110199103199384e-22,                        Residual of the X-component observation.                                                                unit: m
    "fZSigmaObsVal": 1e-05,                                     Z-component observation standard error (1 sigma).                                                       unit: m
    "station": {},                                              Station point (see the adjustable point section).                                                       unit: m
    "obsValue": {                                                                                      
        "fVector": [-1110.1608, 10452.39482, 2415.25327],       [X, Y, Z] components of the observation.                                                                unit: [m, m, m]
        "fCoordSys": {"fCoordSysId": 1}                         Indicates the type of the coordinate system for the provisional coordinates (see Table 2).                                   
    }
},
{...}]
```

### fORIE measurements

```json
"fORIE": [{
    "fConstantAngle": 0.0,                      Constant angle of orientation                                                                                           unit: rad
    "instrument": {},                           Structure described in the 'Instrument structure' section, in the 'POLAR: Polar instrument' subsection.
    "instrumentPos": "LHC.GGPSO.7L5.",          Point name of the instrument position.
    "line": 191,                                Line number of the input file where the observation is given.
    "measORIE": [{                              List of structures containing information about ORIE type of measurements.
        *1*,                                    Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                                     Observation values for length and angle measurements, described in 'Observation Values' section.
    }],                                        
    "orieSummary_": {},                         Structure described in the Observation summary structure section.
    "romId": 0                                  Identification number of the present round of measurements (ROM).
},
{...}]
```

### fPDOR measurements

```json
"fPDOR": {
    *1*,                                            Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
    "calaPt": "LHC.TRIPOD.4L514.",                  Point name of the relevant CALA point.
    "fbearing": 1.1839472428447029,                 Bearing of the point of orientation.                                                                                unit: rad
    "fbearingResidual": 5.409068877311691e-19,      Residual of the bearing.                                                                                            unit: rad
    "fDefined": false,                              Indicates whether the bearing value is defined by the user.
    "fIsInitialise": true,                          Indicates whether the observation is initialized.
    "fSigmaObsVal": 1.5707963267948966e-09,         Bearing observation standard error (1 sigma).                                                                       unit: rad
}
```

### fRADI measurements

```json
"fRADI": [{
    *1*,                                        Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
    "fAngleCnstr": 1.1637150339280384,          Angle value to constrain the orientation.                                                                               unit: rad
    "fConstAngleVal": 0.0,                      Constant angle to be added to the given bearing.                                                                        unit: rad      
    "fResidual": 9.327842970315748e-05,         Radial residual (with respect to the angle), sign: provisional - estimated                                              unit: m            
    "fSigmaObsVal": 0.0001,                     Observation standard error (1 sigma).                                                                                   unit: m
    "station": {}                               Station point (see the adjustable point section).
},
{...}]
```

### fTSTN measurements

```json
"fTSTN": [{                                    
    "ihfix": true,                              Indicates whether the instrument height is fixed (true) or variable (false).
    "instrument": {},                           Structure described in the 'Instrument structure' section, in the 'POLAR: Polar instrument' subsection.                                          
    "instrumentHeightAdjustable": {},           Structure described in the 'LGC data structure' section, in the 'Adjustable lengths' subsection.
    "instrumentPos": "LHC.GGPSO.7L5.",          Point name of the instrument position.
    "line": 295,                                Line number of the input file where the observation is given.
    "roms": [{                                  List of the rounds of measurements (ROM).
        "active_": true,                        Activation status of the round of measurements (ROM).
        "acst": 0.0,                            Constant angle/orientation of the station. Defaults value is set to zero.                                       unit: rad
        "anglActive": true,                     Activation status of the ANGL round of measurements (ROM).
        "anglSummary_": {},                     Structure described in the Observation summary structure section.
        "defaultTargetId": "CCR1.5.6930",       Name of the default target.
        "dhorActive": true,                     Activation status of the DHOR round of measurements (ROM).
        "distActive": true,                     Activation status of the DIST round of measurements (ROM).
        "distSummary_": {},                     Structure described in the Observation summary structure section.
        "ecdirActive": true,                    Activation status of the ECDIR round of measurements (ROM).
        "ecthActive": true,                     Activation status of the ECTH round of measurements (ROM).
        "measANGL": [{                          List of structures containing information about ANGL type of measurements.
            *1*,                                Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
            *2*,                                Observation values for length and angle measurements, described in 'Observation Values' section.
            "fAllFixedV0": null                 Fixed value for all reference orientation parameters. (null when a value is not provided by the used).          unit: rad
        },                                     
        {...}],                                
        "measDIST": [{                          List of structures containing information about DIST type of measurements.
            *1*,                                Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
            *2*,                                Observation values for length and angle measurements, described in 'Observation Values' section.
            "fAllFixedCs": null,                Fixed value for all distance correction parameters. (null when a value is not provided by the used).            unit: m
            "fAllFixedHi": null                 Fixed value for all instrument height parameters. (null when a value is not provided by the used).              unit: m
        },                                     
        {...}],                                
        "measZEND": [{                          List of structures containing information about ZEND type of measurements.
            *1*,                                Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section. 
            *2*,                                Observation values for length and angle measurements, described in 'Observation Values' section.
            "fAllFixedHi": null                 Fixed value for all instrument height parameters. (null when a value is not provided by the used).              unit: m
        },                                     
        {...}],                                
        "plrActive": true,                      Activation status of the PLR3D round of measurements (ROM).
        "romId": 0,                             Identification number of the present round of measurements (ROM).
        "v0": {},                               Structure described in the 'LGC data structure' section, in the 'Adjustable angles' subsection.
        "zendActive": true,                     Activation status of the ZEND round of measurements (ROM).
        "zendSummary_": {}                      Structure described in the Observation summary structure section.
    }],                                        
    "rot3D": false,                             Indicates whether the keyword ROT3D is used for the present ROM.
    "stnId": 0                                  Identification number of the present instrument station.
},
{...}]
```

### fECWI measurements

```json
"fECWI": [{                                                        
    "ecwiSummary_": {                                           Structure of the compound summaries of the ECWI observation type.
        "xObsSum": {},                                          Structure described in the 'Observation summary structure' section.
        "zObsSum": {}                                           Structure described in the 'Observation summary structure' section.
    },                                                         
    "instrument": {},                                           Structure described in the 'Instrument structure' section, in the 'WPSR: Wire positioning system instrument' subsection.
    "line": 73,                                                 Line number of the input file where the observation is given.
    "referencePoint": {                                         Reference point of the wire, defined as the middle of the 2 anchor points.
        "fVector": [-1051.98695, 10476.75135, 2414.55956],      [X, Y, Z] Components of the reference point.                                                            unit: [m, m, m]
        "fCoordSys": {"fCoordSysId": 1}                         Indicates the coordinate system of the estimated coordinates (see Table 2).            
        }                                                      
    },                                                         
    "fWireDx": {},                                              Structure described in the 'LGC data structure' section, in the 'Adjustable lengths' subsection.
    "fWireDz": {},                                              Structure described in the 'LGC data structure' section, in the 'Adjustable lengths' subsection.
    "fWireBearing": {},                                         Structure described in the 'LGC data structure' section, in the 'Adjustable angles' subsection.
    "fWireSlope": {},                                           Structure described in the 'LGC data structure' section, in the 'Adjustable angles' subsection.
    "sagfix": true,                                             Indicates whether the sag value of the wire is fixed (true) or adjustable (false).
    "sagAdjustable": {},                                        Structure described in the 'LGC data structure' section, in the 'Adjustable lengths' subsection.
    "sigmaWire": 0.0,                                           Wire standard error (1 sigma).                                                                          unit: m
    "anchorPtFirst": "LHC.GISP.UPS4L5..WIRET",                  Point name of the first anchor point.
    "anchorPtSecond": "LHC.GISP.UPS4R5..WIRET",                 Point name of the second anchor point.
    "romId": 0,                                                 Identification number of the present round of measurements (ROM).
    "romName": "WIRET",                                         Identification name of the present round of measurements (ROM).
    "measECWI": [{                                              List of structures containing information about ECWI type of measurements.
        *1*,                                                    Standard parameters for all types of measurements, described in 'Standard Measurement Parameters' section.
        *2*                                                     Observation values for length and angle measurements, described in 'Observation Values' section.        
    },                                                                         
    {...}]                                                                     
},                                                                                 
{...}]
```

#### Standard Measurement Parameters

Standard measurement parameters for all types of measurements. In the 'Observation structure' subsection these parameters are substituted by the symbol '\*1\*'.

```json
"eolcomment": "",                           End of line comment, as given in the LGC input file.
"fFirstEquationIndex": 0,                   Equation Index of the observation.
"fFirstObservationIndex": 0,                Observation Index of the observation.
"line": 24,                                 Line number of the input file where the observation is given.
"obsID": "",                                Unique identification name of the observation.
"measCounter": 88,                          Total number of observations.
"measId": 0,                                Identification number of the present observation.
"target": {},                               Structure described in the 'Instrument structure' section, in the 'CAMD target' subsection.
"targetPos": "C1",                          Point name of the target position.

```

Notice:

- In the PDOR structure (fPDOR) the field "targetPos" is named as "orientationPt".
- Some distance measurements have also the unitless "distance sensibility" value attached. This is the case for \*DSPT observations with EDM instruments and \*PLR3D + \*DIST observations with TSTN instruments.

```json
"fDistSensi": "1.2345"                      Distance sensibility value, unitless.

```

#### Observation Values

Observation values for length and angle measurements. In the 'Observation structure' subsection these parameters are substituted by the symbol '\*2\*'.

```json
"angles": [{                                List of angle observations.            
    "fValue": 1.5685438048622729            Angle observation value                 unit: rad                
},
{...}],                                                            
"anglesResiduals": [{                       List of angle residuals.
    "fValue": 1.6543612251060553e-24        Angle observation value                 unit: rad
},
{...}],                                                            
"distances": [{                             List of length observations.
    "fValue": 0.20012                       Length observation value                unit: m
},
{...}],                                        
"distancesResiduals": [{                    List of length residuals.
    "fValue": -4.713064382644e-05           Length observation residual value       unit: m
},
{...}]
```

## Observation summary structure

Structure of the statistics concerning the residuals of the measurements of a specific observation type. **The values are given in mm or in cc depending of the type of the observations (i.e. mm for lengths and cc for angles).**

```json
{
    "fAngleType": true,                                     Indicates whether the observations are angles (true) or lengths (false).
    "fCreateHistogram": true,                               Indicates whether the histogram will be printed in the result file.
    "fHistoData": [0, 1, 2, 0, 1, ..., , 2, 1, 1, 0],       Number of values per histogram bin (number of star symbols in the histogram of the result file. Empty when fCreateHistogram is false.
    "fHistoList": [-0.00537, -0.00797, ..., 0.01477],       Histogram data corresponding to the observation residuals.
    "fIsInitialised": true,                                 Indicates whether the observation summary is initialized.
    "fMean": 0.013963219979151656,                          Mean of the residuals.
    "fMeanHiLimit": 0.06273436484001098,                    Upper confidence limit for the mean of the residuals.
    "fMeanLoLimit": -0.06273436484001098,                   Lower confidence limit for the mean of the residuals.
    "fNumberOfObs": 61,                                     Number of observations.
    "fNumberOutsideHisto": 0,                               Number of residuals outside the histogram limits.
    "fObsText": "",                                         String to identify the observation type.
    "fResMax": 1.0890365522803096,                          Maximum residual.
    "fResMin": -0.719486246129187,                          Minimum residual.
    "fSumRes": 0.851756418728251,                           Sum of the residuals.
    "fSumRes2": 3.6118955091644693,                         Sum of the residuals squared.
    "fStdHiLimit": 0.29820969404111414,                     Upper confidence limit for the standard deviation.
    "fStdev": 0.24494905072011447,                          Standard deviation of the residuals.
    "fStdLoLimit": 0.20789064488706835                      Lower confidence limit for the standard deviation.
}
```