from esphome.components import button
import esphome.config_validation as cv
import esphome.codegen as cg
from .. import CONF_RA2413MT_ID, RA2413MTComponent, ra2413mt_ns

ResetConfButton = ra2413mt_ns.class_("ResetConfButton", button.Button)
AutoSensitivityButton = ra2413mt_ns.class_("AutoSensitivityButton", button.Button)

CONF_RESET_CONF = "reset_conf"
CONF_AUTO_SENSITIVITY = "auto_sensitivity"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_RA2413MT_ID): cv.use_id(RA2413MTComponent),
        cv.Optional(CONF_RESET_CONF): button.button_schema(
            ResetConfButton,
            icon="mdi:text-box-check-outline",
        ),
        cv.Optional(CONF_AUTO_SENSITIVITY): button.button_schema(
            AutoSensitivityButton,
            icon="mdi:tune-vertical",
        ),
    }
)


async def to_code(config):
    if reset_conf_config := config.get(CONF_RESET_CONF):
        b = await button.new_button(reset_conf_config)
        await cg.register_parented(b, config[CONF_RA2413MT_ID])
    if auto_sensitivity_config := config.get(CONF_AUTO_SENSITIVITY):
        b = await button.new_button(auto_sensitivity_config)
        await cg.register_parented(b, config[CONF_RA2413MT_ID])
