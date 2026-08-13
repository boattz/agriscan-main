# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Agriscan is an IoT system for real-time soil monitoring. An ESP32 reads soil sensors (moisture, temperature, EC, pH, NPK) via RS485 Modbus and serves a real-time dashboard. A Flask backend provides mock data for development/deployment.

## Commands

```bash
# Backend
pip install -r backend/requirements.txt
cd backend && python app.py          # dev (port 5000)
cd backend && gunicorn app:app       # prod

# Dashboard
open dashboard/index.html            # browser, no build step

# ESP32
# Flash via Arduino IDE with ModbusMaster library
```

No linter, no tests, no build tooling.

## Architecture

```
esp32/agriscan/
  agriscan.ino          ← firmware: RS485 sensor read + WiFi + WebServer
  dashboard.h           ← embedded dashboard as C++ raw string literal

dashboard/
  index.html            ← standalone browser dashboard
  script.js             ← polling, IP fallback chain, recommendations
  style.css             ← dark green glassmorphism theme

backend/
  app.py                ← Flask mock data API (used for Render.com deploy)
```

**Data flow:** ESP32 polls sensor → serves JSON at `/data` → dashboard polls every 5s.

**Fallback chain** (in `script.js`): custom IP from localStorage → working URL → page origin → agriscan.local → 192.168.1.1 → localhost:5000 (mock).

**ESP32 is self-contained:** `dashboard.h` embeds the full dashboard HTML/CSS/JS as a C++ string literal, served directly from the microcontroller.

## Key Files

- `esp32/agriscan/agriscan.ino` — main firmware
- `esp32/agriscan/dashboard.h` — embedded dashboard (keep in sync with `dashboard/`)
- `dashboard/script.js` — client-side logic, IP management, sensor recommendations
- `backend/app.py` — mock API, returns random sensor values

## Dependencies

- **ESP32**: ModbusMaster (Arduino library)
- **Backend**: Flask, flask-cors, gunicorn only — no heavy ML/data libs despite legacy `requirements.txt`

## Conventions

- UI language: Thai
- No framework — vanilla HTML/CSS/JS for dashboard
- ESP32 uses Arduino WebServer library
- CORS is open (`*`) — intentional for local network access
- **Keep `agriscan-presentation.html` updated** — whenever project files, thresholds, architecture, or behavior change, sync the presentation doc (sensor table, data flow, recommendations, limitations) with the new reality.
