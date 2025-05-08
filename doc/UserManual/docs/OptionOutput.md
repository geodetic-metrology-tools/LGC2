# Output options

## APRI

A priori sigma ( = 1) is used for all the calculation. Even if Sigma 0 a posteriori is outside the range, it isn't apply to the residual.

```text
Usage:
    *APRI
```

## COVAR

Create an output file (.cov) listing all frames and their points and giving the covariance matrix for each variable object.

```
Usage:
    *COVAR
    

```

Output format:

- Frame:
    
    ```text
    name   tx(m)   ty(m)    tz(m)   rx(rad)   ry(rad)   rz(rad)   scale(-)
    vartx(m²)    cov_txty(m²)    cov_txtz(m²)        cov_txrx(m*rad)      cov_txry(m*rad)         cov_txrz(m*rad)        cov_txl(m*-)
                  varty(m²)      cov_tytz(m²)        cov_tyrx(m*rad)      cov_tyry(m*rad)         cov_tyrz(m*rad)        cov_tyl(m*-)
                                  vartz(m²)          cov_tzrx(m*rad)      cov_tzry(m*rad)         cov_tzrz(m*rad)        cov_tzl(m*-)
                                                      varrx(rad²)         cov_rxry(rad²)          cov_ryrz(rad²)         cov_rxl(m*-)
                                                                            varry(rad²)           cov_ryrz(rad²)         cov_ryl(m*-)
                                                                                                    varrz(rad²)          cov_rzl(m*-)
                                                                                                                          varl(-)
    ```
    
- Point:
    
    ```text
    name   X (m)   Y(m)   Z(m)
    varx(m²)     cov_xy(m²)       cov_xz(m²)
                  vary(m²)        cov_yz(m²)
                                  varz(m²)
    ```
    

## DEFA

Allows to create an input file for deform software. This file contains the upper triangular entries of the variance-covariance matrix of the point coordinates row by row (equivalent to the lower triangular entries column by column).

```text
Usage:
    *DEFA
```

## EREL

List of point pairs for which the relative error will be computed. Per default the relative error will be computed with respect to coordinates in the "ROOT" frame.

As an optional argument a frame name can be given to compute the relative error with respect to this frame.

See also https://edms.cern.ch/document/2890230/1

```text
Usage:
    *EREL 
    PT1 PT2          % will compute the relative error between PT1 and PT2 in frame "ROOT"
    PT5 PT3 myFrame  % will compute the relative error between PT5 and PT3 in frame "myFrame"
```
![EREL](assets/EREL.png)

## ERELFRAME

The keyword ERELFRAME can be used to compute the Helmert parameters corresponding to the relative transformation between two specified frames.

This computation will be carried out after the estimation process and also includes the precisions and covariances of the relative transformation.

```text
Usage:

*ERELFRAME  
F1 F2      % will compute the relative transformation going from frame F1 to frame F2
F1 ROOT    % this computes the transformation going from frame F1 to ROOT frame (the ROOT frame always exists)
```

## FAUT

Allows calculation of the reliability parameters of each observation and the blunders.  
We distinguish local reliability, internal, external and even for a global network. All of these tools are summarised in the .err file

```
Usage:
    *FAUT [alpha beta]
```

- The alpha percentage is the significance level of the tests, and the risk of rejecting a right measure. By default alpha is equal to 1%.
- The beta percentage is the risk of accepting a false measure. By default beta is equal to 10% (write 0.1)

### Alpha

alpha (significance level of the tests)

- 1 - alpha (confidence level)
- (1-alpha) is the upper limit wherein residuals are less than a tolerance value Wmax.

Example: If the tolerance is 2 times wmax the measurement accuracy, the probability that any residual is less than wmax, this tolerance is 97%.

### Beta

The beta percentage is the risk of accepting a false measure. More dangerous risk than alpha.

Example: if we deliberately falsifies a measure introducing him a blunder, the symmetry axis for the residual distribution will be shifted to a very big willful misconduct, for a little less displaced.

Follow this link for more information: [link to blunder detection](https://confluence.cern.ch/display/SUS/Reliability+and+Blunder+detection)

## FMTP

Allows to format the punc file. By default the separator is a column. If another separator is expected call:

```text
Usage:
    *FMTP SEP "separator"  %where separator is the expected separator. The double quote are needed.
    *FMTP COL              %is also the default separator.
```

## HIST

Allows to write an histogram with the residual per measurement type in the output file. Write the histogram if at least 5 measurements are done.

```text
Usage:
    *HIST
```

## JSON

Allows to write the JSON object that contains the serialized output data. See also the documentation [JSON](https://confluence.cern.ch/display/SUS/JSON).

```text
Usage:
    *JSON        %saves only the main FILENAME.json file.
    *JSON COVAR  %saves covariance matrices in addition to the main object. They will be saved in FILENAME_covar.json.
```

## NODUP

Returns an error if duplicated measurements are found in the file.

```text
Usage:
    *NODUP
```

## PDOR

Allow to give an orientation of the network. It is defined as a bearing between a station point and an observed point.

```text
Usage:
    *PDOR 
    point_name [bearing]  %where the bearing is a gon value
```

- The station point is the first CALA point following a CALA keyword. Therefore: At least one CALA point has to be defined in the ROOT frame.
- The observed point is the point_name and must be defined in POIN or VXY.
- Only one unique PDOR is considered for constraining the orientation of the network.
- In the least square model, PDOR is implemented as an observation with a precision (1 sigma) of 0.001 CC. The precision cannot be changed via the LGC input file.
- \*PDOR keyword must be placed between \*INSTR and the points declaration section: not allowed after keywords: \*CALA, \*POIN, \*VXY, \*VYZ, \*VXZ, \*VZ.

The bearing value is optional. if no bearing is given LGC will compute a theoretical bearing between the station and observed points using the provisional (approx.) coordinate of those points.

For instance, let's take this configuration:

```text
*PDOR
LHC.MSDA.E4L6.S
*CALA
LHC.MQY.4L6.S 2012.596349 10544.114382 377.178563
*POIN
LHC.MSDA.E4L6.S 2139.507073 10492.669225 375.434050
```

No bearing is given therefore LGC will recalculate a bearing between the station and observed points with respect to their provisional values. In this case: 124.5176755 gon.  
<br/>if the PDOR value is set to a value:

```text
*PDOR
LHC.MSDA.E4L6.S  0
```

LGC will consider this bearing to be 0. The computation will try to put the station and the observed points on the Y axis.

In a simple case where only one CALA is declared, the other points are declared in POIN and no additional orientation constraints are defined: it will result in a rotation of your whole computation around the station point.

The value 0 is taken for simplicity in the example, it could be any numerical value.

if the observed PDOR is set to the theoretical bearing coming from the points' provisional values:

```text
*PDOR
LHC.MSDA.E4L6.S  124.5176755
```

The result will be the same as the case without any values.

[Link to an example file with the correct use of a PDOR keyword](https://confluence.cern.ch/download/attachments/22153243/PDOR_example.lgc?version=1&modificationDate=1672910324000&api=v2)

## PRES

Gives [ellips or ellispodal error](https://confluence.cern.ch/display/SUS/Calculation+of+Ellips+and+Ellipsoid+error) for the variable points (POIN, VXY, VXZ or VYZ). The ellips or ellipsoidal data are given in the subframe of the point.

```text
Usage:
    *PRES
```

## PUNC

Writes a list of points with optional data (.coo or .punc). Only one \*PUNC keyword is allowed from the following list:

```text
            % Output Format:
*PUNC       % Point_Name X Y Z
*PUNC E     % Point_NameX Y Z Sx Sy Sz Cxy Cxz Cyz Dx Dy Dz
*PUNC EE    % Point_Name X Y Z direction_1 direction_2 direction_3 half_axis_1 half_axis_2 half_axis_3 Dx Dy Dz
            % The ellipsoid give 3 axis which are defined by an associated direction. 
            % For an ellipse we fill in only the first direction as the bearing of the major axis (half_axis 1 or 2)
*PUNC H     % Point_Name X Y H
*PUNC Z     % Point_Name X Y Z
*PUNC HZ    % Point_Name X Y Z H
*PUNC HN    % Point_Name X Y H N
*PUNC ZHN   % Point_Name X Y Z H N
*PUNC T     % Point_Name X Y Z Sx Sy Sz
*PUNC OUT1  % Point_Name X Y H ID dx dy dz DCum
            % creates a .coo file
*PUNC OUT3  % (not implemented)  
            % write a .mes file with all measurements for GEODE


```

All points coordinates are given in the root frame, so if additional data should be written, only initial root points will have this information.

## SOBS

Write an input file with the simulated observation. Need to be used with SIMU.

```text
Usage:
    *SOBS
```

## CHABA (since v2.02.00)

Write an output file for the best fit calculation with a similar format to CHABA. See page [here](https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=55117116) for more information.