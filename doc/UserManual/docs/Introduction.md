**LGC** is a software toolkit developed and maintained by CERN to perform geodetic data processing using least-squares adjustment methods. It supports a wide range of measurement types and is designed to handle complex geospatial configurations common in large-scale scientific installations.

Originally developed in the 1980s, LGC has undergone several major updates to meet evolving technical requirements. The current version is built in C++, leveraging a modular design and incorporating advanced models for geodetic observations, instruments, and reference frames. It relies on **SurveyLib**, a dedicated C++ library that provides core mathematical routines, spatial object models, and geodetic transformations.

This version of LGC introduces key features such as:

- Support for an arbitrary hierarchy of local Cartesian frames, defined using the `*FRAME` keyword.
    
- Flexible modeling of observation and parameter relationships across different frames.
    
- Detailed handling of systematic and random errors for instruments and targets.
    
- Enhanced error propagation control for high-precision applications.
    

While these capabilities offer significant improvements in flexibility and accuracy, they also increase the complexity of the input structure. LGC is fully controlled via a plain-text input file, which specifies observations, parameters, reference frames, and solver options.

This manual explains the structure and usage of the LGC input file. Each section describes a specific component of the configuration, with examples and recommendations for best practices.

Whether you are a new user or an experienced surveyor, this guide aims to help you make effective use of LGC for your geodetic adjustment tasks.