#include <print>

#include "DashboardWindow.hpp"
#include "PlotFrame.hpp"

int main(int argc, const char *argv[])
{
    using PlotType = PlotFrame<int>;
    using PlotWindowType = PlotWindow<PlotType>;

    auto plot = PlotType{640, 480};
    auto win = PlotWindowType{plot};
    auto data = PlotType::ContainerType{};

    for (int i = 0; i < 1000000; i++)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));

        if (i > 1000)
            data.pop_front();

        data.push_back({i, i % 107});
        plot.update_data(data);
    }

    std::println("Program exited.");
}