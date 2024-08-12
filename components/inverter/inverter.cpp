#include "inverter.h"
#include "esphome/core/log.h"
#include <string>
using namespace std;

static const char *TAG = "inverter";

namespace esphome {
namespace inverter {

void Inverter::setup() {
     this->state_ = STATE_IDLE;
     struct PollingCommand this->commands_[] = {
          {(uint8_t*)"QPI", 0, 0, 3, 0, 0},               // Device Protocol ID
          {(uint8_t*)"QID", 0, 0, 3, 0, 1},               // The device serial number
          {(uint8_t*)"QSID", 0, 0, 4, 0, 2},              // The device serial number (the length is more than 14)
          {(uint8_t*)"QVFW", 0, 0, 4, 0, 3},              // Main CPU Firmware version
          {(uint8_t*)"QVFW3", 0, 0, 5, 0, 4},             // Another CPU (remote panel) Firmware version
          {(uint8_t*)"VERFW", 0, 0, 4, 0, 5},             // Bluetooth version
          {(uint8_t*)"QPIRI", 0, 0, 5, 0, 6},             // Device Rating Information
          {(uint8_t*)"QFLAG", 0, 0, 5, 0, 7},             // Device flag status
          {(uint8_t*)"QPIGS", 1000, 0, 5, 0, 8},          // Device general status parameters
//          {(uint8_t*)"QPIGS2", 0, 0, 6, 0, 9},          // Device general status parameters inquiry (Only 48V model)
//          {(uint8_t*)"QPGSn", 0, 0, 5, 0, 10},           // Parallel Information inquiry (Only 48V model)
          {(uint8_t*)"QMOD", 5000, 0, 4, 0, 11},              // Device Mode
          {(uint8_t*)"QPIWS", 0, 0, 5, 0, 12},             // Device Warning Status
          {(uint8_t*)"QDI", 0, 0, 3, 0, 13},               // The default setting value
          {(uint8_t*)"QMCHGCR", 0, 0, 7, 0, 14},           // Enquiry selectable value about max charging current
          {(uint8_t*)"QMUCHGCR", 0, 0, 8, 0, 15},          // Enquiry selectable value about max utility charging current
          {(uint8_t*)"QOPPT", 0, 0, 5, 0, 16},             // The device output source priority time order
          {(uint8_t*)"QCHPT", 0, 0, 5, 0, 17},             // The device charger source priority time order
          {(uint8_t*)"QT", 3600000, 0, 2, 0, 18},          // Time
          {(uint8_t*)"QBEQI", 0, 0, 5, 0, 19},             // Battery equalization status parameters
          {(uint8_t*)"QMN", 0, 0, 3, 0, 20},               // Query model name
          {(uint8_t*)"QGMN", 0, 0, 4, 0, 21},              // Query general model name
          {(uint8_t*)"QET", 3600000, 0, 3, 0, 22},          // Query total PV generated energy
          {(uint8_t*)"QEYyyyy", 3600000, 0, 7, 0, 23},      // Query PV generated energy of year
          {(uint8_t*)"QEMyyyymm", 3600000, 0, 9, 0, 24},    // Query PV generated energy of month
          {(uint8_t*)"QEDyyyymmdd", 3600000, 0, 11, 0, 25}, // Query PV generated energy of day
          {(uint8_t*)"QLT", 3600000, 0, 3, 0, 26},          // Query total output load energy
          {(uint8_t*)"QLYyyyy", 3600000, 0, 7, 0, 27},      // Query output load energy of year
          {(uint8_t*)"QLMyyyymm", 3600000, 0, 9, 0, 28},    // Query output load energy of month
          {(uint8_t*)"QLDyyyymmdd", 3600000, 0, 11, 0, 29}, // Query output load energy of day
          {(uint8_t*)"QBMS", 0, 0, 4, 0, 20},              // BMS message
          {(uint8_t*)"PBMS", 0, 0, 4, 0, 31},              // BMS message
          {(uint8_t*)"QLED", 0, 0, 4, 0, 32},              // LED status parameters

     };
     
     for (auto &used_polling_command : this->commands_) { 
          if (used_polling_command.interval >0) {
               ESP_LOGD(TAG, "Commands: %s", used_polling_command.command);
          }
     } 
}

void Inverter::empty_uart_buffer_() {
  uint8_t byte;
  while (this->available()) {
    this->read_byte(&byte);
  }
}

void Inverter::loop() {
       // Read message
     if (this->state_ == STATE_IDLE) {
          switch (this->send_next_command_()) {
/*               case 0:
                    // no command send (empty queue) time to poll
                    if (millis() - this->last_poll_ > this->update_interval_) {
                         this->send_next_poll_();
                         this->last_poll_ = millis();
                    }
                    return;
                    break;
*/               case 1:
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

               // end of answer
               if (byte == 0x0D) {
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
               //const char *command = this->command_queue_[this->command_queue_position_].c_str();
               const char *command = "QPIGS";
               this->command_start_millis_ = millis();
               ESP_LOGD(TAG, "timeout command from queue: %s", command);
               //this->command_queue_[this->command_queue_position_] = std::string("");
               //this->command_queue_position_ = (command_queue_position_ + 1) % COMMAND_QUEUE_LENGTH;
               this->state_ = STATE_IDLE;
               return;
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
//  if (this->command_queue_[this->command_queue_position_].length() != 0) {
//    const char *command = this->command_queue_[this->command_queue_position_].c_str();
    const char *command = "QPIGS";
    uint8_t byte_command[16];
/*    uint8_t length = this->command_queue_[this->command_queue_position_].length();
    for (uint8_t i = 0; i < length; i++) {
      byte_command[i] = (uint8_t) this->command_queue_[this->command_queue_position_].at(i);
    } */
    uint8_t length = 5;
    for (uint8_t i = 0; i < length; i++) {
          byte_command[i] = command[i];
    }
    this->state_ = STATE_COMMAND;
//    this->command_start_millis_ = millis();
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
//  }
  return 0;
}

void Inverter::add_polling_command_(const char *command, ENUMPollingCommand polling_command) {
     for (auto &used_polling_command : this->used_polling_commands_) {
          if (used_polling_command.length == strlen(command)) {
               uint8_t len = strlen(command);
               if (memcmp(used_polling_command.command, command, len) == 0) {
                    return;
               }
          }
          if (used_polling_command.length == 0) {
               size_t length = strlen(command) + 1;
               const char *beg = command;
               const char *end = command + length;
               used_polling_command.command = new uint8_t[length];  // NOLINT(cppcoreguidelines-owning-memory)
               size_t i = 0;
               for (; beg != end; ++beg, ++i) {
                    used_polling_command.command[i] = (uint8_t)(*beg);
               }
               used_polling_command.errors = 0;
               used_polling_command.identifier = polling_command;
               used_polling_command.length = length - 1;
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
  ESP_LOGD(TAG, "checking crc on incoming message");
  if (((uint8_t)((crc16) >> 8)) == read_buffer_[read_pos_ - 3] &&
      ((uint8_t)((crc16) &0xff)) == read_buffer_[read_pos_ - 2]) {
    ESP_LOGD(TAG, "CRC OK");
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