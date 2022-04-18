#ifndef __PLT_UTILITY_HPP__
#define __PLT_UTILITY_HPP__

#include "modules.hpp"

namespace matplotlibcpp
{
using KeyWords = std::map<std::string, std::string>;

namespace detail
{
class Load_func
{
public:
    Load_func(std::string fname, PyObject* module)
    {
        this->fn = PyObject_GetAttrString(module, fname.c_str());
        if (!fn)
            throw std::runtime_error(std::string("Couldn't find required function: ") + fname);
    }

    ~Load_func()
    {
        Py_DECREF(this->fn);
        this->decref_res();
    }

    void call(PyObject* args = nullptr, PyObject* kwargs = nullptr)
    {
        if (kwargs == nullptr) {
            this->res = this->res = PyObject_CallObject(this->fn, args);
        } else {
            if (args == nullptr) {
                PyObject* empty_tuple = PyTuple_New(0);
                this->res             = PyObject_Call(this->fn, empty_tuple, kwargs);
                Py_XDECREF(empty_tuple);
            } else {
                this->res = PyObject_Call(this->fn, args, kwargs);
            }
        }
        if (!this->res)
            throw std::runtime_error("Call failed.");
    }

    void decref_res()
    {
        if (this->res) {
            Py_DECREF(this->res);
        }
    }

    void incref_res()
    {
        Py_INCREF(this->res);
    }

    PyObject* fn;
    PyObject* res;
};

inline NewRef get_keywords(const KeyWords& keywords)
{
    PyObject* kwargs = PyDict_New();
    for (auto it = keywords.begin(); it != keywords.end(); ++it) {
        PyDict_SetItemString(kwargs, it->first.c_str(), PyUnicode_FromString(it->second.c_str()));
    }
    return kwargs;
}
}  // namespace detail

}  // namespace matplotlibcpp

#endif  // !__PLT_UTILITY_HPP__