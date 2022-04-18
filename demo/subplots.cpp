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
