#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/button/button.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
    namespace ra2413mt
    {

        static const char *const TAG = "RA2413MT";
        static const uint8_t CMD_START[14] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
        static const uint8_t CMD_END[12] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
        static const uint8_t GET_CONF[12] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x61, 0x00, 0x04, 0x03, 0x02, 0x01};
        static uint8_t SET_DISTANCE_UNATTENDED[30] = {0xFD, 0xFC, 0xFB, 0xFA, 0x14, 0x00, 0x60, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x02, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};
        static uint8_t SET_SENSITIVITY[30] = {0xFD, 0xFC, 0xFB, 0xFA, 0x14, 0x00, 0x64, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x02, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01};

        class RA2413MTListener
        {
        public:
            virtual void on_move_distance(uint16_t move_distance) {};
            virtual void on_move_energy(uint16_t move_energy) {};
            virtual void on_static_distance(uint16_t static_distance) {};
            virtual void on_static_energy(uint16_t static_energy) {};
        };

        class RA2413MTComponent : public Component, public uart::UARTDevice
        {
        public:
            void loop() override;
            void dump_config() override;
            void register_listener(RA2413MTListener *listener);
            int two_byte_to_int(uint8_t firstbyte, uint8_t secondbyte);
            void send_command(const uint8_t *command, int command_size);
            void log_bytes(std::vector<uint8_t> bytes);

            button::Button *get_conf_button_{nullptr};
            button::Button *reset_conf_button_{nullptr};
            number::Number *max_move_distance_number_{nullptr};
            number::Number *max_static_distance_number_{nullptr};
            number::Number *move_sensitivity_number_{nullptr};
            number::Number *static_sensitivity_number_{nullptr};
            number::Number *unattended_duration_number_{nullptr};
            sensor::Sensor *move_distance_sensor_{nullptr};
            sensor::Sensor *move_energy_sensor_{nullptr};
            sensor::Sensor *static_distance_sensor_{nullptr};
            sensor::Sensor *static_energy_sensor_{nullptr};

            void set_get_conf_button(button::Button *button) { this->get_conf_button_ = button; };
            void set_reset_conf_button(button::Button *button) { this->reset_conf_button_ = button; };
            void set_max_move_distance_number(number::Number *number) { this->max_move_distance_number_ = number; };
            void set_max_static_distance_number_(number::Number *number) { this->max_static_distance_number_ = number; };
            void set_move_sensitivity_number_(number::Number *number) { this->move_sensitivity_number_ = number; };
            void set_static_sensitivity_number_(number::Number *number) { this->static_sensitivity_number_ = number; };
            void set_unattended_duration_number(number::Number *number) { this->unattended_duration_number_ = number; };
            void set_move_distance_sensor(sensor::Sensor *sensor) { this->move_distance_sensor_ = sensor; }
            void set_move_energy_sensor(sensor::Sensor *sensor) { this->move_energy_sensor_ = sensor; }
            void set_static_distance_sensor(sensor::Sensor *sensor) { this->static_distance_sensor_ = sensor; }
            void set_static_energy_sensor(sensor::Sensor *sensor) { this->static_energy_sensor_ = sensor; }

            float default_max_move_distance;
            float default_max_static_distance;
            float default_move_sensitivity;
            float default_static_sensitivity;
            float default_unattended_duration;

            void set_default_max_move_distance(float value)
            {
                this->default_max_move_distance = value;
            }
            void set_default_max_static_distance(float value)
            {
                this->default_max_static_distance = value;
            }
            void set_default_move_sensitivity(float value)
            {
                this->default_move_sensitivity = value;
            }
            void set_default_static_sensitivity(float value)
            {
                this->default_static_sensitivity = value;
            }
            void set_default_unattended_duration(float value)
            {
                this->default_unattended_duration = value;
            }

        protected:
            std::vector<RA2413MTListener *> listeners_{};
        };

    } // namespace ra2413mt
} // namespace esphome
