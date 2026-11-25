# Changelog

All notable changes are documented here.
Format: [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Versions: [SemVer](https://semver.org/).

---

## [2.1.1_mod_ron]

### Removed

- **WPL 13 E support** — `wpl13e.yaml`, `signal_requests_wpl13e.h` and `tests/models/wpl13e_smoke.json` are gone. `lwz5splus.yaml` is the reference model; the CI entry points use it.
- **WPF10 and ESP32-S2 / MCP2515 support** — model package, signal table and smoke stub for WPF10, `board_esp32s2.yaml`, `can_mcp2515.yaml`, `make compile-s2` and the CI S2 job are gone. ESP32-S3 (TWAI) is the only supported board.
- SG Ready entities and topics moved from `MANAGER` to `KESSEL` (`heatingpump/KESSEL/SG_READY_*`).
- Manager-based DHW setpoint writables (`WW_SOLL_TAG` / `EINSTELL_SPEICHERSOLLTEMP2` on `MANAGER`). The DHW setpoint is written to `KESSEL`; SG Ready writes to `KESSEL` as well.

### Added

- Writable MQTT entities for LWZ5SPLUS: numbers, selects (value tables), switches, buttons (reset buttons disabled by default) and texts (switching programs as "HH:MM-HH:MM" / "---").
- Calculated sensors `heatpump_available` (binary_sensor, connectivity) and `heatpump_availability_status`: the heat pump counts as available if it sends CAN messages, its clock deviates by at most 5 min from Home Assistant time and it answers our writes. Thresholds in `config.h` (`AVAILABILITY_*`). Needs the new `time:` component in `common.yaml` (SNTP, Europe/Berlin, plus homeassistant sync).

- Self healing: the CAN controller is restarted if the heat pump (KESSEL) was alive but is silent for 10 min, the ESP reboots after 30 min of silence or if the free internal heap stays below 20 kB for 5 min. No reboot if the heat pump never answered since boot (no reboot loops). Thresholds in `config.h` (`SELF_HEAL_*`).
- Diagnostic sensors: free internal heap, largest free heap block, minimum free heap since boot, uptime and number of automatic CAN restarts.

- Writable entity discoveries are published paced (`DISCOVERY_PUMP_BATCH` per second) with retry instead of in one burst after the MQTT connect. The ESPHome MQTT client queues only 30 outbound messages, the burst of ~150 retained discoveries was lost, so number/switch/button/text entities were missing in Home Assistant until the next 15 min republish.

- 194 signals that answered a read-only probe of the KESSEL (generic Elster names, not verified yet).
- Calculated sensor `VERDICHTERSTARTS_GESAMT` on the KESSEL device (MQTT) instead of an ESPHome template sensor.
- Read-only signal probe (`heatingpump/probe/start`).

### Fixed

- Reads and writes always use the extended index form (0xFA); short-form requests were not answered.
- Buffer overflow in `processCanMessage()` (16-byte value buffer, "not used time domain" is 20 chars) that rebooted the device.
- Internal heap: request schedule as a vector, no uid cache, discovered signals as 32-bit keys (24 kB → 140 kB free).
- Read requests of other bus members and frames from unknown senders are ignored.
- One wildcard subscription for all `.../set` command topics (the per-topic subscriptions overflowed the MQTT event queue).

### Changed

- Faster startup of the signal request manager: all requested signal names are resolved in one pass over the Elster table (`PreloadElsterNameCache`), 72 ms instead of ~1.4 s.
- Sensor `TEMPORALE_LUEFTUNGSSTUFE_DAUER` has the unit `min` (as the number entity).

- `EINSTELL_SPEICHERSOLLTEMP` (0x0013) renamed to `WW_SOLL_TAG`; topics and entity ids change accordingly.

---