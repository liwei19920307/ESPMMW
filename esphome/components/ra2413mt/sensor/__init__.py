from esphome.components import sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    UNIT_CENTIMETER,
    UNIT_PERCENT,
    ICON_FLASH,
    ICON_SIGNAL,
)
from .. import CONF_RA2413MT_ID, RA2413MTComponent, ra2413mt_ns

MoveDistanceSensor = ra2413mt_ns.class_(
    "MoveDistanceSensor", sensor.Sensor, cg.Component
)
MoveEnergySensor = ra2413mt_ns.class_("MoveEnergySensor", sensor.Sensor, cg.Component)
StaticDistanceSensor = ra2413mt_ns.class_(
    "StaticDistanceSensor", sensor.Sensor, cg.Component
)
StaticEnergySensor = ra2413mt_ns.class_(
    "StaticEnergySensor", sensor.Sensor, cg.Component
)

CONF_MOVE_DISTANCE = "move_distance"
CONF_MOVE_ENERGY = "move_energy"
CONF_STATIC_DISTANCE = "static_distance"
CONF_STATIC_ENERGY = "static_energy"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_RA2413MT_ID): cv.use_id(RA2413MTComponent),
        cv.Optional(CONF_MOVE_DISTANCE): sensor.sensor_schema(
            MoveDistanceSensor,
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(CONF_MOVE_ENERGY): sensor.sensor_schema(
            MoveEnergySensor,
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_FLASH,
        ),
        cv.Optional(CONF_STATIC_DISTANCE): sensor.sensor_schema(
            StaticDistanceSensor,
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(CONF_STATIC_ENERGY): sensor.sensor_schema(
            StaticEnergySensor,
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_FLASH,
        ),
    }
)


async def to_code(config):
    ra2413mt_component = await cg.get_variable(config[CONF_RA2413MT_ID])
    if move_distance_config := config.get(CONF_MOVE_DISTANCE):
        sens = await sensor.new_sensor(move_distance_config)
        await cg.register_parented(sens, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_move_distance_sensor(sens))
        cg.add(ra2413mt_component.register_listener(sens))
    if move_energy_config := config.get(CONF_MOVE_ENERGY):
        sens = await sensor.new_sensor(move_energy_config)
        await cg.register_parented(sens, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_move_energy_sensor(sens))
        cg.add(ra2413mt_component.register_listener(sens))
    if static_distance_config := config.get(CONF_STATIC_DISTANCE):
        sens = await sensor.new_sensor(static_distance_config)
        await cg.register_parented(sens, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_static_distance_sensor(sens))
        cg.add(ra2413mt_component.register_listener(sens))
    if static_energy_config := config.get(CONF_STATIC_ENERGY):
        sens = await sensor.new_sensor(static_energy_config)
        await cg.register_parented(sens, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_static_energy_sensor(sens))
        cg.add(ra2413mt_component.register_listener(sens))
