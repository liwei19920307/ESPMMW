#include "ra2413mt_button.h"

namespace esphome {
namespace ra2413mt {

void ResetConfButton::press_action() { this->parent_->reset_config(); }

void AutoSensitivityButton::press_action() { this->parent_->start_auto_sensitivity(); }

}  // namespace ra2413mt
}  // namespace esphome
