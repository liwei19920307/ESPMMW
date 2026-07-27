#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "../ra2413mt.h"

namespace esphome {
namespace ra2413mt {

class MaxMoveDistanceNumber : public number::Number, public Parented<RA2413MTComponent> {
 protected:
  void control(float value) override;
};

class MaxStaticDistanceNumber : public number::Number, public Parented<RA2413MTComponent> {
 protected:
  void control(float value) override;
};

class MoveSensitivityNumber : public number::Number, public Parented<RA2413MTComponent> {
 protected:
  void control(float value) override;
};

class StaticSensitivityNumber : public number::Number, public Parented<RA2413MTComponent> {
 protected:
  void control(float value) override;
};

class UnattendedDurationNumber : public number::Number, public Parented<RA2413MTComponent> {
 protected:
  void control(float value) override;
};

}  // namespace ra2413mt
}  // namespace esphome
