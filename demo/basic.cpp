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
