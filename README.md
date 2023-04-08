# C++ binding for Python matplotlib


# ***deprecated*** Please use [Pybind11](https://github.com/pybind/pybind11) for plotting, see [example](demo_pybind11) below:

```c++
// demo_pybind11/demo_pybind11.cpp

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
    auto fig      = subplots[0];
    py::tuple ax  = subplots[1];

    ax[0].attr("plot")(x, y, "--ob");
    ax[1].attr("plot")(x, y, ":or");
    plt.attr("show")();
}

int main()
{
    matplotlib_example();
}
```

## pybind11 build with cmake

- CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.11)
project(demo_pybind11)

include(FetchContent)

FetchContent_Declare(
    pybind11
    URL https://github.com/pybind/pybind11/archive/refs/tags/v2.10.0.tar.gz
)
FetchContent_MakeAvailable(pybind11)

add_executable(demo_pybind11 demo_pybind11.cpp)
target_link_libraries(demo_pybind11 pybind11::embed)
```

- build 
```bash
cd demo_pybind11
mkdir build
cd build 
cmake ..
make
```

---
---
---


## How to use 

1. basic example
   
```c++
#include "matplotlib.hpp"

int main()
{
    auto plt = matplotlibcpp::PLT();
    plt.plot({1, 3, 2}, "--o", {{"label", "cos"}});
    plt.xticks({0, 2});
    plt.legend();

    plt.savefig("basic.png");
    plt.show();
}
```

![](.res/basic.png)


2. subplots example
   
```c++
#include <cmath>
#include "matplotlib.hpp"

int main()
{
    auto plt = matplotlibcpp::PLT();
    int n    = 100;
    std::vector<double> x(n), y(n), z(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i * 10.0 / n;
        y[i] = std::sin(x[i]);
        z[i] = std::cos(x[i]);
    }

    auto ax = plt.subplots(2, 1, {7, 7}, {{"sharex", "all"}}).second;

    ax[0].plot(x, y, "-", {{"color", "red"}, {"linewidth", "6"}, {"label", "sin"}});
    ax[0].set_title("sin(x)");
    ax[0].legend();

    ax[1].plot(x, z, "b--");
    ax[1].set_title("cos(x)");

    plt.savefig("subplots.png");
    plt.show();
}
```

![](.res/subplots.png)


3. twinx example

```c++
#include "matplotlib.hpp"

int main()
{
    auto plt = matplotlibcpp::PLT();
    int n    = 100;
    std::vector<double> x(n), y(n), z(n);
    for (int i = 0; i < n; ++i) {
        x[i] = i * 10.0 / n;
        y[i] = std::sin(x[i]);
        z[i] = 2 * std::cos(x[i]);
    }

    auto ax = plt.subplots().second;

    ax.plot(x, y, "--o", {{"label", "cos"}});
    ax.legend();

    auto tax = ax.twinx();
    tax.plot(x, z, ":r");

    plt.savefig("twinx.png");
    plt.show();
}
```

![](.res/twinx.png)


## How to compile

```bash
cd matplotlibcpp
mkdir build
cd build
cmake ..
make
```


## Reference

1. https://docs.python.org/3/c-api/index.html
2. https://pythonextensionpatterns.readthedocs.io/en/latest/cpp_and_cpython.html
3. https://matplotlib.org/stable/index.html
4. https://github.com/lava/matplotlib-cpp
