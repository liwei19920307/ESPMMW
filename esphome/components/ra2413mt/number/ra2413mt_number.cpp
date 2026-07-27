#include "ra2413mt_number.h"

namespace esphome {
namespace ra2413mt {

void MaxMoveDistanceNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_max_move_distance(value);
}

void MaxStaticDistanceNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_max_static_distance(value);
}

void MoveSensitivityNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_move_sensitivity(value);
}

void StaticSensitivityNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_static_sensitivity(value);
}

void UnattendedDurationNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_unattended_duration(value);
}

}  // namespace ra2413mt
}  // namespace esphome
