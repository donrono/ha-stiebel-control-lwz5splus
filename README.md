# ha-stiebel-control-lwz5splus

A project that enables monitoring and controlling the Stiebel Eltron LWZ5s Plus heat pump from Home Assistant via MQTT. 

ESPHome firmware for monitoring and controlling Stiebel Eltron heat pumps via CAN bus.
Connects an ESP32 to the heat pump's internal Elster CAN bus and automatically publishes
signals to Home Assistant via MQTT discovery — no manual entity configuration needed.

Based on the Elster protocol by [Jürg Müller](http://juerg5524.ch/list_data.php) and
community work by [roberreiters](https://community.home-assistant.io/t/configured-my-esphome-with-mcp2515-can-bus-for-stiebel-eltron-heating-pump/366053) and
[bullitt186].

---

## Supported Hardware

| Option | Board | CAN interface | Recommended |
|--------|-------|---------------|-------------|
| ESP32-S3 | Waveshare ESP32-S3-RS485-CAN (or similar) | Built-in TWAI | ✅ Yes |