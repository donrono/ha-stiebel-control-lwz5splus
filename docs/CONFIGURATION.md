# Configuration Reference

---

## `secrets.yaml` Keys

All keys are required unless marked optional.

| Key | Description | Example |
|-----|-------------|---------|
| `wifi_ssid` | Wi-Fi network name | `"MyNetwork"` |
| `wifi_password` | Wi-Fi password | `"MyPassword"` |
| `mqtt_broker` | MQTT broker IP or hostname | `192.168.1.10` |
| `mqtt_username` | MQTT username | `"iotuser"` |
| `mqtt_password` | MQTT password | `"secret"` |
| `api_encryption_key` | 32-byte base64 key for ESPHome native API | `"abc...=="` |

Generate `api_encryption_key`:
```bash
python3 -c "import base64, os; print(base64.b64encode(os.urandom(32)).decode())"
```

The key must match what HA's ESPHome integration has stored for this device. If you change it, you need to re-adopt the device in HA (Settings → Devices & Services → ESPHome → delete entry, then re-add).

---

## `heatingpump.yaml` Substitutions

| Substitution | Description | Default |
|--------------|-------------|---------|
| `device_name` | ESPHome device hostname | `heatingpump` |
| `friendly_name` | Display name in HA | `"Stiebel Eltron Wärmepumpe"` |
| `device_model` | Heat pump model — selects signal table and HA device identifier | `"lwz5splus"` |
| `language` | Display language for HA entity names: `DE` (German, default) or `EN` (English) | `"DE"` |
| `can_tx_pin` | CAN TX GPIO (ESP32-S3 built-in TWAI) | board-specific — see note below |
| `can_rx_pin` | CAN RX GPIO (ESP32-S3 built-in TWAI) | board-specific — see note below |
| `can_id_pc` | CAN bus ID for the ESP32 (PC role) | `"0x69F"` (0x680 does not work) |

> **Pin assignments are board-specific.** Always verify against your board's schematic
> before wiring. Known working values for common boards:
>
> **Waveshare ESP32-S3-RS485-CAN** (recommended board):
> `can_tx_pin: GPIO15`, `can_rx_pin: GPIO16`
>
> These are the values shipped in `heatingpump.yaml`. Other boards may use different pins.

---

## MQTT Topic Structure

All raw CAN signal values:
```
heatingpump/{CAN_MEMBER}/{SIGNAL_NAME}/state     ← read
heatingpump/{CAN_MEMBER}/{SIGNAL_NAME}/set       ← write (writable signals only)
```

Examples:
```
heatingpump/MANAGER/PROGRAMMSCHALTER/state       → "Tagbetrieb"
heatingpump/MANAGER/WW_SOLL_TAG/set ← "55"
heatingpump/KESSEL/SPEICHERISTTEMP/state         → "51.8"
```

CAN members: `MANAGER` (0x480), `KESSEL` (0x180), `HEIZMODUL` (0x500)

Availability:
```
heatingpump/status    → "online" or "offline"
```

---

## Polling Frequencies

Edit `esphome/ha-stiebel-control/signal_requests_lwz5splus.h` (or your model's file):

```cpp
const SignalRequest signalRequests[] = {
    {"AUSSENTEMP",   FREQ_30S,  cm_kessel},    // every 30 seconds
    {"HEIZKURVE",    FREQ_10MIN, cm_manager},   // every 10 minutes
};
```

Available frequency constants:

| Constant | Value |
|----------|-------|
| `FREQ_10S` | 10 seconds |
| `FREQ_30S` | 30 seconds |
| `FREQ_1MIN` | 60 seconds |
| `FREQ_5MIN` | 5 minutes |
| `FREQ_10MIN` | 10 minutes |
| `FREQ_30MIN` | 30 minutes |
| `FREQ_60MIN` | 60 minutes |

---

## Blacklisting Signals

To permanently suppress a signal (prevent it from appearing in HA), set `isBlacklisted: true`
in its `ElsterTable.h` entry:

```cpp
{ "NOISY_SIGNAL", 0x0123, et_default, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  true,   // ← isBlacklisted
  false }
```

Blacklisted signals are skipped before any processing — they never generate MQTT messages
or HA entities.
