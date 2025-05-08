# 3D Helmert Transformation

## Overview

The **Helmert transformation** is a standard method used to convert coordinates from one 3D Cartesian reference frame to another. It consists of **three main components**:

1.  A **rotation** matrix defined by three successive Euler angles,
2.  A **uniform scale factor**,
3.  A **translation vector**.

This transformation is widely used in geodesy and surveying, including for applications at CERN. In **LGC** the rotation are following a **RxRyRz** convention with each rotation following a left-hand rule. **Watchout** about the rotation order and convention, in the industry the **RzRyRx** with right-hand rule is more common (but conversion to LGC convention is trivial)

## RxRyRz Rotation Matrix

The rotation matrix **R** is constructed as a product of three fundamental rotation matrices:

**R = Rₓ · Rᵧ · R𝓏**

Where:

- **R𝓏(κ)**: rotation about the Z-axis following left-hand rule,
- **Rᵧ(φ)**: rotation about the Y-axis following left-hand rule,
- **Rₓ(ω)**: rotation about the X-axis following left-hand rule,

Each component is defined as:

$$
R_Z(κ) =
\begin{bmatrix}
\cos κ & -\sin κ & 0 \\
\sin κ & \cos κ & 0 \\
0 & 0 & 1
\end{bmatrix}
$$

$$
R_Y(φ) =
\begin{bmatrix}
\cos φ & 0 & \sin φ \\
0 & 1 & 0 \\
- \sin φ & 0 & \cos φ
\end{bmatrix}
$$

$$
R_X(ω) =
\begin{bmatrix}
1 & 0 & 0 \\
0 & \cos ω & -\sin ω \\
0 & \sin ω & \cos ω
\end{bmatrix}
$$

Note: The angles ω (omega), φ (phi), and κ (kappa) should be given in radians when evaluating trigonometric functions.

The composite rotation matrix in the RxRyRz convention is expressed as:

$$
\mathbf{R}_{\mathbf{xyz}} =
\begin{bmatrix}
\cos{\kappa}\cos{\varphi} & \sin{\kappa}\cos{\varphi} & -\sin{\varphi} \\
\cos{\kappa}\sin{\varphi}\sin{\omega} - \sin{\kappa}\cos{\omega} & \sin{\kappa}\sin{\varphi}\sin{\omega} + \cos{\kappa}\cos{\omega} & \cos{\varphi}\sin{\omega} \\
\cos{\kappa}\sin{\varphi}\cos{\omega} + \sin{\kappa}\sin{\omega} & \sin{\kappa}\sin{\varphi}\cos{\omega} - \cos{\kappa}\sin{\omega} & \cos{\varphi}\cos{\omega}
\end{bmatrix}
$$

Given a general 3D rotation matrix, the parameters may be derived for any given form of the matrix. However, it should be noted that **each specific form of the rotation matrix has a failure case**, where the parameters cannot be uniquely determined.

If we consider this matrix in the form \( R_x R_y R_z \), the rotation parameters can be derived from the matrix as:

$$
\omega = \arcsin(r_{32})
$$

$$
\varphi = \arctan2(-r_{31}, r_{33})
$$

$$
\kappa = \arctan2(-r_{12}, r_{22})
$$

> _Note: The functions `arcsin` and `arctan2` are used to ensure the correct quadrant of the angles. This form is common but may fail when gimbal lock or axis-alignment occurs._

## Direct Transformation

Let **x₀** be a point in the source (Passive) reference frame and **xₙ** the corresponding point in the target (Active) frame. The 3D Helmert transformation is defined as:

**xₙ = l · R · x₀ + T**  
Where:

- **l** is the scale factor (unitless),
- **R** is a 3×3 rotation matrix,
- **T** is a 3×1 translation vector.

## Inverse Transformation

The inverse of the Helmert transformation is given by:

**x₀ = (1/l) · Rᵀ · (xₙ - T)**

Where:

- **Rᵀ** is the transpose of the rotation matrix **R**,
- All other terms are as defined above.
- This allows transforming coordinates from the Active frame back to the Passive frame.