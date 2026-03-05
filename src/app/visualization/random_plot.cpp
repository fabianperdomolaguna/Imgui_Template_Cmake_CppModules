module;

#include <cstdint>
#include <format>
#include <tuple>
#include <vector>

#include "pybind11/embed.h"

export module app.visualization.random_plot;

import app.python.manager;
import beryl.logger;

namespace py = pybind11;

export namespace app::visualization
{
    struct PlotBuffer
    {
        std::vector<uint8_t> pixels;
        int width = 0;
        int height = 0;
        bool success = false;
    };

    PlotBuffer MplRandomPlot()
    {
        PlotBuffer result;

        app::python::PyMgr().BeginSession();

        py::gil_scoped_acquire acquire;

        try
        {
            py::module_::import("matplotlib").attr("use")("Agg");

            {
                auto np = app::python::PyMgr().ImportModule("numpy");
                auto plt = app::python::PyMgr().ImportModule("matplotlib.pyplot");
                auto agg = app::python::PyMgr().ImportModule("matplotlib.backends.backend_agg");

                plt.attr("clf")();
                pybind11::object fig = plt.attr("figure")();
                plt.attr("plot")(np.attr("random").attr("randn")(100));
                plt.attr("title")("Random numbers");

                pybind11::object canvas = agg.attr("FigureCanvasAgg")(fig);
                canvas.attr("draw")();
                py::object renderer = canvas.attr("get_renderer")();
                py::buffer buffer = renderer.attr("buffer_rgba")();
                py::buffer_info info = buffer.request();

                std::tie(result.width, result.height) =
                    py::cast<std::tuple<int, int>>(canvas.attr("get_width_height")());

                auto* ptr = static_cast<uint8_t*>(info.ptr);
                auto data_size = static_cast<size_t>(result.width) * result.height * 4; // RGBA = 4 bytes

                std::span<uint8_t> data_view(ptr, data_size);
                result.pixels.assign(data_view.begin(), data_view.end());

                plt.attr("close")(fig);
                plt.attr("close")("all");
            }

            auto gc = app::python::PyMgr().ImportModule("gc");
            gc.attr("collect")();

            result.success = true;
        }
        catch (py::error_already_set& e)
        {
            beryl::logger::Error(std::format("[Visualization] Python Error: {}", e.what()));
        }

        return result;
    } 
}