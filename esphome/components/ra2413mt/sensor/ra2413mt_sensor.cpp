#include "ra2413mt_sensor.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace ra2413mt
    {

        void MoveDistanceSensor::on_move_distance(uint16_t move_distance)
        {
            this->publish_state(move_distance);
        }

        void MoveEnergySensor::on_move_energy(uint16_t move_energy)
        {
            this->publish_state(move_energy);
        }

        void StaticDistanceSensor::on_static_distance(uint16_t static_distance)
        {
            this->publish_state(static_distance);
        }

        void StaticEnergySensor::on_static_energy(uint16_t static_energy)
        {
            this->publish_state(static_energy);
        }

    } // namespace ra2413mt
} // namespace esphome