#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
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
		.def("getMisclosure", &LGCController::getMisclosure, py::arg("par"))
		.def("getConstraintMisclosure", &LGCController::getConstraintMisclosure, py::arg("par"))
		.def("getFirstDesignMatrix", &LGCController::getFirstDesignMatrix, py::arg("par"))
		.def("getFirstConstraintDesignMatrix", &LGCController::getFirstConstraintDesignMatrix,  py::arg("par"))
		.def("getSecondDesignMatrix", &LGCController::getSecondDesignMatrix, py::arg("par"))
		.def("getPv", &LGCController::getPv, py::arg("par"));
	//Eigen::SparseMatrix<double> 
//Eigen::SparseMatrix<double> 
//Eigen::SparseMatrix<double> 
//Eigen::SparseMatrix<double> 


}
