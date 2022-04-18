#ifndef __PLT_FIGURE_HPP__
#define __PLT_FIGURE_HPP__

#include "utility.hpp"

namespace matplotlibcpp
{
namespace detail
{
class Figure
{
public:
    Figure(PyObject* fig) : fig(fig) {}

private:
    PyObject* fig;
};
}  // namespace detail
}  // namespace matplotlibcpp

#endif  // !__PLT_FIGURE_HPP__