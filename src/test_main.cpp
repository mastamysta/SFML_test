#include <print>

#include "DashboardWindow.hpp"
#include "PlotFrame.hpp"

using namespace dash_components;

int main(int argc, const char *argv[])
{
    using PlotType = PlotFrame<int>;

    auto win_size = std::pair<std::size_t, std::size_t>{1000, 1000};
    auto win = PlotWindow{win_size};
    auto plot_size = std::pair<std::size_t, std::size_t>{300, 300};
    auto plot_pos = std::pair<std::size_t, std::size_t>{50, 50};
    auto& plot = win.get_plot_frame<int>(plot_size, 
                                        plot_pos, 
                                        PlotType::HorizontalScalingMode::FIT_TO_MINIMUM);

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