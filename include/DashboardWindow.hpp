#pragma once

#include <thread>
#include <memory>

#include <SFML/Graphics.hpp>

#include "PlotFrame.hpp"

namespace dash_components
{

    class PlotWindow
    {
    private:

        sf::RenderWindow m_window;
        std::pair<std::size_t, std::size_t> m_dimensions;
        std::vector<StatefulDrawable*> m_graphical_elems;

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
            for (auto& elem: m_graphical_elems)
                elem->update_state();
        }

        auto render()
        {
            m_window.clear();

            for (const auto& elem: m_graphical_elems)
                m_window.draw(*elem);

            m_window.display();
        }

    public:
        PlotWindow(std::pair<std::size_t, std::size_t>);

        PlotWindow(const PlotWindow &) = delete;

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

        // We basically want the window to own the plot frame and tear them down at exit...
        template <typename T>
        auto& get_plot_frame(std::pair<std::size_t, std::size_t> dimensions,
                            std::pair<std::size_t, std::size_t> position, 
                            PlotFrame<T>::HorizontalScalingMode horizontal_scale_mode)
        {
            auto plot_frame = new PlotFrame<T>(dimensions, 
                                                std::pair{position.first, m_dimensions.second - position.second},
                                                horizontal_scale_mode);
            m_graphical_elems.push_back(plot_frame);
            return *plot_frame;
        }
    };


    PlotWindow::PlotWindow(std::pair<std::size_t, std::size_t> dimensions): 
                    m_dimensions(dimensions),
                    m_graphical_elems({})
    {
        auto run_wrapper = [this]()
        {
            m_window = sf::RenderWindow{sf::VideoMode({static_cast<unsigned>(m_dimensions.first), 
                                                        static_cast<unsigned>(m_dimensions.second)}),
                                                        "SFMLDemo"};
            run();
        };

        auto plot_thread = std::thread{run_wrapper};
        plot_thread.detach();
    }

}
