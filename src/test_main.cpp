#include <vector>
#include <format>
#include <print>
#include <algorithm>
#include <ranges>
#include <cmath>
#include <thread>
#include <deque>

#include <SFML/Graphics.hpp>

class PlotWindow
{
private:
    using PlotScalarType = float;
    using DataPointType = std::pair<PlotScalarType, PlotScalarType>;
    using ContainerType = std::deque<DataPointType>;

    sf::RenderWindow m_window;
    const ContainerType& m_data;

    enum class KeyEventKind
    {
        PRESSED,
        RELEASED
    };

    auto handle_events()
    {
        while (const auto event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                m_window.close();
                break;
            }
        }
    }

    auto update_state(sf::Time delta_time)
    {
    }

    auto draw_data()
    {
        const auto [max_x, max_y] = std::ranges::fold_right(m_data,
                                                      DataPointType{std::numeric_limits<PlotScalarType>::min(),
                                                                    std::numeric_limits<PlotScalarType>::min()},
                                                      [](auto foldin, auto maxs)
                                                      { return DataPointType{std::max(maxs.first, foldin.first),
                                                                             std::max(maxs.second, foldin.second)}; });

        auto vertex_buffer = std::vector<sf::Vertex>{};
        vertex_buffer.resize(m_data.size());
        auto win_height = m_window.getSize().y;
        auto win_width = m_window.getSize().x;

        for (const auto& [index, data_pair]: std::views::enumerate(m_data))
        {
            vertex_buffer[index].position = {(data_pair.first / max_x) * win_width, 
                                win_height - ((data_pair.second / max_y) * win_height)};
        }

        m_window.draw(vertex_buffer.data(), vertex_buffer.size(), sf::PrimitiveType::Points);
    }

    auto render()
    {
        m_window.clear();
        draw_data();
        m_window.display();
    }

public:
    explicit PlotWindow(const ContainerType& initial_data):
        m_window(sf::VideoMode({640, 480}), "SFMLDemo"),
        m_data(initial_data)
    {
    }

    auto run()
    {   
        auto clock = sf::Clock{};

        while (m_window.isOpen())
        {
            handle_events();
            update_state(clock.restart());
            render();
        }
    }
};

int main(int argc, const char *argv[])
{
    auto data = std::deque<std::pair<float, float>>{};
    data.resize(10000);

    for (float i = 0; i < data.size(); i++)
    {
        data[i] = {i, i * i / 27};
    }

    auto plot = PlotWindow{data};
    plot.run();
    // auto run_wrapper = [&plot]()
    // {
    //     plot.run();
    // };

    // auto plot_thread = std::thread{run_wrapper};
    // plot_thread.detach();

    // for (int i = 0; i < 10000; i++)
    // {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(2));

    // }



    std::println("Program exited.");
}