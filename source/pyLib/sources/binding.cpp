#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>

#include "LGCController.h"

namespace py = pybind11;

PYBIND11_MODULE(LGCmodule, m)
{
	py::class_<LGCController>(m, "LGCController").def(py::init<const std::string &>()).def("getMisclosure", &LGCController::getMisclosure);
}
