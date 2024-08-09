#include "inverter.h"
#include "esphome/core/log.h"

static const char *TAG = "inverter";

namespace esphome {
namespace inverter {

void Inverter::setup() {}
void Inverter::loop() {
     if (this->device_protocol_id_) {
          char* substr = ['P','I','3','0'];
          this->device_protocol_id_->publish_state(substr);
     }
}
void Inverter::update() {}
void Inverter::dump_config() {
     ESP_LOGCONFIG(TAG, "Inverter component");
}

}
}