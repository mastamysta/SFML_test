#pragma once

#include <thread>

#include <SFML/Graphics.hpp>

template <typename PlotFrameType>
class PlotWindow
{
private:

    sf::RenderWindow m_window;
    const PlotFrameType& m_plot;

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
        // Some procedures about the internal state of our GUI.
    }

    auto render()
    {
        m_window.clear();
        m_window.draw(m_plot);
        m_window.display();
    }

public:
    PlotWindow(const PlotFrameType& plot): m_plot(plot)
    {
        auto run_wrapper = [this]()
        {
            m_window = sf::RenderWindow{sf::VideoMode({640, 480}), "SFMLDemo"};
            run();
        };

        auto plot_thread = std::thread{run_wrapper};
        plot_thread.detach();
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
