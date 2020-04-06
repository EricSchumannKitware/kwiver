#ifndef KWIVER_VITAL_PYTHON_MERGE_DETECTION_H_
#define KWIVER_VITAL_PYTHON_MERGE_DETECTION_H_

#include <pybind11/pybind11.h>

namespace py = pybind11;

void merge_detection_sets(py::module &m);
#endif
