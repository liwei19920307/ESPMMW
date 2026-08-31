#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include <deque>
#include <vector>

namespace esphome {
namespace ra2413mt {

static const char *const TAG = "ra2413mt";

static const uint8_t CMD_HEADER[4] = {0xFD, 0xFC, 0xFB, 0xFA};
static const uint8_t CMD_FOOTER[4] = {0x04, 0x03, 0x02, 0x01};
static const uint8_t DATA_HEADER[4] = {0xF4, 0xF3, 0xF2, 0xF1};
static const uint8_t DATA_FOOTER[4] = {0xF8, 0xF7, 0xF6, 0xF5};

static const uint8_t CMD_ENABLE_CONFIG[2] = {0xFF, 0x00};
static const uint8_t CMD_DISABLE_CONFIG[2] = {0xFE, 0x00};
static const uint8_t CMD_QUERY_PARAMS[2] = {0x61, 0x00};
static const uint8_t CMD_SET_DISTANCE[2] = {0x60, 0x00};
static const uint8_t CMD_SET_SENSITIVITY[2] = {0x64, 0x00};

static const float GATE_SIZE_M = 0.75f;
static const uint8_t MIN_GATE = 2;
static const uint8_t MAX_GATE = 6;
static const size_t RX_BUFFER_LIMIT = 128;
static const uint32_t COMMAND_GAP_MS = 50;
static const uint32_t DEFAULT_THROTTLE_MS = 1000;

class RA2413MTComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_max_move_distance_number(number::Number *number) { this->max_move_distance_number_ = number; }
  void set_max_static_distance_number(number::Number *number) { this->max_static_distance_number_ = number; }
  void set_move_sensitivity_number(number::Number *number) { this->move_sensitivity_number_ = number; }
  void set_static_sensitivity_number(number::Number *number) { this->static_sensitivity_number_ = number; }
  void set_unattended_duration_number(number::Number *number) { this->unattended_duration_number_ = number; }
  void set_move_distance_sensor(sensor::Sensor *sensor) { this->move_distance_sensor_ = sensor; }
  void set_move_energy_sensor(sensor::Sensor *sensor) { this->move_energy_sensor_ = sensor; }
  void set_static_distance_sensor(sensor::Sensor *sensor) { this->static_distance_sensor_ = sensor; }
  void set_static_energy_sensor(sensor::Sensor *sensor) { this->static_energy_sensor_ = sensor; }
  void set_max_detection_range_text_sensor(text_sensor::TextSensor *sensor) {
    this->max_detection_range_text_sensor_ = sensor;
  }

  void set_default_max_move_distance(float value) { this->default_max_move_distance_ = value; }
  void set_default_max_static_distance(float value) { this->default_max_static_distance_ = value; }
  void set_default_move_sensitivity(float value) { this->default_move_sensitivity_ = value; }
  void set_default_static_sensitivity(float value) { this->default_static_sensitivity_ = value; }
  void set_default_unattended_duration(float value) { this->default_unattended_duration_ = value; }
  void set_throttle(uint32_t throttle_ms) { this->throttle_ms_ = throttle_ms; }

  void request_config();
  void reset_config();
  void start_auto_sensitivity();
  void set_max_move_distance(float meters);
  void set_max_static_distance(float meters);
  void set_move_sensitivity(float value);
  void set_static_sensitivity(float value);
  void set_unattended_duration(float seconds);

  float get_max_move_distance_m() const;
  float get_max_static_distance_m() const;
  float get_move_sensitivity() const;
  float get_static_sensitivity() const;
  float get_unattended_duration() const;

 protected:
  enum class FrameKind : uint8_t { NONE, COMMAND, DATA };

  void poll_uart_();
  void reset_frame_();
  bool match_prefix_(const uint8_t *prefix, size_t len) const;
  bool match_suffix_(const uint8_t *suffix, size_t len) const;
  void handle_frame_();
  void handle_data_frame_();
  void handle_command_frame_();
  void handle_query_ack_(const uint8_t *payload, size_t len);
  void publish_max_detection_range_();

  void enqueue_command_(std::vector<uint8_t> payload);
  void pump_command_queue_();
  void write_raw_(const uint8_t *data, size_t len);
  void write_inner_frame_(const std::vector<uint8_t> &inner);
  void write_framed_(const uint8_t *cmd, size_t cmd_len, const uint8_t *value, size_t value_len);
  void send_enable_config_();
  void send_disable_config_();
  void send_set_distance_(uint8_t max_move_gate, uint8_t max_static_gate, uint16_t duration_s);
  void send_set_sensitivity_(uint16_t move_sensitivity, uint16_t static_sensitivity);
  void finish_auto_sensitivity_();

  static uint8_t meters_to_gate_(float meters);
  static float gate_to_meters_(uint8_t gate);
  static uint16_t read_le16_(const uint8_t *data);
  static void write_le16_(uint8_t *data, uint16_t value);
  static void write_le32_(uint8_t *data, uint32_t value);

  number::Number *max_move_distance_number_{nullptr};
  number::Number *max_static_distance_number_{nullptr};
  number::Number *move_sensitivity_number_{nullptr};
  number::Number *static_sensitivity_number_{nullptr};
  number::Number *unattended_duration_number_{nullptr};
  sensor::Sensor *move_distance_sensor_{nullptr};
  sensor::Sensor *move_energy_sensor_{nullptr};
  sensor::Sensor *static_distance_sensor_{nullptr};
  sensor::Sensor *static_energy_sensor_{nullptr};
  text_sensor::TextSensor *max_detection_range_text_sensor_{nullptr};

  float default_max_move_distance_{4.5f};
  float default_max_static_distance_{4.5f};
  float default_move_sensitivity_{99.0f};
  float default_static_sensitivity_{15.0f};
  float default_unattended_duration_{5.0f};
  uint32_t throttle_ms_{DEFAULT_THROTTLE_MS};

  std::vector<uint8_t> rx_buffer_;
  FrameKind frame_kind_{FrameKind::NONE};
  uint16_t frame_data_len_{0};
  bool frame_len_known_{false};
  uint32_t last_data_millis_{0};

  std::deque<std::vector<uint8_t>> command_queue_;
  bool command_busy_{false};

  bool auto_sensitivity_active_{false};
  uint8_t auto_move_peak_{0};
  uint8_t auto_static_peak_{0};
};

}  // namespace ra2413mt
}  // namespace esphome
