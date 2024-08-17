#pragma once
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/select/select.h"
#include "./select/select.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/time.h"
#include "esphome/components/time/real_time_clock.h"


namespace esphome {
namespace inverter {

class InverterSelect;
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
#define INVERTER_SWITCH(name, polling_command) INVERTER_ENTITY_(switch_::Switch, name, polling_command)
#define INVERTER_SELECT(name, polling_command) INVERTER_ENTITY_(inverter::InverterSelect, name, polling_command)
#define INVERTER_VALUED_SELECT(name, polling_command, value_type) \
  INVERTER_VALUED_ENTITY_(inverter::InverterSelect, name, polling_command, value_type)
#define INVERTER_BINARY_SENSOR(name, polling_command, value_type) \
    INVERTER_VALUED_ENTITY_(binary_sensor::BinarySensor, name, polling_command, value_type)
#define INVERTER_VALUED_TEXT_SENSOR(name, polling_command, value_type) \
  INVERTER_VALUED_ENTITY_(text_sensor::TextSensor, name, polling_command, value_type)
#define INVERTER_TEXT_SENSOR(name, polling_command) INVERTER_ENTITY_(text_sensor::TextSensor, name, polling_command)

class Inverter : public uart::UARTDevice, public PollingComponent {
    // QPIGS values
    INVERTER_SENSOR(grid_voltage, QPIGS, float)
    INVERTER_SENSOR(grid_frequency, QPIGS, float)
    INVERTER_SENSOR(ac_output_voltage, QPIGS, float)
    INVERTER_SENSOR(ac_output_frequency, QPIGS, float)
    INVERTER_SENSOR(ac_output_apparent_power, QPIGS, int)
    INVERTER_SENSOR(ac_output_active_power, QPIGS, int)
    INVERTER_SENSOR(output_load_percent, QPIGS, int)
    INVERTER_SENSOR(bus_voltage, QPIGS, int)
    INVERTER_SENSOR(battery_voltage, QPIGS, float)
    INVERTER_SENSOR(battery_charging_current, QPIGS, int)
    INVERTER_SENSOR(battery_capacity_percent, QPIGS, int)
    INVERTER_SENSOR(inverter_heat_sink_temperature, QPIGS, int)
    INVERTER_SENSOR(pv_input_current, QPIGS, float)
    INVERTER_SENSOR(pv_input_voltage, QPIGS, float)
    INVERTER_SENSOR(battery_voltage_scc, QPIGS, float)
    INVERTER_SENSOR(battery_discharge_current, QPIGS, int)

    INVERTER_BINARY_SENSOR(add_sbu_priority_version, QPIGS, int)
    INVERTER_BINARY_SENSOR(configuration_status, QPIGS, int)
    INVERTER_BINARY_SENSOR(scc_firmware_version, QPIGS, int)
    INVERTER_BINARY_SENSOR(load_status, QPIGS, int)
    INVERTER_BINARY_SENSOR(battery_voltage_to_steady_while_charging, QPIGS, int)
    INVERTER_BINARY_SENSOR(charging_status, QPIGS, int)
    INVERTER_BINARY_SENSOR(scc_charging_status, QPIGS, int)
    INVERTER_BINARY_SENSOR(ac_charging_status, QPIGS, int)

    INVERTER_SENSOR(battery_voltage_offset_for_fans_on, QPIGS, int)  //.1 scale
    INVERTER_SENSOR(eeprom_version, QPIGS, int)
    INVERTER_SENSOR(pv_charging_power, QPIGS, int)

    INVERTER_BINARY_SENSOR(charging_to_floating_mode, QPIGS, int)
    INVERTER_BINARY_SENSOR(switch_on, QPIGS, int)
    INVERTER_BINARY_SENSOR(dustproof_installed, QPIGS, int)

    // QPIRI values
    INVERTER_SENSOR(grid_rating_voltage, QPIRI, float)
    INVERTER_SENSOR(grid_rating_current, QPIRI, float)
    INVERTER_SENSOR(ac_output_rating_voltage, QPIRI, float)
    INVERTER_SENSOR(ac_output_rating_frequency, QPIRI, float)
    INVERTER_SENSOR(ac_output_rating_current, QPIRI, float)
    INVERTER_SENSOR(ac_output_rating_apparent_power, QPIRI, int)
    INVERTER_SENSOR(ac_output_rating_active_power, QPIRI, int)
    INVERTER_SENSOR(battery_rating_voltage, QPIRI, float)
    INVERTER_SENSOR(battery_recharge_voltage, QPIRI, float)
    INVERTER_SENSOR(battery_under_voltage, QPIRI, float)
    INVERTER_SENSOR(battery_bulk_voltage, QPIRI, float)
    INVERTER_SENSOR(battery_float_voltage, QPIRI, float)
    INVERTER_SENSOR(battery_type, QPIRI, int)
    INVERTER_SENSOR(current_max_ac_charging_current, QPIRI, int)
    INVERTER_SENSOR(current_max_charging_current, QPIRI, int)
    INVERTER_SENSOR(input_voltage_range, QPIRI, int)
    INVERTER_SENSOR(output_source_priority, QPIRI, int)
    INVERTER_SENSOR(charger_source_priority, QPIRI, int)
    INVERTER_SENSOR(parallel_max_num, QPIRI, int)
    INVERTER_SENSOR(machine_type, QPIRI, int)
    INVERTER_SENSOR(topology, QPIRI, int)
    INVERTER_SENSOR(output_mode, QPIRI, int)
    INVERTER_SENSOR(battery_redischarge_voltage, QPIRI, float)
    INVERTER_SENSOR(pv_ok_condition_for_parallel, QPIRI, int)
    INVERTER_SENSOR(pv_power_balance, QPIRI, int)

    //QET
    INVERTER_SENSOR(total_pv_generated_energy, QET, float)
    //QEY
    INVERTER_SENSOR(year_pv_generated_energy, QEY, float)
    //QEM
    INVERTER_SENSOR(month_pv_generated_energy, QEM, float)
    //QED
    INVERTER_SENSOR(day_pv_generated_energy, QED, float)
    //QLT
    INVERTER_SENSOR(total_output_load_energy, QLT, float)
    //QLY
    INVERTER_SENSOR(year_output_load_energy, QLY, float)
    //QLM
    INVERTER_SENSOR(month_output_load_energy, QLM, float)
    //QLD
    INVERTER_SENSOR(day_output_load_energy, QLD, float)

    // QMOD values
//    INVERTER_VALUED_TEXT_SENSOR(device_mode, QMOD, char)

    // QPI values
//    INVERTER_VALUED_TEXT_SENSOR(device_protocol_id, QPI, char*)
/*
    // QT values
    INVERTER_SENSOR(device_time, QT, int)

    // QFLAG values
    INVERTER_BINARY_SENSOR(silence_buzzer_open_buzzer, QFLAG, int)
    INVERTER_BINARY_SENSOR(overload_bypass_function, QFLAG, int)
    INVERTER_BINARY_SENSOR(lcd_escape_to_default, QFLAG, int)
    INVERTER_BINARY_SENSOR(overload_restart_function, QFLAG, int)
    INVERTER_BINARY_SENSOR(over_temperature_restart_function, QFLAG, int)
    INVERTER_BINARY_SENSOR(backlight_on, QFLAG, int)
    INVERTER_BINARY_SENSOR(alarm_on_when_primary_source_interrupt, QFLAG, int)
    INVERTER_BINARY_SENSOR(fault_code_record, QFLAG, int)
    INVERTER_BINARY_SENSOR(power_saving, QFLAG, int)

    // QPIWS values
    INVERTER_BINARY_SENSOR(warnings_present, QPIWS, bool)
    INVERTER_BINARY_SENSOR(faults_present, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_power_loss, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_inverter_fault, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_bus_over, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_bus_under, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_bus_soft_fail, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_line_fail, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_opvshort, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_inverter_voltage_too_low, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_inverter_voltage_too_high, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_over_temperature, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_fan_lock, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_battery_voltage_high, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_battery_low_alarm, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_battery_under_shutdown, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_battery_derating, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_over_load, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_eeprom_failed, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_inverter_over_current, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_inverter_soft_failed, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_self_test_failed, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_op_dc_voltage_over, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_battery_open, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_current_sensor_failed, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_battery_short, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_power_limit, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_pv_voltage_high, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_mppt_overload, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_mppt_overload, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_battery_too_low_to_charge, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_dc_dc_over_current, QPIWS, bool)
    INVERTER_BINARY_SENSOR(fault_code, QPIWS, int)
    INVERTER_BINARY_SENSOR(warnung_low_pv_energy, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_high_ac_input_during_bus_soft_start, QPIWS, bool)
    INVERTER_BINARY_SENSOR(warning_battery_equalization, QPIWS, bool)

    // QBATCD values
    INVERTER_BINARY_SENSOR(discharge_onoff, QBATCD, bool)
    INVERTER_BINARY_SENSOR(discharge_with_standby_onoff, QBATCD, bool)
    INVERTER_BINARY_SENSOR(charge_onoff, QBATCD, bool)

    INVERTER_TEXT_SENSOR(last_qpigs, QPIGS)
    INVERTER_TEXT_SENSOR(last_qpiri, QPIRI)
    INVERTER_TEXT_SENSOR(last_qmod, QMOD)
    INVERTER_TEXT_SENSOR(last_qflag, QFLAG)
    INVERTER_TEXT_SENSOR(last_qpiws, QPIWS)
    INVERTER_TEXT_SENSOR(last_qt, QT)
    INVERTER_TEXT_SENSOR(last_qpi, QPI)
    INVERTER_TEXT_SENSOR(last_qmn, QMN)
    INVERTER_TEXT_SENSOR(last_qbatcd, QBATCD)

    INVERTER_SWITCH(output_source_priority_utility_switch, QPIRI)
    INVERTER_SWITCH(output_source_priority_solar_switch, QPIRI)
    INVERTER_SWITCH(output_source_priority_battery_switch, QPIRI)
*/
    INVERTER_SWITCH(input_voltage_range_switch, QPIRI)
/*
    INVERTER_SWITCH(pv_ok_condition_for_parallel_switch, QPIRI)
    INVERTER_SWITCH(pv_power_balance_switch, QPIRI)
*/
    INVERTER_SELECT(output_source_priority_select, QPIRI)
    INVERTER_VALUED_SELECT(charging_discharging_control_select, QBATCD, std::string)

    void switch_command(const std::string &command);
    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;

    protected:
        friend class InverterSelect;
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
        time::RealTimeClock *time_;
        uint8_t last_polling_command_ = 0;
        struct PollingCommand MAX_commands[33] = {
        //                       interval last_run   length  errors  status
          {(uint8_t*)"QPI",      86400,   0,         3,      0,      0},     // Device Protocol ID
          {(uint8_t*)"QID",      86400,   0,         3,      0,      0},     // The device serial number
          {(uint8_t*)"QSID",     86400,   0,         4,      0,      0},     // The device serial number (the length is more than 14)
          {(uint8_t*)"QVFW",     86400,   0,         4,      0,      0},     // Main CPU Firmware version
          {(uint8_t*)"QVFW3",    86400,   0,         5,      0,      0},     // Another CPU (remote panel) Firmware version
          {(uint8_t*)"VERFW",    86400,   0,         4,      0,      0},     // Bluetooth version
          {(uint8_t*)"QPIRI",    10,      0,         5,      0,      0},     // Device Rating Information
          {(uint8_t*)"QFLAG",    5,       0,         5,      0,      0},     // Device flag status
          {(uint8_t*)"QPIGS",    1,       0,         5,      0,      0},     // Device general status parameters
          {(uint8_t*)"QPIGS2",   0,       0,         6,      0,      0},     // Device general status parameters inquiry (Only 48V model)
          {(uint8_t*)"QPGSn",    0,       0,         5,      0,      0},     // Parallel Information inquiry (Only 48V model)
          {(uint8_t*)"QMOD",     5,       0,         4,      0,      0},     // Device Mode
          {(uint8_t*)"QPIWS",    5,       0,         5,      0,      0},     // Device Warning Status
          {(uint8_t*)"QDI",      86400,   0,         3,      0,      0},     // The default setting value
          {(uint8_t*)"QMCHGCR",  10,      0,         7,      0,      0},     // Enquiry selectable value about max charging current
          {(uint8_t*)"QMUCHGCR", 10,      0,         8,      0,      0},     // Enquiry selectable value about max utility charging current
          {(uint8_t*)"QOPPT",    10,      0,         5,      0,      0},     // The device output source priority time order
          {(uint8_t*)"QCHPT",    10,      0,         5,      0,      0},     // The device charger source priority time order
          {(uint8_t*)"QT",       3600,    0,         2,      0,      0},     // Time
          {(uint8_t*)"QBEQI",    60,      0,         5,      0,      0},     // Battery equalization status parameters
          {(uint8_t*)"QMN",      86400,   0,         3,      0,      0},     // Model name
          {(uint8_t*)"QGMN",     86400,   0,         4,      0,      0},     // General model name
          {(uint8_t*)"QET",      3600,    0,         3,      0,      0},     // Total PV generated energy
          {(uint8_t*)"QEY",      3600,    0,         3,      0,      0},     // PV generated energy of year
          {(uint8_t*)"QEM",      3600,    0,         3,      0,      0},     // PV generated energy of month
          {(uint8_t*)"QED",      3600,    0,         3,      0,      0},     // PV generated energy of day
          {(uint8_t*)"QLT",      3600,    0,         3,      0,      0},     // Total output load energy
          {(uint8_t*)"QLY",      3600,    0,         3,      0,      0},     // Output load energy of year
          {(uint8_t*)"QLM",      3600,    0,         3,      0,      0},     // Output load energy of month
          {(uint8_t*)"QLD",      3600,    0,         3,     0,      0},     // Output load energy of day
          {(uint8_t*)"QBMS",     10,      0,         4,      0,      0},     // BMS message
          {(uint8_t*)"PBMS",     10,      0,         4,      0,      0},     // BMS message
          {(uint8_t*)"QLED",     10,      0,         4,      0,      0},     // LED status parameters
     };
};
}
}