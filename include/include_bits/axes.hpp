#ifndef __PLT_AXES_HPP__
#define __PLT_AXES_HPP__

#include "utility.hpp"

namespace matplotlibcpp
{
namespace detail
{
class Axes
{
public:
    Axes() : ax(nullptr), nrows(1), ncols(1) {}

    Axes(PyObject* ax, long nrows = 1, long ncols = 1) : ax(ax), nrows(nrows), ncols(ncols)
    {
        if (!ax) {
            throw std::runtime_error("ax is nullptr");
        }
    }

    Axes operator[](int i)
    {
        long tot_size = this->nrows * this->ncols;
        assert((tot_size != 1) && (tot_size > i));
        auto tmp  = (PyArrayObject*)this->ax;
        auto axes = (PyObject**)PyArray_DATA(tmp);
        return Axes(axes[i]);
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void plot(const std::vector<ScalarX>& x,
              const std::vector<ScalarY>& y,
              const std::string& format = "",
              const KeyWords& keywords  = {})
    {
        assert(this->nrows * this->ncols == 1 && x.size() == y.size());
        detail::PyContainer args;
        args << x << y << format;
        auto func = this->get_func("plot");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void plot(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const KeyWords& keywords)
    {
        this->plot<ScalarX, ScalarY>(x, y, "", keywords);
    }

    template <typename Scalar = double>
    void plot(const std::vector<Scalar>& y, const std::string& format = "", const KeyWords& keywords = {})
    {
        std::vector<Scalar> x(y.size());
        for (size_t i = 0; i < x.size(); ++i)
            x[i] = static_cast<Scalar>(i);
        this->plot(x, y, format, keywords);
    }

    template <typename Scalar = double>
    void plot(const std::vector<Scalar>& y, const KeyWords& keywords)
    {
        this->plot<Scalar>(y, "", keywords);
    }

    void grid(bool visible             = true,
              const std::string& which = "major",
              const std::string& axis  = "both",
              const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << visible << which << axis;
        auto func = this->get_func("grid");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void set_xlim(double left, double right)
    {
        detail::PyContainer list;
        list << left << right;
        detail::NewRef args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, list.to_tuple());
        auto func = this->get_func("set_xlim");
        func.call(args);
    }

    void set_ylim(double left, double right)
    {
        detail::PyContainer list;
        list << left << right;
        detail::NewRef args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, list.to_tuple());
        auto func = this->get_func("set_ylim");
        func.call(args);
    }

    void set_xlabel(const std::string& str, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << str;
        auto func = this->get_func("set_xlabel");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void set_ylabel(const std::string& str, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << str;
        auto func = this->get_func("set_ylabel");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void set_title(const std::string& str, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << str;
        auto func = this->get_func("set_title");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename Scalar = double>
    void set_xticks(const std::vector<Scalar>& ticks,
                    const std::vector<std::string>& labels = {},
                    const KeyWords& keywords               = {})
    {
        assert(labels.size() == 0 || ticks.size() == labels.size());
        detail::PyContainer args;
        args << ticks;
        if (!labels.empty()) {
            args << labels;
        }
        auto func = this->get_func("set_xticks");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename Scalar = double>
    void set_yticks(const std::vector<Scalar>& ticks,
                    const std::vector<std::string>& labels = {},
                    const KeyWords& keywords               = {})
    {
        assert(labels.size() == 0 || ticks.size() == labels.size());
        detail::PyContainer args;
        args << ticks;
        if (!labels.empty()) {
            args << labels;
        }
        auto func = this->get_func("set_yticks");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void axvline(double x, double ymin = 0., double ymax = 1., const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << x << ymin << ymax;
        auto func = this->get_func("axvline");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void axvline(double x, const KeyWords& keywords)
    {
        this->axvline(x, 0.0, 1.0, keywords);
    }

    void axhline(double y, double xmin = 0., double xmax = 1., const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << y << xmin << xmax;
        auto func = this->get_func("axhline");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void axhline(double y, const KeyWords& keywords)
    {
        this->axhline(y, 0.0, 1.0, keywords);
    }

    void vlines(double x, double ymin, double ymax, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << x << ymin << ymax;
        auto func = this->get_func("vlines");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void hlines(double y, double xmin, double xmax, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << y << xmin << xmax;
        auto func = this->get_func("hlines");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    void legend(const KeyWords& keywords = {})
    {
        auto func = this->get_func("legend");
        func.call(nullptr, detail::get_keywords(keywords));
    }

    detail::Axes twinx()
    {
        auto func = this->get_func("twinx");
        func.call();
        func.incref_res();
        return detail::Axes(func.res);
    }

    void cla()
    {
        auto func = this->get_func("cla");
        func.call();
    }

    PyObject* get_ax()
    {
        return this->ax;
    }

private:
    Load_func get_func(const std::string name)
    {
        return Load_func(name, this->ax);
    }

private:
    PyObject* ax;
    long nrows;
    long ncols;
};
}  // namespace detail
}  // namespace matplotlibcpp

#endif  // !__PLT_AXES_HPP__