#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/select/select.h"
//#include "esphome/components/pipsolar/select/pipsolar_select.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace inverter {
enum ENUMPollingCommand {
  POLLING_QPIRI = 0,
  POLLING_QPIGS = 1,
  POLLING_QMOD = 2,
  POLLING_QFLAG = 3,
  POLLING_QPIWS = 4,
  POLLING_QT = 5,
  POLLING_QMN = 6,
  POLLING_QBATCD = 7,
  POLLING_QPI = 8,
};
struct PollingCommand {
  uint8_t *command;
  uint8_t length = 0;
  uint8_t errors;
  ENUMPollingCommand identifier;
};
#define PIPSOLAR_ENTITY_(type, name, polling_command) \
    protected: \
        type *name##_{}; /* NOLINT */ \
\
    public: \
        void set_##name(type *name) { /* NOLINT */ \
            this->name##_ = name; \
            this->add_polling_command_(#polling_command, POLLING_##polling_command); \
        }

#define PIPSOLAR_VALUED_ENTITY_(type, name, polling_command, value_type) \
    protected: \
        value_type value_##name##_; \
        PIPSOLAR_ENTITY_(type, name, polling_command)

#define PIPSOLAR_SENSOR(name, polling_command, value_type) \
    PIPSOLAR_VALUED_ENTITY_(sensor::Sensor, name, polling_command, value_type)

class Inverter : public uart::UARTDevice, public PollingComponent {
    // QPI values
    //PIPSOLAR_SENSOR(device_protocol_id, QPI, char*)
    protected:
        char* value_device_protocol_id_; 
        sensor::Sensor *device_protocol_id_{};
    public:
        void set_device_protocol_id(sensor::Sensor *device_protocol_id) { 
            //this->device_protocol_id_ = device_protocol_id; 
        }
    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;
};
}
}