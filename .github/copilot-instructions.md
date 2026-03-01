# Copilot Instructions — funkbox-buttons

## Project Overview

ESP-IDF project (requires ≥5.5.2) targeting **ESP32-C6** (RISC-V). This is a pit-radio button box for motorsport ("funkbox") that reads physical button presses (PIT, FUEL, FCK, STINT, ALARM) and publishes them as JSON messages over MQTT to a broker running on the car's Raspberry Pi gateway (`carpi`).

There is a "primary" and "secondary" variant per unit, controlled via the `PRIMARY` CMake build flag.

## Build Commands

Requires ESP-IDF v5.5.2+ environment (`idf.py` must be on PATH via `export.sh` / `export.bat`).

```sh
# Standard build (secondary unit)
idf.py build

# Build for primary unit
idf.py -DPRIMARY=1 build

# Flash and monitor
idf.py -p COMx flash monitor

# Clean build
idf.py fullclean
```

The `PRIMARY` CMake flag controls WiFi SSID/password suffixes: the base credentials (defined in `CMakeLists.txt` as `CONFIG_SSID`/`CONFIG_PWD`) get `"prim"` appended for primary units and `"sec"` for secondary units (see `wlan.c:extract_credentials()`).

## Architecture

**Boot sequence** (`main.c`): NVS init → netif init → event loop → `led_status_init` → `wlan_start` (blocks until connected or reboots) → `mqttcomm_start` → `button_handler_start`.

**Communication flow**: Physical GPIO buttons → `button_handler` (espressif/button component, `BUTTON_PRESS_DOWN` event) → `broadcaster_send_button` → `mqttcomm_publish` → MQTT topic `funkbox/buttons` on the gateway broker.

**Message format**: `{"button":"PIT","state":"PRESSED"}` — button names: `PIT`, `FUEL`, `FCK`, `STINT`, `ALARM`; states: `PRESSED`, `DEPRESSED`.

**Key modules**:
- `wlan.c` — WiFi STA connection; blocks in `app_main` until IP obtained, reboots on failure
- `mqttcomm.c` — MQTT client; broker URI auto-derived from WiFi gateway IP (`mqtt://<gw_ip>`); reconnects every 5s on failure
- `led_status.c` — Status LED on GPIO 8; solid ON = MQTT connected, flashing ~2Hz = disconnected
- `button_handler.c` — Registers 5 GPIO buttons using `espressif/button` component; only `BUTTON_PRESS_DOWN` is handled (no release events currently)
- `funkbox_types.h` — Shared `ButtonName` / `ButtonState` enums
- `status_broadcaster.c` — Formats and publishes JSON button events via `mqttcomm`

## Conventions

- All source files must be listed explicitly in `main/CMakeLists.txt` `idf_component_register(SRCS ...)` — add new `.c` files there or they won't compile
- Button GPIO pin mapping is hardcoded in `button_handler.c` — GPIO 18=ALARM, 19=STINT, 20=FCK, 21=FUEL, 14=PIT
- LED status pin (GPIO 8) is hardcoded in `led_status.c`
- MQTT topic `funkbox/buttons` is defined as a `#define` in `status_broadcaster.c`
- MQTT broker address is auto-detected at runtime from the WiFi gateway IP — no hardcoded broker address
- WiFi base credentials (`CONFIG_SSID` / `CONFIG_PWD`) are hardcoded in the root `CMakeLists.txt` via `add_compile_definitions`; the `PRIMARY` flag appends `"prim"` or `"sec"` to both values at runtime in `wlan.c`
- ESP-IDF component dependencies are managed via `main/idf_component.yml` (espressif component registry)
- Custom partition table in `partitions.csv` includes OTA slots (ota_0, ota_1 × 1M each), Zigbee storage, and NVS
