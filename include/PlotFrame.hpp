#pragma once

#include <vector>
#include <algorithm>
#include <ranges>
#include <cmath>
#include <deque>
#include <mutex>
#include <type_traits>
#include <cstddef>
#include <print>

#include "StatefulDrawable.hpp"

namespace dash_components
{

    template <typename PlotScalarType>
    class PlotFrame: public StatefulDrawable
    {
    public:
        using DataPointType = std::pair<PlotScalarType, PlotScalarType>;
        using ContainerType = std::deque<DataPointType>;

        enum class HorizontalScalingMode
        {
            FIT_TO_MINIMUM,
            FIT_TO_ORIGIN
        };

        PlotFrame(std::pair<std::size_t, std::size_t>,
                std::pair<std::size_t, std::size_t>,
                HorizontalScalingMode);

        PlotFrame(const PlotFrame &) = delete;

        auto update_state() -> void;

        auto update_data(const ContainerType &data);


    private:
        std::pair<std::size_t, std::size_t> m_dimensions;
        std::pair<std::size_t, std::size_t> m_position;
        HorizontalScalingMode m_horizontal_scale_mode;

        std::vector<sf::Vertex> m_vertex_buffer;    // Actual vertices to render.
        std::mutex m_data_mutex;                    // Mutex to prevent data from being updated while we process it.
        ContainerType m_data;                       // Data to be plotted.
        bool m_data_updated;                        // Was the data updated since last render loop?

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
        {
            std::println("Printing stuff!");
            target.draw(m_vertex_buffer.data(), m_vertex_buffer.size(), sf::PrimitiveType::Points);
        }
    };

    template <typename PlotScalarType>
    inline PlotFrame<PlotScalarType>::PlotFrame(std::pair<std::size_t, std::size_t> dimensions,
                                                std::pair<std::size_t, std::size_t> position, 
                                                HorizontalScalingMode horizontal_scale_mode):
            m_dimensions(dimensions),
            m_position(position),
            m_horizontal_scale_mode(horizontal_scale_mode),
            m_data_updated(false)
    {
    }

    template <typename PlotScalarType>
    inline auto PlotFrame<PlotScalarType>::update_state() -> void
    {
        auto lg = std::lock_guard(m_data_mutex);

        const auto [max_x, max_y] = std::ranges::fold_right(m_data,
                                                        DataPointType{std::numeric_limits<PlotScalarType>::min(),
                                                                    std::numeric_limits<PlotScalarType>::min()},
                                                        [](auto foldin, auto maxs)
                                                        { return DataPointType{std::max(maxs.first, foldin.first),
                                                                                std::max(maxs.second, foldin.second)}; });
        const auto [min_x, min_y] = std::ranges::fold_right(m_data,
                                                        DataPointType{std::numeric_limits<PlotScalarType>::max(),
                                                                    std::numeric_limits<PlotScalarType>::max()},
                                                        [](auto foldin, auto maxs)
                                                        { return DataPointType{std::min(maxs.first, foldin.first),
                                                                                std::min(maxs.second, foldin.second)}; });

        m_vertex_buffer = std::vector<sf::Vertex>(m_data.size());

        auto frame_height = static_cast<float>(m_dimensions.second);
        auto frame_width = static_cast<float>(m_dimensions.first);

        for (const auto& [index, data_pair]: std::views::enumerate(m_data))
        {

            if (m_horizontal_scale_mode == HorizontalScalingMode::FIT_TO_MINIMUM)
            {
                auto x_ratio = static_cast<float>(max_x - min_x);
                m_vertex_buffer[index].position.x = (static_cast<float>(data_pair.first - min_x) / x_ratio) * frame_width;
            }
            else
            {
                m_vertex_buffer[index].position.x = (static_cast<float>(data_pair.first) / max_x) * frame_width;
            }

            m_vertex_buffer[index].position.x += m_position.first;

            m_vertex_buffer[index].position.y = 
                                m_position.second - ((static_cast<float>(data_pair.second) / static_cast<float>(max_y)) * frame_height);

            std::println("{} {}", m_vertex_buffer[index].position.x, m_vertex_buffer[index].position.y);
        }

        m_data_updated = false;
    }

    template <typename PlotScalarType>
    inline auto PlotFrame<PlotScalarType>::update_data(const ContainerType &data)
    {
        auto lg = std::lock_guard(m_data_mutex);
        m_data = data;
        m_data_updated = true;
    }

}
