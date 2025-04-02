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

        // Custom constructor binding that accepts a Python string (str) and converts it to std::stringstream
        .def(py::init([](const std::string& content) {
            std::stringstream stream(content);  // Create a stringstream from the Python string
            return TLSEvaluator(stream);    // if you "return new TLSEvaluator(stream)" it will do some weird stuff probably
        }), py::arg("content"))  // Bind the constructor to a string input

        // Get and set parameters
        .def("setParameters", &TLSEvaluator::setParameters, py::arg("para"))
        .def("getEstParams", &TLSEvaluator::getEstParams)
        .def("testParameterSetAndGet", &TLSEvaluator::testParameterSetAndGet)
        .def("testEvaluate", &TLSEvaluator::testEvaluate)

        // tryLGCSolve bindings: using lambda to return a tuple of bool and solution
        .def("tryLGCSolve", [](TLSEvaluator &self) {
            // Convert the Python object (list or array) to the corresponding C++ vector
            Eigen::VectorXd solution = self.getEstParams();
            bool result = self.tryLGCSolve(solution); // Call the method
            return py::make_tuple(result, solution);  // Return both result and solution as a tuple
        })

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

        //// Additional setter and getter helpers
        //.def("setPointParams", &TLSEvaluator::setPointParams, py::arg("para"))
        //.def("setAngleParams", &TLSEvaluator::setAngleParams, py::arg("para"))
        //.def("setPlaneParams", &TLSEvaluator::setPlaneParams, py::arg("para"))
        //.def("setLengthParams", &TLSEvaluator::setLengthParams, py::arg("para"))
        //.def("setTransformationParams", &TLSEvaluator::setTransformationParams, py::arg("para"))
        //.def("setLineParams", &TLSEvaluator::setLineParams, py::arg("para"))

        //.def("getPointParams", &TLSEvaluator::getPointParams)
        //.def("getAngleParams", &TLSEvaluator::getAngleParams)
        //.def("getPlaneParams", &TLSEvaluator::getPlaneParams)
        //.def("getLengthParams", &TLSEvaluator::getLengthParams)
        //.def("getTransformationParams", &TLSEvaluator::getTransformationParams)
        //.def("getLineParams", &TLSEvaluator::getLineParams)
        //
        //// Add any additional helper functions you might need, if required
        //.def("isUptoDate", &TLSEvaluator::isUptoDate)  // Example additional method
    ;
}
