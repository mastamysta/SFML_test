#pragma once

#include <SFML/Graphics.hpp>

namespace dash_components
{

    class RectangularDrawable: public sf::Drawable
    {
    public:
        using DimensionType = std::pair<std::size_t, std::size_t>;

        RectangularDrawable();

        auto set_parent_dimensions(const DimensionType&) -> void;

        auto set_position(const DimensionType&) -> void;

        auto set_dimensions(const DimensionType&) -> void;

    protected:
        DimensionType m_parent_dimensions;
        DimensionType m_dimensions;
        DimensionType m_position_x_y;
        DimensionType m_position;
    };

    RectangularDrawable::RectangularDrawable():
                            m_dimensions({0, 0}),
                            m_position({0, 0}),
                            m_parent_dimensions({0, 0})
    {
    }

    auto RectangularDrawable::set_parent_dimensions(const DimensionType& parent_dimensions) -> void
    {
        // TODO: We may need to resize this element after the parent updated it's size.

        m_parent_dimensions = parent_dimensions;

        set_position(m_position_x_y);
    }

    auto RectangularDrawable::set_position(const DimensionType& position) -> void
    {
        // TODO: Validate that position is within parent's bounds.

        m_position_x_y = position;

        m_position.first = position.first;
        m_position.second = m_parent_dimensions.second - position.second;
    }

    auto RectangularDrawable::set_dimensions(const DimensionType& dimensions) -> void
    {
        // TODO: Validate that the new dimensions fit within the parent, given the current position of this element.

        m_dimensions = dimensions;
    }
}
