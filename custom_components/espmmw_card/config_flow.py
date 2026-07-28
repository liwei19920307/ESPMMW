"""Config flow for ESPMMW Card."""

from __future__ import annotations

from homeassistant.config_entries import ConfigFlow, ConfigFlowResult

from .const import DOMAIN


class EspmmwCardConfigFlow(ConfigFlow, domain=DOMAIN):
    """Handle a config flow."""

    VERSION = 1

    async def async_step_user(self, user_input: dict | None = None) -> ConfigFlowResult:
        """Single-instance install: confirm and create entry."""
        if self._async_current_entries():
            return self.async_abort(reason="single_instance_allowed")

        if user_input is not None:
            return self.async_create_entry(title="ESPMMW Card", data={})

        return self.async_show_form(step_id="user")
