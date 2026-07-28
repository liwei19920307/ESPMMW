/**
 * ESPMMW Distance Card — 1D mmWave linear range visualization
 * Styled to match Home Assistant official tile card.
 *
 * Install: copy to /config/www/ and add Lovelace resource
 *   url: /local/espmmw-distance-card.js
 *   type: module
 */
class EspmmwDistanceCard extends HTMLElement {
  static getStubConfig() {
    return {
      title: "ESPMMW",
      presence: "",
      move_distance: "",
      move_energy: "",
      static_distance: "",
      static_energy: "",
      brightness: "",
      max_distance: 450,
      unit: "cm",
    };
  }

  setConfig(config) {
    if (!config) throw new Error("Invalid configuration");
    this._config = {
      title: "ESPMMW",
      max_distance: 450,
      unit: "cm",
      ...config,
    };
    this._render();
  }

  set hass(hass) {
    this._hass = hass;
    if (this._root) this._update();
  }

  getCardSize() {
    return 3;
  }

  connectedCallback() {
    if (this._config && !this._root) this._render();
  }

  _state(entityId) {
    if (!entityId || !this._hass) return null;
    return this._hass.states[entityId] || null;
  }

  _num(entityId) {
    const s = this._state(entityId);
    if (!s) return null;
    const v = parseFloat(s.state);
    return Number.isFinite(v) ? v : null;
  }

  _present() {
    const s = this._state(this._config.presence);
    if (!s) return null;
    return s.state === "on";
  }

  _render() {
    if (!this._root) {
      this._root = this.attachShadow({ mode: "open" });
    }

    this._root.innerHTML = `
      <style>
        :host { display: block; height: 100%; }
        ha-card {
          height: 100%;
          display: flex;
          flex-direction: column;
          justify-content: space-between;
          box-sizing: border-box;
          padding: 12px;
          --esp-move: var(--orange-color, #ff9800);
          --esp-static: var(--blue-color, #2196f3);
          --esp-bright: var(--amber-color, #ffc107);
          --esp-inactive: var(--disabled-color, #9e9e9e);
          --esp-track: color-mix(in srgb, var(--disabled-color, #9e9e9e) 28%, transparent);
        }

        /* —— tile content row —— */
        .content {
          display: flex;
          align-items: center;
          gap: 12px;
          min-width: 0;
        }
        .icon {
          position: relative;
          flex: none;
          display: flex;
          align-items: center;
          justify-content: center;
          width: 36px;
          height: 36px;
          border-radius: var(--ha-tile-icon-border-radius, 50%);
          background: color-mix(in srgb, var(--tile-color, var(--esp-inactive)) 20%, transparent);
          color: var(--tile-color, var(--esp-inactive));
          cursor: pointer;
          transition: background 180ms ease, color 180ms ease, transform 120ms ease;
        }
        .icon:hover { transform: scale(1.04); }
        .icon:active { transform: scale(0.96); }
        .icon ha-icon,
        .icon .mdi {
          --mdc-icon-size: 20px;
          width: 20px;
          height: 20px;
          display: block;
        }
        .icon .mdi svg { width: 20px; height: 20px; fill: currentColor; }
        .icon-badge {
          position: absolute;
          top: -3px;
          right: -3px;
          width: 16px;
          height: 16px;
          border-radius: 50%;
          border: 2px solid var(--card-background-color, var(--ha-card-background, #fff));
          background: var(--esp-inactive);
          box-sizing: border-box;
        }
        .icon-badge.on { background: var(--success-color, #4caf50); }
        .icon-badge.off { background: var(--esp-inactive); }

        .info {
          display: flex;
          flex-direction: column;
          justify-content: center;
          min-width: 0;
          flex: 1;
          cursor: pointer;
          border-radius: 8px;
          margin: -4px;
          padding: 4px;
        }
        .info:hover { background: color-mix(in srgb, var(--primary-text-color) 5%, transparent); }
        .primary {
          font-size: var(--ha-font-size-m, 14px);
          font-weight: var(--ha-font-weight-medium, 500);
          line-height: var(--ha-line-height-normal, 20px);
          letter-spacing: 0.1px;
          color: var(--primary-text-color);
          white-space: nowrap;
          overflow: hidden;
          text-overflow: ellipsis;
        }
        .secondary {
          font-size: var(--ha-font-size-s, 12px);
          font-weight: var(--ha-font-weight-normal, 400);
          line-height: var(--ha-line-height-condensed, 16px);
          letter-spacing: 0.4px;
          color: var(--primary-text-color);
          opacity: 0.7;
          white-space: nowrap;
          overflow: hidden;
          text-overflow: ellipsis;
        }

        /* —— tile features —— */
        .features {
          display: flex;
          flex-direction: column;
          gap: 8px;
          margin-top: 12px;
        }
        .stats {
          display: grid;
          grid-template-columns: 1fr 1fr;
          gap: 8px;
        }
        .stat {
          display: flex;
          flex-direction: column;
          gap: 2px;
          min-width: 0;
          cursor: pointer;
          border-radius: var(--ha-card-features-border-radius, 10px);
          padding: 8px 10px;
          background: var(--esp-track);
          transition: background 160ms ease;
        }
        .stat:hover {
          background: color-mix(in srgb, var(--disabled-color, #9e9e9e) 38%, transparent);
        }
        .stat .label {
          font-size: var(--ha-font-size-xs, 11px);
          font-weight: var(--ha-font-weight-medium, 500);
          letter-spacing: 0.3px;
          color: var(--secondary-text-color, #888);
          text-transform: uppercase;
        }
        .stat .value {
          font-size: var(--ha-font-size-l, 16px);
          font-weight: var(--ha-font-weight-medium, 500);
          font-variant-numeric: tabular-nums;
          line-height: 1.2;
          color: var(--primary-text-color);
        }
        .stat.move .value { color: var(--esp-move); }
        .stat.static .value { color: var(--esp-static); }
        .stat .energy {
          font-size: var(--ha-font-size-xs, 11px);
          color: var(--secondary-text-color, #888);
          min-height: 1em;
        }

        .feature {
          display: flex;
          flex-direction: column;
          gap: 6px;
          cursor: pointer;
          border-radius: var(--ha-card-features-border-radius, 10px);
          padding: 2px 0;
        }
        .feature.hidden { display: none; }
        .feature-head {
          display: flex;
          align-items: baseline;
          justify-content: space-between;
          gap: 8px;
          padding: 0 2px;
        }
        .feature-label {
          font-size: var(--ha-font-size-xs, 11px);
          font-weight: var(--ha-font-weight-medium, 500);
          letter-spacing: 0.3px;
          color: var(--secondary-text-color, #888);
          text-transform: uppercase;
        }
        .feature-value {
          font-size: var(--ha-font-size-s, 12px);
          font-weight: var(--ha-font-weight-medium, 500);
          font-variant-numeric: tabular-nums;
          color: var(--primary-text-color);
        }

        .control {
          position: relative;
          height: 16px;
          border-radius: 999px;
          background: var(--esp-track);
        }
        .control.thin { height: 10px; overflow: visible; }
        .control.fill {
          height: 100%;
          width: 0%;
          border-radius: 999px;
          background: var(--esp-bright);
          transition: width 0.35s ease;
        }
        .ticks {
          display: flex;
          justify-content: space-between;
          padding: 0 1px;
          font-size: 10px;
          line-height: 1;
          color: var(--secondary-text-color, #888);
          font-variant-numeric: tabular-nums;
          opacity: 0.85;
        }

        .marker {
          position: absolute;
          top: 50%;
          width: 14px;
          height: 14px;
          border-radius: 50%;
          transform: translate(-50%, -50%);
          box-shadow: 0 0 0 2px var(--card-background-color, var(--ha-card-background, #fff));
          transition: left 0.35s ease, opacity 0.25s ease, width 0.25s ease, height 0.25s ease;
          pointer-events: none;
          z-index: 2;
        }
        .marker.move { background: var(--esp-move); }
        .marker.static { background: var(--esp-static); }
        .marker.hidden { opacity: 0; }
        .marker-label {
          position: absolute;
          top: -14px;
          transform: translateX(-50%);
          font-size: 10px;
          font-weight: 600;
          white-space: nowrap;
          transition: left 0.35s ease, opacity 0.25s ease;
          pointer-events: none;
          z-index: 3;
        }
        .marker-label.move { color: var(--esp-move); }
        .marker-label.static { color: var(--esp-static); }
        .marker-label.hidden { opacity: 0; }

        .ruler {
          position: relative;
          padding-top: 14px;
        }

        .footer {
          display: none;
          margin-top: 8px;
          font-size: var(--ha-font-size-xs, 11px);
          color: var(--error-color, #f44336);
        }
        .footer.show { display: block; }
      </style>
      <ha-card>
        <div class="content">
          <div class="icon" data-entity="presence" role="button" tabindex="0" title="查看详情">
            <span class="mdi" aria-hidden="true">
              <svg viewBox="0 0 24 24"><path d="M12.5 2C9.85 2 7.45 3.08 5.72 4.81L7.14 6.23C8.5 4.88 10.36 4.05 12.5 4.05C16.64 4.05 20 7.41 20 11.55C20 13.69 19.17 15.55 17.82 16.91L19.24 18.33C20.97 16.6 22.05 14.2 22.05 11.55C22.05 6.27 17.78 2 12.5 2M7.14 16.91C5.79 15.55 4.96 13.69 4.96 11.55H2.91C2.91 14.2 3.99 16.6 5.72 18.33L7.14 16.91M12.5 7.1C10.04 7.1 8.05 9.09 8.05 11.55H10.1C10.1 10.22 11.17 9.15 12.5 9.15V7.1M16.95 11.55C16.95 9.09 14.96 7.1 12.5 7.1V9.15C13.83 9.15 14.9 10.22 14.9 11.55H16.95M6.04 21.5H18.96V19.45H6.04V21.5Z"/></svg>
            </span>
            <span class="icon-badge"></span>
          </div>
          <div class="info" data-entity="presence" role="button" tabindex="0" title="查看详情">
            <div class="primary"></div>
            <div class="secondary"></div>
          </div>
        </div>

        <div class="features">
          <div class="stats">
            <div class="stat move" data-entity="move_distance" role="button" tabindex="0" title="查看详情">
              <span class="label">运动</span>
              <span class="value">—</span>
              <span class="energy" data-entity="move_energy"></span>
            </div>
            <div class="stat static" data-entity="static_distance" role="button" tabindex="0" title="查看详情">
              <span class="label">静止</span>
              <span class="value">—</span>
              <span class="energy" data-entity="static_energy"></span>
            </div>
          </div>

          <div class="feature" data-entity="presence" role="button" tabindex="0" title="查看详情">
            <div class="feature-head">
              <span class="feature-label">距离</span>
              <span class="feature-value dist-summary"></span>
            </div>
            <div class="ruler">
              <div class="marker-label move hidden"></div>
              <div class="marker-label static hidden"></div>
              <div class="control thin">
                <div class="marker move hidden"></div>
                <div class="marker static hidden"></div>
              </div>
            </div>
            <div class="ticks dist-ticks"></div>
          </div>

          <div class="feature brightness hidden" data-entity="brightness" role="button" tabindex="0" title="查看详情">
            <div class="feature-head">
              <span class="feature-label">环境亮度</span>
              <span class="feature-value bvalue">—</span>
            </div>
            <div class="control"><div class="fill bfill"></div></div>
            <div class="ticks bticks"></div>
          </div>
        </div>
        <div class="footer"></div>
      </ha-card>
    `;

    this._els = {
      card: this._root.querySelector("ha-card"),
      icon: this._root.querySelector(".icon"),
      iconBadge: this._root.querySelector(".icon-badge"),
      primary: this._root.querySelector(".primary"),
      secondary: this._root.querySelector(".secondary"),
      moveVal: this._root.querySelector(".stat.move .value"),
      moveEn: this._root.querySelector(".stat.move .energy"),
      staticVal: this._root.querySelector(".stat.static .value"),
      staticEn: this._root.querySelector(".stat.static .energy"),
      brightness: this._root.querySelector(".feature.brightness"),
      bfill: this._root.querySelector(".bfill"),
      bvalue: this._root.querySelector(".bvalue"),
      bticks: this._root.querySelector(".bticks"),
      distTicks: this._root.querySelector(".dist-ticks"),
      distSummary: this._root.querySelector(".dist-summary"),
      moveMk: this._root.querySelector(".marker.move"),
      staticMk: this._root.querySelector(".marker.static"),
      moveLb: this._root.querySelector(".marker-label.move"),
      staticLb: this._root.querySelector(".marker-label.static"),
      footer: this._root.querySelector(".footer"),
    };

    this._moreInfoBound = false;
    this._bindMoreInfo();
    this._buildTicks();
    this._buildBrightnessTicks();
    this._update();
  }

  _moreInfo(entityId) {
    if (!entityId || !this._state(entityId)) return;
    const ev = new Event("hass-more-info", {
      bubbles: true,
      composed: true,
    });
    ev.detail = { entityId };
    this.dispatchEvent(ev);
  }

  _bindMoreInfo() {
    if (!this._root || this._moreInfoBound) return;
    this._moreInfoBound = true;
    const onActivate = (ev) => {
      const hit = ev.target.closest("[data-entity]");
      if (!hit || !this._config) return;
      const key = hit.dataset.entity;
      let entityId = this._config[key];
      if (!entityId && key === "move_energy") entityId = this._config.move_distance;
      if (!entityId && key === "static_energy") entityId = this._config.static_distance;
      if (!entityId) return;
      ev.stopPropagation();
      this._moreInfo(entityId);
    };
    this._root.addEventListener("click", onActivate);
    this._root.addEventListener("keydown", (ev) => {
      if (ev.key !== "Enter" && ev.key !== " ") return;
      onActivate(ev);
      ev.preventDefault();
    });
  }

  _maxDistance() {
    const cfg = this._config.max_distance;
    let max = null;
    if (typeof cfg === "string" && cfg.includes(".")) {
      const n = this._num(cfg);
      if (n && n > 0) max = n;
    } else {
      const n = parseFloat(cfg);
      if (Number.isFinite(n) && n > 0) max = n;
    }
    if (max == null) max = 450;

    const unit = this._unit();
    if ((unit === "cm" || unit === "mm") && max <= 20) {
      max = max * (unit === "mm" ? 1000 : 100);
    }
    return max;
  }

  _buildTicks() {
    if (!this._els.distTicks) return;
    const max = this._maxDistance();
    const unit = this._unit();
    const steps = 5;
    const parts = [];
    for (let i = 0; i <= steps; i++) {
      const v = (max * i) / steps;
      const num = v % 1 === 0 ? v.toFixed(0) : v.toFixed(1);
      parts.push(`<span>${i === steps ? `${num} ${unit}` : num}</span>`);
    }
    this._els.distTicks.innerHTML = parts.join("");
    this._lastMax = max;
    this._lastUnit = unit;
  }

  _buildBrightnessTicks() {
    if (!this._els.bticks) return;
    const parts = [];
    for (let i = 0; i <= 5; i++) {
      const v = (100 * i) / 5;
      parts.push(`<span>${i === 5 ? `${v}%` : v}</span>`);
    }
    this._els.bticks.innerHTML = parts.join("");
  }

  _pct(distance, max) {
    if (distance == null || max <= 0) return null;
    return Math.max(0, Math.min(100, (distance / max) * 100));
  }

  _sizeFromEnergy(energy) {
    if (energy == null) return 14;
    const e = Math.max(0, Math.min(100, energy));
    return 10 + (e / 100) * 12;
  }

  _opacityFromEnergy(energy) {
    if (energy == null) return 0.9;
    return 0.35 + (Math.max(0, Math.min(100, energy)) / 100) * 0.65;
  }

  _placeMarker(mk, lb, distance, energy, present, max) {
    const pct = this._pct(distance, max);
    const show = pct != null && distance > 0;
    mk.classList.toggle("hidden", !show);
    lb.classList.toggle("hidden", !show);
    if (!show) return;
    const size = this._sizeFromEnergy(energy);
    const opacity = this._opacityFromEnergy(energy) * (present === false ? 0.45 : 1);
    mk.style.left = `${pct}%`;
    mk.style.width = `${size}px`;
    mk.style.height = `${size}px`;
    mk.style.opacity = String(opacity);
    lb.style.left = `${pct}%`;
    lb.style.opacity = present === false ? "0.55" : "1";
    lb.textContent = this._fmtDist(distance);
  }

  _unit() {
    if (this._config.unit) return this._config.unit;
    const s = this._state(this._config.move_distance);
    const u = s && s.attributes && s.attributes.unit_of_measurement;
    if (u === "m" || u === "cm" || u === "mm") return u;
    return "cm";
  }

  _fmtDist(v) {
    if (v == null) return "—";
    if (v <= 0) return "—";
    const u = this._unit();
    if (u === "m") return `${v.toFixed(2)} m`;
    if (u === "mm") return `${Math.round(v)} mm`;
    return `${Math.round(v)} cm`;
  }

  _fmtEnergy(v) {
    if (v == null) return "";
    return `能量 ${Math.round(v)}`;
  }

  _fmtBrightness(v) {
    if (v == null) return "—";
    return `${Math.round(Math.max(0, Math.min(100, v)))}%`;
  }

  _updateBrightness() {
    const id = this._config.brightness;
    const row = this._els.brightness;
    if (!id) {
      row.classList.add("hidden");
      return;
    }
    row.classList.remove("hidden");
    const v = this._num(id);
    const pct = v == null ? 0 : Math.max(0, Math.min(100, v));
    this._els.bfill.style.width = `${pct}%`;
    this._els.bvalue.textContent = this._fmtBrightness(v);
  }

  _update() {
    if (!this._els || !this._config) return;

    const max = this._maxDistance();
    const unit = this._unit();
    if (this._lastMax !== max || this._lastUnit !== unit) this._buildTicks();

    this._els.primary.textContent = this._config.title || "ESPMMW";

    const present = this._present();
    const bri = this._config.brightness ? this._num(this._config.brightness) : null;

    // tile icon color + badge (active = green / inactive = grey)
    const active = present === true;
    const tileColor = active
      ? "var(--success-color, #4caf50)"
      : "var(--disabled-color, #9e9e9e)";
    this._els.card.style.setProperty("--tile-color", tileColor);
    this._els.iconBadge.className = `icon-badge ${
      present === null ? "" : present ? "on" : "off"
    }`;

    const stateText =
      present === null ? "无实体" : present ? "有人" : "无人";
    const briText = bri == null ? null : `亮度 ${this._fmtBrightness(bri)}`;
    this._els.secondary.textContent = [stateText, briText].filter(Boolean).join(" · ");

    const md = this._num(this._config.move_distance);
    const me = this._num(this._config.move_energy);
    const sd = this._num(this._config.static_distance);
    const se = this._num(this._config.static_energy);

    this._els.moveVal.textContent = this._fmtDist(md);
    this._els.moveEn.textContent = this._fmtEnergy(me);
    this._els.staticVal.textContent = this._fmtDist(sd);
    this._els.staticEn.textContent = this._fmtEnergy(se);
    this._updateBrightness();

    this._placeMarker(this._els.moveMk, this._els.moveLb, md, me, present, max);
    this._placeMarker(this._els.staticMk, this._els.staticLb, sd, se, present, max);

    const parts = [];
    if (md != null && md > 0) parts.push(`动 ${this._fmtDist(md)}`);
    if (sd != null && sd > 0) parts.push(`静 ${this._fmtDist(sd)}`);
    this._els.distSummary.textContent = parts.join(" · ") || "—";

    const missing = [
      this._config.presence,
      this._config.move_distance,
      this._config.static_distance,
      this._config.brightness,
    ].filter((id) => id && !this._state(id));

    if (missing.length) {
      this._els.footer.textContent = `未找到: ${missing.join(", ")}`;
      this._els.footer.classList.add("show");
    } else {
      this._els.footer.textContent = "";
      this._els.footer.classList.remove("show");
    }
  }

  getGridOptions() {
    return { columns: 6, min_columns: 3, max_columns: 12, rows: 3 };
  }
}

customElements.define("espmmw-distance-card", EspmmwDistanceCard);

window.customCards = window.customCards || [];
window.customCards.push({
  type: "espmmw-distance-card",
  name: "ESPMMW Distance Card",
  description: "1D mmWave linear distance ruler for X-RA2413MT / ESPMMW",
  preview: true,
});
