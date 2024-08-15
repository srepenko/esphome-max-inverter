#include "esphome/components/switch/"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace inverter {

static const char *const TAG = "inverter.switch";

void InverterSwitch::dump_config() { LOG_SWITCH("", "Inverter Switch", this); }
void InverterSwitch::write_state(bool state) {
  if (state) {
    if (this->on_command_.length() > 0) {
      this->parent_->switch_command(this->on_command_);
    }
  } else {
    if (this->off_command_.length() > 0) {
      this->parent_->switch_command(this->off_command_);
    }
  }
}

}
}
