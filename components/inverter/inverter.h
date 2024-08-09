//#include "esphome/components/binary_sensor/binary_sensor.h"
//#include "esphome/components/sensor/sensor.h"
//#include "esphome/components/switch/switch.h"
//#include "esphome/components/select/select.h"
//#include "esphome/components/pipsolar/select/pipsolar_select.h"
//#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace inverter {

class Inverter : public uart::UARTDevice, public PollingComponent {
  void setup() override;
  void loop() override;
  void update() override;
};
}
}