#pragma once

#include <thread>
#include <memory>

#include <SFML/Graphics.hpp>

#include "PlotFrame.hpp"

namespace dash_components
{

    template <typename... GraphicalElementTypes>
    class DashBoardWindow
    {
    private:

        sf::RenderWindow m_window;
        std::pair<std::size_t, std::size_t> m_dimensions;
        bool m_window_size_changed;

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

        auto push_updated_window_size()
        {
            sf::Vector2u win_size = m_window.getSize();
            std::pair<std::size_t, std::size_t> win_size_p = {win_size.x, win_size.y};

            auto update_winsize = [&](auto &graphical_element)
            {
                graphical_element.set_parent_dimensions(win_size_p);
            };

            do_on_graphical_elements(update_winsize);
        }

        auto update_state(sf::Time delta_time)
        {
            if (m_window_size_changed)
            {
                push_updated_window_size();
                m_window_size_changed = false;
            }

            auto update_state = [](auto& graphical_element)
            {
                graphical_element.update_state();
            };

            do_on_graphical_elements(update_state);
        }

        auto render()
        {
            m_window.clear();

            auto draw = [&](auto& graphical_element)
            {
                m_window.draw(graphical_element);
            };

            do_on_graphical_elements(draw);

            m_window.display();
        }

        template <int I, typename... Ts>
        struct type_at_index;

        template <int I, typename T, typename... Ts>
        struct type_at_index<I, T, Ts...>
        {
            using type = typename type_at_index<I - 1, Ts...>::type;
        };

        template <typename T, typename... Ts>
        struct type_at_index<0, T, Ts...>
        {
            using type = T;
        };

        auto do_on_graphical_elements(auto callable) -> void
        {
            do_on_graphical_elements_impl<sizeof...(GraphicalElementTypes)-1>(callable);
        }

        template <std::size_t I>
        auto do_on_graphical_elements_impl(auto callable) -> void
        {
            callable(get_graphical_element<I>());

            if constexpr (I == 0)
                return;
            else
                do_on_graphical_elements_impl<I - 1>(callable);
        }


    public:
        DashBoardWindow(std::pair<std::size_t, std::size_t>);

        DashBoardWindow(const DashBoardWindow &) = delete;

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

        template <int I>
        auto& get_graphical_element()
        {
            static auto graphical_element = typename type_at_index<I, GraphicalElementTypes...>::type{};
            return graphical_element;
        }
    };

    template <typename... GraphicalElementTypes>
    DashBoardWindow<GraphicalElementTypes...>::DashBoardWindow(std::pair<std::size_t, std::size_t> dimensions): 
                    m_dimensions(dimensions),
                    m_window_size_changed(true)
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
