from esphome.components import button
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    DEVICE_CLASS_RESTART,
    ICON_RESTART_ALERT,
)
from .. import CONF_RA2413MT_ID, RA2413MTComponent, ra2413mt_ns

GetConfButton = ra2413mt_ns.class_("GetConfButton", button.Button)
RestConfButton = ra2413mt_ns.class_("RestConfButton", button.Button)

CONF_GET_CONF = "get_conf"
CONF_RESET_CONF = "reset_conf"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_RA2413MT_ID): cv.use_id(RA2413MTComponent),
        cv.Optional(CONF_GET_CONF): button.button_schema(
            GetConfButton,
            icon="mdi:text-box-search-outline",
        ),
        cv.Optional(CONF_RESET_CONF): button.button_schema(
            RestConfButton,
            icon="mdi:text-box-check-outline",
        ),
    }
)


async def to_code(config):
    ra2413mt_component = await cg.get_variable(config[CONF_RA2413MT_ID])
    if get_conf_config := config.get(CONF_GET_CONF):
        b = await button.new_button(get_conf_config)
        await cg.register_parented(b, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_get_conf_button(b))
    if reset_conf_config := config.get(CONF_RESET_CONF):
        b = await button.new_button(reset_conf_config)
        await cg.register_parented(b, config[CONF_RA2413MT_ID])
        cg.add(ra2413mt_component.set_reset_conf_button(b))
