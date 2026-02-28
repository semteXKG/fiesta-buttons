# fiesta-buttons

ESP32-C6 firmware for the **Funkbox** — a pit-radio button box used in motorsport. Physical buttons are read and published as MQTT messages over WiFi to a Raspberry Pi gateway running in the car.

## System Overview

```
[Button Box (ESP32-C6)]  --WiFi MQTT-->  [carpi (Raspberry Pi Zero 2 W)]  --> telemetry system
```

- **fiesta-buttons** — this repo; firmware running on the ESP32-C6 in the button box
- **carpi** — Raspberry Pi Zero 2 W (`hostname: carpi`, `10.0.0.211`), acts as the WiFi access point and MQTT broker in the car. The ESP32 auto-detects the broker from the WiFi gateway IP at boot.

Two car variants are supported: **Ford** and **Vectra**. Each car has a **primary** and **secondary** button box unit.

## Hardware

- **MCU**: ESP32-C6 (RISC-V)
- **Buttons** (5): PIT, FUEL, FCK, STINT, ALARM — GPIO pins 18, 19, 20, 21, 14
- **Status LED**: GPIO 8 — solid when MQTT connected, flashing (~2 Hz) when disconnected
- **Storage**: SPIFFS partition with a web UI for configuration

## Button Events

Events are published to the MQTT topic **`funkbox/buttons`** as JSON:

```json
{"button": "PIT", "state": "PRESSED"}
{"button": "PIT", "state": "RELEASED"}
```

**Button names**: `PIT`, `FUEL`, `FCK`, `STINT`, `ALARM`  
**States**: `PRESSED`, `RELEASED`

## Build

Requires [ESP-IDF v5.4+](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/get-started/index.html).

```sh
# Secondary Vectra unit (default)
idf.py build

# Primary unit
idf.py -DPRIMARY=1 build

# Ford variant
idf.py -DFORD=1 build

# Primary Ford unit
idf.py -DPRIMARY=1 -DFORD=1 build

# Flash and monitor
idf.py -p COMx flash monitor

# Clean
idf.py fullclean
```

## Configuration Flags

| CMake Flag | Effect |
|---|---|
| *(none)* | Secondary Vectra unit |
| `PRIMARY=1` | Primary unit (changes WiFi SSID/password suffix to `prim`) |
| `FORD=1` | Ford car variant (changes WiFi credentials) |

WiFi credentials are derived at compile time from `CONFIG_SSID` / `CONFIG_PWD` base values plus a `prim` or `sec` suffix based on the `PRIMARY` flag.

## Partitions

Custom partition table (`partitions.csv`):
- App (with OTA slots)
- `storage` — SPIFFS web UI (`data/`)
- NVS — pre-populated from `nvs.csv`
- Zigbee storage

## carpi Setup

The **carpi** (Raspberry Pi Zero 2 W) is a fresh Debian 13 (Trixie) install. It needs an MQTT broker installed to receive button events from the ESP32 units:

```sh
sudo apt install mosquitto mosquitto-clients
sudo systemctl enable --now mosquitto
```

Subscribe to button events:
```sh
mosquitto_sub -h localhost -t "funkbox/buttons" -v
```

## Project Structure

```
main/
├── main.c                 # Boot sequence: NVS → WiFi → MQTT → buttons
├── wlan.c / .h            # WiFi STA connection
├── mqttcomm.c / .h        # MQTT client (auto-detects broker from gateway IP)
├── button_handler.c / .h  # GPIO button registration (espressif/button component)
├── button_types.h         # ButtonName / ButtonState enums
├── status_broadcaster.c   # Serialises events to JSON, publishes via MQTT
├── led_status.c / .h      # Status LED control
└── idf_component.yml      # Component dependencies
components/
└── net-logging/           # Network logging component (UDP/TCP/MQTT/HTTP)
data/                      # SPIFFS web UI (HTML/CSS)
```
