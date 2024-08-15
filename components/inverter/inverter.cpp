#include "inverter.h"
#include "esphome/core/log.h"
#include <string>
using namespace std;

static const char *TAG = "inverter";

namespace esphome {
namespace inverter {

void Inverter::setup() {
     this->state_ = STATE_IDLE;
     for (auto &used_polling_command : this->MAX_commands) { 
          //if (used_polling_command.interval >0) {
               ESP_LOGD(TAG, "Commands: %s", used_polling_command.command);
               uint16_t crc16 = cal_crc_half_(used_polling_command.command, used_polling_command.length);
               used_polling_command.command[used_polling_command.length] = ((uint8_t)((crc16) >> 8));
               used_polling_command.command[used_polling_command.length+1] = ((uint8_t)((crc16) &0xff));
               ESP_LOGD(TAG, "Commands: %s", used_polling_command.command);
          //}
     } 

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
          ESP_LOGD(TAG, "command successful");
          //ESP_LOGI(TAG, "STATE: %d", this->state_);
          ESP_LOGI(TAG, "Read %d byte: %s", this->read_pos_, this->read_buffer_);
          this->state_ = STATE_IDLE;
     }
     if (this->state_ == STATE_POLL_DECODED) {
          std::string mode;
          this->state_ = STATE_IDLE;
     }
     if (this->state_ == STATE_POLL_COMPLETE) {
          //ESP_LOGI(TAG, "Recive %d byte: %s", this->read_pos_, this->read_buffer_);
          if (this->check_incoming_crc_()) {
               if (this->read_buffer_[0] == '(' && this->read_buffer_[1] == 'N' && this->read_buffer_[2] == 'A' &&
               this->read_buffer_[3] == 'K') {
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
               ESP_LOGD(TAG, "timeout command from queue: %s", command);
               this->command_queue_[this->command_queue_position_] = std::string("");
               this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
               this->state_ = STATE_IDLE;
               return;
          } else {
          }
     }
     if (this->state_ == STATE_POLL_CHECKED) {
          bool enabled = true;
          //std::string fc;
          char tmp[READ_BUFFER_LENGTH];
          sprintf(tmp, "%s", this->read_buffer_);
          std::string cmd((const char *)this->MAX_commands[this->last_polling_command_].command);
          cmd = cmd.substr(0, MAX_commands[this->last_polling_command_].length); 
          //ESP_LOGD(TAG, "Decode %s - millis: %d", cmd, millis()-this->command_start_millis_);
          if (cmd == "QPIRI") {
               ESP_LOGD(TAG, "Decode QPIRI");
               this->state_ = STATE_POLL_DECODED;
          } else if (cmd == "QPIGS") {     
               ESP_LOGD(TAG, "Decode QPIGS");
               // Response examples of the PIP 2424MSE1
               // 226.7 49.9 226.7 49.9 0498 0479 016 427 27.00 005 100 0035 01.9 255.1 00.00 00000 10010110 00 00 00510 110 (2424MSE1)
               // 225.8 49.9 225.8 49.9 0609 0565 020 427 27.00 005 100 0035 02.2 259.9 00.00 00000 10010110 00 00 00590 110 (2424MSE1)
               // 247.3 50.0 239.0 50.0 0931 0805 025 360 26.10 007 060 0017 04.6 179.2 00.00 00001 00010110 00 00 00831 011 (Axpert VM IV 24v 3600w)
               // 232.6 50.0 229.9 49.9 0391 0312 007 402 54.40 042 072 0066 0042 284.6 00.00 00000 00010010 00 00 02901 010 (PIP-5048Mg FW71.85)
               // 218.1 49.9 218.1 49.9 0327 0295 005 360 51.20 000 100 0037 00.0 000.0 00.00 00000 00010000 00 00 00002 011 0 00 0000 (PIP6048MT)
               sscanf(                                                                                                  // NOLINT
                    tmp,                                                                                                 // NOLINT
                    "(%f %f %f %f %d %d %d %d %f %d %d %d %f %f %f %d %1d%1d%1d%1d%1d%1d%1d%1d %d %d %d %1d%1d%1d",      // NOLINT
                    // 225.8   |              |            |                |              |               |       1     // NOLINT
                    //   49.9  |              |            |                |              |               |       2     // NOLINT
                    //      225.8             |            |                |              |               |       3     // NOLINT
                    //         49.9           |            |                |              |               |       4     // NOLINT
                    //            0609        |            |                |              |               |       5     // NOLINT
                    //               0565     |            |                |              |               |       6     // NOLINT
                    //                  020   |            |                |              |               |       7     // NOLINT
                    //                     427|            |                |              |               |       8     // NOLINT
                    //                        27.00        |                |              |               |       9     // NOLINT
                    //                           005       |                |              |               |      10     // NOLINT
                    //                              100    |                |              |               |      11     // NOLINT
                    //                                 0035|                |              |               |      12     // NOLINT
                    //                                     02.2             |              |               |      13     // NOLINT
                    //                                       259.9          |              |               |      14     // NOLINT
                    //                                          00.00       |              |               |      15     // NOLINT
                    //                                             00000    |              |               |      16     // NOLINT
                    //                                                1     |              |               |      17     // NOLINT
                    //                                                   0  |              |               |      18     // NOLINT
                    //                                                      0              |               |      19     // NOLINT
                    //                                                         1           |               |      20     // NOLINT
                    //                                                            0        |               |      21     // NOLINT
                    //                                                               1     |               |      22     // NOLINT
                    //                                                                  1  |               |      23     // NOLINT
                    //                                                                     0               |      24     // NOLINT
                    //                                                                         00          |      25     // NOLINT
                    //                                                                            00       |      26     // NOLINT
                    //                                                                               00590 |      27     // NOLINT
                    //                                                                                  1  |      28     // NOLINT
                    //                                                                                     1      29     // NOLINT
                    //                                                                                        0   30     // NOLINT
                    //                                                                                                   // NOLINT
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
                    &value_pv_input_current_for_battery_,                                             //          13     // NOLINT
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
               if (this->last_qpigs_) {
                    this->last_qpigs_->publish_state(tmp);
               }
               this->state_ = STATE_POLL_DECODED;
          } else if (cmd == "QPI") {
               ESP_LOGD(TAG, "Decode QPI");
               this->state_ = STATE_POLL_DECODED;
          } else if (cmd == "QT") {
               ESP_LOGD(TAG, "Decode QPI");
               this->state_ = STATE_POLL_DECODED;
          } else {
               this->state_ = STATE_IDLE;
          }
          return;
     }
     if (this->state_ == STATE_POLL) {
          if (millis() - this->command_start_millis_ > esphome::inverter::Inverter::COMMAND_TIMEOUT) {
               // command timeout
               this->MAX_commands[this->last_polling_command_].last_run = 0;
               std::string str((const char *)this->MAX_commands[this->last_polling_command_].command);
               str = str.substr(0, this->MAX_commands[this->last_polling_command_].length); 
               ESP_LOGD(TAG, "timeout command to poll: %s, last_run: %d", str.c_str(), this->MAX_commands[this->last_polling_command_].last_run);
               this->state_ = STATE_IDLE;
          } else {
          }
     }
}

void Inverter::update() {
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
     this->state_ = STATE_POLL;
     this->command_start_millis_ = millis();
     this->MAX_commands[this->last_polling_command_].last_run = this->command_start_millis_;
     this->empty_uart_buffer_();
     this->read_pos_ = 0;
     this->write_array(this->MAX_commands[this->last_polling_command_].command, this->MAX_commands[this->last_polling_command_].length+3); 
     std::string str((const char *)this->MAX_commands[this->last_polling_command_].command);
     str = str.substr(0, MAX_commands[this->last_polling_command_].length); 
     ESP_LOGD(TAG, "Sending polling command : %s", str.c_str());
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

}
}