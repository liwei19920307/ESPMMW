from esphome.components import sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CENTIMETER,
    UNIT_PERCENT,
    ICON_FLASH,
    ICON_SIGNAL,
)
from .. import CONF_RA2413MT_ID, RA2413MTComponent

CONF_MOVE_DISTANCE = "move_distance"
CONF_MOVE_ENERGY = "move_energy"
CONF_STATIC_DISTANCE = "static_distance"
CONF_STATIC_ENERGY = "static_energy"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_RA2413MT_ID): cv.use_id(RA2413MTComponent),
        cv.Optional(CONF_MOVE_DISTANCE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_SIGNAL,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MOVE_ENERGY): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_FLASH,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATIC_DISTANCE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_SIGNAL,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATIC_ENERGY): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_FLASH,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    ra2413mt_component = await cg.get_variable(config[CONF_RA2413MT_ID])
    if move_distance_config := config.get(CONF_MOVE_DISTANCE):
        sens = await sensor.new_sensor(move_distance_config)
        cg.add(ra2413mt_component.set_move_distance_sensor(sens))
    if move_energy_config := config.get(CONF_MOVE_ENERGY):
        sens = await sensor.new_sensor(move_energy_config)
        cg.add(ra2413mt_component.set_move_energy_sensor(sens))
    if static_distance_config := config.get(CONF_STATIC_DISTANCE):
        sens = await sensor.new_sensor(static_distance_config)
        cg.add(ra2413mt_component.set_static_distance_sensor(sens))
    if static_energy_config := config.get(CONF_STATIC_ENERGY):
        sens = await sensor.new_sensor(static_energy_config)
        cg.add(ra2413mt_component.set_static_energy_sensor(sens))
