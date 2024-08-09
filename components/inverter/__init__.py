import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@srepenko"]
AUTO_LOAD = ["binary_sensor", "text_sensor", "sensor", "switch", "output", "select"]
MULTI_CONF = True

CONF_INVERTER_ID = "inverter_id"

inverter_ns = cg.esphome_ns.namespace("inverter")
InverterComponent = inverter_ns.class_("Inverter", cg.Component)

INVERTER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_INVERTER_ID): cv.use_id(InverterComponent),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({cv.GenerateID(): cv.declare_id(InverterComponent)})
    .extend(cv.polling_component_schema("1s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
