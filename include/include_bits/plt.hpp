#ifndef __PLT_MATPLOTLIBCPP__
#define __PLT_MATPLOTLIBCPP__

#include <atomic>
#include "axes.hpp"
#include "figure.hpp"
#include "modules.hpp"

namespace matplotlibcpp
{
struct PLT
{
public:
    static std::atomic_int plt_count;
    detail::Modules modules;

private:
    detail::Load_func get_func(const std::string& name, PyObject* module = nullptr)
    {
        if (module == nullptr)
            module = this->modules.plt;
        return detail::Load_func(name, module);
    }

public:
    PLT(const std::string& backend = "")
    {
        if (PLT::plt_count != 0) {
            throw std::runtime_error("plt_count: " + std::to_string(PLT::plt_count));
        }
        PLT::plt_count++;
        this->modules.init(backend);
    }

    ~PLT()
    {
        this->modules.release();
    }

    inline std::pair<detail::Figure, detail::Axes> subplots(long nrows                       = 1,
                                                            long ncols                       = 1,
                                                            const std::vector<long>& figsize = {},
                                                            const KeyWords& keywords         = {})
    {
        detail::PyContainer args;
        args << nrows << ncols;

        auto kwargs = detail::get_keywords(keywords);

        if (!figsize.empty()) {
            assert(figsize.size() == 2);
            detail::NewRef size = PyTuple_New(2);
            PyTuple_SetItem(size, 0, PyLong_FromLong(figsize[0]));
            PyTuple_SetItem(size, 1, PyLong_FromLong(figsize[1]));
            PyDict_SetItemString(kwargs, "figsize", size);
        }
        auto func = this->get_func("subplots");
        func.call(args.to_tuple(), kwargs);
        func.incref_res();
        return {detail::Figure(PyTuple_GetItem(func.res, 0)), detail::Axes(PyTuple_GetItem(func.res, 1), nrows, ncols)};
    }

    inline void show(bool block = true)
    {
        auto func = this->get_func("show");
        if (block) {
            func.call();
        } else {
            detail::NewRef args = PyDict_New();
            PyDict_SetItemString(args, "block", Py_False);
            func.call(args);
        }
    }

    inline void annotate(std::string annotation, double x, double y)
    {
        detail::NewRef xy = PyTuple_New(2);
        PyObject* str     = PyUnicode_FromString(annotation.c_str());

        PyTuple_SetItem(xy, 0, PyFloat_FromDouble(x));
        PyTuple_SetItem(xy, 1, PyFloat_FromDouble(y));

        detail::NewRef kwargs = PyDict_New();
        PyDict_SetItemString(kwargs, "xy", xy);

        detail::NewRef args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, str);

        auto func = this->get_func("annotate");
        func.call(args, kwargs);
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void plot(const std::vector<ScalarX>& x,
              const std::vector<ScalarY>& y,
              const std::string& format = "",
              const KeyWords& keywords  = {})
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y << format;
        auto func = this->get_func("plot");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename Scalar = double>
    void plot(const std::vector<Scalar>& y, const std::string& format = "", const KeyWords& keywords = {})
    {
        std::vector<Scalar> x(y.size());
        std::iota(x.begin(), x.end(), Scalar(0));
        plot(x, y, format, keywords);
    }

    template <typename ScalarX = double, typename ScalarY = double, typename ScalarZ = double>
    void contour(const std::vector<std::vector<ScalarX>>& x,
                 const std::vector<std::vector<ScalarY>>& y,
                 const std::vector<std::vector<ScalarZ>>& z,
                 const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << x << y << z;
        auto kwargs                  = detail::get_keywords(keywords);
        detail::BorrowedRef coolwarm = PyObject_GetAttrString(this->modules.cm, "coolwarm");
        PyDict_SetItemString(kwargs, "cmap", coolwarm);
        auto func = this->get_func("contour");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename Scalar = double>
    void spy(const std::vector<std::vector<Scalar>>& x, long markersize = -1, const KeyWords& keywords = {})
    {
        auto kwargs = detail::get_keywords(keywords);
        if (markersize != -1) {
            PyDict_SetItemString(kwargs, "markersize", PyLong_FromLong(markersize));
        }
        detail::PyContainer args;
        args << x;
        auto func = this->get_func("spy");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void stem(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const KeyWords& keywords)
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y;
        auto func = this->get_func("stem");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void fill(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const KeyWords& keywords)
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y;
        auto func = this->get_func("fill");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename ScalarX = double, typename ScalarY1 = double, typename ScalarY2 = double>
    void fill_between(const std::vector<ScalarX>& x,
                      const std::vector<ScalarY1>& y1,
                      const std::vector<ScalarY2>& y2,
                      const KeyWords& keywords = {})
    {
        assert(x.size() == y1.size());
        assert(x.size() == y2.size());
        detail::PyContainer args;
        args << x << y1 << y2;

        detail::NewRef kwargs = PyDict_New();
        for (auto&& v : keywords) {
            if (v.first == "alpha") {
                PyDict_SetItemString(kwargs, v.first.c_str(), PyFloat_FromDouble(std::stod(v.second)));
            } else {
                PyDict_SetItemString(kwargs, v.first.c_str(), PyUnicode_FromString(v.second.c_str()));
            }
        }
        auto func = this->get_func("fill_between");
        func.call(args.to_tuple(), kwargs);
    }

    /**
     * @brief Add an arrow to the Axes.

     * This draws an arrow from (x, y) to (x+dx, y+dy).
     */
    void arrow(double x, double y, double dx, double dy, const KeyWords& keywords)
    {
        detail::PyContainer args;
        args << x << y << dx << dy;
        detail::NewRef kwargs = PyDict_New();
        for (auto&& v : keywords) {
            if (v.first == "width" || v.first == "head_width" || v.first == "head_length" || v.first == "overhang") {
                PyDict_SetItemString(kwargs, v.first.c_str(), PyFloat_FromDouble(std::stod(v.second)));
            } else if (v.first == "length_includes_head" || v.first == "head_starts_at_zero") {
                PyDict_SetItemString(kwargs, v.first.c_str(), v.second == "True" ? Py_True : Py_False);
            } else {
                PyDict_SetItemString(kwargs, v.first.c_str(), PyUnicode_FromString(v.second.c_str()));
            }
        }
        auto func = this->get_func("arrow");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename Scalar = double>
    void hist(const std::vector<Scalar>& y,
              long bins         = 10,
              std::string color = "b",
              double alpha      = 1.0,
              bool cumulative   = false)
    {
        detail::NewRef kwargs = PyDict_New();
        PyDict_SetItemString(kwargs, "bins", PyLong_FromLong(bins));
        PyDict_SetItemString(kwargs, "color", PyUnicode_FromString(color.c_str()));
        PyDict_SetItemString(kwargs, "alpha", PyFloat_FromDouble(alpha));
        PyDict_SetItemString(kwargs, "cumulative", cumulative ? Py_True : Py_False);

        detail::PyContainer args;
        args << y;
        auto func = this->get_func("hist");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void scatter(const std::vector<ScalarX>& x,
                 const std::vector<ScalarY>& y,
                 const double s           = 1.0,
                 const KeyWords& keywords = {})
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y;
        auto kwargs = detail::get_keywords(keywords);
        PyDict_SetItemString(kwargs, "s", PyLong_FromLong(s));
        auto func = this->get_func("scatter");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename Scalar = double>
    void boxplot(const std::vector<std::vector<Scalar>>& data,
                 const std::vector<std::string>& labels = {},
                 const KeyWords& keywords               = {})
    {
        detail::PyContainer args;
        args << data;
        auto kwargs = detail::get_keywords(keywords);
        if (!labels.empty() && labels.size() == data.size()) {
            PyDict_SetItemString(kwargs, "labels", detail::get_pylist(labels));
        }
        auto func = this->get_func("boxplot");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename Scalar = double>
    void boxplot(const std::vector<Scalar>& data, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << data;
        auto func = this->get_func("boxplot");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    detail::Axes twinx(detail::Axes ax = detail::Axes())
    {
        auto func = this->get_func("twinx");
        if (ax.get_ax() == nullptr) {
            func.call();
            func.incref_res();
        } else {
            detail::NewRef args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, ax.get_ax());
            func.call(args);
        }
        func.incref_res();
        return detail::Axes(func.res);
    }

    detail::Axes twiny(detail::Axes ax = detail::Axes())
    {
        auto func = this->get_func("twiny");
        if (ax.get_ax() == nullptr) {
            func.call();
            func.incref_res();
        } else {
            detail::NewRef args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, ax.get_ax());
            func.call(args);
        }
        func.incref_res();
        return detail::Axes(func.res);
    }

    template <typename Scalar = double>
    void bar(const std::vector<Scalar>& x,
             const std::vector<Scalar>& y,
             std::string ec           = "black",
             std::string ls           = "-",
             double lw                = 1.0,
             const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << x << y;
        auto kwargs = detail::get_keywords(keywords);
        PyDict_SetItemString(kwargs, "ec", PyUnicode_FromString(ec.c_str()));
        PyDict_SetItemString(kwargs, "ls", PyUnicode_FromString(ls.c_str()));
        PyDict_SetItemString(kwargs, "lw", PyFloat_FromDouble(lw));
        auto func = this->get_func("bar");
        func.call(args.to_tuple(), kwargs);
    }

    template <typename Scalar = double>
    void bar(const std::vector<Scalar>& y,
             std::string ec           = "black",
             std::string ls           = "-",
             double lw                = 1.0,
             const KeyWords& keywords = {})
    {
        using T = typename std::remove_reference<decltype(y)>::type::value_type;

        std::vector<T> x(y.size());
        for (std::size_t i = 0; i < y.size(); i++) {
            x[i] = static_cast<T>(i);
        }
        bar(x, y, ec, ls, lw, keywords);
    }

    template <typename Scalar = double>
    void barh(const std::vector<Scalar>& x,
              const std::vector<Scalar>& y,
              std::string ec           = "black",
              std::string ls           = "-",
              double lw                = 1.0,
              const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << x << y;
        auto kwargs = detail::get_keywords(keywords);
        PyDict_SetItemString(kwargs, "ec", PyUnicode_FromString(ec.c_str()));
        PyDict_SetItemString(kwargs, "ls", PyUnicode_FromString(ls.c_str()));
        PyDict_SetItemString(kwargs, "lw", PyFloat_FromDouble(lw));
        auto func = this->get_func("barh");
        func.call(args.to_tuple(), kwargs);
    }

    inline void subplots_adjust(const std::map<std::string, double>& keywords = {})
    {
        detail::NewRef kwargs = PyDict_New();
        for (std::map<std::string, double>::const_iterator it = keywords.begin(); it != keywords.end(); ++it) {
            PyDict_SetItemString(kwargs, it->first.c_str(), PyFloat_FromDouble(it->second));
        }
        auto func = this->get_func("subplots_adjust");
        func.call(nullptr, kwargs);
    }

    template <typename ScalarX = double, typename ScalarY = double, typename ScalarZ = double>
    void contour(const std::vector<ScalarX>& x,
                 const std::vector<ScalarY>& y,
                 const std::vector<ScalarZ>& z,
                 const KeyWords& keywords = {})
    {
        assert(x.size() == y.size() && x.size() == z.size());

        detail::PyContainer args;
        args << x << y << z;
        auto func = this->get_func("contour");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename ScalarX = double,
              typename ScalarY = double,
              typename ScalarU = double,
              typename ScalarW = double>
    void quiver(const std::vector<ScalarX>& x,
                const std::vector<ScalarY>& y,
                const std::vector<ScalarU>& u,
                const std::vector<ScalarW>& w,
                const KeyWords& keywords = {})
    {
        assert(x.size() == y.size() && x.size() == u.size() && u.size() == w.size());
        detail::PyContainer args;
        args << x << y << u << w;
        auto func = this->get_func("quiver");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void stem(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const std::string& s = "")
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y << s;
        auto func = this->get_func("stem");
        func.call(args.to_tuple());
    }

    template <typename Scalar = double>
    bool stem(const std::vector<Scalar>& y, const std::string& format = "")
    {
        std::vector<Scalar> x(y.size());
        for (size_t i = 0; i < x.size(); ++i)
            x[i] = i;
        return stem(x, y, format);
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void semilogx(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const std::string& s = "")
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y << s;
        auto func = this->get_func("semilogx");
        func.call(args.to_tuple());
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void semilogy(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const std::string& s = "")
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y << s;
        auto func = this->get_func("semilogy");
        func.call(args.to_tuple());
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void loglog(const std::vector<ScalarX>& x, const std::vector<ScalarY>& y, const std::string& s = "")
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y << s;
        auto func = this->get_func("loglog");
        func.call(args.to_tuple());
    }

    template <typename ScalarX = double, typename ScalarY = double>
    void errorbar(const std::vector<ScalarX>& x,
                  const std::vector<ScalarY>& y,
                  const std::vector<ScalarX>& yerr,
                  const KeyWords& keywords = {})
    {
        assert(x.size() == y.size());
        detail::PyContainer args;
        args << x << y;
        auto yerrarray = detail::get_pylist(yerr);
        auto kwargs    = detail::get_keywords(keywords);
        PyDict_SetItemString(kwargs, "yerr", yerrarray);
        auto func = this->get_func("errorbar");
        func.call(args.to_tuple(), kwargs);
    }

    void text(double x, double y, const std::string& s = "")
    {
        detail::PyContainer args;
        args << x << y << s;
        auto func = this->get_func("text");
        func.call(args.to_tuple());
    }

    inline long figure(long number = -1)
    {
        auto func = this->get_func("figure");
        if (number == -1) {
            func.call();
        } else {
            assert(number > 0);
            detail::NewRef args = PyTuple_New(1);
            PyTuple_SetItem(args, 0, PyLong_FromLong(number));
            func.call(args);
        }
        detail::DecRefDtor num = PyObject_GetAttrString(func.res, "number");
        return PyLong_AsLong(num);
    }

    inline bool fignum_exists(long number)
    {
        detail::PyContainer args;
        args << number;
        auto func = this->get_func("fignum_exists");
        func.call(args.to_tuple());
        return PyObject_IsTrue(func.res);
    }

    detail::Axes gca(const KeyWords& keywords = {})
    {
        auto func = this->get_func("gca");
        func.call(detail::get_keywords(keywords));
        func.incref_res();
        return detail::Axes(func.res);
    }

    detail::Figure gcf()
    {
        auto func = this->get_func("gcf");
        func.call();
        func.incref_res();
        return detail::Figure(func.res);
    }

    inline void figure_size(const std::vector<double>& figsize, long dpi = 100)
    {
        detail::NewRef kwargs = PyDict_New();
        PyDict_SetItemString(kwargs, "figsize", detail::get_pylist(figsize));
        PyDict_SetItemString(kwargs, "dpi", PyLong_FromSize_t(dpi));
        auto func = this->get_func("figure");
        func.call(nullptr, kwargs);
    }

    inline void legend(const KeyWords& keywords = {})
    {
        auto func = this->get_func("legend");
        func.call(nullptr, detail::get_keywords(keywords));
    }

    inline void set_aspect(double ratio)
    {
        detail::PyContainer args;
        args << ratio;
        auto gca = this->get_func("gca");
        gca.call();
        detail::BorrowedRef set_aspect = PyObject_GetAttrString(gca.res, "set_aspect");
        detail::DecRefDtor res         = PyObject_CallObject(set_aspect, args.to_tuple());
    }

    inline void set_aspect_equal()
    {
        detail::NewRef args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, PyUnicode_FromString("equal"));
        auto gca = this->get_func("gca");
        gca.call();
        detail::BorrowedRef set_aspect = PyObject_GetAttrString(gca.res, "set_aspect");
        detail::DecRefDtor res         = PyObject_CallObject(set_aspect, args);
    }

    void ylim(double left, double right)
    {
        detail::PyContainer args;
        args << std::vector<double>{left, right};
        auto func = this->get_func("ylim");
        func.call(args.to_tuple());
    }

    void xlim(double left, double right)
    {
        detail::PyContainer args;
        args << std::vector<double>{left, right};
        auto func = this->get_func("xlim");
        func.call(args.to_tuple());
    }

    inline std::array<double, 2> xlim()
    {
        auto xlim = this->get_func("xlim");
        xlim.call();
        PyObject* left  = PyTuple_GetItem(xlim.res, 0);
        PyObject* right = PyTuple_GetItem(xlim.res, 1);
        return {PyFloat_AsDouble(left), PyFloat_AsDouble(right)};
    }

    inline std::array<double, 2> ylim()
    {
        auto ylim = this->get_func("ylim");
        ylim.call();
        PyObject* left  = PyTuple_GetItem(ylim.res, 0);
        PyObject* right = PyTuple_GetItem(ylim.res, 1);
        return {PyFloat_AsDouble(left), PyFloat_AsDouble(right)};
    }

    template <typename Scalar = double>
    inline void xticks(const std::vector<Scalar>& ticks,
                       const std::vector<std::string>& labels = {},
                       const KeyWords& keywords               = {})
    {
        assert(labels.size() == 0 || ticks.size() == labels.size());
        detail::PyContainer args;
        args << ticks;
        if (!labels.empty()) {
            args << labels;
        }
        auto func = this->get_func("xticks");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    template <typename Scalar = double>
    inline void yticks(const std::vector<Scalar>& ticks,
                       const std::vector<std::string>& labels = {},
                       const KeyWords& keywords               = {})
    {
        assert(labels.size() == 0 || ticks.size() == labels.size());
        detail::PyContainer args;
        args << ticks;
        if (!labels.empty()) {
            args << labels;
        }
        auto func = this->get_func("yticks");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void margins(double margin)
    {
        detail::PyContainer args;
        args << margin;
        auto func = this->get_func("margins");
        func.call(args.to_tuple());
    }

    inline void margins(double margin_x, double margin_y)
    {
        detail::PyContainer args;
        args << margin_x << margin_y;
        auto func = this->get_func("margins");
        func.call(args.to_tuple());
    }

    inline void tick_params(const KeyWords& keywords, const std::string axis = "both")
    {
        detail::PyContainer args;
        args << axis;
        auto func = this->get_func("tick_params");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void subplot(long nrows, long ncols, long plot_number)
    {
        detail::PyContainer args;
        args << nrows << ncols << plot_number;
        auto func = this->get_func("subplot");
        func.call(args.to_tuple());
    }

    inline void subplot2grid(long nrows, long ncols, long rowid = 0, long colid = 0, long rowspan = 1, long colspan = 1)
    {
        detail::NewRef shape = PyTuple_New(2);
        PyTuple_SetItem(shape, 0, PyLong_FromLong(nrows));
        PyTuple_SetItem(shape, 1, PyLong_FromLong(ncols));

        detail::NewRef loc = PyTuple_New(2);
        PyTuple_SetItem(loc, 0, PyLong_FromLong(rowid));
        PyTuple_SetItem(loc, 1, PyLong_FromLong(colid));

        detail::NewRef args = PyTuple_New(4);
        PyTuple_SetItem(args, 0, shape);
        PyTuple_SetItem(args, 1, loc);
        PyTuple_SetItem(args, 2, PyLong_FromLong(rowspan));
        PyTuple_SetItem(args, 3, PyLong_FromLong(colspan));

        auto func = this->get_func("subplot2grid");
        func.call(args);
    }

    inline void title(const std::string& titlestr, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << titlestr;
        auto func = this->get_func("title");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void suptitle(const std::string& suptitlestr, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << suptitlestr;
        auto func = this->get_func("suptitle");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void axis(const std::string& axisstr)
    {
        detail::PyContainer args;
        args << axisstr;
        auto func = this->get_func("axis");
        func.call(args.to_tuple());
    }

    inline void axhline(double y, double xmin = 0., double xmax = 1., const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << y << xmin << xmax;
        auto func = this->get_func("axhline");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void axvline(double x, double ymin = 0., double ymax = 1., const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << x << ymin << ymax;
        auto func = this->get_func("axhline");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void axvspan(double xmin, double xmax, double ymin = 0., double ymax = 1., const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << xmin << xmax << ymin << ymax;

        detail::NewRef kwargs = PyDict_New();
        for (auto it = keywords.begin(); it != keywords.end(); ++it) {
            if (it->first == "linewidth" || it->first == "alpha") {
                PyDict_SetItemString(kwargs, it->first.c_str(), PyFloat_FromDouble(std::stod(it->second)));
            } else {
                PyDict_SetItemString(kwargs, it->first.c_str(), PyUnicode_FromString(it->second.c_str()));
            }
        }
        auto func = this->get_func("axvspan");
        func.call(args.to_tuple(), kwargs);
    }

    inline void xlabel(const std::string& str, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << str;
        auto func = this->get_func("xlabel");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void ylabel(const std::string& str, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << str;
        auto func = this->get_func("ylabel");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
    }

    inline void grid(bool flag)
    {
        detail::PyContainer args;
        args << flag;
        auto func = this->get_func("grid");
        func.call(args.to_tuple());
    }

    inline void close()
    {
        auto func = this->get_func("close");
        func.call();
    }

    inline void xkcd()
    {
        auto func = this->get_func("xkcd");
        func.call();
    }

    inline void draw()
    {
        auto func = this->get_func("draw");
        func.call();
    }

    inline void pause(double interval)
    {
        detail::PyContainer args;
        args << interval;
        auto func = this->get_func("pause");
        func.call(args.to_tuple());
    }

    inline void savefig(const std::string& filename, long dpi = 100, const std::string format = "")
    {
        detail::PyContainer args;
        args << filename;
        detail::NewRef kwargs = PyDict_New();
        if (dpi > 0)
            PyDict_SetItemString(kwargs, "dpi", PyLong_FromLong(dpi));
        if (!format.empty())
            PyDict_SetItemString(kwargs, "format", PyUnicode_FromString(format.c_str()));
        auto func = this->get_func("savefig");
        func.call(args.to_tuple(), kwargs);
    }

    inline void rcparams(const KeyWords& keywords = {})
    {
        detail::NewRef kwargs = PyDict_New();
        for (auto it = keywords.begin(); it != keywords.end(); ++it) {
            if ("text.usetex" == it->first)
                PyDict_SetItemString(kwargs, it->first.c_str(), PyLong_FromLong(std::stoi(it->second.c_str())));
            else
                PyDict_SetItemString(kwargs, it->first.c_str(), PyUnicode_FromString(it->second.c_str()));
        }
        auto rcparams = this->get_func("rcParams");

        detail::BorrowedRef update = PyObject_GetAttrString(rcparams.fn, "update");
        detail::NewRef empty_tuple = PyTuple_New(0);
        detail::DecRefDtor res     = PyObject_Call(update, empty_tuple, kwargs);
        if (!res)
            throw std::runtime_error("Call to rcParams.update() failed.");
    }

    inline void clf()
    {
        auto func = this->get_func("clf");
        func.call();
    }

    inline void cla()
    {
        auto func = this->get_func("cla");
        func.call();
    }

    inline void ion()
    {
        auto func = this->get_func("ion");
        func.call();
    }

    inline std::vector<std::array<double, 2>> ginput(const int numClicks = 1, const KeyWords& keywords = {})
    {
        detail::PyContainer args;
        args << numClicks;
        auto func = this->get_func("ginput");
        func.call(args.to_tuple(), detail::get_keywords(keywords));
        if (!func.res)
            throw std::runtime_error("Call to ginput() failed.");

        const size_t len = PyList_Size(func.res);
        std::vector<std::array<double, 2>> out;
        out.reserve(len);
        for (size_t i = 0; i < len; i++) {
            PyObject* current = PyList_GetItem(func.res, i);
            std::array<double, 2> position;
            position[0] = PyFloat_AsDouble(PyTuple_GetItem(current, 0));
            position[1] = PyFloat_AsDouble(PyTuple_GetItem(current, 1));
            out.push_back(position);
        }
        return out;
    }

    inline void tight_layout()
    {
        auto func = this->get_func("tight_layout");
        func.call();
    }
};  // class PLT

std::atomic_int PLT::plt_count{0};
}  // end namespace matplotlibcpp

#endif  // !__PLT_MATPLOTLIBCPP__
