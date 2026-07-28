"""ESPMMW Card — registers the Lovelace card for HACS / HA install."""

from __future__ import annotations

import logging
from pathlib import Path

from homeassistant.components.frontend import add_extra_js_url
from homeassistant.components.http import StaticPathConfig
from homeassistant.config_entries import ConfigEntry
from homeassistant.core import HomeAssistant
from homeassistant.helpers.typing import ConfigType

from .const import CARD_FILENAME, CARD_VERSION, DOMAIN

_LOGGER = logging.getLogger(__name__)

CARD_URL = f"/{DOMAIN}/{CARD_FILENAME}"
# 旧集成路径，升级时一并替换资源 URL
_LEGACY_RESOURCE_MARKERS = ("espmmw_distance_card", "espmmw-distance-card.js", DOMAIN, CARD_FILENAME)


async def async_setup(hass: HomeAssistant, config: ConfigType) -> bool:
    """Set up the integration (YAML optional; mainly for HACS discovery)."""
    return True


async def async_setup_entry(hass: HomeAssistant, entry: ConfigEntry) -> bool:
    """Register static files and Lovelace resource when integration is added."""
    await _async_register_card(hass)
    return True


async def async_unload_entry(hass: HomeAssistant, entry: ConfigEntry) -> bool:
    """Unload — leave Lovelace resource in place (safe on re-add)."""
    return True


async def _async_register_card(hass: HomeAssistant) -> None:
    """Serve JS and register it as a Lovelace module resource."""
    if hass.data.get(DOMAIN, {}).get("registered"):
        return

    www_path = Path(__file__).parent / "www"
    await hass.http.async_register_static_paths(
        [
            StaticPathConfig(
                f"/{DOMAIN}",
                str(www_path),
                cache_headers=False,
            )
        ]
    )

    versioned_url = f"{CARD_URL}?v={CARD_VERSION}"
    await _async_ensure_lovelace_resource(hass, versioned_url)

    hass.data.setdefault(DOMAIN, {})["registered"] = True
    hass.data[DOMAIN]["url"] = versioned_url
    _LOGGER.info("ESPMMW Card registered at %s", versioned_url)


async def _async_ensure_lovelace_resource(hass: HomeAssistant, versioned_url: str) -> None:
    """Add/update Lovelace resource (storage mode) or fall back to extra JS."""
    try:
        resources = hass.data["lovelace"].resources
    except (KeyError, AttributeError):
        add_extra_js_url(hass, versioned_url)
        return

    # Storage-mode dashboards
    try:
        from homeassistant.components.lovelace.resources import ResourceStorageCollection

        if isinstance(resources, ResourceStorageCollection):
            await resources.async_get_info()
            existing = [
                item
                for item in resources.async_items()
                if any(m in item.get("url", "") for m in _LEGACY_RESOURCE_MARKERS)
            ]
            if existing:
                current = existing[0]
                if current.get("url") != versioned_url:
                    await resources.async_update_item(
                        current["id"],
                        {"res_type": "module", "url": versioned_url},
                    )
            else:
                await resources.async_create_item(
                    {"res_type": "module", "url": versioned_url}
                )
            return
    except Exception:  # noqa: BLE001 — fall back below
        _LOGGER.debug("Lovelace resource storage unavailable, using add_extra_js_url", exc_info=True)

    add_extra_js_url(hass, versioned_url)
