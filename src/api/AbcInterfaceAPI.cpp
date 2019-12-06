/**
 * @file AbcInterfaceAPI.cpp
 * @brief The Python interface for the classes AbcInterface
 * @author Keren Zhu
 * @date 10/23/2019
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "interface/AbcInterface.h"

namespace py = pybind11;
void initAbcInterfaceAPI(py::module &m)
{
    py::class_<PROJECT_NAMESPACE::AbcInterface>(m , "AbcInterface")
        .def(py::init<>())
        .def("start", &PROJECT_NAMESPACE::AbcInterface::start, "Start the ABC framework")
        .def("end", &PROJECT_NAMESPACE::AbcInterface::end, "Stop the ABC framework")
        .def("read", &PROJECT_NAMESPACE::AbcInterface::read, "Read a file")
        .def("aigStats", &PROJECT_NAMESPACE::AbcInterface::aigStats, "Get the AIG stats from the ABC framework`")
        .def("balance", &PROJECT_NAMESPACE::AbcInterface::balance, "balance action",
                py::arg("l") = false, py::arg("d") = false, py::arg("s") = false, py::arg("x") = false)
        .def("resub", &PROJECT_NAMESPACE::AbcInterface::resub, "resub action",
                py::arg("k") = -1, py::arg("n") = -1, py::arg("f") = -1,
                py::arg("l") = false, py::arg("z") = false)
        .def("rewrite", &PROJECT_NAMESPACE::AbcInterface::rewrite, "rewrite action",
                py::arg("l") = false, py::arg("z") = false)
        .def("refactor", &PROJECT_NAMESPACE::AbcInterface::refactor, "refactor action",
                py::arg("n") = -1, py::arg("l") = false, py::arg("z") = false)
        .def("compress2rs", &PROJECT_NAMESPACE::AbcInterface::compress2rs)
        .def("aigNode", &PROJECT_NAMESPACE::AbcInterface::aigNode, "Get one AigNode")
        .def("numNodes", &PROJECT_NAMESPACE::AbcInterface::numNodes, "Get the number of nodes");

    py::class_<PROJECT_NAMESPACE::AigStats>(m , "AigStats")
        .def(py::init<>())
        .def_property("numIn", &PROJECT_NAMESPACE::AigStats::numIn, &PROJECT_NAMESPACE::AigStats::setNumIn)
        .def_property("numOut", &PROJECT_NAMESPACE::AigStats::numOut, &PROJECT_NAMESPACE::AigStats::setNumOut)
        .def_property("numLat", &PROJECT_NAMESPACE::AigStats::numLat, &PROJECT_NAMESPACE::AigStats::setNumLat)
        .def_property("numAnd", &PROJECT_NAMESPACE::AigStats::numAnd, &PROJECT_NAMESPACE::AigStats::setNumAnd)
        .def_property("lev", &PROJECT_NAMESPACE::AigStats::lev, &PROJECT_NAMESPACE::AigStats::setLev);

    py::class_<PROJECT_NAMESPACE::AigNode>(m, "AigNode")
        .def(py::init<>())
        .def("hasFanin0", &PROJECT_NAMESPACE::AigNode::hasFanin0, "Whether the node has fanin0")
        .def("fanin0", &PROJECT_NAMESPACE::AigNode::fanin0, "The node index of fanin 0")
        .def("hasFanin1", &PROJECT_NAMESPACE::AigNode::hasFanin1, "Whether the node has fanin1")
        .def("fanin1", &PROJECT_NAMESPACE::AigNode::fanin1, "The node index of fanin 1")
        .def("nodeType", &PROJECT_NAMESPACE::AigNode::nodeType, "The node type. 0: const 1, 1: PO, 2: PI, 3: a and b, 4: not a and b, 5: not a and not b, 6 unknown")
        .def("numFanouts", &PROJECT_NAMESPACE::AigNode::numFanouts, "The number of fanouts")
        .def("fanout", &PROJECT_NAMESPACE::AigNode::fanout, "A fanout node");

}
