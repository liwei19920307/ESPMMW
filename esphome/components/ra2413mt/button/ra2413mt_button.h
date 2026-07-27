#pragma once

#include "esphome/components/button/button.h"
#include "../ra2413mt.h"

namespace esphome {
namespace ra2413mt {

class ResetConfButton : public button::Button, public Parented<RA2413MTComponent> {
 protected:
  void press_action() override;
};

class AutoSensitivityButton : public button::Button, public Parented<RA2413MTComponent> {
 protected:
  void press_action() override;
};

}  // namespace ra2413mt
}  // namespace esphome
