#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "LGCController.h"

namespace py = pybind11;

PYBIND11_MODULE(pyLGC, m)
{
	py::class_<LGCController>(m, "LGCController")
		.def(py::init<const std::string &>(), py::arg("path"))
		// get the currently set parameter values
		.def("getParameter", &LGCController::getParameters)
		// set the parameter values
		.def("setParameter", &LGCController::setParameters, py::arg("par"))
		// compute and get the constraint misclosure
		.def("getW", &LGCController::getMisclosure, py::arg("par"))
		// compute and get the constraint misclosure
		.def("getW2", &LGCController::getConstraintMisclosure, py::arg("par"))
		// compute and get the A matrix (d misclosure /d parameter)
		.def("getA", &LGCController::getFirstDesignMatrix, py::arg("par"))
		// compute and get the A2 matrix (d constraint misclosure /d parameter)
		.def("getA2", &LGCController::getFirstConstraintDesignMatrix, py::arg("par"))
		// compute and get the B matrix (d misclosure /d observation)
		.def("getB", &LGCController::getSecondDesignMatrix, py::arg("par"))
		// diagonal of weight matrix
		.def("getPv", &LGCController::getPv, py::arg("par"))
		// given a row-index of a element of the misclosure vector or A matrix get the linenumber in the inputfile where the measurement is defined
		.def("getLineNumber", &LGCController::getLineNumber, py::arg("par"))
		// for controlling the masking:
		// parameter masking will split the parameters in active and inactive (masked). in this state the evaluator expects a vector of length (active) and evaluates at a
		// combination of the actively set parameters and at the incative parameters, which have the values they had at the moment of setting the mask observation masking essentially is projecting the model function onto the unmasked indices.
		.def("setObservationMask", &LGCController::setObservationMask, py::arg("par"))
		.def("setParameterMask", &LGCController::setParameterMask, py::arg("par"))
		.def("getObservationMask", &LGCController::getObservationMask)
		.def("getParameterMask", &LGCController::getParameterMask)
		.def("unmask", &LGCController ::unmask);


}
