#pragma once

#include <cstdint>
#include <vector>

namespace app::visualization
{
    struct PlotBuffer
    {
        std::vector<uint8_t> pixels;
        int width = 0;
        int height = 0;
        bool success = false;
    };

    PlotBuffer MplRandomPlot();
}