@.github/copilot-instructions.md

---

## Claude Code — Development Commands

Run these `make` targets from the repo root:

| Target | What it does |
|--------|-------------|
| `make compile` | Compile the ESP32-S3 variant |
| `make check` | Unit tests + compile — full quality gate |
| `make logs` | Stream live logs from connected device (ESP32-S3 config) |
| `make upload` | Compile and OTA-flash to production device (heatingpump.local) |
| `make config` | Dump merged YAML (useful for debugging package includes) |
| `make smoke-test` | MQTT regression test: verify all required signals appear within 120s |
| `make capture-baseline` | Capture new MQTT baseline for current model (300s, overwrites model file) |

**Before uploading**: always run `make check` first. Verify flash usage stays <50% in the compile output.

## After OTA flash — mandatory checklist

After every `make upload`, run these steps in order:

1. Watch `heatingpump/status` go `offline` → `online` (within ~60s):
   ```bash
   mosquitto_sub -h 192.168.30.10 -u iotuser \
     --pw "$(grep mqtt_password esphome/secrets.yaml | sed 's/mqtt_password: *//')" \
     -t "heatingpump/status" -v
   ```
2. Run the MQTT smoke test — **must pass before considering the flash successful**:
   ```bash
   make smoke-test
   ```
3. Check HA: `sensor.heatingpump_sg_ready_aktueller_zustand` still shows a value.

If `make smoke-test` fails, check `heatingpump/debug` for errors and investigate before declaring success.

## Secrets

`esphome/secrets.yaml` contains real WiFi/MQTT credentials — it is gitignored and must **never** be committed. For CI, a credential-free stub lives at `esphome/ci_secrets.yaml`.

## MCP Tools Available

- `mcp__mqtt__*` — publish/receive MQTT messages to verify sensor values after a firmware change
- `mcp__home-assistant__*` — check entity states in HA after deploy

---

## Project state and conventions (LWZ 5S Plus)

### Scope
- Only the **LWZ 5S Plus** (`lwz5splus.yaml` / `signal_requests_lwz5splus.h`) and the **ESP32-S3 with
  built-in TWAI** are supported. WPL13E, WPF10 and ESP32-S2/MCP2515 were removed — do not re-add them.
- The ESP sends as `PC` with CAN id **0x69F** (`can_id_pc` in `heatingpump.yaml`, injected as
  `-DHA_CAN_ID_PC`). **0x680 does not work** on this heat pump — never use it.
- All signals are read from / written to **KESSEL (0x180)** unless a probe proves otherwise.
  MANAGER / HEIZMODUL do not exist on this bus.

### Source of truth for signals
- `tmp/mqtt.yaml` (validated entities of the user's previous setup) and `tmp/StiebelEltron.json`
  (Elster index, Sender, Converter, Unit, German names) are the **verified truth**.
  Join them via the MQTT topic (`lwz5splus` + `MqttTopic`). `ElsterTable.h` names from the generic
  Elster table are often wrong for this model — rename to the meaning from mqtt.yaml/JSON.
- Known JSON error: "Sommerzeit Manuell Ende (Tag/Monat)" is listed as 0xFDD0/0xFDD1, but the heat pump
  only answers 0xFDE0/0xFDE1 (right after Beginn 0xFDDE/0xFDDF). If an entity stays `unknown`, check
  whether the index answers at all (probe) before trusting the JSON.
- `FEHLERMELDUNG` (0x0001): the **high byte** is the fault code shown on the display (0x0F0A → 15),
  the low byte is extra info. The code texts in `ErrorDescription[]` follow the official Stiebel
  "Inbetriebnahme und Meldungsliste" (LWZ 5 S Plus / LWZ 8 CS); the mapping in `tmp/mqtt.yaml` is
  wrong from code 5 on (e.g. 15 = Warmwassertemperatur, not "Durchflussproblem").
- Signal names: derived from the mqtt.yaml `name:` (upper case, ASCII: ä→AE, ö→OE, ü→UE, ß→SS,
  non-alphanumerics → `_`). Meter variants get the unit as suffix (`…_WH`, `…_KWH`, `…_MWH`).
- Data types in `ElsterTable.h` must follow the JSON `Converter`: Dec→`et_dec_val`, Cent→`et_cent_val`,
  Bool→`et_bool`, LittleEndian→`et_little_endian`, TimeRange→`et_time_domain`,
  TimeRangeLittleEndian→`et_time_domain_le`, Time (quarter hours)→`et_time_quarter`.
  A wrong type writes wrong values to the heat pump (e.g. 55 °C sent as 5.5 °C).
- When a signal is renamed in `ElsterTable.h`, rename it in `signal_requests_lwz5splus.h`,
  the writable arrays in `ha-stiebel-control.h` and the topics in `common.yaml` as well.
- Every entry in `signal_requests_lwz5splus.h` carries `// 0x<index> <German name>` as comment.
- Requested signals need `friendlyName` and `hasMetadata = true` in `ElsterTable.h`, otherwise HA
  shows the raw signal name.

### CAN frames
- Reads and writes always use the **extended index form** (`0xFA` + 16-bit index), as can2mqtt does.
  The LWZ 5S Plus and its BEDIENMODUL_2 do not answer the short form (index < 0x100 in byte 2).
- Frame type = low nibble of byte 0: 0 write, 1 read, 2 response (see can2mqtt `CanFrame.cs`).
- BEDIENMODUL_2 (0x301) answers the room / heating-circuit-1 signals itself; the KESSEL does not.
- Device limits are **not** readable: the min/max reads with switch byte `0xF8`/`0xF9` instead of `0xFA`
  (upstream PR bullitt186/ha-stiebel-control#90, measured on a WPL-A 07) got no answer on the LWZ 5S Plus.
  Test 2026-11-25: 216 read requests (0xF8 + 0xF9 for every `writableNumbers[]` entry except the virtual
  SG Ready numbers, each sent to its configured member KESSEL or BEDIENMODUL_2), run twice, 0 answers; a
  raw-frame log of the second run showed no frame with the probed index or a switch byte 0xF7–0xF9.
  The ranges in `writableNumbers[]` stay static (JSON `MinValue`/`MaxValue`, manual); don't port PR #90.
- Every writable entity needs a matching read request (same signal, same member) in
  `signal_requests_lwz5splus.h`, otherwise HA never gets its state.
- Signals from the probe of 2026-10-02: only those verified via `tmp/StiebelEltron.json` are requested;
  the others are commented out in `signal_requests_lwz5splus.h` (raw probe value + research confidence
  in the comment). Re-enable one only after its meaning, type and unit are confirmed.
- Jürg's generic Elster table has names shifted by one index (n → n−1) in 0x0295–0x073B, from 0xC350
  and from 0xFDBA. Unverified names in these ranges were corrected after ElsterHeatingBridge
  (`KElsterTableInc.cs`, commit 29011ef); verified signals keep their names.

### Internal heap
- The ESPHome PSRAM setup uses `CONFIG_SPIRAM_USE_CAPS_ALLOC`: `malloc`/`std::` containers always use
  internal RAM. Avoid per-signal `std::string` maps/sets (one string key ≈ 80–150 B × ~450 signals).
  Current free internal heap after boot ≈ 140 kB; self-heal reboots below 20 kB.

### Writable entities
- Arrays in `ha-stiebel-control.h`: `writableNumbers[]`, `writableSelects[]` (optional value table
  option ↔ CAN value), `writableSwitches[]`, `writableButtons[]`, `writableTexts[]`.
- Commands arrive via **one wildcard subscription** `heatingpump/+/+/set` (set up in `mqtt.on_connect`);
  each topic has its own `script` `set_<MEMBER>_<SIGNAL>` in `common.yaml` (payload = parameter `x`).
  New writable entity: array entry + script + line in the `commandHandlers` map.
- Discoveries of writable entities are published by the **discovery pump** (`DISCOVERY_PUMP_BATCH`
  per second, retry on failure). Never publish many MQTT messages in one loop: the ESPHome MQTT
  client queues only **30 outbound / 32 inbound** events, a burst is silently lost.
- Buttons without read-back use `writeSignalNoAck()`; all other writes are tracked by the
  availability sensor (answer expected within 15 s).

### Runtime features
- Availability sensor `heatpump_available` (+ status text): CAN liveness, heat pump clock vs. SNTP/HA
  time, answered writes. Self healing: CAN restart after 10 min silence, reboot after 30 min or on
  low heap. Thresholds `AVAILABILITY_*` / `SELF_HEAL_*` in `config.h`.
- Read-only signal probe: publish to `heatingpump/probe/start` → results on
  `heatingpump/probe/result`, summary on `heatingpump/probe/done`.
- `processAndUpdate()` ignores Elster read requests (frame type 1) and unknown senders (OTHER).
- Per-request logs (`READ …`, `processCanMessage`) are `ESP_LOGD`; set `logger: level: DEBUG`
  temporarily to see them.

### Build, upload and verification (Windows)
- **Build and upload from PowerShell**, not from the Bash tool: in Git Bash `esphome compile`
  finished in ~6 s without compiling anything and reused stale binaries. A real build prints
  `Building … object` lines; check that and the exit code, not only "Successfully compiled".
  ```powershell
  cd esphome; esphome compile heatingpump.yaml; esphome upload heatingpump.yaml --device 192.168.178.158
  ```
- The device is at **192.168.178.158** (`wifi.use_address`). OTA uses the API encryption key
  (`ota: encryption:`), no OTA password.
- Unit tests (`make test`) cannot run on this machine (missing DLL); use
  `g++ -std=c++17 -I. -Itests -Iesphome/ha-stiebel-control -Iesphome/ha-stiebel-control/elster -fsyntax-only tests/test_can_logic.cpp`
  as a compile check. Host stubs live in `tests/esphome_stubs.h`, `tests/driver/twai.h`,
  `tests/esp_heap_caps.h`, `tests/esp_system.h`.
- No `mosquitto_sub` here: read MQTT with paho from the ESPHome Python
  (`C:\Users\INIT1035\AppData\Local\Python\pythoncore-3.14-64\python.exe`), credentials from
  `esphome/secrets.yaml` (never print them). Live messages have `retain == False`.
- `make smoke-test` is not valid for the LWZ 5S Plus yet (`_common_smoke.json` expects MANAGER /
  HEIZMODUL topics, no `lwz5splus_smoke.json`).
- Upload and log analysis only after the user confirmed all pending changes.
- After every OTA, verify that the **new** build really runs: `esphome logs` prints
  `compiled on <time>` on API connect — check it again after > 60 s. A firmware that crashes before
  safe_mode marks the boot successful is rolled back silently (`OTA rollback detected!`), and the
  device keeps publishing with the old firmware ("online" alone proves nothing).
  Caveat: `compiled on` only changes when the YAML config changes; after a C++-only change it keeps
  the old timestamp. Then prove the new build by its behaviour (e.g. the fixed signal now answers) and
  by `safe_mode: Boot seems successful` in the fresh boot log.
- Never build large containers from initializer lists in lambdas (e.g. the `commandHandlers` table in
  `mqtt.on_connect`): the temporaries land on the 8 kB loop task stack. A `std::map<std::string, …>`
  with ~245 entries overflowed it and crashed the ESP at every MQTT connect; it is a plain
  `const char*` array now.
- Discoveries published before MQTT is connected (e.g. calculated sensors in `republishAllDiscoveries()`
  at boot) are lost; publish `heatingpump/republish_discoveries` to resend everything.

### Editing pitfalls seen in this project
- Files mix LF (`common.yaml`, `ha-stiebel-control.h`, `ElsterTable.h`, `signal_requests_lwz5splus.h`)
  and CRLF (`KElsterTable.*`, `config.h`, `lang_*.h`) — keep the existing line endings. In Git Bash
  `grep -c $'\r'` is not reliable for this; count with PowerShell.
- In PowerShell, pass concatenated replacement strings in parentheses (`Must $s $rx ('a'+$nl+'b')`);
  without them only the first part is used. Variable names are case-insensitive (`$l` overwrites `$L`).
- `$ErrorActionPreference='Stop'` turns ESPHome's stderr into a terminating error — don't use it
  around `esphome` calls.
