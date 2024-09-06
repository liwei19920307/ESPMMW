#include "ra2413mt_number.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace ra2413mt
    {

        void MaxMoveDistanceNumber::control(float max_move_distance)
        {
            float max_static_distance = this->parent_->max_static_distance_number_->state;
            float unattended_duration = this->parent_->unattended_duration_number_->state;
            SET_DISTANCE_UNATTENDED[10] = max_move_distance / 0.75;
            SET_DISTANCE_UNATTENDED[16] = max_static_distance / 0.75;
            SET_DISTANCE_UNATTENDED[22] = unattended_duration;
            this->parent_->send_command(SET_DISTANCE_UNATTENDED, sizeof(SET_DISTANCE_UNATTENDED));
            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

        void MaxStaticDistanceNumber::control(float max_static_distance)
        {
            float max_move_distance = this->parent_->max_move_distance_number_->state;
            float unattended_duration = this->parent_->unattended_duration_number_->state;
            SET_DISTANCE_UNATTENDED[10] = max_move_distance / 0.75;
            SET_DISTANCE_UNATTENDED[16] = max_static_distance / 0.75;
            SET_DISTANCE_UNATTENDED[22] = unattended_duration;
            this->parent_->send_command(SET_DISTANCE_UNATTENDED, sizeof(SET_DISTANCE_UNATTENDED));
            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

        void MoveSensitivityNumber::control(float move_sensitivity)
        {
            float static_sensitivity = this->parent_->static_sensitivity_number_->state;
            SET_SENSITIVITY[16] = move_sensitivity;
            SET_SENSITIVITY[22] = static_sensitivity;
            this->parent_->send_command(SET_SENSITIVITY, sizeof(SET_SENSITIVITY));
            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

        void StaticSensitivityNumber::control(float static_sensitivity)
        {
            float move_sensitivity = this->parent_->move_sensitivity_number_->state;
            SET_SENSITIVITY[16] = move_sensitivity;
            SET_SENSITIVITY[22] = static_sensitivity;
            this->parent_->send_command(SET_SENSITIVITY, sizeof(SET_SENSITIVITY));
            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

        void UnattendedDurationNumber::control(float unattended_duration)
        {
            float max_move_distance = this->parent_->max_move_distance_number_->state;
            float max_static_distance = this->parent_->max_static_distance_number_->state;
            SET_DISTANCE_UNATTENDED[10] = max_move_distance / 0.75;
            SET_DISTANCE_UNATTENDED[16] = max_static_distance / 0.75;
            SET_DISTANCE_UNATTENDED[22] = unattended_duration;
            this->parent_->send_command(SET_DISTANCE_UNATTENDED, sizeof(SET_DISTANCE_UNATTENDED));
            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

    } // namespace ra2413mt
} // namespace esphome