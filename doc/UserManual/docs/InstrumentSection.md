# Instruments (*INSTR)

All instruments and targets involved in the measurements must be declared in the `*INSTR` section of the input file.

The section begins with the keyword `*INSTR`, followed by one or more instrument-specific keywords:

- `*POLAR` Total station or laser tracker
- `*CAMD` Camera type instrument
- `*EDM` Electronic distance meter
- `*LEVEL` Levelling instrument
- `*SCALE` Standard (desktop) scale
- `*INCL` Inclinometer instrument
- `*HLSR` Hydrostatic levelling sensor
- `*WPSR` Wire Positioning sensor

Each instrument used in the project must be declared with its corresponding keyword.

For instruments such as `POLAR`, `EDM`, or `LEVEL`, a list of associated targets should follow. Each target is defined on a separate line, beginning with the Target ID.

All targets involved in measurements for a given instrument must be listed.

Instrument-specific parameters—as well as parameters for instrument/target pairs—are specified within this section.

Some arguments are provided directly on the instrument declaration line, while others are associated with individual target lines. **All required parameters must be included**.

Even if no instrument is required by the observations defined in the file, the `*INSTR` keyword **must still be present** in order to use the LGC2 formatting. In such cases, it can appear without any content.

If the `*INSTR` keyword is missing, LGC will interpret the file as using **LGC1 formatting**, which differs significantly from the LGC2 format described in this guide.  
**LGC1 formatting is not covered in this manual and will be deprecated in future versions.**

## \*POLAR

Defines an instrument that makes polar measurements, such as a total station or laser tracker, and it's associated targets.

```text
Usage:
    *POLAR instr_ID    default_tgt_ID   instr_height_m    sigma_instr_height_mm    sigma_instr_centering_mm   const_angl_gon
    tgt_ID   sigma_ANGL_cc  sigma_ZEND_ cc   sigma_DIST_mm ppm_mm   dCorr_adjustable  dCorr_m   sigma_dCorr_mm   sigma_tgt_centering_mm   tgt_height_m   sigma_tgt_height_mm
    -----
    
    Instrument definition arguments:
    instr_ID:                 A unique name for the instrument.
    default_tgt_ID:           The name of the default target to use with this instrument. The target must be defined in this instrument section.
    instr_height_m:           Height of the instrument [m].
    sigma_instr_height_mm:    1-sigma precision of the instrument height [mm].
    sigma_instr_centering_mm: 1-sigma precision of the instrument centering [mm].
    const_angl_gon:           A known horizontal angle constant of the instrument [gon].
    Target definition arguments:
    tgt_ID:                   Target ID name. At least one must be defined and one shall be defined as default in the Instrument definition.
    sigma_ANGL_cc:            1-Sigma precision of the "Horizontal" Angle [cc].
    sigma_ZEND_cc:            1-Sigma precision for the Zenith Distance ("Vertical" angle) [cc].
    sigma_DIST_mm:            1-Sigma precision for the Distance measured [mm].
    ppm_mm:                   ppm to apply to the measured distance [mm/km].
    dCorr_adjustable:         bool(0 or 1) Identifies if the distance correction value is an "unknown value" to be determined in the adjustment.
    dCorr_m:                  Distance correction value for the measurements [m].
    sigma_dCorr_mm:           1-Sigma precision for the distance correction [mm].
    sigma_tgt_centering_mm:   1-Sigma precision for the target centering [mm].
    tgt_height_m:             Height of the target [m].
    sigma_tgt_height_mm:      1-Sigma precision of the target height [mm]
    
Example:
    *POLAR     AT401_1     CCR2     0.2598     0     0    0
    CCR1     3    3     0.02     6     1     0     0    0     0     0
    CCR2     5    6     0.03     8     0     -0.2  0    0     0     0
    *POLAR     AT401_2     CCR3     0.28       0     0    0
    CCR3     3    3     0.02     6     1     0     0    0     0     0
```

**Important remark:**

In LGC2, the prism constant (`dCorr_m`) is defined for a given pair instrument/target. In other words, a prism defined with another instrument will be considered as a different prism with a different constant even if the prism name (`tgt_ID`) is declared identical for both instruments. When defined as unknown (both instrument parameters `dCorr_adjustable` set to 1), the Least Square process will try to estimate 2 distinct parameters (unknowns) and not a single one, even though it concerns one unique physical target.

## \*CAMD

Defines a camera instrument and it's associated targets.

A camera observation on a target is defined by a unit vector. See the CAM definition for an example.

```text
Usage:
    *CAMD instr_ID  default_tgt_ID sigma_instr_centering_mm
    tgt_ID sigma_X  sigma_Y  sigma_dist_mm sigma_tgt_centering_mm 
    -----
    
    Instrument definition arguments:
    instr_ID:                 A unique name for the instrument.
    default_tgt_ID:           The name of the default target to use with this instrument. The target must be defined in this instrument section.
    sigma_instr_centering_mm: 1-sigma precision of the instrument centering [mm].
    Target definition arguments:
    tgt_ID:                   Target ID name. At least one must be defined and one shall be defined as default in the Instrument definition.
    sigma_X:                  Standard deviation of a measured X component of the unit vector [mm/m].
    sigma_Y:                  Standard deviation of a measured Y component of the unit vector [mm/m].
    sigma_dist_mm:            Standard deviation of a distance measurement from the station to the target [mm]. Used only for UVD.
    sigma_tgt_centering_mm:   Standard deviation of the centering of the target[mm].
```

Note: Even if the distance are not taken by the instrument, the distance precision must be specified.

## \*EDM

Defines an electronic distance meter that can be stationed on a point and its associated targets.

```text
sage:
    *EDM   instr_ID   default_tgt_ID   instr_height_m   sigma_instr_height_mm   sigma_instr_centering_mm
    tgt_ID   sigma_dist_mm  ppm_mm  dCorr_adjustable   dCorr_m   sigma_dCorr_mm  sigma_tgt_centering_mm   tgt_height_m   sigma_tgt_height_mm
    -----
    
    Instrument definition arguments:
    instr_ID:                 A unique name for the instrument.
    default_tgt_ID:           The name of the default target to use with this instrument. The target must be defined in this instrument section.
    instr_height_m:           Height of the instrument [m].
    sigma_instr_height_mm:    1-sigma precision of the instrument height [mm].
    sigma_instr_centering_mm: 1-sigma precision of the instrument centering [mm].
    
    Target definition arguments:
    tgt_ID:                   Target ID name. At least one must be defined and one shall be defined as default in the Instrument definition.
    sigma_dist_mm:            1-Sigma precision for the Distance measured [mm].
    ppm_mm:                   ppm to apply to the measured distance [mm/km].
    dCorr_adjustable:         bol(0 or 1): Identifies if the distance correction value is an "unknown value" to be determined in the adjustment.
    dCorr_m:                  Distance correction value for the measurements [m].
    sigma_dCorr_mm:           1-Sigma precision for the distance correction [mm].
    sigma_tgt_centering_mm:   1-Sigma precision for the target centering [mm].
    tgt_height_m:             Height of the target [m].
    sigma_tgt_height_mm:      1-Sigma precision of the target height [mm].

Example:
    *EDM Instr_Theo Prism_Theo 0 0 0
     Prism_Theo .1 .1 0 0 0 0 0 0

```

Important Remark: Instrument and target are working in pairs (see remark in `POLAR` definition).

## \*LEVEL

Defines a (digital) levelling instrument and its associated levelling staves.

```text
Usage:
    *LEVEL instr_ID default_staff_ID instr_height_m sigma_instr_height_mm collimationAngle_adjustable collimationAngle_gon    
    staff_ID   sigma_dist_mm   ppm_mm   dCorr_m   sigma_dCorr_mm   tgt_height_m   sigma_tgt_height_mm
     -----

    Instrument definition arguments:

    instr_ID:                    A unique name for the instrument.
    default_staff_ID:            The name of the default staff to use with this instrument. The target must be defined in this instrument section.
    instr_height_m:              Height of the instrument [m]. Associated flag: IH.
    sigma_instr_height_mm:       1-sigma precision of the instrument height [mm]. Associated flag: IHSE
    collimationAngle_adjustable: Identifies if the collimation angle is an "unknown value" to be determined in the adjustment. Values: 1 for unknown, 0 for known.
    collimationAngle_gon:        Value of the collimation angle [gon].

    Staff definition arguments:
    staff_ID:                    Staff ID name. At least one must be defined and one shall be defined as default in the Instrument definition.
    sigma_dist_mm:               1-Sigma precision for the Distance measured [mm]. Associated flag: OBSE.
    ppm_mm:                      ppm to apply to the measured distance [mm/km]. Associated flag: PPM.
    dCorr_m:                     Distance correction value for the measurements [m].
    sigma_dCorr_mm:              1-Sigma precision for the distance correction [mm].
    tgt_height_m:                Height of the target [m]. Associated flag: TH.
    sigma_tgt_height_mm:         1-Sigma precision of the target height [mm]. Associated flag: THSE.

Prior to LGC v2.8.0, the format was:
    *LEVEL   instr_ID   default_staff_ID  collimationAngle_adjustable  collimationAngle_gon
```

Keyword needed if the \*DLEV type of measurements need to be used. Most of the levelling is defined with the type \*DVER. In that case no \*LEVEL instruments need to de specified.

```text
Example:
  *LEVEL default_Level default_Staff 0 0 0 0
  default_Staff 0.017 0 0 0 0 0
  *LEVEL coll_Level default_Staff 1 0.01 1 0
  default_Staff 0.017 1 0.02 0.03 0.1 0.01
```

## \*SCALE

Defines a scale instrument (e.g. ruler, offset measurement device). Does not use a target, and as a consequence it does not have additional lines like the other instruments.

```text
Usage:
    *SCALE  instr_ID  sigma_dist_mm  ppm_mm  dCorr_m  sigma_dCorr_mm  sigma_instr_centering_mm
    -----
    
    Instrument definition arguments:
    instr_ID:                    An unique name for the instrument.
    sigma_dist_mm:               1-Sigma precision for the Distance measured [mm].
    ppm_mm:                      ppm to apply to the measured distance [mm/km].
    dCorr_m:                     Distance correction value for the measurements [m].
    sigma_dCorr_mm:              1-Sigma precision for the distance correction [mm].
    sigma_instr_centering_mm:    1-sigma precision of the instrument centering [mm].

Example:
    *SCALE RS_12   .1   0   0   0   0
    *SCALE RS_13   .2   0   0   0   0
```

## \*INCL

Available from version 2.03.01

Defines an inclinometer instrument, i.e. measuring an angle. Does not use a target, and as a consequence it does not have additional lines like the other instruments.

```text
Usage:
    *INCL  instr_ID  sigma_angle_cc  sigma_ppm  aCorr_gon  sigma_aCorr_cc  refAngle_gon  sigma_refAngle_cc
    -----
    
    Instrument definition arguments:
    instr_ID:                    A unique name for the instrument.
    sigma_angle_cc:              1-Sigma precision for the angle measured [cc].
    sigma_ppm:                   1-Sigma precision for the angle measured [μrad or μm/m]. It is added (NOT QUADRATICALLY) in the sigma_angle_cc. Available from version 2.6.1
    aCorr_gon:                   Angle correction value for the measurements [gon]. This value can be used to define an instrumental constant. It follows a Right-Hand rule (anti-clockwise) and is added to the observation.
    sigma_aCorr_cc:              1-Sigma precision for the angle correction [cc].
    refAngle_gon:                Default reference angle correction value for the measurements [gon]. This value can be used to define the reference angle as defined in GEODE DB.  It follows a right-Hand rule (anti-clockwise) and is added to the observation.
    sigma_refAngle_cc:           1-Sigma precision for the reference angle correction [cc].

 Example:
    *INCL I_12   5   3   1  1  0  0 
    *INCL I_13   10  3  -5  2  1  2
```

## \*HLSR

From version 2.4.1 : Defines a hydrostatic levelling sensor, i.e. measuring a distance. Does not use a target, and as a consequence it does not have additional lines like the other instruments.

```text
Usage:
    *HLSR  instr_ID  sigma_dist_mm  sigma_instr_height_mm  sigma_instr_centering_mm
    -----
    
    Instrument definition arguments:
    instr_ID:                    An unique name for the instrument.
    sigma_dist_mm:               1-Sigma precision for the Distance measured [mm].
    sigma_instr_height_mm:       1-sigma precision of the instrument height [mm].
    sigma_instr_centering_mm:    1-sigma precision of the instrument centering [mm].

 Example:
    *HLSR HL_SNSR_1   0.03   0     0
    *HLSR HL_SNSR_2   0.05   0.02  1
```

## \*WPSR

From version 2.6.0: Defines a Wire Positioning Sensor, i.e. measuring 2 distances. Does not use a target, and as a consequence it does not have additional lines like the other instruments.

```text
Usage:
    *WPSR  instr_ID  sigma_dist_X_mm  sigma_dist_Z_mm  sigma_instr_centering_X_mm  sigma_instr_centering_Z_mm
    -----
    
    Instrument definition arguments:
    instr_ID:                    An unique name for the instrument.
    sigma_dist_X_mm:             1-Sigma precision for the X Distance measured [mm].
    sigma_dist_Z_mm:             1-sigma precision for the Z Distance measured [mm].
    sigma_instr_centering_X_mm:  1-sigma precision of the instrument centering in the station's X-axis coordinate system [mm].
    sigma_instr_centering_Z_mm:  1-sigma precision of the instrument centering in the station's Z-axis coordinate system [mm].
    
Example:
    *WPSR   WPS_1   0.03    0.03    0.04    0.04
    *WPSR   WPS_2   0.07    0.01    0.14    0.01
```