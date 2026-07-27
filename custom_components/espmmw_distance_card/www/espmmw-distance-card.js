/**
 * ESPMMW Distance Card — 1D mmWave linear range visualization
 * For X-RA2413MT / ESPMMW (no angle, distance + energy only)
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
      // 雷达上报距离为 cm
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
        :host { display: block; }
        ha-card {
          padding: 12px 16px 16px;
          --esp-move: #e67e22;
          --esp-static: #3498db;
          --esp-track: var(--divider-color, rgba(127,127,127,0.35));
          --esp-muted: var(--secondary-text-color, #888);
        }
        .header {
          display: flex;
          align-items: baseline;
          justify-content: space-between;
          gap: 8px;
          margin-bottom: 10px;
        }
        .title {
          font-size: 1.05rem;
          font-weight: 500;
          color: var(--primary-text-color);
        }
        .badge {
          font-size: 0.8rem;
          padding: 2px 8px;
          border-radius: 999px;
          background: var(--esp-track);
          color: var(--esp-muted);
        }
        .badge.on {
          background: color-mix(in srgb, var(--success-color, #2ecc71) 22%, transparent);
          color: var(--success-color, #2ecc71);
        }
        .badge.off {
          background: color-mix(in srgb, var(--esp-muted) 18%, transparent);
        }
        .stats {
          display: grid;
          grid-template-columns: 1fr 1fr;
          gap: 8px;
          margin-bottom: 14px;
        }
        .stat {
          display: flex;
          flex-direction: column;
          gap: 2px;
          min-width: 0;
        }
        .stat .label {
          font-size: 0.72rem;
          color: var(--esp-muted);
          letter-spacing: 0.02em;
        }
        .stat .value {
          font-size: 1.25rem;
          font-variant-numeric: tabular-nums;
          font-weight: 600;
          line-height: 1.2;
        }
        .stat.move .value { color: var(--esp-move); }
        .stat.static .value { color: var(--esp-static); }
        .stat .energy {
          font-size: 0.72rem;
          color: var(--esp-muted);
        }
        .ruler-wrap {
          position: relative;
          padding: 18px 4px 4px;
        }
        .track {
          position: relative;
          height: 10px;
          border-radius: 999px;
          background: var(--esp-track);
          overflow: visible;
        }
        .ticks {
          display: flex;
          justify-content: space-between;
          margin-top: 6px;
          font-size: 0.68rem;
          color: var(--esp-muted);
          font-variant-numeric: tabular-nums;
        }
        .marker {
          position: absolute;
          top: 50%;
          width: 14px;
          height: 14px;
          border-radius: 50%;
          transform: translate(-50%, -50%);
          box-shadow: 0 0 0 2px var(--card-background-color, #fff);
          transition: left 0.35s ease, opacity 0.25s ease, width 0.25s ease, height 0.25s ease;
          pointer-events: none;
          z-index: 2;
        }
        .marker.move { background: var(--esp-move); }
        .marker.static { background: var(--esp-static); }
        .marker.hidden { opacity: 0; }
        .marker-label {
          position: absolute;
          top: -2px;
          transform: translateX(-50%);
          font-size: 0.65rem;
          font-weight: 600;
          white-space: nowrap;
          transition: left 0.35s ease, opacity 0.25s ease;
          pointer-events: none;
          z-index: 3;
        }
        .marker-label.move { color: var(--esp-move); }
        .marker-label.static { color: var(--esp-static); }
        .marker-label.hidden { opacity: 0; }
        .empty {
          color: var(--esp-muted);
          font-size: 0.9rem;
          padding: 8px 0;
        }
        .footer {
          margin-top: 8px;
          font-size: 0.7rem;
          color: var(--esp-muted);
        }
      </style>
      <ha-card>
        <div class="header">
          <div class="title"></div>
          <div class="badge"></div>
        </div>
        <div class="stats">
          <div class="stat move">
            <span class="label">运动</span>
            <span class="value">—</span>
            <span class="energy"></span>
          </div>
          <div class="stat static">
            <span class="label">静止</span>
            <span class="value">—</span>
            <span class="energy"></span>
          </div>
        </div>
        <div class="ruler-wrap">
          <div class="marker-label move hidden"></div>
          <div class="marker-label static hidden"></div>
          <div class="track">
            <div class="marker move hidden"></div>
            <div class="marker static hidden"></div>
          </div>
          <div class="ticks"></div>
        </div>
        <div class="footer"></div>
      </ha-card>
    `;

    this._els = {
      title: this._root.querySelector(".title"),
      badge: this._root.querySelector(".badge"),
      moveVal: this._root.querySelector(".stat.move .value"),
      moveEn: this._root.querySelector(".stat.move .energy"),
      staticVal: this._root.querySelector(".stat.static .value"),
      staticEn: this._root.querySelector(".stat.static .energy"),
      moveMk: this._root.querySelector(".marker.move"),
      staticMk: this._root.querySelector(".marker.static"),
      moveLb: this._root.querySelector(".marker-label.move"),
      staticLb: this._root.querySelector(".marker-label.static"),
      ticks: this._root.querySelector(".ticks"),
      footer: this._root.querySelector(".footer"),
      card: this._root.querySelector("ha-card"),
    };

    this._buildTicks();
    this._update();
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

    // number.*_max_* 多为米；距离传感器多为 cm —— 自动对齐量程
    const unit = this._unit();
    if ((unit === "cm" || unit === "mm") && max <= 20) {
      max = max * (unit === "mm" ? 1000 : 100);
    }
    return max;
  }

  _buildTicks() {
    const max = this._maxDistance();
    const steps = 5;
    const parts = [];
    for (let i = 0; i <= steps; i++) {
      const v = (max * i) / steps;
      parts.push(`<span>${v % 1 === 0 ? v.toFixed(0) : v.toFixed(1)}</span>`);
    }
    this._els.ticks.innerHTML = parts.join("");
    this._lastMax = max;
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
    const show = present !== false && pct != null && distance > 0;
    mk.classList.toggle("hidden", !show);
    lb.classList.toggle("hidden", !show);
    if (!show) return;
    const size = this._sizeFromEnergy(energy);
    mk.style.left = `${pct}%`;
    mk.style.width = `${size}px`;
    mk.style.height = `${size}px`;
    mk.style.opacity = String(this._opacityFromEnergy(energy));
    lb.style.left = `${pct}%`;
    lb.textContent = this._fmtDist(distance);
  }

  _unit() {
    // X-RA2413MT 距离为 cm；仅当配置显式写 unit: m 时才用米
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

  _update() {
    if (!this._els || !this._config) return;

    const max = this._maxDistance();
    if (this._lastMax !== max) this._buildTicks();

    this._els.title.textContent = this._config.title || "ESPMMW";

    const present = this._present();
    const badge = this._els.badge;
    if (present === null) {
      badge.textContent = "无实体";
      badge.className = "badge";
    } else if (present) {
      badge.textContent = "有人";
      badge.className = "badge on";
    } else {
      badge.textContent = "无人";
      badge.className = "badge off";
    }

    const md = this._num(this._config.move_distance);
    const me = this._num(this._config.move_energy);
    const sd = this._num(this._config.static_distance);
    const se = this._num(this._config.static_energy);

    this._els.moveVal.textContent = this._fmtDist(md);
    this._els.moveEn.textContent = this._fmtEnergy(me);
    this._els.staticVal.textContent = this._fmtDist(sd);
    this._els.staticEn.textContent = this._fmtEnergy(se);

    this._placeMarker(this._els.moveMk, this._els.moveLb, md, me, present, max);
    this._placeMarker(this._els.staticMk, this._els.staticLb, sd, se, present, max);

    const missing = [
      this._config.presence,
      this._config.move_distance,
      this._config.static_distance,
    ].filter((id) => id && !this._state(id));

    const u = this._unit();
    this._els.footer.textContent = missing.length
      ? `未找到: ${missing.join(", ")}`
      : `量程 0 – ${max} ${u}`;
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
