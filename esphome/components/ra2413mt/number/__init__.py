from esphome.components import number
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    ICON_TIMELAPSE,
    UNIT_METER,
    UNIT_PERCENT,
    UNIT_SECOND,
)
from .. import CONF_RA2413MT_ID, RA2413MTComponent, ra2413mt_ns

MaxMoveDistanceNumber = ra2413mt_ns.class_("MaxMoveDistanceNumber", number.Number)
MaxStaticDistanceNumber = ra2413mt_ns.class_("MaxStaticDistanceNumber", number.Number)
MoveSensitivityNumber = ra2413mt_ns.class_("MoveSensitivityNumber", number.Number)
StaticSensitivityNumber = ra2413mt_ns.class_("StaticSensitivityNumber", number.Number)
UnattendedDurationNumber = ra2413mt_ns.class_("UnattendedDurationNumber", number.Number)

CONF_MAX_MOVE_DISTANCE = "max_move_distance"
CONF_MAX_STATIC_DISTANCE = "max_static_distance"
CONF_MOVE_SENSITIVITYE = "move_sensitivity"
CONF_STATIC_SENSITIVITY = "static_sensitivity"
CONF_UNATTENDED_DURATION = "unattended_duration"

CONF_DEFAULT = "default"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_RA2413MT_ID): cv.use_id(RA2413MTComponent),
        cv.Optional(CONF_MAX_MOVE_DISTANCE): number.number_schema(
            MaxMoveDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_METER,
            icon="mdi:map-marker-distance",
        ).extend(
            {
                cv.Optional(CONF_DEFAULT, default=4.5): cv.float_range(
                    min=1.5, max=4.5
                ),
            }
        ),
        cv.Optional(CONF_MAX_STATIC_DISTANCE): number.number_schema(
            MaxStaticDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_METER,
            icon="mdi:map-marker-distance",
        ).extend(
            {
                cv.Optional(CONF_DEFAULT, default=4.5): cv.float_range(
                    min=1.5, max=4.5
                ),
            }
        ),
        cv.Optional(CONF_MOVE_SENSITIVITYE): number.number_schema(
            MoveSensitivityNumber,
            unit_of_measurement=UNIT_PERCENT,
            icon="mdi:run",
        ).extend(
            {
                cv.Optional(CONF_DEFAULT, default=99): cv.float_range(min=0, max=100),
            }
        ),
        cv.Optional(CONF_STATIC_SENSITIVITY): number.number_schema(
            StaticSensitivityNumber,
            unit_of_measurement=UNIT_PERCENT,
            icon="mdi:run",
        ).extend(
            {
                cv.Optional(CONF_DEFAULT, default=99): cv.float_range(min=0, max=100),
            }
        ),
        cv.Optional(CONF_UNATTENDED_DURATION): number.number_schema(
            UnattendedDurationNumber,
            unit_of_measurement=UNIT_SECOND,
            icon=ICON_TIMELAPSE,
        ).extend(
            {
                cv.Optional(CONF_DEFAULT, default=5): cv.float_range(min=0, max=60),
            }
        ),
    }
)


async def to_code(config):
    ra2413mt_component = await cg.get_variable(config[CONF_RA2413MT_ID])
    if max_move_distance_config := config.get(CONF_MAX_MOVE_DISTANCE):
        n = await number.new_number(
            max_move_distance_config, min_value=1.5, max_value=4.5, step=0.75
        )
        await cg.register_parented(n, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_max_move_distance_number(n))
        cg.add(
            ra2413mt_component.set_default_max_move_distance(
                max_move_distance_config[CONF_DEFAULT]
            )
        )
    if max_static_distance_config := config.get(CONF_MAX_STATIC_DISTANCE):
        n = await number.new_number(
            max_static_distance_config, min_value=1.5, max_value=4.5, step=0.75
        )
        await cg.register_parented(n, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_max_static_distance_number_(n))
        cg.add(
            ra2413mt_component.set_default_max_static_distance(
                max_static_distance_config[CONF_DEFAULT]
            )
        )
    if move_sensitivity_config := config.get(CONF_MOVE_SENSITIVITYE):
        n = await number.new_number(
            move_sensitivity_config, min_value=0, max_value=100, step=1
        )
        await cg.register_parented(n, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_move_sensitivity_number_(n))
        cg.add(
            ra2413mt_component.set_default_move_sensitivity(
                move_sensitivity_config[CONF_DEFAULT]
            )
        )
    if static_sensitivity_config := config.get(CONF_STATIC_SENSITIVITY):
        n = await number.new_number(
            static_sensitivity_config, min_value=0, max_value=100, step=1
        )
        await cg.register_parented(n, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_static_sensitivity_number_(n))
        cg.add(
            ra2413mt_component.set_default_static_sensitivity(
                static_sensitivity_config[CONF_DEFAULT]
            )
        )
    if unattended_duration_config := config.get(CONF_UNATTENDED_DURATION):
        n = await number.new_number(
            unattended_duration_config, min_value=0, max_value=60, step=1
        )
        await cg.register_parented(n, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_unattended_duration_number(n)),
        cg.add(
            ra2413mt_component.set_default_unattended_duration(
                unattended_duration_config[CONF_DEFAULT]
            )
        )
