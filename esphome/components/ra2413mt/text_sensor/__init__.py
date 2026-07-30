from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC
from .. import CONF_RA2413MT_ID, RA2413MTComponent

CONF_MAX_DETECTION_RANGE = "max_detection_range"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_RA2413MT_ID): cv.use_id(RA2413MTComponent),
        cv.Optional(CONF_MAX_DETECTION_RANGE): text_sensor.text_sensor_schema(
            icon="mdi:arrow-expand-horizontal",
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    ra2413mt_component = await cg.get_variable(config[CONF_RA2413MT_ID])
    if max_detection_range_config := config.get(CONF_MAX_DETECTION_RANGE):
        sens = await text_sensor.new_text_sensor(max_detection_range_config)
        cg.add(ra2413mt_component.set_max_detection_range_text_sensor(sens))
