#ifndef __PLT_MODULES_HPP__
#define __PLT_MODULES_HPP__

#include "pycpp.hpp"

namespace matplotlibcpp
{
namespace detail
{
struct Modules
{
    PyObject* matplotlib;
    PyObject* plt;
    PyObject* cm;
    bool need_init_python;

    Modules() : matplotlib(nullptr), plt(nullptr), cm(nullptr), need_init_python(true) {}

    void init(const std::string& backend = "", bool need_init_python = true)
    {
        this->need_init_python = need_init_python;
        if (this->need_init_python)
            Py_Initialize();
        this->matplotlib = PyImport_Import(PyUnicode_FromString("matplotlib"));
        if (!this->matplotlib) {
            throw std::runtime_error("Error loading module matplotlib!");
        }

        if (!backend.empty()) {
            PyObject_CallMethod(matplotlib, "use", "s", backend.c_str());
        }

        this->plt = PyImport_Import(PyUnicode_FromString("matplotlib.pyplot"));
        if (!this->plt) {
            throw std::runtime_error("Error loading module matplotlib.pyplot!");
        }

        this->cm = PyImport_Import(PyUnicode_FromString("matplotlib.cm"));
        if (!this->cm) {
            throw std::runtime_error("Error loading module matplotlib.cm!");
        }
    }

    void release()
    {
        Py_XDECREF(this->plt);
        Py_XDECREF(this->cm);
        Py_XDECREF(this->matplotlib);
        if (this->need_init_python)
            Py_Finalize();
    }
};
}  // end namespace detail
}  // namespace matplotlibcpp
#endif  // !__PLT_MODULES_HPP__