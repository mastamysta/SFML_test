#pragma once

#include <vector>
#include <algorithm>
#include <ranges>
#include <cmath>
#include <deque>
#include <mutex>
#include <type_traits>
#include <cstddef>

#include <SFML/Graphics.hpp>

template <typename PlotScalarType>
class PlotFrame: public sf::Drawable
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

    auto update_data(const ContainerType &data);

private:
    std::vector<sf::Vertex> m_vertex_buffer;
    std::pair<std::size_t, std::size_t> m_dimensions;
    std::pair<std::size_t, std::size_t> m_position;
    HorizontalScalingMode m_horizontal_scale_mode;

    auto draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_vertex_buffer.data(), m_vertex_buffer.size(), sf::PrimitiveType::Points);
    }
};

template <typename PlotScalarType>
inline PlotFrame<PlotScalarType>::PlotFrame(std::pair<std::size_t, std::size_t> dimensions,
                                            std::pair<std::size_t, std::size_t> position, 
                                            HorizontalScalingMode horizontal_scale_mode):
        m_dimensions(dimensions),
        m_position(position),
        m_horizontal_scale_mode(horizontal_scale_mode)
{
}

template <typename PlotScalarType>
inline auto PlotFrame<PlotScalarType>::update_data(const ContainerType &data)
{
    const auto [max_x, max_y] = std::ranges::fold_right(data,
                                                    DataPointType{std::numeric_limits<PlotScalarType>::min(),
                                                                std::numeric_limits<PlotScalarType>::min()},
                                                    [](auto foldin, auto maxs)
                                                    { return DataPointType{std::max(maxs.first, foldin.first),
                                                                            std::max(maxs.second, foldin.second)}; });
    const auto [min_x, min_y] = std::ranges::fold_right(data,
                                                    DataPointType{std::numeric_limits<PlotScalarType>::max(),
                                                                std::numeric_limits<PlotScalarType>::max()},
                                                    [](auto foldin, auto maxs)
                                                    { return DataPointType{std::min(maxs.first, foldin.first),
                                                                            std::min(maxs.second, foldin.second)}; });

    m_vertex_buffer = std::vector<sf::Vertex>(data.size());
    auto frame_height = static_cast<float>(m_dimensions.second);
    auto frame_width = static_cast<float>(m_dimensions.first);

    for (const auto& [index, data_pair]: std::views::enumerate(data))
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
                            frame_height - ((static_cast<float>(data_pair.second) / static_cast<float>(max_y)) * frame_height) - m_position.second;
    }
}