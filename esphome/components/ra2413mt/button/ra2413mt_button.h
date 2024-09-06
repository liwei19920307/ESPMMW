#pragma once

#include "../ra2413mt.h"
#include "esphome/components/button/button.h"

namespace esphome
{
    namespace ra2413mt
    {

        class GetConfButton : public button::Button, public Parented<RA2413MTComponent>
        {
        protected:
            void press_action() override;
        };

        class RestConfButton : public button::Button, public Parented<RA2413MTComponent>
        {
        protected:
            void press_action() override;
        };

    } // namespace ra2413mt
} // namespace esphome