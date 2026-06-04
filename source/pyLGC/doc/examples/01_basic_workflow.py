"""
Basic workflow: Load a network, inspect it, and run a Least Squares Adjustment.
This script demonstrates the core API mechanics of pyLGC for surveyors.
"""

import pyLGC

# 1. LOAD THE NETWORK
# The Evaluator parses the .lgc2 file and initializes the network geometry.
ev = pyLGC.Evaluator("Title-Example.lgc2")

# getProblemDimensions() returns an object containing the number of Observations (OIndex),
# Equations (EIndex), and Unknown parameters to estimate (UIndex).
idx = ev.getProblemDimensions()
print("=== Network Adjustment Summary ===")
print(f"Observations: {idx.OIndex} | Unknowns: {idx.UIndex}")


# 2. INSPECT THE STATIONS AND FRAMES
# getPoint() and getFrame() return objects representing network elements.
print("\n=== Inspecting Network Elements ===")
for name in ["H4.XBPF.22716.E", "H4.XBPF.22716.S"]:
    pt = ev.getPoint(name)
    # getRelativeUnknIndices() returns a NumPy array of indices. 
    # If the array is empty, the point is fully fixed. Otherwise, it is estimated.
    status = "Estimated" if len(pt.getRelativeUnknIndices()) > 0 else "Fixed"
    print(f"Station '{pt.getName()}': {status}")


# 3. PRE-ADJUSTMENT EVALUATION
# IMPORTANT: You must call evaluateAtParameters() before retrieving matrices or 
# misclosures. This triggers the internal C++ engine to calculate theoretical 
# measurements based on the current coordinates.
ev.evaluateAtParameters()

# getMisclosure() returns a 1D NumPy array representing the difference between 
# field measurements and theoretical measurements (the 'w' vector).
w_vector = ev.getMisclosure()
rms_before = (sum(x*x for x in w_vector) / len(w_vector))**0.5
print(f"\nInitial RMS of Misclosures: {rms_before:.6e}")


# 4. RUN THE LEAST SQUARES ADJUSTMENT
# solve() returns a tuple: (success_boolean, solution_array)
# The solution array contains the FINAL absolute estimated parameters (coordinates),
# NOT the iterative shifts (dX).
ok, solution = ev.solve()

if ok:
    # 5. POST-ADJUSTMENT ANALYSIS
    # setParameters() loads the final adjusted coordinates into memory, BUT...
    ev.setParameters(solution)
    
    # ...you MUST call evaluateAtParameters() again to force the engine to 
    # re-calculate the network geometry with the new coordinates!
    ev.evaluateAtParameters()
    
    # Now getMisclosure() returns the a posteriori residuals (the 'v' vector).
    v_vector = ev.getMisclosure()
    
    rms_after = (sum(x*x for x in v_vector) / len(v_vector))**0.5
    print(f"Adjustment Succeeded! Post-Adjustment RMS: {rms_after:.6e}")
else:
    print("Adjustment Failed to Converge!")
