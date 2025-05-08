## \*PDOR

Allow to give an orientation of the network.

```text
Usage:
    *PDOR 
    point_name [bearing]
    -----
        
    Mandatory arguments:    
        bearing:            Value of the bearing [gon].
```

Pre-requisite:

- At least one CALA point has to be defined in ROOT : the first point following the CALA keyword is considered as the unique fixed point from which the bearing is defined,
- The point name on which the orientation will considered point must be defined in POIN or VXY.
- Only one unique PDOR is considered for constraining the orientation of the network.
- The bearing must be defined

NB: Only one unique PDOR is considered for constraining the orientation of the network calculations.

**PDOR must be defined between the instruments and the points.**

PDOR keyword is not allowed after keywords: CALA, POIN, VXY, VYZ, VXZ, VZ.

## Points

The basic declaration of points does not differ from the previous version of LGC.

One of the point type keyword can be followed by several points of this type, one per line following always the same structure:

```text
Usage:
*Point_Type
    Point_Name x_m y_m z_m
    
with:
*Point_Type:    Type of point, can be of value: *POIN, *CALA, *VXY, *VYZ or *VZ.
Point_Name:     Name of the point, a point can only be declared once in the whole file.
x_m:            X coordinate of the point [m].
y_m:            Y coordinate of the point [m].
z_m:            Z coordinate of the point [m].
```

The table below lists the degrees of freedom for each point type in the frame it is defined in.

| Keyword | Vx  | Vy  | Vz  | Comments |
| :--- | :--- | :--- | :--- | :--- |
| POIN | x   | x   | x   | Point totally free, ex: point measured by a laser tracker. Since v2.8.0: can also welcome constraints. |
| CALA |     |     |     | Point totally constrained, ex: reference point |
| VXY | x   | x   |     | Moving in the XY-Plane, ex: point measured by wire |
| VXZ | x   |     | x   | Moving in the XZ-Plane |
| VYZ |     | x   | x   | Moving in the YZ-Plane |
| VZ  |     |     | x   | Moving only in Z, ex: point observed only by a level |

### Point declaration in the root frame

One point type keyword must include all the declaration of the same type. This means that all points of a defined point type should be declared together.  
The declaration shall necessarily be done between the instrument and the measurement sections.

```text
Example:

*CALA
LHC.MQ.24L8.E                                    4800.60321     6074.89790    329.03192
LHC.MQ.24L8.S                                    4800.18209     6072.08288    329.0337
LHC.MB.C24L8.E                                   4799.54724     6067.89932    329.03298
*POIN
LHC.MB.C24L8.S                                   4797.91877     6057.22215    329.03512
LHC.MB.B24L8.E                                   4797.17329     6052.42131    329.03576
LHC.MB.B24L8.S                                   4795.49054     6041.75318    329.04039
LHC.MB.A24L8.E                                   4794.71689     6036.95967    329.0376
*VXY
LHC.MB.A24L8.S                                   4792.98037     6026.29701    329.04296
*VXZ
LHC.MQ.23L8.E                                    4792.29507     6022.10132    329.04920
LHC.MQ.23L8.S                                    4791.83184     6019.29436    329.05074
*VYZ
LHC.MB.B23L8.S                                   4786.67425     5989.03963    329.06361
```

### Point declaration in a sub-frame

A point can be declared also within a frame.

Following table specifies where can particular point type keywords appear:

| Context | POIN | CALA | VXZ | VXY | VYZ | VZ  | Comments |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| Root frame | x   | x   | x   | x   | x   | x   |     |
| Sub frames | x   | x   |     |     |     |     |     |

In a sub-frame only POIN and CALA may occur.

The coordinates of the points are always given in the sub-frame in which the point is declared.

The points can be declared within a frame with degrees of freedoms. Only \*CALA point can have an influence on the reconstruction by LS of the frame degree of freedoms.

```text
Example:

*FRAME  F1    1000    2000    3000    10    2    350   1  
*CALA
PT1                                   4800.60321     6074.89790    2000
PT2                                    4800.18209     6072.08288    2410
*POIN
PT3                                   4795.49054     6041.75318    2510
    *FRAME  F2    20    40    60    350    100    200   1  
    *CALA
    PT4                                   4799.54724     6067.89932    -250
    *POIN
    PT5                                   4797.91877     6057.22215    350
    PT6                                   4797.17329     6052.42131    375
    PT7                                   4794.71689     6036.95967    259
    *ENDFRAME
*ENDFRAME
%In that case points are defined in the sub-frame F1 and its child frame F2.
```

### Point with Sigma (from LGC 2.8.0)

The POIN keyword allows defining a 1-sigma error ellipsoid around provisional point coordinates through either a rotated system or a covariance matrix. This ellipsoid can be defined in two mutually exclusive ways:

**Option 1:** Define the ellipsoid by a rotation matrix (4 parameters: bearing, slope, roll, half-deflection) with 1-sigma precision along the XYZ axes of the rotated system.  
**Option 2:** Define the ellipsoid using a covariance matrix.

The ellipsoid is always defined in the FRAME object where the point is declared.

```text
Usage:
    Point_Name x_m y_m z_m   [SX sigmaX_mm] [SY sigmaY_mm] [SZ sigmaZ_mm] [BEAR bearing_gon] [SLOPE slope_gon] [ROLL roll_gon] [HDEFL hdefl_gon] [APRICOV covmatrix]
------

Mandatory arguments:
    Point_Name:     Name of the point, a point can only be declared once in the whole file.
    x_m:            X coordinate of the point [m].
    y_m:            Y coordinate of the point [m].
    z_m:            Z coordinate of the point [m].
    
Optional arguments:
Option 1:
    SX:             1-sigma precision of the observation along the rotated system X-axis [mm]. Positive value and 0 (indicating fixing this dimension) are allowed.
    SY:             1-sigma precision of the observation along the rotated system Y-axis [mm]. Positive value and 0 (indicating fixing this dimension) are allowed.
    SZ:             1-sigma precision of the observation along the rotated system Z-axis [mm]. Positive value and 0 (indicating fixing this dimension) are allowed.
    BEAR:           Bearing angle used to define the rotation matrix [gon].  Convention: Left hand rule / clockwise. 
    SLOPE:          Slope angle used to define the rotation matrix [gon]. Convention: Right hand rule / anti-clockwise. 
    ROLL:           Roll angle used to define the rotation matrix [gon]. Convention: Left hand rule / clockwise. 
    HDEFL:          Half-Deflection angle used to define the rotation matrix [gon]. Convention: Right hand rule / anti-clockwise. 
Option 2:
    APRICOV:       Covariance matrix input. Flag must be followed by the definition of a covariance matrix in one of the following form: 
        DIAG(xx,yy,zz)                      Defines 3x3 diagonal covariance matrix by expressing only the diagonal. The values xx, yy and zz must be separated by commas, no space is allowed and are expressed in [m2]. 
        MAT(xx,xy,xz,yx,yy,yz,zx,zy,zz)     Defines 3x3 covariance matrix by expressing the 9 individual values [m2]. The matrix should be symmetric and positive definite. The values xx, xy, etc... must be separated by commas, no space is allowed and are expressed in [m2]. 
 
```

The rotation matrix described by the Flag in option one is based on the [beamline Element](https://edms.cern.ch/document/1476360) edms document defining the following LGC2 style Frame for vectors:

|     | Frame Name | Tx  | Ty  | Tz  | Rx  | Ry  | Rz  | Scale |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| \*FRAME | RSTI | 0   | 0   | 0   | 0   | 0   | BEAR | 1   |
| \*FRAME | RST | 0   | 0   | 0   | \- SLOPE | ROLL | \-HDEFL | 1   |

This frame modelization is standard and goes from the CCS to the RST definition of an element. This frame structure is also used by Geode for the creation of Elements' frames. The SX, SY and SZ are then expressed in this RST system.

The Flags in Option 1 can be all or individually set. Only setting a SX, for instance, will tell that the point is constraint along the active frame X-axis, while the other dimensions are unconstraint.

The FRAME modelization can be combined to one rotation matrix applied in the system in which the point is declared.

Examples

```text
*POIN
%Points with sigma are applied in the root FRAME + rotation matrix when applicable.
P1 1 2 3 SX 1
P2 1 2 3 SX 1 SY 2 SZ 3
P3 1 2 3 SX 1 SY 2 SZ 3 BEAR 10 SLOPE 20 ROLL 30 HDEFL 10
P4 1 2 3 BEAR 10 SLOPE 20 ROLL 30 HDEFL 10
P5 1 2 3 BEAR 20 SX 1
P6 1 2 3 APRICOV MAT(1,0,0,0,4,0,0,0,9)
P7 1 0 0 APRICOV DIAG(1,2,3)
P9 1 0 0
ST 0 0 0 SX 0.1 SY 0.2 SZ 0.4
*FRAME test 0 0 0 10 20 30 1
*POIN
%Points with sigma are applied in the test FRAME + rotation matrix when applicable.
P8 1 2 3 SX 1 SY 3
P11 1 2 3 SX 1
P12 1 2 3 SX 1 SY 2 SZ 3
P13 1 2 3 SX 1 SY 2 SZ 3 BEAR 10 SLOPE 20 ROLL 30 HDEFL 10 
P14 1 2 3 BEAR 10 SLOPE 20 ROLL 30 HDEFL 10
P15 1 2 3 BEAR 20 SX 1
P16 1 2 3 APRICOV MAT(1,0,0,0,4,0,0,0,9)
P17 1 0 0 APRICOV DIAG(1,2,3)
P19 1 0 0
*CALA
TEST 1 2 3
*ENDFRAME
```

#### Points with standard deviations in x/y/z

For points declared within a \*POIN section there is the option to give a-priori precisions using the tags SX, SY and SZ.

It is possible to specify the standard deviations only for a subset of coordinates and it is possible to block certain coordinates by giving a 0 value as precision.

Examples:

```erl
*POIN
A 1 2 3 SX 4 SY 5 SZ 6      % the point is free but its coordinates are connected to the given coordinates (1,2,3 [m]) with standard deviations in x,y,z direction of (4,5,6 [mm])
B 1 2 3 SX 4 SY 5           % as point A but the z coordinate has no weight assigned, it remains totally free
C 1 2 3 SX 0 SY 1           % the point has a fixed x-coordinate value of 1m, the y coordinate associated to the provisional value with standard deviation of 1mm, z coordinate totally free
D 1 2 3 SX 0 SY 0           % x and y coordinate are fixed, z free (equivalent to *VZ)
```

Assigning standard deviations of 0 \[mm\] can be used to achieve the equivalent effect of defining a point using the `*VXZ`, `*VXY` ... keywords. All combinations can be used and the standard deviations can also be set for points defined in subframes.

#### Point with standard deviations in rotated coordinate system

Angles (bearing, roll, slope, half deflection) describing a transformation between the CCS and RST coordinates can be specified optionally using the tags BEAR,SLOPE,ROLL,HDEFL.

In this case, the standard deviations are with respect to the accordingly rotated RST coordinate system. A description of the CCS → RST transformation motivated by the Beamline Definitions can be found in https://edms.cern.ch/document/1476360/3 (p.20 Figure 6 and Section 7).

Usually these angles come from databases like GEODE.

```lgc
*POIN
E 1 2 3 SX 4 SY 5 SZ 6  BEAR 1 SLOPE 2 ROLL 3 HDEFL 4 % as point A but the standard deviations are given in a rotated RST coordinate system. Angles are given in [gon]
```

#### Point with full apriori covariance matrix (APRICOV)

Instead of giving standard deviations on the x,y,z coordinates, it is also possible to specify a full 3x3 apriori covariance matrix for the point coordinates using the tag APRICOV. Such a matrix can for example be extracted from a previous computation.

LGC checks the apriori covariance matrix for symmetry and positive definiteness.

Below example lists and explains the two possibilities to enter a covariance matrix for a point:

```lgc
*POIN
F 0 0 0 APRICOV DIAG(1,2,3)                 % the DIAG tag can be used to define the diagonal of the apriori covariance matrix
G 0 0 0 APRICOV MAT(2,1,0,1,2,1,0,1,2)      % the MAT tag can be used to define the full apriori covariance matrix, row by row
```

When using DIAG or MAT, make sure the entries are between parenthesis, separated by commas and without spaces.

The combination of apriori covariance matrices and angles is not possible.

### Result File

In the result file, at the beginning of the file, all points are listed in the ROOT Frame.