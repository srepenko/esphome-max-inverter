#include "select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace inverter {

static const char *const TAG = "inverter.select";

void InverterSelect::dump_config() { LOG_SELECT(TAG, "Inverter Controller Select", this); }

void InverterSelect::control(const std::string &value) {
  ESP_LOGD(TAG, "got option: %s", value.c_str());
  if (this->mapping_.find(value) != this->mapping_.end()) {
    ESP_LOGD(TAG, "found mapped option %s for option %s", this->mapping_[value].c_str(), value.c_str());
    this->parent_->switch_command(this->mapping_[value]);
  } else {
    ESP_LOGD(TAG, "could not find option %s in mapping", value.c_str());
    return;
  }
  // auto options = this->traits.get_options();
  // auto opt_it = std::find(options.cbegin(), options.cend(), value);
  // size_t idx = std::distance(options.cbegin(), opt_it);
  // std::string mapval = this->mapping_[idx];
  // ESP_LOGD(TAG, "Found value %s for option '%s'", *mapval.c_str(), value.c_str());

  if (this->optimistic_)
    this->publish_state(value);
    //this->active_index(2);
}

void InverterSelect::map_and_publish(std::string &value) {
  ESP_LOGD(TAG, "got value: %s", value.c_str());
  if (this->status_mapping_.find(value) != this->status_mapping_.end()) {
    ESP_LOGD(TAG, "found mapped option %s for option %s", this->status_mapping_[value].c_str(), value.c_str());
    this->publish_state(this->status_mapping_[value]);
  } else {
    ESP_LOGD(TAG, "could not find option %s in mapping", value.c_str());
    return;
  }
}

}  // namespace inverter
}  // namespace esphome
