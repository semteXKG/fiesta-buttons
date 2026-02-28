# Copilot Instructions — funkbox-buttons

## Project Overview

ESP-IDF 5.4.0 project targeting **ESP32-C6** (RISC-V). This is a pit-radio button box for motorsport ("funkbox") that reads physical button presses (PIT, FUEL, FCK, STINT, ALARM) and broadcasts them as Protobuf-encoded messages over UDP multicast to a larger telemetry system.

Part of a multi-device ecosystem — there is a "primary" and "secondary" variant per car (FORD / VECTRA), controlled via CMake build flags.

## Build Commands

Requires ESP-IDF v5.4+ environment (`idf.py` must be on PATH via `export.sh` / `export.bat`).

```sh
# Standard build
idf.py build

# Build for primary unit
idf.py -DPRIMARY=1 build

# Build for Ford car variant
idf.py -DFORD=1 build

# Combined (primary Ford unit)
idf.py -DPRIMARY=1 -DFORD=1 build

# Flash and monitor
idf.py -p COMx flash monitor

# Clean build
idf.py fullclean
```

The `PRIMARY` and `FORD` CMake flags set compile-time defines that control WiFi SSID/password suffixes and naming. Default (no flags) builds a secondary Vectra unit.

## Architecture

**Boot sequence** (`main.c`): NVS init → netif init → event loop → WiFi STA connect → UDP multicast socket → button handler registration.

**Communication flow**: Physical GPIO buttons → `button_handler` (espressif/button component) → `status_broadcaster` (JSON serialize) → `mqttcomm` (MQTT publish to `funkbox/buttons` topic on gateway broker).

**Key modules**:
- `wlan.c` — WiFi STA connection with retry logic; credentials derived from `CONFIG_SSID`/`CONFIG_PWD` + primary/secondary suffix
- `mqttcomm.c` — MQTT client using ESP-IDF `esp_mqtt`; auto-detects broker from WiFi gateway IP, reconnects every 5s on failure
- `led_status.c` — Status LED on GPIO 8; solid ON when MQTT connected, flashing (~2Hz) when disconnected
- `button_handler.c` — Registers 5 GPIO buttons (pins 18–21, 14) using `espressif/button` component
- `button_types.h` — Shared `ButtonName` / `ButtonState` enums used by button_handler and status_broadcaster
- `status_broadcaster.c` — Serializes button events as JSON (`{"button":"PIT","state":"PRESSED"}`) and publishes via MQTT

**Custom component**: `components/net-logging` — Network-based logging (UDP/TCP/MQTT/HTTP) configurable via Kconfig.

**Static assets**: `data/` contains a SPIFFS web UI (HTML/CSS) for configuration, flashed to the `storage` partition.

## Conventions

- Protobuf-c generated files live in `main/proto/` — regenerate with `protoc-c` if the `.proto` file changes, don't hand-edit
- All source files are registered explicitly in `main/CMakeLists.txt` `SRCS` list — add new `.c` files there
- NVS partition is pre-populated from `nvs.csv` and flashed alongside the app
- Custom partition table in `partitions.csv` includes OTA slots, SPIFFS storage, and Zigbee storage
- Button GPIO pin mapping is hardcoded in `button_handler.c` — update there for hardware changes
- LED status pin (GPIO 8) is hardcoded in `led_status.c`
- MQTT topic `funkbox/buttons` is defined in `status_broadcaster.c`
- MQTT broker address is auto-detected from WiFi gateway IP at startup
- WiFi credentials use a pattern: base SSID/PWD from CMake defines + "prim"/"sec" suffix based on `PRIMARY` flag
- ESP-IDF component dependencies are managed via `main/idf_component.yml` (espressif component registry)
