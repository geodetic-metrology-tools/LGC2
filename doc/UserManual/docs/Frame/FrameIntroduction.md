The FRAME feature in LGC introduces the concept of hierarchical local Cartesian coordinate systems. It enables users to organize points and measurements in a structured way, supporting nested frames that reflect the real-world hierarchical relationships between components of a physical installation.

Originally introduced in LGC v2 and significantly extended since version 2.6.0, frames allow users to constrain and parameterize translations, rotations, and scale, offering greater flexibility and accuracy in geodetic computations.

Each `*FRAME` block defines a local frame in terms of:

- 3 translations along X, Y, Z axes
- 3 rotations about those axes
- 1 scale factor
- Optional constraints on which parameters are free to estimate (Degree of freedom)