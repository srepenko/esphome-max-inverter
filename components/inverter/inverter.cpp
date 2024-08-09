#include "inverter.h"
#include "esphome/core/log.h"

static const char *TAG = "inverter";

namespace esphome {
namespace inverter {

void Inverter::setup() {}
void Inverter::loop() {}
void Inverter::update() {}
void Inverter::dump_config() {
     ESP_LOGCONFIG(TAG, "Inverter component");
}

}
}