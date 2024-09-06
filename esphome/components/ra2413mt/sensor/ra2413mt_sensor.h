#pragma once

#include "../ra2413mt.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace ra2413mt
    {

        class MoveDistanceSensor : public RA2413MTListener, public sensor::Sensor, public Parented<RA2413MTComponent>
        {
        public:
            void on_move_distance(uint16_t move_distance) override;
        };

        class MoveEnergySensor : public RA2413MTListener, public sensor::Sensor, public Parented<RA2413MTComponent>
        {
        public:
            void on_move_energy(uint16_t move_energy) override;
        };

        class StaticDistanceSensor : public RA2413MTListener, public sensor::Sensor, public Parented<RA2413MTComponent>
        {
        public:
            void on_static_distance(uint16_t static_distance) override;
        };

        class StaticEnergySensor : public RA2413MTListener, public sensor::Sensor, public Parented<RA2413MTComponent>
        {
        public:
            void on_static_energy(uint16_t static_energy) override;
        };

    } // namespace ra2413mt
} // namespace esphome