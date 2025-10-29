#pragma once

#include <SFML/Graphics.hpp>

namespace dash_components
{

    class StatefulDrawable: public sf::Drawable
    {
    public:
        virtual void update_state() = 0;
    };

}
