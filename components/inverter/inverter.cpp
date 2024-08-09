#include "inverter.h"
#include "esphome/core/log.h"

static const char *TAG = "inverter";

namespace esphome {
namespace inverter {

void Inverter::setup() {}
void Inverter::loop() {
     if (this->device_protocol_id_) {
          this->device_protocol_id_->publish_state(30);
     }
}
void Inverter::update() {}
void Inverter::dump_config() {
     ESP_LOGCONFIG(TAG, "Inverter component");
}

}
}