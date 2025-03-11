#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "TLSEvaluator.h"

namespace py = pybind11;

PYBIND11_MODULE(pyLGC, m)
{
    py::class_<TLSEvaluator>(m, "TLSEvaluator")
        // Constructor: updated to accept a shared_ptr to TLGCData or a stringstream
        .def(py::init<std::shared_ptr<TLGCData>>(), py::arg("data"))
        .def(py::init<std::stringstream &>(), py::arg("fileStream"))

        // Get and set parameters
        .def("setParameters", &TLSEvaluator::setParameters, py::arg("para"))
        .def("getEstParams", &TLSEvaluator::getEstParams)
        .def("testParameterSetAndGet", &TLSEvaluator::testParameterSetAndGet)
        .def("testEvaluate", &TLSEvaluator::testEvaluate)

        // Get observations
        .def("getObservations", &TLSEvaluator::getObservations)

        // Evaluation and matrices
        .def("evaluate", &TLSEvaluator::evaluate)
        .def("getMisclosure", &TLSEvaluator::getMisclosure)
        .def("getAMatrix", &TLSEvaluator::getAMatrix)
        .def("getBMatrix", &TLSEvaluator::getBMatrix)
        .def("getConstraintMisclosure", &TLSEvaluator::getConstraintMisclosure)
        .def("getA2Matrix", &TLSEvaluator::getA2Matrix)
        .def("getPMatrix", &TLSEvaluator::getPMatrix)

        // Additional setter and getter helpers
        .def("setPointParams", &TLSEvaluator::setPointParams, py::arg("para"))
        .def("setAngleParams", &TLSEvaluator::setAngleParams, py::arg("para"))
        .def("setPlaneParams", &TLSEvaluator::setPlaneParams, py::arg("para"))
        .def("setLengthParams", &TLSEvaluator::setLengthParams, py::arg("para"))
        .def("setTransformationParams", &TLSEvaluator::setTransformationParams, py::arg("para"))
        .def("setLineParams", &TLSEvaluator::setLineParams, py::arg("para"))

        .def("getPointParams", &TLSEvaluator::getPointParams)
        .def("getAngleParams", &TLSEvaluator::getAngleParams)
        .def("getPlaneParams", &TLSEvaluator::getPlaneParams)
        .def("getLengthParams", &TLSEvaluator::getLengthParams)
        .def("getTransformationParams", &TLSEvaluator::getTransformationParams)
        .def("getLineParams", &TLSEvaluator::getLineParams)
        
        // Add any additional helper functions you might need, if required
        .def("isUptoDate", &TLSEvaluator::isUptoDate)  // Example additional method
    ;
}
