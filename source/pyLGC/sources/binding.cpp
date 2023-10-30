#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>

#include "LGCController.h"

namespace py = pybind11;

PYBIND11_MODULE(pyLGC, m)
{
	py::class_<LGCController>(m, "LGCController")
		.def(py::init<const std::string &>(), py::arg("path"))
		.def("get_misclosure", &LGCController::getMisclosure, py::arg("par"))
		.def("get_parameter", &LGCController::getParameter);
}
