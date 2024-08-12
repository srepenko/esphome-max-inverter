#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/select/select.h"
//#include "esphome/components/pipsolar/select/pipsolar_select.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/time.h"


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
  uint32_t interval;
  uint32_t last_run;
  uint8_t length;
  uint8_t errors;
//  uint8_t identifier;
//  ENUMPollingCommand identifier;
};



#define INVERTER_ENTITY_(type, name, polling_command) \
    protected: \
        type *name##_{}; /* NOLINT */ \
\
    public: \
        void set_##name(type *name) { /* NOLINT */ \
            this->name##_ = name; \
            this->add_polling_command_(#polling_command, POLLING_##polling_command); \
        }

#define INVERTER_VALUED_ENTITY_(type, name, polling_command, value_type) \
    protected: \
        value_type value_##name##_; \
        INVERTER_ENTITY_(type, name, polling_command)

#define INVERTER_SENSOR(name, polling_command, value_type) \
    INVERTER_VALUED_ENTITY_(sensor::Sensor, name, polling_command, value_type)

class Inverter : public uart::UARTDevice, public PollingComponent {
    // QPI values
    INVERTER_SENSOR(device_protocol_id, QPI, int)
    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;

    protected:
        static const size_t READ_BUFFER_LENGTH = 200;  // maximum supported answer length
        static const size_t COMMAND_QUEUE_LENGTH = 10;
        static const size_t COMMAND_TIMEOUT = 1000;
        void add_polling_command_(const char *command, ENUMPollingCommand polling_command);
        void empty_uart_buffer_();
        uint8_t check_incoming_crc_();
        uint8_t check_incoming_length_(uint8_t length);
        uint16_t cal_crc_half_(uint8_t *msg, uint8_t len);
        uint8_t send_next_command_();
        void queue_command_(const char *command, uint8_t length);
        std::string command_queue_[COMMAND_QUEUE_LENGTH];
        uint8_t command_queue_position_ = 0;
        uint8_t read_buffer_[READ_BUFFER_LENGTH];
        size_t read_pos_{0};  
        uint32_t command_start_millis_ = 0;
        uint8_t state_;
        enum State {
            STATE_IDLE = 0,
            STATE_POLL = 1,
            STATE_COMMAND = 2,
            STATE_POLL_COMPLETE = 3,
            STATE_COMMAND_COMPLETE = 4,
            STATE_POLL_CHECKED = 5,
            STATE_POLL_DECODED = 6,
        };

        PollingCommand used_polling_commands_[15];
        struct PollingCommand MAX_commands[33] = {
          {(uint8_t*)"QPI", 0, 0, 3, 0},                 // Device Protocol ID
          {(uint8_t*)"QID", 0, 0, 3, 0},                 // The device serial number
          {(uint8_t*)"QSID", 0, 0, 4, 0},                // The device serial number (the length is more than 14)
          {(uint8_t*)"QVFW", 0, 0, 4, 0},                // Main CPU Firmware version
          {(uint8_t*)"QVFW3", 0, 0, 5, 0},               // Another CPU (remote panel) Firmware version
          {(uint8_t*)"VERFW", 0, 0, 4, 0},               // Bluetooth version
          {(uint8_t*)"QPIRI", 0, 0, 5, 0},               // Device Rating Information
          {(uint8_t*)"QFLAG", 0, 0, 5, 0},               // Device flag status
          {(uint8_t*)"QPIGS", 1, 0, 5, 0},               // Device general status parameters
          {(uint8_t*)"QPIGS2", 0, 0, 6, 0},              // Device general status parameters inquiry (Only 48V model)
          {(uint8_t*)"QPGSn", 0, 0, 5, 0},               // Parallel Information inquiry (Only 48V model)
          {(uint8_t*)"QMOD", 5, 0, 4, 0},                // Device Mode
          {(uint8_t*)"QPIWS", 0, 0, 5, 0},              // Device Warning Status
          {(uint8_t*)"QDI", 0, 0, 3, 0},                // The default setting value
          {(uint8_t*)"QMCHGCR", 0, 0, 7, 0},            // Enquiry selectable value about max charging current
          {(uint8_t*)"QMUCHGCR", 0, 0, 8, 0},           // Enquiry selectable value about max utility charging current
          {(uint8_t*)"QOPPT", 0, 0, 5, 0},              // The device output source priority time order
          {(uint8_t*)"QCHPT", 0, 0, 5, 0},              // The device charger source priority time order
          {(uint8_t*)"QT", 3600, 0, 2, 0},              // Time
          {(uint8_t*)"QBEQI", 0, 0, 5, 0},              // Battery equalization status parameters
          {(uint8_t*)"QMN", 0, 0, 3, 0},                // Query model name
          {(uint8_t*)"QGMN", 0, 0, 4, 0},               // Query general model name
          {(uint8_t*)"QET", 0, 0, 3, 0},                // Query total PV generated energy
          {(uint8_t*)"QEYyyyy", 0, 0, 7, 0},            // Query PV generated energy of year
          {(uint8_t*)"QEMyyyymm", 0, 0, 9, 0},          // Query PV generated energy of month
          {(uint8_t*)"QEDyyyymmdd", 3600, 0, 11, 0},    // Query PV generated energy of day
          {(uint8_t*)"QLT", 0, 0, 3, 0},                // Query total output load energy
          {(uint8_t*)"QLYyyyy", 0, 0, 7, 0},            // Query output load energy of year
          {(uint8_t*)"QLMyyyymm", 0, 0, 9, 0},          // Query output load energy of month
          {(uint8_t*)"QLDyyyymmdd", 3600, 0, 11, 0},    // Query output load energy of day
          {(uint8_t*)"QBMS", 0, 0, 4, 0},               // BMS message
          {(uint8_t*)"PBMS", 0, 0, 4, 0},               // BMS message
          {(uint8_t*)"QLED", 0, 0, 4, 0},               // LED status parameters
     };
};
}
}