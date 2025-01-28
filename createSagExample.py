import random
import numpy as np

# Function to generate random 3D coordinates with 8 digits
maxCoord=3.5
# version that generates a cylinder
def generate_coordinates(n):
    radius=0.3
    height=10
    return [(round(radius*np.cos(alpha := np.random.uniform(0, 2*np.pi)), 8), 
             round(np.random.uniform(-height/2, height/2), 8), 
             round(radius*np.sin(alpha), 8)) 
            for _ in range(n)]
# def generate_coordinates(n):
#     ##return [(round(random.uniform(-maxCoord,maxCoord), 8), round(random.uniform(-maxCoord,maxCoord), 8), round(random.uniform(-maxCoord/50,maxCoord/50), 8)) for _ in range(n)]
#     return [(round(random.uniform(-maxCoord,maxCoord), 8), round(random.uniform(-10*maxCoord,10*maxCoord), 8), 0) for _ in range(n)]

# Function to create second list with modified third coordinate
# Vivien: magnitude of sag around 0.2mm / 3m 
vertCurv=0.234567
# sag should be around curvature*maxCoord^2
#vertSag=-vertCurv*maxCoord**2
vertSag=1
#radCurv=0.023456
#radSag = 0.3456
radCurv=0
radSag = 0
#radSag = 12.345
def modify_coordinates(coords):
    return [(round(radSag+x +radCurv * (y)**2, 8),y, round(vertSag+z + vertCurv * (y)**2, 8)) for x, y, z in coords]
    #return [(x, y, round(vertSag+z + vertCurv * (y)**2, 8)) for x, y, z in coords]

# Function to add perturbations to coordinates
sigmaOBSXYZmm=0.1
# to be consistent with the obsxyz
pert=sigmaOBSXYZmm*1e-3
def perturb_coordinates(coords):
    return [(round(x + random.uniform(-pert, pert), 8),
             round(y + random.uniform(-pert, pert), 8),
             round(z + random.uniform(-pert, pert), 8)) for x, y, z in coords]

# Generate list of 3D coordinates
num_coordinates = 100
coordinates = generate_coordinates(num_coordinates)

# Create second list with modified third coordinate
modified_coordinates = modify_coordinates(coordinates)

# Add perturbations to modified coordinates
perturbed_coordinates = perturb_coordinates(modified_coordinates)

# Write results to a text file
with open('sagTestGenerated.lgc', 'w') as file:
    file.write("*TITR\n")
    file.write("testing sag calculation\n")
    file.write("*OLOC\n")
    file.write("*CONSI\n")
    file.write("*APRI\n")
    file.write("*JSON\n")
    file.write("*INSTR\n")
    file.write("% defining the adjustable sag element\n")
    file.write("% the angle is the the bearing of subframe y axis\n")
    file.write("*SAGELEMENT sag0 testFrame 10 0 1 0 0 VS VC RS RC \n")
    file.write("\n*FRAME testFrame 0 0 0 0 0 0 1 RZ \n")
    file.write("*ENDFRAME\n")
    file.write("\n*FRAME rotFrame 0 0 0 0 0 0 1 \n")
    file.write("*CALA\n")
    for i, coord in enumerate(coordinates):
        file.write(f"P{i+1} {coord[0]:.8f} {coord[1]:.8f} {coord[2]:.8f}\n")
    
    file.write("*POIN\n")
    for i, coord in enumerate(modified_coordinates):
        file.write(f"P{i+1}_sagged {coord[0]:.8f} {coord[1]:.8f} {coord[2]:.8f}\n")
    
    file.write("\n*OBSXYZ\n")
    for i, coord in enumerate(perturbed_coordinates):
        file.write(f"P{i+1}_sagged {coord[0]:.8f} {coord[1]:.8f} {coord[2]:.8f} {sigmaOBSXYZmm} {sigmaOBSXYZmm} {sigmaOBSXYZmm} \n")
    
    file.write(f"*SAGCONNECT sag0\n")
    for i in range(num_coordinates):
        file.write(f"P{i+1} P{i+1}_sagged\n")
    file.write("*ENDFRAME\n")

print("Coordinates have been written to 'sagTestGenerated.lgc'")
