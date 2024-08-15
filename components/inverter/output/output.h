#pragma once

#include "../inverter.h"
#include "esphome/components/output/float_output.h"
#include "esphome/core/component.h"

namespace esphome {
namespace inverter {

class Inverter;

class InverterOutput : public output::FloatOutput {
 public:
  InverterOutput() {}
  void set_parent(Inverter *parent) { this->parent_ = parent; }
  void set_set_command(const std::string &command) { this->set_command_ = command; };
  void set_possible_values(std::vector<float> possible_values) { this->possible_values_ = std::move(possible_values); }
  void set_value(float value) { this->write_state(value); };

 protected:
  void write_state(float state) override;
  std::string set_command_;
  Inverter *parent_;
  std::vector<float> possible_values_;
};

template<typename... Ts> class SetOutputAction : public Action<Ts...> {
 public:
  SetOutputAction(InverterOutput *output) : output_(output) {}

  TEMPLATABLE_VALUE(float, level)

  void play(Ts... x) override { this->output_->set_value(this->level_.value(x...)); }

 protected:
  InverterOutput *output_;
};

}  // namespace inverter
}  // namespace esphome
