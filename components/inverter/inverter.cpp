#include "./inverter.h"
#include "esphome/core/log.h"
#include <string>
using namespace std;

static const char *TAG = "inverter";

namespace esphome {
namespace inverter {

void Inverter::setup() {
     this->state_ = STATE_IDLE;
     /*
     for (auto &used_polling_command : this->MAX_commands) { 
          uint16_t crc16 = cal_crc_half_(used_polling_command.command, used_polling_command.length);
          used_polling_command.command[used_polling_command.length] = ((uint8_t)((crc16) >> 8));
          used_polling_command.command[used_polling_command.length+1] = ((uint8_t)((crc16) &0xff));
          ESP_LOGD(TAG, "Commands: %s", used_polling_command.command);
     } 
     */
//     uint16_t crc16 = cal_crc_half_(byte_command, length-3);
//     byte_command[lenght-3] = ((uint8_t)((crc16) >> 8));
//     byte_command[lenght-2] = ((uint8_t)((crc16) &0xff));
}

void Inverter::empty_uart_buffer_() {
  uint8_t byte;
  while (this->available()) {
    this->read_byte(&byte);
     //this->flush();
  }
}

void Inverter::loop() {
     // Read message
     if (this->state_ == STATE_IDLE) {
          switch (this->send_next_command_()) {
               case 0:
                    // no command send (empty queue) time to poll
                    this->send_next_poll_();
                    return;
                    break;
               case 1:
                    // command send
                    return;
                    break;
          }
     }
     if (this->state_ == STATE_COMMAND_COMPLETE) {
          if (this->check_incoming_length_(4)) {
               ESP_LOGD(TAG, "response length for command OK");
               if (this->check_incoming_crc_()) {
               // crc ok
               if (this->read_buffer_[1] == 'A' && this->read_buffer_[2] == 'C' && this->read_buffer_[3] == 'K') {
                    ESP_LOGD(TAG, "command successful");
               } else {
                    ESP_LOGD(TAG, "command not successful");
               }
               this->command_queue_[this->command_queue_position_] = std::string("");
               this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
               this->state_ = STATE_IDLE;

               } else {
               // crc failed
               this->command_queue_[this->command_queue_position_] = std::string("");
               this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
               this->state_ = STATE_IDLE;
               }
          } else {
               ESP_LOGD(TAG, "response length for command %s not OK: with length %zu",
                         this->command_queue_[this->command_queue_position_].c_str(), this->read_pos_);
               this->command_queue_[this->command_queue_position_] = std::string("");
               this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
               this->state_ = STATE_IDLE;
          }
     }
     if (this->state_ == STATE_POLL_DECODED) {
          std::string cmd((const char *)this->MAX_commands[this->last_polling_command_].command);
          //cmd = cmd.substr(0, MAX_commands[this->last_polling_command_].length); 
          if (cmd == "QPIRI") {
               //QPIRI
               //if (this->last_qpiri_) {this->last_qpiri_->publish_state(tmp);}               
               if (this->grid_rating_voltage_) {this->grid_rating_voltage_->publish_state(value_grid_rating_voltage_);}
               if (this->grid_rating_current_) {this->grid_rating_current_->publish_state(value_grid_rating_current_);}
               if (this->ac_output_rating_voltage_) {this->ac_output_rating_voltage_->publish_state(value_ac_output_rating_voltage_);}
               if (this->ac_output_rating_frequency_) {this->ac_output_rating_frequency_->publish_state(value_ac_output_rating_frequency_);}
               if (this->ac_output_rating_current_) {this->ac_output_rating_current_->publish_state(value_ac_output_rating_current_);}
               if (this->ac_output_rating_apparent_power_) {this->ac_output_rating_apparent_power_->publish_state(value_ac_output_rating_apparent_power_);}
               if (this->ac_output_rating_active_power_) {this->ac_output_rating_active_power_->publish_state(value_ac_output_rating_active_power_);}
               if (this->battery_rating_voltage_) {this->battery_rating_voltage_->publish_state(value_battery_rating_voltage_);}
               if (this->battery_recharge_voltage_) {this->battery_recharge_voltage_->publish_state(value_battery_recharge_voltage_);}
               if (this->battery_under_voltage_) {this->battery_under_voltage_->publish_state(value_battery_under_voltage_);}
               if (this->battery_bulk_voltage_) {this->battery_bulk_voltage_->publish_state(value_battery_bulk_voltage_);}
               if (this->battery_float_voltage_) {this->battery_float_voltage_->publish_state(value_battery_float_voltage_);}
               if (this->battery_type_) {this->battery_type_->publish_state(value_battery_type_);}
               if (this->current_max_ac_charging_current_) {this->current_max_ac_charging_current_->publish_state(value_current_max_ac_charging_current_);}
               if (this->current_max_charging_current_) {this->current_max_charging_current_->publish_state(value_current_max_charging_current_);}
               if (this->input_voltage_range_) {this->input_voltage_range_->publish_state(value_input_voltage_range_);}
               // special for input voltage range switch
               if (this->input_voltage_range_switch_) {this->input_voltage_range_switch_->publish_state(value_input_voltage_range_ == 1);}
               if (this->output_source_priority_) {this->output_source_priority_->publish_state(value_output_source_priority_);}
               // special for output source priority select
               if (this->output_source_priority_select_) {std::string value = esphome::to_string(value_output_source_priority_);this->output_source_priority_select_->map_and_publish(value);} 
               // special for output source priority switches
               if (this->output_source_priority_utility_switch_) {this->output_source_priority_utility_switch_->publish_state(value_output_source_priority_ == 0);}
               if (this->output_source_priority_solar_switch_) {this->output_source_priority_solar_switch_->publish_state(value_output_source_priority_ == 1);}
               if (this->output_source_priority_battery_switch_) {this->output_source_priority_battery_switch_->publish_state(value_output_source_priority_ == 2);}
               if (this->charger_source_priority_) {this->charger_source_priority_->publish_state(value_charger_source_priority_);}
               // special for charger source priority select
               if (this->charger_source_priority_select_) {std::string value = esphome::to_string(value_charger_source_priority_);this->charger_source_priority_select_->map_and_publish(value);} 
               if (this->parallel_max_num_) {this->parallel_max_num_->publish_state(value_parallel_max_num_);}
               if (this->machine_type_) {this->machine_type_->publish_state(value_machine_type_);}
               if (this->topology_) {this->topology_->publish_state(value_topology_);}
               if (this->output_mode_) {this->output_mode_->publish_state(value_output_mode_);}
               if (this->battery_redischarge_voltage_) {this->battery_redischarge_voltage_->publish_state(value_battery_redischarge_voltage_);}
               if (this->pv_ok_condition_for_parallel_) {this->pv_ok_condition_for_parallel_->publish_state(value_pv_ok_condition_for_parallel_);}
               // special for pv ok condition switch
               if (this->pv_ok_condition_for_parallel_switch_) {this->pv_ok_condition_for_parallel_switch_->publish_state(value_pv_ok_condition_for_parallel_ == 1);}
               if (this->pv_power_balance_) {this->pv_power_balance_->publish_state(value_pv_power_balance_ == 1);}
               // special for power balance switch
               if (this->pv_power_balance_switch_) {this->pv_power_balance_switch_->publish_state(value_pv_power_balance_ == 1);}
               
          }
          if (cmd == "QPIGS") {
               //QPIGS
               /*
               if (this->last_qpigs_) {
                    this->last_qpigs_->publish_state(tmp);
               }
               */
               if (this->grid_voltage_) {this->grid_voltage_->publish_state(value_grid_voltage_);}
               if (this->grid_frequency_) {this->grid_frequency_->publish_state(value_grid_frequency_);}
               if (this->ac_output_voltage_) {this->ac_output_voltage_->publish_state(value_ac_output_voltage_);}
               if (this->ac_output_frequency_) {this->ac_output_frequency_->publish_state(value_ac_output_frequency_);}
               if (this->ac_output_apparent_power_) {this->ac_output_apparent_power_->publish_state(value_ac_output_apparent_power_);}
               if (this->ac_output_active_power_) {this->ac_output_active_power_->publish_state(value_ac_output_active_power_);}
               if (this->output_load_percent_) {this->output_load_percent_->publish_state(value_output_load_percent_);}
               if (this->bus_voltage_) {this->bus_voltage_->publish_state(value_bus_voltage_);}
               if (this->battery_voltage_) {this->battery_voltage_->publish_state(value_battery_voltage_);}
               if (this->battery_charging_current_) {this->battery_charging_current_->publish_state(value_battery_charging_current_);}
               if (this->battery_charging_power_) {this->battery_charging_power_->publish_state(value_battery_charging_current_*value_battery_voltage_);}
               if (this->battery_capacity_percent_) {this->battery_capacity_percent_->publish_state(value_battery_capacity_percent_);}
               if (this->inverter_heat_sink_temperature_) {this->inverter_heat_sink_temperature_->publish_state(value_inverter_heat_sink_temperature_);}
               if (this->pv_input_current_) {this->pv_input_current_->publish_state(value_pv_input_current_);}
               if (this->pv_input_voltage_) {this->pv_input_voltage_->publish_state(value_pv_input_voltage_);}
               if (this->battery_voltage_scc_) {this->battery_voltage_scc_->publish_state(value_battery_voltage_scc_);}
               if (this->battery_discharge_current_) {this->battery_discharge_current_->publish_state(value_battery_discharge_current_);}
               if (this->battery_discharge_power_) {this->battery_discharge_power_->publish_state(value_battery_discharge_current_*value_battery_voltage_);}
               if (this->add_sbu_priority_version_) {this->add_sbu_priority_version_->publish_state(value_add_sbu_priority_version_);}
               if (this->configuration_status_) {this->configuration_status_->publish_state(value_configuration_status_);}
               if (this->scc_firmware_version_) {this->scc_firmware_version_->publish_state(value_scc_firmware_version_);}
               if (this->load_status_) {this->load_status_->publish_state(value_load_status_);}
               if (this->battery_voltage_to_steady_while_charging_) {this->battery_voltage_to_steady_while_charging_->publish_state(value_battery_voltage_to_steady_while_charging_);}
               if (this->charging_status_) {this->charging_status_->publish_state(value_charging_status_);}
               if (this->scc_charging_status_) {this->scc_charging_status_->publish_state(value_scc_charging_status_);}
               if (this->ac_charging_status_) {this->ac_charging_status_->publish_state(value_ac_charging_status_);}
               if (this->battery_voltage_offset_for_fans_on_) {this->battery_voltage_offset_for_fans_on_->publish_state(value_battery_voltage_offset_for_fans_on_ / 10.0f);} 
               if (this->eeprom_version_) {this->eeprom_version_->publish_state(value_eeprom_version_);}
               if (this->pv_charging_power_) {this->pv_charging_power_->publish_state(value_pv_charging_power_);}
               if (this->charging_to_floating_mode_) {this->charging_to_floating_mode_->publish_state(value_charging_to_floating_mode_);}
               if (this->switch_on_) {this->switch_on_->publish_state(value_switch_on_);}
               if (this->dustproof_installed_) {this->dustproof_installed_->publish_state(value_dustproof_installed_);}
          }
          this->state_ = STATE_IDLE;
     }
     if (this->state_ == STATE_POLL_CHECKED) {
          bool enabled = true;
          //std::string fc;
          char tmp[READ_BUFFER_LENGTH];
          sprintf(tmp, "%s", this->read_buffer_);
          std::string cmd((const char *)this->MAX_commands[this->last_polling_command_].command);
          //cmd = cmd.substr(0, MAX_commands[this->last_polling_command_].length); 
          //ESP_LOGD(TAG, "Decode %s - millis: %d", cmd, millis()-this->command_start_millis_);
          if (cmd == "QPIRI") {
               ESP_LOGD(TAG, "Decode QPIRI");
               sscanf(tmp, "(%f %f %f %f %f %d %d %f %f %f %f %f %d %d %d %d %d %d %d %d %d %d %f %d %d",             // NOLINT
                         &value_grid_rating_voltage_, &value_grid_rating_current_, &value_ac_output_rating_voltage_,  // NOLINT
                         &value_ac_output_rating_frequency_, &value_ac_output_rating_current_,                        // NOLINT
                         &value_ac_output_rating_apparent_power_, &value_ac_output_rating_active_power_,              // NOLINT
                         &value_battery_rating_voltage_, &value_battery_recharge_voltage_,                            // NOLINT
                         &value_battery_under_voltage_, &value_battery_bulk_voltage_, &value_battery_float_voltage_,  // NOLINT
                         &value_battery_type_, &value_current_max_ac_charging_current_,                               // NOLINT
                         &value_current_max_charging_current_, &value_input_voltage_range_,                           // NOLINT
                         &value_output_source_priority_, &value_charger_source_priority_, &value_parallel_max_num_,   // NOLINT
                         &value_machine_type_, &value_topology_, &value_output_mode_,                                 // NOLINT
                         &value_battery_redischarge_voltage_, &value_pv_ok_condition_for_parallel_,                   // NOLINT
                         &value_pv_power_balance_);                                                                   // NOLINT

               this->state_ = STATE_POLL_DECODED;
          } else if (cmd == "QPIGS") {     
               ESP_LOGD(TAG, "Decode QPIGS");
               sscanf(                                                                                                  // NOLINT
                    tmp,                     
                    //(230.6 50.0 230.6 50.0 0484 0435 012 421 28.40 000 099 0040 00.0 060.3 00.00 00000 00010110 00 00 00000 111 0 00 0000
                    "(%f %f %f %f %d %d %d %d %f %d %d %d %f %f %f %d %1d%1d%1d%1d%1d%1d%1d%1d %d %d %d %1d%1d%1d",      // NOLINT
                    &value_grid_voltage_,                                                             //           1     // NOLINT
                    &value_grid_frequency_,                                                           //           2     // NOLINT
                    &value_ac_output_voltage_,                                                        //           3     // NOLINT
                    &value_ac_output_frequency_,                                                      //           4     // NOLINT
                    &value_ac_output_apparent_power_,                                                 //           5     // NOLINT
                    &value_ac_output_active_power_,                                                   //           6     // NOLINT
                    &value_output_load_percent_,                                                      //           7     // NOLINT
                    &value_bus_voltage_,                                                              //           8     // NOLINT
                    &value_battery_voltage_,                                                          //           9     // NOLINT
                    &value_battery_charging_current_,                                                 //          10     // NOLINT
                    &value_battery_capacity_percent_,                                                 //          11     // NOLINT
                    &value_inverter_heat_sink_temperature_,                                           //          12     // NOLINT
                    &value_pv_input_current_,                                                         //          13     // NOLINT
                    &value_pv_input_voltage_,                                                         //          14     // NOLINT
                    &value_battery_voltage_scc_,                                                      //          15     // NOLINT
                    &value_battery_discharge_current_,                                                //          16     // NOLINT
                    &value_add_sbu_priority_version_,                                                 //          17     // NOLINT
                    &value_configuration_status_,                                                     //          18     // NOLINT
                    &value_scc_firmware_version_,                                                     //          19     // NOLINT
                    &value_load_status_,                                                              //          20     // NOLINT
                    &value_battery_voltage_to_steady_while_charging_,                                 //          21     // NOLINT
                    &value_charging_status_,                                                          //          22     // NOLINT
                    &value_scc_charging_status_,                                                      //          23     // NOLINT
                    &value_ac_charging_status_,                                                       //          24     // NOLINT
                    &value_battery_voltage_offset_for_fans_on_,                                       //          25     // NOLINT
                    &value_eeprom_version_,                                                           //          26     // NOLINT
                    &value_pv_charging_power_,                                                        //          27     // NOLINT
                    &value_charging_to_floating_mode_,                                                //          28     // NOLINT
                    &value_switch_on_,                                                                //          29     // NOLINT
                    &value_dustproof_installed_                                                       //          30     // NOLINT
               );
               this->state_ = STATE_POLL_DECODED;
          } else if (cmd == "QPI") {
               ESP_LOGD(TAG, "Decode QPI");
               this->state_ = STATE_IDLE;
          } else if (cmd == "QET") {
               sscanf(tmp, "(%f", &value_total_pv_generated_energy_);
               value_total_pv_generated_energy_ = value_total_pv_generated_energy_/1000;
               if (this->total_pv_generated_energy_) {this->total_pv_generated_energy_->publish_state(value_total_pv_generated_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QEY") {
               sscanf(tmp, "(%f", &value_year_pv_generated_energy_);
               value_year_pv_generated_energy_ = value_year_pv_generated_energy_/1000;
               if (this->year_pv_generated_energy_) {this->year_pv_generated_energy_->publish_state(value_year_pv_generated_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QEM") {
               sscanf(tmp, "(%f", &value_month_pv_generated_energy_);
               value_month_pv_generated_energy_ = value_month_pv_generated_energy_/1000;
               if (this->month_pv_generated_energy_) {this->month_pv_generated_energy_->publish_state(value_month_pv_generated_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QED") {
               sscanf(tmp, "(%f", &value_day_pv_generated_energy_);
               value_day_pv_generated_energy_ = value_day_pv_generated_energy_/1000;
               if (this->day_pv_generated_energy_) {this->day_pv_generated_energy_->publish_state(value_day_pv_generated_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QLT") {
               sscanf(tmp, "(%f", &value_total_output_load_energy_);
               value_total_output_load_energy_ = value_total_output_load_energy_/1000;
               if (this->total_output_load_energy_) {this->total_output_load_energy_->publish_state(value_total_output_load_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QLY") {
               sscanf(tmp, "(%f", &value_year_output_load_energy_);
               value_year_output_load_energy_ = value_year_output_load_energy_/1000;
               if (this->year_output_load_energy_) {this->year_output_load_energy_->publish_state(value_year_output_load_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QLM") {
               sscanf(tmp, "(%f", &value_month_output_load_energy_);
               value_month_output_load_energy_ = value_month_output_load_energy_/1000;
               if (this->month_output_load_energy_) {this->month_output_load_energy_->publish_state(value_month_output_load_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QLD") {
               sscanf(tmp, "(%f", &value_day_output_load_energy_);
               value_day_output_load_energy_ = value_day_output_load_energy_/1000;
               if (this->day_output_load_energy_) {this->day_output_load_energy_->publish_state(value_day_output_load_energy_);}
               this->state_ = STATE_IDLE;
          } else if (cmd == "QT") {
               ESP_LOGD(TAG, "Decode QT");
               this->state_ = STATE_IDLE;
          } else {
               this->state_ = STATE_IDLE;
          }
          
          return;
     }
     if (this->state_ == STATE_POLL_COMPLETE) {
          
          if (this->check_incoming_crc_()) {
               if (this->read_buffer_[0] == '(' && this->read_buffer_[1] == 'N' && this->read_buffer_[2] == 'A' &&
               this->read_buffer_[3] == 'K') {
                    std::string str((const char *)this->MAX_commands[this->last_polling_command_].command);
                    ESP_LOGW(TAG, "Recive NAK  - %s", str.c_str());
                    this->state_ = STATE_IDLE;
                    return;
               }
               // crc ok
               this->state_ = STATE_POLL_CHECKED;
               return;
          } else {
               this->state_ = STATE_IDLE;
          }
     }
     if (this->state_ == STATE_COMMAND || this->state_ == STATE_POLL) {
          while (this->available()) {
               uint8_t byte;
               this->read_byte(&byte);
               if (this->read_pos_ == READ_BUFFER_LENGTH) {
                         this->read_pos_ = 0;
                         this->empty_uart_buffer_();
               }
               this->read_buffer_[this->read_pos_] = byte;
               this->read_pos_++;
               if (byte == 0x0D) {
               // end of answer
                    this->read_buffer_[this->read_pos_] = 0;
                    this->empty_uart_buffer_();
                    if (this->state_ == STATE_POLL) {
                         this->state_ = STATE_POLL_COMPLETE;
                    }
                    if (this->state_ == STATE_COMMAND) {
                         this->state_ = STATE_COMMAND_COMPLETE;
                    }
               }
          }  // available
     }
     if (this->state_ == STATE_COMMAND) {
          if (millis() - this->command_start_millis_ > esphome::inverter::Inverter::COMMAND_TIMEOUT) {
               // command timeout
               const char *command = (char *)this->command_queue_[this->command_queue_position_].c_str();
               this->command_start_millis_ = millis();
               ESP_LOGW(TAG, "timeout command from queue: %s", command);
               this->command_queue_[this->command_queue_position_] = std::string("");
               this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
               this->state_ = STATE_IDLE;
               return;
          } else {
          }
     }
     if (this->state_ == STATE_POLL) {
          if (millis() - this->command_start_millis_ > esphome::inverter::Inverter::COMMAND_TIMEOUT) {
               // command timeout
               std::string str((const char *)this->MAX_commands[this->last_polling_command_].command);
               ESP_LOGW(TAG, "timeout command to poll: %s", str);
               this->MAX_commands[this->last_polling_command_].last_run = 0;
               this->state_ = STATE_IDLE;
          } else {
          }
     }
}

void Inverter::update() {

     //auto time = this->time_->now();
     //ESP_LOGI(TAG, "Time now: %s", time.strftime("%Y-%m-%d %H:%M"));
}

void Inverter::dump_config() {
     ESP_LOGCONFIG(TAG, "Inverter component");
}

uint8_t Inverter::send_next_command_() {
  uint16_t crc16;
  if (this->command_queue_[this->command_queue_position_].length() != 0) {
    const char *command = this->command_queue_[this->command_queue_position_].c_str();
    uint8_t byte_command[16];
    uint8_t length = this->command_queue_[this->command_queue_position_].length();
    for (uint8_t i = 0; i < length; i++) {
      byte_command[i] = (uint8_t) this->command_queue_[this->command_queue_position_].at(i);
    } 
    this->state_ = STATE_COMMAND;
    this->command_start_millis_ = millis();
    this->empty_uart_buffer_();
    this->read_pos_ = 0;
    crc16 = cal_crc_half_(byte_command, length);
    this->write_str(command);
    // checksum
    this->write(((uint8_t)((crc16) >> 8)));   // highbyte
    this->write(((uint8_t)((crc16) &0xff)));  // lowbyte
    // end Byte
    this->write(0x0D);
    ESP_LOGD(TAG, "Sending command from queue: %s with length %d", command, length);
    return 1;
  }
  return 0;
}

void Inverter::send_next_poll_() {
     uint16_t crc16;
     this->last_polling_command_ = this->last_polling_command_ + 1;
     if (this->last_polling_command_ == (sizeof(MAX_commands)/sizeof(MAX_commands[0]))) {
          this->last_polling_command_ = 0;
     }
     if (this->MAX_commands[this->last_polling_command_].state == 0){
          return;
     }    
     if (this->MAX_commands[this->last_polling_command_].last_run != 0 
          && millis() - this->MAX_commands[this->last_polling_command_].last_run 
               < this->MAX_commands[this->last_polling_command_].interval*1000) {
          return;
     }
     if (this->last_polling_command_ == 0) {
          if (this->last_poll_ != 0) {
               if (millis() - this->last_poll_ < this->update_interval_) { 
                    return;
               } 
          }
          this->last_poll_ = millis();
     }
     //QEY QEM QED QLY QLM QLD 
     std::string cmd((const char *)this->MAX_commands[this->last_polling_command_].command);
     uint8_t len = this->MAX_commands[this->last_polling_command_].length;
     auto time = this->time_->now();
     if (cmd == "QEY" || cmd == "QLY"){
          cmd.append(time.strftime("%Y"));
          len += 4;
     } else if (cmd == "QEM" || cmd == "QLM"){
          cmd.append(time.strftime("%Y%m"));
          len += 6;
     } else if (cmd == "QED" || cmd == "QLD"){
          cmd.append(time.strftime("%Y%m%d"));
          len += 8;
     }
     crc16 = cal_crc_half_((uint8_t*)cmd.c_str(), len);
     this->state_ = STATE_POLL;
     this->command_start_millis_ = millis();
     this->MAX_commands[this->last_polling_command_].last_run = millis();
     this->empty_uart_buffer_();
     this->read_pos_ = 0;
     this->write_array((uint8_t*)cmd.c_str(), len); 
     this->write(((uint8_t)((crc16) >> 8)));   // highbyte
     this->write(((uint8_t)((crc16) &0xff)));  // lowbyte
     this->write(0x0D);
     ESP_LOGD(TAG, "Sending polling command : %s", cmd.c_str());
}

void Inverter::add_polling_command_(const char *command) {
     std::string cmd(command);
     std::string str;
     for (auto &used_polling_command : this->MAX_commands) { 
          str = (const char *)used_polling_command.command;
          str = str.substr(0, used_polling_command.length); 
          if (str == cmd) {
               used_polling_command.state = 1;
               return;
          }
     }


}

uint8_t Inverter::check_incoming_length_(uint8_t length) {
  if (this->read_pos_ - 3 == length) {
    return 1;
  }
  return 0;
}

uint8_t Inverter::check_incoming_crc_() {
  uint16_t crc16;
  crc16 = cal_crc_half_(read_buffer_, read_pos_ - 3);
  //ESP_LOGD(TAG, "checking crc on incoming message");
  if (((uint8_t)((crc16) >> 8)) == read_buffer_[read_pos_ - 3] &&
      ((uint8_t)((crc16) &0xff)) == read_buffer_[read_pos_ - 2]) {
    //ESP_LOGD(TAG, "CRC OK");
    read_buffer_[read_pos_ - 1] = 0;
    read_buffer_[read_pos_ - 2] = 0;
    read_buffer_[read_pos_ - 3] = 0;
    return 1;
  }
  ESP_LOGD(TAG, "CRC NOK expected: %X %X but got: %X %X", ((uint8_t)((crc16) >> 8)), ((uint8_t)((crc16) &0xff)),
           read_buffer_[read_pos_ - 3], read_buffer_[read_pos_ - 2]);
  return 0;
}

uint16_t Inverter::cal_crc_half_(uint8_t *msg, uint8_t len) {
     uint16_t crc;

     uint8_t da;
     uint8_t *ptr;
     uint8_t b_crc_hign;
     uint8_t b_crc_low;

     uint16_t crc_ta[16] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
                         0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef};

     ptr = msg;
     crc = 0;

     while (len-- != 0) {
          da = ((uint8_t)(crc >> 8)) >> 4;
          crc <<= 4;
          crc ^= crc_ta[da ^ (*ptr >> 4)];
          da = ((uint8_t)(crc >> 8)) >> 4;
          crc <<= 4;
          crc ^= crc_ta[da ^ (*ptr & 0x0f)];
          ptr++;
     }

     b_crc_low = crc;
     b_crc_hign = (uint8_t)(crc >> 8);

     if (b_crc_low == 0x28 || b_crc_low == 0x0d || b_crc_low == 0x0a)
          b_crc_low++;
     if (b_crc_hign == 0x28 || b_crc_hign == 0x0d || b_crc_hign == 0x0a)
          b_crc_hign++;

     crc = ((uint16_t) b_crc_hign) << 8;
     crc += b_crc_low;
     return (crc);
}
void Inverter::switch_command(const std::string &command) {
  ESP_LOGD(TAG, "got command: %s", command.c_str());
  queue_command_(command.c_str(), command.length());
}
void Inverter::queue_command_(const char *command, uint8_t length) {
  uint8_t next_position = command_queue_position_;
  for (uint8_t i = 0; i < COMMAND_QUEUE_LENGTH; i++) {
    uint8_t testposition = (next_position + i) % COMMAND_QUEUE_LENGTH;
    if (command_queue_[testposition].length() == 0) {
      command_queue_[testposition] = command;
      ESP_LOGD(TAG, "Command queued successfully: %s with length %u at position %d", command,
               command_queue_[testposition].length(), testposition);
      return;
    }
  }
  ESP_LOGD(TAG, "Command queue full dropping command: %s", command);
}

}
}