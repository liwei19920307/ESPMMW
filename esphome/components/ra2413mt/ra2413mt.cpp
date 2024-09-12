#include "ra2413mt.h"
#include <vector>

namespace esphome
{
    namespace ra2413mt
    {

        const size_t MAX_BUFFER_SIZE = 34;
        const uint32_t REFRESH_MILLIS = 1000;
        std::vector<uint8_t> buffer;
        size_t buffer_szie;
        uint32_t latest_millis = 0;

        void RA2413MTComponent::loop()
        {
            while (available())
            {

                buffer.push_back(read());
                buffer_szie = buffer.size();

                if (buffer_szie > MAX_BUFFER_SIZE)
                {
                    std::vector<uint8_t>().swap(buffer);
                    continue;
                }

                if (buffer[0] == 0xF4)
                {
                    if (!(buffer[buffer_szie - 4] == 0xF8 && buffer[buffer_szie - 3] == 0xF7 && buffer[buffer_szie - 2] == 0xF6 && buffer[buffer_szie - 1] == 0xF5))
                    {
                        continue;
                    }

                    if (millis() - latest_millis > REFRESH_MILLIS)
                    {
                        for (auto &listener : this->listeners_)
                        {
                            listener->on_move_distance(two_byte_to_int(buffer[9], buffer[10]));
                            listener->on_move_energy(buffer[11]);
                            listener->on_static_distance(two_byte_to_int(buffer[12], buffer[13]));
                            listener->on_static_energy(buffer[14]);
                        }
                        latest_millis = millis();
                    }
                }

                if (buffer[0] == 0xFD)
                {
                    if (!(buffer[buffer_szie - 4] == 0x04 && buffer[buffer_szie - 3] == 0x03 && buffer[buffer_szie - 2] == 0x02 && buffer[buffer_szie - 1] == 0x01))
                    {
                        continue;
                    }

                    if (buffer_szie == MAX_BUFFER_SIZE)
                    {
                        this->max_move_distance_number_->publish_state(buffer[12] * 0.75);
                        this->max_static_distance_number_->publish_state(buffer[13] * 0.75);
                        this->move_sensitivity_number_->publish_state(buffer[14]);
                        this->static_sensitivity_number_->publish_state(buffer[23]);
                        this->unattended_duration_number_->publish_state(two_byte_to_int(buffer[28], buffer[29]));
                    }
                    log_bytes(buffer);
                }

                std::vector<uint8_t>().swap(buffer);
            }
        }

        void RA2413MTComponent::dump_config()
        {
            this->send_command(GET_CONF, sizeof(GET_CONF));
        }

        void RA2413MTComponent::register_listener(RA2413MTListener *listener)
        {
            this->listeners_.push_back(listener);
        }

        void RA2413MTComponent::send_command(const uint8_t *command, int command_size)
        {
            this->write_array(CMD_START, sizeof(CMD_START));
            delay(50);
            this->write_array(command, command_size);
            delay(50);
            this->write_array(CMD_END, sizeof(CMD_END));
        }

        void RA2413MTComponent::log_bytes(std::vector<uint8_t> bytes)
        {
            std::string str;
            char buf[5];
            for (size_t i = 0; i < bytes.size(); i++)
            {
                if (i > 0)
                {
                    str += " ";
                }
                sprintf(buf, "%02X", bytes[i]);
                str += buf;
            }
            ESP_LOGD(TAG, "%s", str.c_str());
        }

        int RA2413MTComponent::two_byte_to_int(uint8_t firstbyte, uint8_t secondbyte)
        {
            return (int16_t)(secondbyte << 8) + firstbyte;
        }

    } // namespace ra2413mt
} // namespace esphome
