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
struct PollingCommand {
    uint8_t *command;
    uint32_t interval;
    uint32_t last_run;
    uint8_t length;   
    uint8_t errors;
    bool state;
};



#define INVERTER_ENTITY_(type, name, polling_command) \
    protected: \
        type *name##_{}; /* NOLINT */ \
\
    public: \
        void set_##name(type *name) { /* NOLINT */ \
            this->name##_ = name; \
            this->add_polling_command_(#polling_command); \
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
    // QPIRI values
    INVERTER_SENSOR(grid_rating_voltage, QPIRI, int)
    INVERTER_SENSOR(grid_rating_current, QPIRI, int)
    // QPIGS values
    INVERTER_SENSOR(grid_voltage, QPIGS, int)
    INVERTER_SENSOR(grid_frequency, QPIGS, int)

    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;

    protected:
        static const size_t READ_BUFFER_LENGTH = 200;  // maximum supported answer length
        static const size_t COMMAND_QUEUE_LENGTH = 10;
        static const size_t COMMAND_TIMEOUT = 2000;
        uint32_t last_poll_ = 0;
        void add_polling_command_(const char *command);
        void empty_uart_buffer_();
        uint8_t check_incoming_crc_();
        uint8_t check_incoming_length_(uint8_t length);
        uint16_t cal_crc_half_(uint8_t *msg, uint8_t len);
        uint8_t send_next_command_();
        void send_next_poll_();
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

        uint8_t last_polling_command_ = 0;
        PollingCommand used_polling_commands_[15];
        struct PollingCommand MAX_commands[33] = {
        //                              interval last_run   length  errors  status
          {(uint8_t*)"QPI__\r",         86400,   0,         3,      0,      0},     // Device Protocol ID
          {(uint8_t*)"QID__\r",         0,       0,         3,      0,      0},     // The device serial number
          {(uint8_t*)"QSID__\r",        0,       0,         4,      0,      0},     // The device serial number (the length is more than 14)
          {(uint8_t*)"QVFW__\r",        0,       0,         4,      0,      0},     // Main CPU Firmware version
          {(uint8_t*)"QVFW3__\r",       0,       0,         5,      0,      0},     // Another CPU (remote panel) Firmware version
          {(uint8_t*)"VERFW__\r",       0,       0,         4,      0,      0},     // Bluetooth version
          {(uint8_t*)"QPIRI__\r",       10,      0,         5,      0,      0},     // Device Rating Information
          {(uint8_t*)"QFLAG__\r",       5,       0,         5,      0,      0},     // Device flag status
          {(uint8_t*)"QPIGS__\r",       1,       0,         5,      0,      0},     // Device general status parameters
          {(uint8_t*)"QPIGS2__\r",      0,       0,         6,      0,      0},     // Device general status parameters inquiry (Only 48V model)
          {(uint8_t*)"QPGSn__\r",       0,       0,         5,      0,      0},     // Parallel Information inquiry (Only 48V model)
          {(uint8_t*)"QMOD__\r",        0,       0,         4,      0,      0},     // Device Mode
          {(uint8_t*)"QPIWS__\r",       0,       0,         5,      0,      0},     // Device Warning Status
          {(uint8_t*)"QDI__\r",         0,       0,         3,      0,      0},     // The default setting value
          {(uint8_t*)"QMCHGCR__\r",     0,       0,         7,      0,      0},     // Enquiry selectable value about max charging current
          {(uint8_t*)"QMUCHGCR__\r",    0,       0,         8,      0,      0},     // Enquiry selectable value about max utility charging current
          {(uint8_t*)"QOPPT__\r",       0,       0,         5,      0,      0},     // The device output source priority time order
          {(uint8_t*)"QCHPT__\r",       0,       0,         5,      0,      0},     // The device charger source priority time order
          {(uint8_t*)"QT__\r",          3600,    0,         2,      0,      0},     // Time
          {(uint8_t*)"QBEQI__\r",       0,       0,         5,      0,      0},     // Battery equalization status parameters
          {(uint8_t*)"QMN__\r",         0,       0,         3,      0,      0},     // Model name
          {(uint8_t*)"QGMN__\r",        0,       0,         4,      0,      0},     // General model name
          {(uint8_t*)"QET__\r",         3600,    0,         3,      0,      0},     // Total PV generated energy
          {(uint8_t*)"QEYyyyy__\r",     3600,    0,         7,      0,      0},     // PV generated energy of year
          {(uint8_t*)"QEMyyyymm__\r",   3600,    0,         9,      0,      0},     // PV generated energy of month
          {(uint8_t*)"QEDyyyymmdd__\r", 3600,    0,         11,     0,      0},     // PV generated energy of day
          {(uint8_t*)"QLT__\r",         3600,    0,         3,      0,      0},     // Total output load energy
          {(uint8_t*)"QLYyyyy__\r",     3600,    0,         7,      0,      0},     // Output load energy of year
          {(uint8_t*)"QLMyyyymm__\r",   3600,    0,         9,      0,      0},     // Output load energy of month
          {(uint8_t*)"QLDyyyymmdd__\r", 3600,    0,         11,     0,      0},     // Output load energy of day
          {(uint8_t*)"QBMS__\r",        0,       0,         4,      0,      0},     // BMS message
          {(uint8_t*)"PBMS__\r",        0,       0,         4,      0,      0},     // BMS message
          {(uint8_t*)"QLED__\r",        0,       0,         4,      0,      0},     // LED status parameters
     };
};
}
}