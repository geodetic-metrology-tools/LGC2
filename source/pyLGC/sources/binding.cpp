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
		.def("getParameter", &LGCController::getParameters)
		.def("setParameter", &LGCController::setParameters, py::arg("par"))
		.def("getW", &LGCController::getMisclosure, py::arg("par"))
		.def("getW2", &LGCController::getConstraintMisclosure, py::arg("par"))
		.def("getA", &LGCController::getFirstDesignMatrix, py::arg("par"))
		.def("getA2", &LGCController::getFirstConstraintDesignMatrix, py::arg("par"))
		.def("getB", &LGCController::getSecondDesignMatrix, py::arg("par"))
		.def("getPv", &LGCController::getPv, py::arg("par"))
		.def("getLineNumber", &LGCController::getLineNumber, py::arg("par"))
		.def("setObservationMask", &LGCController::setObservationMask, py::arg("par"))
		.def("setParameterMask", &LGCController::setParameterMask, py::arg("par"))
		.def("getObservationMask", &LGCController::getObservationMask)
		.def("getParameterMask", &LGCController::getParameterMask)
		.def("unmask", &LGCController ::unmask);
	//Eigen::SparseMatrix<double> 
//Eigen::SparseMatrix<double> 
//Eigen::SparseMatrix<double> 
//Eigen::SparseMatrix<double> 


}
