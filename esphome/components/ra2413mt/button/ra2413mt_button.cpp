#include "ra2413mt_button.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace ra2413mt
    {

        void GetConfButton::press_action()
        {
            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

        void RestConfButton::press_action()
        {
            SET_DISTANCE_UNATTENDED[10] = this->parent_->default_max_move_distance / 0.75;
            SET_DISTANCE_UNATTENDED[16] = this->parent_->default_max_static_distance / 0.75F;
            SET_DISTANCE_UNATTENDED[22] = this->parent_->default_unattended_duration;
            this->parent_->send_command(SET_DISTANCE_UNATTENDED, sizeof(SET_DISTANCE_UNATTENDED));

            SET_SENSITIVITY[16] = this->parent_->default_move_sensitivity;
            SET_SENSITIVITY[22] = this->parent_->default_static_sensitivity;
            this->parent_->send_command(SET_SENSITIVITY, sizeof(SET_SENSITIVITY));

            this->parent_->send_command(GET_CONF, sizeof(GET_CONF));
        }

    } // namespace ra2413mt
} // namespace esphome
