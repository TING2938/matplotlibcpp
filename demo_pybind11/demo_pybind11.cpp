#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

void matplotlib_example()
{
    py::scoped_interpreter guard;

    auto plt           = py::module_::import("matplotlib.pyplot");
    std::vector<int> x = {1, 2, 3};
    std::vector<int> y = {3, 2, 5};

    py::tuple subplots = plt.attr("subplots")(2, 1, "figsize"_a = py::make_tuple(10, 8));
    auto fig           = subplots[0];
    py::tuple ax       = subplots[1];

    ax[0].attr("plot")(x, y, "--ob");
    ax[1].attr("plot")(x, y, ":or");
    plt.attr("show")();
}

int main()
{
    matplotlib_example();
}