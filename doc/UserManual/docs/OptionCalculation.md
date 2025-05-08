# Calculation options

## ALLFIXED

When introduced, the program will run as if all points were listed under CALA - that is being fixed in all dimensions. The output file is in the same format, except having a recalculated V0 for all rounds of measurements, or a recalculated instrument height or distance correction.

For ECSP (V0) and PLR3D (V0, Rx, Ry) two solutions can be found. They are written in the output file, the user should analyze the results.

*NB: Usable only with fixed frame.*

```text
Usage:
    *ALLFIXED
```

## LIBR – deprecated in version 2.7, see new CONSI

The adjusted network is calculated as a free network. Some constraints are added depending of the points type: [LIBR constraints identifier](https://confluence.cern.ch/display/SUS/LIBR+constraints+identifier)

*NB: Usable only with fixed frame.*

**NB 2: Can be used with SIMU option.**

```text
Usage:
    *LIBR
```

## CONSI

The keyword \*CONSI triggers a consistency check to detect issues with identifiability of points or groups of points.

This can e.g. occur if measurements are accidentally commented and the geometric measurement configuration has no unique solution, which usually leads to non-converging behavior or matrix inversion problems.

The check is based on analyzing the first design matrix before the first iteration of the least square solution algorithm is carried out. More information on the method can be found at https://edms.cern.ch/document/2788420/1

*Free Network computations (since version 2.7)*

The optional tag LIBR instructs LGC to attempt an automatic detection of a minimal set of constraints (center of gravity, momentum, scale constraint) making the computation possible. The check is based on an analysis of the \*CONSI result and a comparison of the detected problematic directions with the action a virtual Helmert transformation would have on the involved points. The automatic constraint detection will only impose constraints con points that actually are not uniquely determinable in ROOT coordinates.

For more information see also https://indico.cern.ch/event/1390797/

The constraint choice can be manually overridden using any subset of the additional tags \[TX TY TZ RX RY RZ SCL\] and will impose the corresponding translation, rotation and/or scale constraints. Manually imposed constraints are always imposed on all points

```text
Usage:
    *CONSI [LIBR [TX TY TZ RX RY RZ SCL]]
    
Examples:
*CONSI % triggers a consistency check. results are written in the .log2 file. Computation is stopped if a problem is detected

*CONSI LIBR % does a consistency check and in case a problem is detected there will be an attempt to find a minimal set of constraints that makes the computation possible. After this a computation is started with the proposed constraints

*CONSI LIBR TX RY % does a consistency check, displays the results but then attempts the calculation with the user defined constraints imposed on all points. In this example the x coordinate of the center of gravity and the rotation around the y axis will be fixed to the values defined by the provisional positions
```

## SIMU

Simulates the network to gain insight into its error behavior before taking measurements. Measurement values are allowed but ignored. The desired number of simulations needs to be specified in any case.

```text
Usage:
    *SIMU n [ s ]
         where n is the number of simulations in the set of natural numbers {1, 2, 3, …}, and
               s is the seed value in the set of integers {…, −2, −1, 0, 1, 2, …}.

Example:  
    *SIMU 10  
    *SIMU 10 5


```

Remarks

- If n is zero or negative, then only one simulation is executed.
- If s is not provided, then the seed automatically gets a unique value based on time.
- Every following number is ignored, e.g., \*SIMU n s x y z
- A comment can be added in any case, starting with %, e.g., \*SIMU n s % comment
- If no argument is provided by the user, LGC returns in the log file the following statement: "\*SIMU accepts either 1 or 2 arguments but zero (0) arguments were provided."

## PREC

Allows to modify the number of the digits in the results, and fix the convergence criteria, to be consistent with the expected precision . By default, results are given with 5 digits.

The convergence criteria is fixed to 0.5\*(10^-PREC). The computation iterations will only stop if this criteria is reached for every unknown. By lowering the PREC number, error detection can be done in some case on problematic computation.

```text
Usage:
    *PREC n,  where n is an integer from 0 to 7
    
Example:  
    *PREC 6   %Will give a convergence criteria of 0.0000005.
```