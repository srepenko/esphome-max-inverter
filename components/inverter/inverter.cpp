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
/*       // Read message
     if (this->state_ == STATE_IDLE) {
          switch (this->send_next_command_()) {
               case 0:
                    // no command send (empty queue) time to poll
                    if (millis() - this->last_poll_ > this->update_interval_) {
                         this->send_next_poll_();
                         this->last_poll_ = millis();
                    }
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
     if (this->state_ == STATE_POLL_COMPLETE) {
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
          std::string fc;
          char tmp[READ_BUFFER_LENGTH];
          sprintf(tmp, "%s", this->read_buffer_);
          const char *cmd = (const char *)this->used_polling_commands_[this->last_polling_command_].command;
          ESP_LOGD(TAG, "Decode %s - mils: %d", cmd, millis());
          if (cmd == "QPIRI") {
               ESP_LOGD(TAG, "Decode QPIRI");
               this->state_ = STATE_POLL_DECODED;
          } else if (cmd == "QPI") {
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
               ESP_LOGD(TAG, "timeout command to poll: %s - mils: %d", this->used_polling_commands_[this->last_polling_command_].command, millis());
               this->state_ = STATE_IDLE;
          } else {
          }
     }
*/
     if (this->state_ == STATE_IDLE) {
          this->command_start_millis_ = millis();
          this->state_ = STATE_COMMAND;
          this->empty_uart_buffer_();
          this->read_pos_ = 0;
          this->write_str("QPI00\r"); 
          /*
          for (auto &used_polling_command : this->used_polling_commands_) { 
               if (used_polling_command.length != 0) {
                    ESP_LOGD(TAG, "Commands: %s", used_polling_command.command);
               }
          } */
     } 
/*     if (this->state_ == STATE_COMMAND_COMPLETE){
          this->state_ = STATE_IDLE;
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
*/
     if (this->state_ == STATE_COMMAND || this->state_ == STATE_POLL) {
          while (this->available()) {
               //if (this->read_pos_>0) {
               //     ESP_LOGI(TAG, "Read %d %d ms byte: %s", this->read_pos_, millis()-this->command_start_millis_, this->read_buffer_);
               //}
               uint8_t byte;
               this->read_byte(&byte);
               if (byte != 0x0D) {
                    if (this->read_pos_ == READ_BUFFER_LENGTH) {
                         this->read_pos_ = 0;
                         this->empty_uart_buffer_();
                    }
                    this->read_buffer_[this->read_pos_] = byte;
                    this->read_pos_++;
                    
               } else {
                    this->read_buffer_[this->read_pos_] = 0;
                    this->empty_uart_buffer_();
                    ESP_LOGI(TAG, "Read %d %d ms byte: %s", this->read_pos_, millis()-this->command_start_millis_, this->read_buffer_);
                    //if (this->state_ == STATE_POLL) {
                    //     this->state_ = STATE_POLL_COMPLETE;
                    //}
                    //if (this->state_ == STATE_COMMAND) {
                    //     this->state_ = STATE_COMMAND_COMPLETE;
                    //}
                    this->state_ = STATE_IDLE;
               }
          } 
     }
     
}

void Inverter::update() {
     //if (this->state_ == STATE_IDLE) {
          this->command_start_millis_ = millis();
          this->state_ = STATE_COMMAND;
          this->empty_uart_buffer_();
          this->read_pos_ = 0;
          this->write_str("QPI00\r"); 
          /*
          for (auto &used_polling_command : this->used_polling_commands_) { 
               if (used_polling_command.length != 0) {
                    ESP_LOGD(TAG, "Commands: %s", used_polling_command.command);
               }
          } */
     //} 
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
  this->last_polling_command_ = (this->last_polling_command_ + 1) % 15;
  if (this->used_polling_commands_[this->last_polling_command_].length == 0) {
    this->last_polling_command_ = 0;
  }
  if (this->used_polling_commands_[this->last_polling_command_].length == 0) {
    // no command specified
    return;
  }
  this->state_ = STATE_POLL;
  this->command_start_millis_ = millis();
  this->empty_uart_buffer_();
  this->read_pos_ = 0;
  crc16 = cal_crc_half_(this->used_polling_commands_[this->last_polling_command_].command,
                        this->used_polling_commands_[this->last_polling_command_].length);
  this->write_array(this->used_polling_commands_[this->last_polling_command_].command,
                    this->used_polling_commands_[this->last_polling_command_].length);
  // checksum
  this->write(((uint8_t)((crc16) >> 8)));   // highbyte
  this->write(((uint8_t)((crc16) &0xff)));  // lowbyte
  // end Byte
  this->write(0x0D);
  ESP_LOGD(TAG, "Sending polling command : %s with length %d",
           this->used_polling_commands_[this->last_polling_command_].command,
           this->used_polling_commands_[this->last_polling_command_].length);
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
               //used_polling_command.identifier = polling_command;
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
  ESP_LOGD(TAG, "CRC NOK expected: %X %X but got: %X %X %x", ((uint8_t)((crc16) >> 8)), ((uint8_t)((crc16) &0xff)),
           read_buffer_[read_pos_ - 3], read_buffer_[read_pos_ - 2], read_buffer_[read_pos_ - 1]);
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