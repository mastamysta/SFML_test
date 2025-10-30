#include <print>

#include "DashboardWindow.hpp"
#include "PlotFrame.hpp"

#include <functional>

using namespace dash_components;


int main(int argc, const char *argv[])
{

    using PlotType = PlotFrame<int>;

    auto win_size = std::pair<std::size_t, std::size_t>{1000, 1000};
    auto win = DashBoardWindow<PlotType>{win_size};
    auto plot_size = std::pair<std::size_t, std::size_t>{300, 300};
    auto plot_pos = std::pair<std::size_t, std::size_t>{50, 50};

    auto &plot = win.get_graphical_element<0>();

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