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
