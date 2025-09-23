#pragma once

#include <vector>
#include <algorithm>
#include <ranges>
#include <cmath>
#include <deque>
#include <mutex>
#include <type_traits>

#include <SFML/Graphics.hpp>

template <typename PlotScalarType>
class PlotFrame: public sf::Drawable
{
private:
    // TODO: m_vertex_buffer is not thread safe, but we cannot grab a mutex in draw() because
    // it is inherited as `const`. Find a way to safely copy data across.
    std::vector<sf::Vertex> m_vertex_buffer;
    std::size_t m_width;
    std::size_t m_height;

    auto draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_vertex_buffer.data(), m_vertex_buffer.size(), sf::PrimitiveType::Points);
    }

public:
    using DataPointType = std::pair<PlotScalarType, PlotScalarType>;
    using ContainerType = std::deque<DataPointType>;

    PlotFrame(std::size_t width, std::size_t height): 
        m_width(width), 
        m_height(height)
    {
    }

    auto update_data(const ContainerType& data)
    {
        const auto [max_x, max_y] = std::ranges::fold_right(data,
                                                      DataPointType{std::numeric_limits<PlotScalarType>::min(),
                                                                    std::numeric_limits<PlotScalarType>::min()},
                                                      [](auto foldin, auto maxs)
                                                      { return DataPointType{std::max(maxs.first, foldin.first),
                                                                             std::max(maxs.second, foldin.second)}; });

        m_vertex_buffer = std::vector<sf::Vertex>(data.size());
        auto win_height = static_cast<float>(m_height);
        auto win_width = static_cast<float>(m_width);

        for (const auto& [index, data_pair]: std::views::enumerate(data))
        {
            m_vertex_buffer[index].position = {(static_cast<float>(data_pair.first) / static_cast<float>(max_x)) * win_width, 
                                win_height - ((static_cast<float>(data_pair.second) / static_cast<float>(max_y)) * win_height)};
        }
    }
};
