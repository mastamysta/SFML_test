#include <vector>
#include <format>
#include <print>
#include <algorithm>
#include <ranges>
#include <cmath>

#include <SFML/Graphics.hpp>

class PlotWindow
{
private:
    sf::RenderWindow m_window;

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

    template <int ArraySize>
    auto draw_data(const std::array<std::pair<float, float>, ArraySize>& plot_data)
    {
        auto max_x = std::numeric_limits<float>::min();
        auto max_y = std::numeric_limits<float>::min();

        for (int i = 0; i < plot_data.size(); i++)
        {
            max_x = std::max(max_x, plot_data[i].first);
            max_y = std::max(max_y, plot_data[i].second);
        }

        auto data = std::array<sf::Vertex, ArraySize>{};
        auto win_height = m_window.getSize().y;
        auto win_width = m_window.getSize().x;

        for (int i = 0; i < plot_data.size(); i++)
        {
            data[i].position = {(plot_data[i].first / max_x) * win_width, 
                                win_height - ((plot_data[i].second / max_y) * win_height)};
        }

        m_window.draw(data.data(), data.size(), sf::PrimitiveType::Lines);
    }

    auto render(const auto& data)
    {
        m_window.clear();
        draw_data(data);
        m_window.display();
    }

public:
    explicit PlotWindow():
        m_window(sf::VideoMode({640, 480}), "SFMLDemo")
    {
    }


    auto run(const auto& data)
    {   
        auto clock = sf::Clock{};

        while (m_window.isOpen())
        {
            handle_events();
            update_state(clock.restart());
            render(data);
        }
    }
};

int main(int argc, const char *argv[])
{
    std::println("{} hello world!", 5);

    auto data = std::array<std::pair<float, float>, 10000>{};

    for (float i = 0; i < data.size(); i++)
    {
        data[i] = {i, i * i / 27};
    }

    auto plot = PlotWindow{};
    plot.run(data);

    std::println("Program exited.");
}