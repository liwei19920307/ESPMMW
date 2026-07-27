#include "ra2413mt.h"
#include "esphome/core/log.h"

#include <cmath>

namespace esphome {
namespace ra2413mt {

void RA2413MTComponent::setup() {
  this->rx_buffer_.reserve(64);
  this->set_timeout("boot_query", 1500, [this]() { this->request_config(); });
}

void RA2413MTComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "RA2413MT:");
  ESP_LOGCONFIG(TAG, "  Throttle: %ums", this->throttle_ms_);
  LOG_SENSOR("  ", "Move Distance", this->move_distance_sensor_);
  LOG_SENSOR("  ", "Move Energy", this->move_energy_sensor_);
  LOG_SENSOR("  ", "Static Distance", this->static_distance_sensor_);
  LOG_SENSOR("  ", "Static Energy", this->static_energy_sensor_);
  LOG_NUMBER("  ", "Max Move Distance", this->max_move_distance_number_);
  LOG_NUMBER("  ", "Max Static Distance", this->max_static_distance_number_);
  LOG_NUMBER("  ", "Move Sensitivity", this->move_sensitivity_number_);
  LOG_NUMBER("  ", "Static Sensitivity", this->static_sensitivity_number_);
  LOG_NUMBER("  ", "Unattended Duration", this->unattended_duration_number_);
}

void RA2413MTComponent::loop() { this->poll_uart_(); }

void RA2413MTComponent::request_config() { this->enqueue_command_({CMD_QUERY_PARAMS[0], CMD_QUERY_PARAMS[1]}); }

void RA2413MTComponent::reset_config() {
  const uint8_t max_move = meters_to_gate_(this->default_max_move_distance_);
  const uint8_t max_static = meters_to_gate_(this->default_max_static_distance_);
  const uint16_t duration = static_cast<uint16_t>(this->default_unattended_duration_);
  const uint16_t move_sens = static_cast<uint16_t>(this->default_move_sensitivity_);
  const uint16_t static_sens = static_cast<uint16_t>(this->default_static_sensitivity_);

  this->send_set_distance_(max_move, max_static, duration);
  this->send_set_sensitivity_(move_sens, static_sens);
  this->request_config();
}

void RA2413MTComponent::start_auto_sensitivity() {
  if (this->auto_sensitivity_active_) {
    ESP_LOGW(TAG, "Auto sensitivity already running");
    return;
  }

  this->auto_sensitivity_active_ = true;
  this->auto_move_peak_ = 0;
  this->auto_static_peak_ = 0;
  ESP_LOGI(TAG, "Auto sensitivity: sampling empty-room energy for 4s...");
  this->set_timeout("auto_sensitivity", 4000, [this]() { this->finish_auto_sensitivity_(); });
}

void RA2413MTComponent::finish_auto_sensitivity_() {
  this->auto_sensitivity_active_ = false;

  // move: energy must exceed threshold to turn ON → sit above ambient peak
  // static: energy must fall below threshold to turn OFF → sit above empty-room peak
  const uint16_t move_sens =
      clamp(static_cast<uint16_t>(this->auto_move_peak_ + 15), (uint16_t) 20, (uint16_t) 99);
  const uint16_t static_sens =
      clamp(static_cast<uint16_t>(this->auto_static_peak_ + 8), (uint16_t) 5, (uint16_t) 80);

  ESP_LOGI(TAG, "Auto sensitivity: move_peak=%u static_peak=%u → move=%u static=%u", this->auto_move_peak_,
           this->auto_static_peak_, move_sens, static_sens);

  this->send_set_sensitivity_(move_sens, static_sens);
  if (this->move_sensitivity_number_ != nullptr) {
    this->move_sensitivity_number_->publish_state(move_sens);
  }
  if (this->static_sensitivity_number_ != nullptr) {
    this->static_sensitivity_number_->publish_state(static_sens);
  }
  this->request_config();
}

void RA2413MTComponent::set_max_move_distance(float meters) {
  this->send_set_distance_(meters_to_gate_(meters), meters_to_gate_(this->get_max_static_distance_m()),
                           static_cast<uint16_t>(this->get_unattended_duration()));
  this->request_config();
}

void RA2413MTComponent::set_max_static_distance(float meters) {
  this->send_set_distance_(meters_to_gate_(this->get_max_move_distance_m()), meters_to_gate_(meters),
                           static_cast<uint16_t>(this->get_unattended_duration()));
  this->request_config();
}

void RA2413MTComponent::set_unattended_duration(float seconds) {
  this->send_set_distance_(meters_to_gate_(this->get_max_move_distance_m()),
                           meters_to_gate_(this->get_max_static_distance_m()), static_cast<uint16_t>(seconds));
  this->request_config();
}

void RA2413MTComponent::set_move_sensitivity(float value) {
  this->send_set_sensitivity_(static_cast<uint16_t>(value), static_cast<uint16_t>(this->get_static_sensitivity()));
  this->request_config();
}

void RA2413MTComponent::set_static_sensitivity(float value) {
  this->send_set_sensitivity_(static_cast<uint16_t>(this->get_move_sensitivity()), static_cast<uint16_t>(value));
  this->request_config();
}

float RA2413MTComponent::get_max_move_distance_m() const {
  if (this->max_move_distance_number_ != nullptr && !std::isnan(this->max_move_distance_number_->state)) {
    return this->max_move_distance_number_->state;
  }
  return this->default_max_move_distance_;
}

float RA2413MTComponent::get_max_static_distance_m() const {
  if (this->max_static_distance_number_ != nullptr && !std::isnan(this->max_static_distance_number_->state)) {
    return this->max_static_distance_number_->state;
  }
  return this->default_max_static_distance_;
}

float RA2413MTComponent::get_move_sensitivity() const {
  if (this->move_sensitivity_number_ != nullptr && !std::isnan(this->move_sensitivity_number_->state)) {
    return this->move_sensitivity_number_->state;
  }
  return this->default_move_sensitivity_;
}

float RA2413MTComponent::get_static_sensitivity() const {
  if (this->static_sensitivity_number_ != nullptr && !std::isnan(this->static_sensitivity_number_->state)) {
    return this->static_sensitivity_number_->state;
  }
  return this->default_static_sensitivity_;
}

float RA2413MTComponent::get_unattended_duration() const {
  if (this->unattended_duration_number_ != nullptr && !std::isnan(this->unattended_duration_number_->state)) {
    return this->unattended_duration_number_->state;
  }
  return this->default_unattended_duration_;
}

void RA2413MTComponent::poll_uart_() {
  while (this->available()) {
    const uint8_t byte = this->read();

    if (this->rx_buffer_.empty()) {
      if (byte == DATA_HEADER[0]) {
        this->frame_kind_ = FrameKind::DATA;
      } else if (byte == CMD_HEADER[0]) {
        this->frame_kind_ = FrameKind::COMMAND;
      } else {
        continue;
      }
      this->rx_buffer_.push_back(byte);
      continue;
    }

    this->rx_buffer_.push_back(byte);

    if (this->rx_buffer_.size() < 4) {
      const uint8_t *expected = this->frame_kind_ == FrameKind::DATA ? DATA_HEADER : CMD_HEADER;
      if (this->rx_buffer_.back() != expected[this->rx_buffer_.size() - 1]) {
        this->reset_frame_();
      }
      continue;
    }

    if (!this->frame_len_known_ && this->rx_buffer_.size() >= 6) {
      this->frame_data_len_ = read_le16_(&this->rx_buffer_[4]);
      this->frame_len_known_ = true;
      if (this->frame_data_len_ > RX_BUFFER_LIMIT - 10) {
        ESP_LOGW(TAG, "Frame length too large: %u", this->frame_data_len_);
        this->reset_frame_();
        continue;
      }
    }

    if (!this->frame_len_known_) {
      continue;
    }

    const size_t expected_size = 4 + 2 + this->frame_data_len_ + 4;
    if (this->rx_buffer_.size() < expected_size) {
      if (this->rx_buffer_.size() >= RX_BUFFER_LIMIT) {
        ESP_LOGW(TAG, "RX buffer overflow, resetting frame");
        this->reset_frame_();
      }
      continue;
    }

    this->handle_frame_();
    this->reset_frame_();
  }
}

void RA2413MTComponent::reset_frame_() {
  this->rx_buffer_.clear();
  this->frame_kind_ = FrameKind::NONE;
  this->frame_data_len_ = 0;
  this->frame_len_known_ = false;
}

bool RA2413MTComponent::match_prefix_(const uint8_t *prefix, size_t len) const {
  if (this->rx_buffer_.size() < len) {
    return false;
  }
  for (size_t i = 0; i < len; i++) {
    if (this->rx_buffer_[i] != prefix[i]) {
      return false;
    }
  }
  return true;
}

bool RA2413MTComponent::match_suffix_(const uint8_t *suffix, size_t len) const {
  if (this->rx_buffer_.size() < len) {
    return false;
  }
  const size_t start = this->rx_buffer_.size() - len;
  for (size_t i = 0; i < len; i++) {
    if (this->rx_buffer_[start + i] != suffix[i]) {
      return false;
    }
  }
  return true;
}

void RA2413MTComponent::handle_frame_() {
  if (this->frame_kind_ == FrameKind::DATA) {
    if (!this->match_prefix_(DATA_HEADER, 4) || !this->match_suffix_(DATA_FOOTER, 4)) {
      ESP_LOGV(TAG, "Dropping invalid data frame");
      return;
    }
    this->handle_data_frame_();
    return;
  }

  if (this->frame_kind_ == FrameKind::COMMAND) {
    if (!this->match_prefix_(CMD_HEADER, 4) || !this->match_suffix_(CMD_FOOTER, 4)) {
      ESP_LOGV(TAG, "Dropping invalid command frame");
      return;
    }
    this->handle_command_frame_();
  }
}

void RA2413MTComponent::handle_data_frame_() {
  // Inner payload starts at index 6, length = frame_data_len_
  if (this->frame_data_len_ < 11) {
    return;
  }

  const uint8_t *data = &this->rx_buffer_[6];
  const uint8_t data_type = data[0];
  if (data[1] != 0xAA) {
    return;
  }

  // Basic target block is always present for type 0x01/0x02.
  if (data_type != 0x01 && data_type != 0x02) {
    return;
  }

  const uint16_t move_distance = read_le16_(&data[3]);
  const uint8_t move_energy = data[5];
  const uint16_t static_distance = read_le16_(&data[6]);
  const uint8_t static_energy = data[8];

  // Always track peaks while auto-calibrating (ignore throttle).
  if (this->auto_sensitivity_active_) {
    if (move_energy > this->auto_move_peak_) {
      this->auto_move_peak_ = move_energy;
    }
    if (static_energy > this->auto_static_peak_) {
      this->auto_static_peak_ = static_energy;
    }
  }

  const uint32_t now = millis();
  if (now - this->last_data_millis_ < this->throttle_ms_) {
    return;
  }
  this->last_data_millis_ = now;

  if (this->move_distance_sensor_ != nullptr) {
    this->move_distance_sensor_->publish_state(move_distance);
  }
  if (this->move_energy_sensor_ != nullptr) {
    this->move_energy_sensor_->publish_state(move_energy);
  }
  if (this->static_distance_sensor_ != nullptr) {
    this->static_distance_sensor_->publish_state(static_distance);
  }
  if (this->static_energy_sensor_ != nullptr) {
    this->static_energy_sensor_->publish_state(static_energy);
  }
}

void RA2413MTComponent::handle_command_frame_() {
  if (this->frame_data_len_ < 2) {
    return;
  }

  const uint8_t *inner = &this->rx_buffer_[6];
  const uint16_t ack_cmd = read_le16_(inner);  // command | 0x0100

  if ((ack_cmd & 0x00FF) == 0x61) {
    // ACK payload after command word
    this->handle_query_ack_(inner + 2, this->frame_data_len_ - 2);
  }
}

void RA2413MTComponent::handle_query_ack_(const uint8_t *payload, size_t len) {
  // ACK status(2) + 0xAA + N + max_move + max_static + move[N+1] + static[N+1] + duration(2)
  if (len < 8) {
    ESP_LOGW(TAG, "Query ACK too short: %u", (unsigned) len);
    return;
  }

  const uint16_t status = read_le16_(payload);
  if (status != 0) {
    ESP_LOGW(TAG, "Query ACK failed, status=%u", status);
    return;
  }
  if (payload[2] != 0xAA) {
    ESP_LOGW(TAG, "Query ACK missing 0xAA marker");
    return;
  }

  const uint8_t max_gate_n = payload[3];
  if (max_gate_n < 1 || max_gate_n > MAX_GATE) {
    ESP_LOGW(TAG, "Unexpected max gate N=%u", max_gate_n);
    return;
  }

  const size_t sens_count = static_cast<size_t>(max_gate_n) + 1;
  const size_t expected = 2 + 1 + 1 + 1 + 1 + sens_count + sens_count + 2;
  if (len < expected) {
    ESP_LOGW(TAG, "Query ACK length mismatch: got %u expected >= %u (N=%u)", (unsigned) len, (unsigned) expected,
             max_gate_n);
    return;
  }

  const uint8_t max_move_gate = payload[4];
  const uint8_t max_static_gate = payload[5];
  const uint8_t move_sensitivity = payload[6];  // gate 0 as representative (all-gate writes use 0xFFFF)
  const uint8_t static_sensitivity = payload[6 + sens_count];
  const uint16_t duration = read_le16_(&payload[6 + 2 * sens_count]);

  ESP_LOGD(TAG, "Config: move_gate=%u static_gate=%u move_sens=%u static_sens=%u duration=%us N=%u", max_move_gate,
           max_static_gate, move_sensitivity, static_sensitivity, duration, max_gate_n);

  if (this->max_move_distance_number_ != nullptr) {
    this->max_move_distance_number_->publish_state(gate_to_meters_(max_move_gate));
  }
  if (this->max_static_distance_number_ != nullptr) {
    this->max_static_distance_number_->publish_state(gate_to_meters_(max_static_gate));
  }
  if (this->move_sensitivity_number_ != nullptr) {
    this->move_sensitivity_number_->publish_state(move_sensitivity);
  }
  if (this->static_sensitivity_number_ != nullptr) {
    this->static_sensitivity_number_->publish_state(static_sensitivity);
  }
  if (this->unattended_duration_number_ != nullptr) {
    this->unattended_duration_number_->publish_state(duration);
  }
}

void RA2413MTComponent::enqueue_command_(std::vector<uint8_t> payload) {
  this->command_queue_.push_back(std::move(payload));
  this->pump_command_queue_();
}

void RA2413MTComponent::pump_command_queue_() {
  if (this->command_busy_ || this->command_queue_.empty()) {
    return;
  }

  this->command_busy_ = true;
  const std::vector<uint8_t> payload = this->command_queue_.front();
  this->command_queue_.pop_front();

  // enable config -> wait -> command -> wait -> disable config -> wait -> next
  this->send_enable_config_();
  this->set_timeout("ra2413mt_cmd", COMMAND_GAP_MS, [this, payload]() {
    this->write_inner_frame_(payload);
    this->set_timeout("ra2413mt_end", COMMAND_GAP_MS, [this]() {
      this->send_disable_config_();
      this->set_timeout("ra2413mt_next", COMMAND_GAP_MS, [this]() {
        this->command_busy_ = false;
        this->pump_command_queue_();
      });
    });
  });
}

void RA2413MTComponent::write_raw_(const uint8_t *data, size_t len) { this->write_array(data, len); }

void RA2413MTComponent::write_inner_frame_(const std::vector<uint8_t> &inner) {
  std::vector<uint8_t> frame;
  frame.reserve(4 + 2 + inner.size() + 4);
  frame.insert(frame.end(), CMD_HEADER, CMD_HEADER + 4);
  uint8_t len_bytes[2];
  write_le16_(len_bytes, static_cast<uint16_t>(inner.size()));
  frame.push_back(len_bytes[0]);
  frame.push_back(len_bytes[1]);
  frame.insert(frame.end(), inner.begin(), inner.end());
  frame.insert(frame.end(), CMD_FOOTER, CMD_FOOTER + 4);
  this->write_raw_(frame.data(), frame.size());
}

void RA2413MTComponent::write_framed_(const uint8_t *cmd, size_t cmd_len, const uint8_t *value, size_t value_len) {
  std::vector<uint8_t> inner;
  inner.reserve(cmd_len + value_len);
  inner.insert(inner.end(), cmd, cmd + cmd_len);
  if (value != nullptr && value_len > 0) {
    inner.insert(inner.end(), value, value + value_len);
  }
  this->write_inner_frame_(inner);
}

void RA2413MTComponent::send_enable_config_() {
  const uint8_t value[2] = {0x01, 0x00};
  this->write_framed_(CMD_ENABLE_CONFIG, sizeof(CMD_ENABLE_CONFIG), value, sizeof(value));
}

void RA2413MTComponent::send_disable_config_() {
  this->write_framed_(CMD_DISABLE_CONFIG, sizeof(CMD_DISABLE_CONFIG), nullptr, 0);
}

void RA2413MTComponent::send_set_distance_(uint8_t max_move_gate, uint8_t max_static_gate, uint16_t duration_s) {
  max_move_gate = clamp(max_move_gate, MIN_GATE, MAX_GATE);
  max_static_gate = clamp(max_static_gate, MIN_GATE, MAX_GATE);

  // command word is added by write_framed_; queue stores full inner payload including cmd
  std::vector<uint8_t> inner;
  inner.reserve(2 + 18);
  inner.push_back(CMD_SET_DISTANCE[0]);
  inner.push_back(CMD_SET_DISTANCE[1]);

  // param 0: max move gate
  uint8_t chunk[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  write_le16_(chunk, 0x0000);
  write_le32_(chunk + 2, max_move_gate);
  inner.insert(inner.end(), chunk, chunk + 6);

  // param 1: max static gate
  write_le16_(chunk, 0x0001);
  write_le32_(chunk + 2, max_static_gate);
  inner.insert(inner.end(), chunk, chunk + 6);

  // param 2: unattended duration
  write_le16_(chunk, 0x0002);
  write_le32_(chunk + 2, duration_s);
  inner.insert(inner.end(), chunk, chunk + 6);

  this->enqueue_command_(std::move(inner));
}

void RA2413MTComponent::send_set_sensitivity_(uint16_t move_sensitivity, uint16_t static_sensitivity) {
  move_sensitivity = clamp(move_sensitivity, (uint16_t) 0, (uint16_t) 100);
  static_sensitivity = clamp(static_sensitivity, (uint16_t) 0, (uint16_t) 100);

  std::vector<uint8_t> inner;
  inner.reserve(2 + 18);
  inner.push_back(CMD_SET_SENSITIVITY[0]);
  inner.push_back(CMD_SET_SENSITIVITY[1]);

  uint8_t chunk[6] = {};

  // all gates: 0xFFFF
  write_le16_(chunk, 0x0000);
  write_le32_(chunk + 2, 0x0000FFFF);
  inner.insert(inner.end(), chunk, chunk + 6);

  write_le16_(chunk, 0x0001);
  write_le32_(chunk + 2, move_sensitivity);
  inner.insert(inner.end(), chunk, chunk + 6);

  write_le16_(chunk, 0x0002);
  write_le32_(chunk + 2, static_sensitivity);
  inner.insert(inner.end(), chunk, chunk + 6);

  this->enqueue_command_(std::move(inner));
}

uint8_t RA2413MTComponent::meters_to_gate_(float meters) {
  int gate = static_cast<int>(lroundf(meters / GATE_SIZE_M));
  return static_cast<uint8_t>(clamp(gate, (int) MIN_GATE, (int) MAX_GATE));
}

float RA2413MTComponent::gate_to_meters_(uint8_t gate) { return static_cast<float>(gate) * GATE_SIZE_M; }

uint16_t RA2413MTComponent::read_le16_(const uint8_t *data) {
  return static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
}

void RA2413MTComponent::write_le16_(uint8_t *data, uint16_t value) {
  data[0] = value & 0xFF;
  data[1] = (value >> 8) & 0xFF;
}

void RA2413MTComponent::write_le32_(uint8_t *data, uint32_t value) {
  data[0] = value & 0xFF;
  data[1] = (value >> 8) & 0xFF;
  data[2] = (value >> 16) & 0xFF;
  data[3] = (value >> 24) & 0xFF;
}

}  // namespace ra2413mt
}  // namespace esphome
