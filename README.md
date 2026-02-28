# carpi

**Raspberry Pi Zero 2 W** running in the car as the WiFi gateway and MQTT broker for the Funkbox pit-radio system.

## Hardware

- **Board**: Raspberry Pi Zero 2 W
- **OS**: Debian 13 (Trixie), `hostname: carpi`
- **Network**: WiFi only (`wlan0`, `10.0.0.211`)
- **Storage**: 59 GB SD card

## Role

Acts as the WiFi access point and MQTT broker inside the car. ESP32 button box units connect to it over WiFi and publish button events to the `funkbox/buttons` topic. The carpi auto-detected as the broker via the WiFi gateway IP.

## Setup

### MQTT Broker

Mosquitto 2.0.21 is installed and enabled on carpi. It starts automatically on boot.

```sh
# Install (already done)
sudo apt install mosquitto mosquitto-clients
sudo systemctl enable --now mosquitto

# Check status
systemctl status mosquitto
```

Config file: `/etc/mosquitto/mosquitto.conf`

### Verify

```sh
mosquitto_sub -h localhost -t "funkbox/buttons" -v
```

Expected messages:

```json
{"button": "PIT", "state": "PRESSED"}
{"button": "FUEL", "state": "RELEASED"}
```
