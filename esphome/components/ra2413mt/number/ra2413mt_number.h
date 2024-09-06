#pragma once

#include "../ra2413mt.h"
#include "esphome/components/number/number.h"

namespace esphome
{
    namespace ra2413mt
    {

        class MaxMoveDistanceNumber : public number::Number, public Parented<RA2413MTComponent>
        {
        protected:
            void control(float max_move_distance) override;
        };

        class MaxStaticDistanceNumber : public number::Number, public Parented<RA2413MTComponent>
        {
        protected:
            void control(float max_static_distance) override;
        };

        class MoveSensitivityNumber : public number::Number, public Parented<RA2413MTComponent>
        {
        protected:
            void control(float move_sensitivity) override;
        };

        class StaticSensitivityNumber : public number::Number, public Parented<RA2413MTComponent>
        {
        protected:
            void control(float static_sensitivity) override;
        };

        class UnattendedDurationNumber : public number::Number, public Parented<RA2413MTComponent>
        {
        protected:
            void control(float unattended_duration) override;
        };

    } // namespace ra2413mt
} // namespace esphome