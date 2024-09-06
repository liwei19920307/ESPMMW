from esphome.components import uart
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

CODEOWNERS = ["@liwei19920307"]

DEPENDENCIES = ["uart"]

MULTI_CONF = True

ra2413mt_ns = cg.esphome_ns.namespace("ra2413mt")
RA2413MTComponent = ra2413mt_ns.class_(
    "RA2413MTComponent", cg.Component, uart.UARTDevice
)

CONF_RA2413MT_ID = "ra2413mt_id"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(RA2413MTComponent),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "ra2413mt_uart",
    require_tx=True,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
