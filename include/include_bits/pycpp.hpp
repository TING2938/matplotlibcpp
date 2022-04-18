#ifndef __PYCPP_HPP__
#define __PYCPP_HPP__

// Python headers must be included before any system headers, since
// they define _POSIX_C_SOURCE
#include <Python.h>

#include <algorithm>
#include <array>
#include <cstdint>  // <cstdint> requires c++11 support
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>  // std::stod
#include <vector>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace matplotlibcpp
{
namespace detail
{
/** General wrapper around a PyObject*.
 * This decrements the reference count on destruction.
 */
class DecRefDtor
{
public:
    DecRefDtor(PyObject* ref) : m_ref{ref} {}
    Py_ssize_t ref_count() const
    {
        return m_ref ? Py_REFCNT(m_ref) : 0;
    }
    // Allow setting of the (optional) argument with PyArg_ParseTupleAndKeywords
    PyObject** operator&()
    {
        Py_XDECREF(m_ref);
        m_ref = NULL;
        return &m_ref;
    }
    // Access the argument
    operator PyObject*() const
    {
        return m_ref;
    }
    // Test if constructed successfully from the new reference.
    explicit operator bool()
    {
        return m_ref != NULL;
    }
    ~DecRefDtor()
    {
        Py_XDECREF(m_ref);
    }

protected:
    PyObject* m_ref;
};

/** Wrapper around a PyObject* that is a borrowed reference.
 * This increments the reference count on construction and
 * decrements the reference count on destruction.
 */
class BorrowedRef : public DecRefDtor
{
public:
    BorrowedRef(PyObject* borrowed_ref) : DecRefDtor(borrowed_ref)
    {
        Py_XINCREF(m_ref);
    }
};

/** Wrapper around a PyObject* that is a new reference.
 * This owns the reference so does not increment it on construction but
 * does decrement it on destruction.
 * eg: PyLongFromLong
 */
class NewRef : public DecRefDtor
{
public:
    NewRef(PyObject* new_ref) : DecRefDtor(new_ref) {}
};

template <typename Numeric>
inline NewRef get_pylist(const std::vector<Numeric>& v)
{
    PyObject* list = PyList_New(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        PyList_SetItem(list, i, PyFloat_FromDouble(v[i]));
    }
    return list;
}

inline NewRef get_pylist(const std::vector<std::string>& strings)
{
    PyObject* list = PyList_New(strings.size());
    for (std::size_t i = 0; i < strings.size(); ++i) {
        PyList_SetItem(list, i, PyUnicode_FromString(strings[i].c_str()));
    }
    return list;
}

template <typename Numeric>
NewRef get_listlist(const std::vector<std::vector<Numeric>>& ll)
{
    PyObject* listlist = PyList_New(ll.size());
    for (std::size_t i = 0; i < ll.size(); ++i) {
        auto tmp = get_pylist(ll[i]);
        Py_IncRef(tmp);
        PyList_SetItem(listlist, i, tmp);
    }
    return listlist;
}

class PyContainer
{
public:
    PyContainer() = default;
    ~PyContainer()
    {
        for (std::size_t i = 0; i < this->memory.size(); ++i) {
            Py_XDECREF(this->memory[i]);
        }
    }

    // Access the argument
    NewRef to_list()
    {
        PyObject* list = PyList_New(this->memory.size());
        for (std::size_t i = 0; i < this->memory.size(); ++i) {
            Py_INCREF(this->memory[i]);
            PyList_SetItem(list, i, this->memory[i]);
        }
        return list;
    }

    NewRef to_tuple()
    {
        PyObject* list = PyTuple_New(this->memory.size());
        for (std::size_t i = 0; i < this->memory.size(); ++i) {
            Py_INCREF(this->memory[i]);
            PyTuple_SetItem(list, i, this->memory[i]);
        }
        return list;
    }

    PyContainer& operator<<(const double& x)
    {
        this->memory.push_back(PyFloat_FromDouble(x));
        return *this;
    }

    PyContainer& operator<<(const long& x)
    {
        this->memory.push_back(PyLong_FromLong(x));
        return *this;
    }

    PyContainer& operator<<(const int& x)
    {
        return this->operator<<(long(x));
    }

    PyContainer& operator<<(const bool& x)
    {
        this->memory.push_back(x ? Py_True : Py_False);
        return *this;
    }

    PyContainer& operator<<(const std::string& x)
    {
        this->memory.push_back(PyUnicode_FromString(x.c_str()));
        return *this;
    }

    PyContainer& operator<<(PyObject* x)
    {
        this->memory.push_back(x);
        return *this;
    }

    template <typename T = double>
    PyContainer& operator<<(const std::vector<T>& x)
    {
        auto tmp = detail::get_pylist(x);
        Py_INCREF(tmp);
        this->memory.push_back(tmp);
        return *this;
    }

    template <typename T = double>
    PyContainer& operator<<(const std::vector<std::vector<T>>& x)
    {
        auto tmp = detail::get_listlist(x);
        Py_INCREF(tmp);
        this->memory.push_back(tmp);
        return *this;
    }

    void print()
    {
        for (int i = 0; i < this->memory.size(); i++) {
            PyObject_Print(this->memory[i], stdout, 100);
            printf("   %d\n", int(Py_REFCNT(this->memory[i])));
        }
        printf("\n\n");
    }

private:
    std::vector<PyObject*> memory;
};
}  // namespace detail
}  // namespace matplotlibcpp

#endif  // !__PYCPP_HPP__