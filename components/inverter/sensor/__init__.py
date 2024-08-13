import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    CONF_BUS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ICON_CURRENT_AC,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_HERTZ,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_VOLT_AMPS,
    UNIT_WATT,
)

from .. import CONF_INVERTER_ID, INVERTER_COMPONENT_SCHEMA

DEPENDENCIES = ["uart"]

# QPIRI sensors
CONF_GRID_RATING_VOLTAGE = "grid_rating_voltage"
CONF_GRID_RATING_CURRENT = "grid_rating_current"

# QPIGS sensors
CONF_GRID_VOLTAGE = "grid_voltage"
CONF_GRID_FREQUENCY = "grid_frequency"


# QPI
CONF_DEVICE_PROTOCOL_ID = "device_protocol_id"
# QET<CRC><cr>: Query total PV generated energy 
CONF_TOTAL_PV_GENERATED_ENERGY = "total_pv_generated_energy"
# QEYyyyy<CRC><cr>: Query PV generated energy of year
CONF_YEAR_PV_GENERATED_ENERGY = "year_pv_generated_energy"
# QEMyyyymm<CRC><cr>: Query PV generated energy of month 
CONF_MONTH_PV_GENERATED_ENERGY = "month_pv_generated_energy"
# QEDyyyymmdd<CRC><cr>: Query PV generated energy of day 
CONF_DAY_PV_GENERATED_ENERGY = "day_pv_generated_energy"
# QLT<CRC><cr>: Query total output load energy
CONF_DAY_PV_GENERATED_ENERGY = "total_output_load_energy"
# QLYyyyy<CRC><cr>: Query output load energy of year 
CONF_DAY_PV_GENERATED_ENERGY = "year_output_load_energy"
# QLMyyyymm<CRC><cr>: Query output load energy of month 
CONF_DAY_PV_GENERATED_ENERGY = "month_output_load_energy"
# QLDyyyymmdd<CRC><cr>: Query output load energy of day 
CONF_DAY_PV_GENERATED_ENERGY = "day_output_load_energy"
# QBMS<CRC><cr>: BMS message
# PBMS<CRC><cr>: BMS message 

TYPES = {
    CONF_GRID_RATING_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    CONF_GRID_RATING_CURRENT: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
    ),


    CONF_GRID_VOLTAGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    CONF_GRID_FREQUENCY: sensor.sensor_schema(
        unit_of_measurement=UNIT_HERTZ,
        icon=ICON_CURRENT_AC,
        accuracy_decimals=1,
    ),



    CONF_DEVICE_PROTOCOL_ID: sensor.sensor_schema(
        accuracy_decimals=1,
    ),
    CONF_TOTAL_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_YEAR_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_MONTH_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_DAY_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_DAY_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_DAY_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_DAY_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
    CONF_DAY_PV_GENERATED_ENERGY: sensor.sensor_schema(
        unit_of_measurement='kWh',
        accuracy_decimals=2,
        state_class= 'total_increasing',
        device_class= 'energy',
    ),
}

CONFIG_SCHEMA = INVERTER_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, schema in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_INVERTER_ID])

    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(paren, f"set_{type}")(sens)) 