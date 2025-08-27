# ESP32 WS2812 Afterburner — Full Spec for Cursor AI

**Version:** 1.0 • **Date:** 2025‑08‑27 • **Target:** Generate production‑ready firmware (ESP32 + WS2812B) and a React Native controller app via Cursor AI.

---

## 0) Executive summary
Build a throttle‑reactive LED afterburner for an RC jet using an **ESP32** (Arduino framework) driving a **single WS2812B ring (45 LEDs)**. Provide a **BLE (Bluetooth Low Energy)** control channel to live‑tune colors, modes, brightness, and afterburner threshold from a **React Native** app. Persist presets in NVS. The app scans/connects to the device, exposes color pickers and a few animation modes, and can push/save settings.

Cursor should generate:
- ESP32 firmware (Arduino/PlatformIO), with **FastLED**, **NimBLE‑Arduino**, **Preferences**.
- React Native app (TypeScript) using **react‑native‑ble‑plx** (or **@dotintent/react‑native‑ble‑plx**), with a simple UI (color pickers, sliders, mode buttons) and BLE write/notify plumbing.

---

## 1) Goals & non‑goals
**Goals**
1. Smooth throttle‑based color morph **start → end** with flicker.
2. Distinct **Afterburner (AB)** overlay above a threshold; at max throttle the palette should land near **purple**.
3. BLE control for **Mode (3 variants)**, **Start/End color**, **Speed**, **Brightness**, **AB Threshold**, **Num LEDs**; **Save Preset** to NVS.
4. Stable PWM throttle read on **GPIO 34**; optional CRSF/SBUS later (out of scope for v1).
5. Clean, documented code and project structure suitable for Cursor autogeneration and iteration.

**Non‑goals (v1)**
- OTA updates, bonding/whitelist security hardening (recommendations included, not implemented).
- Multi‑ring choreography.

---

## 2) Hardware requirements
**Bill of Materials**
- ESP32 Dev Board (e.g., ESP32‑DevKitC or WROOM32 dev board).
- WS2812B LED ring, **45 LEDs** (or strip arranged circularly).
- **5 V UBEC/DC‑DC** rated **≥ 5 A** (headroom). Worst‑case white: 45 × 60 mA ≈ 2.7 A.
- **74HCT245** (or **74HCT14**) 3.3→5 V level shifter for WS2812 data.
- **330 Ω** series resistor on LED DIN.
- **1000 µF** electrolytic (≥6.3 V) across 5 V at ring.
- Wires, heat‑shrink, optional 3D‑printed mount/diffuser.

**Power & wiring**
- 5 V UBEC → ESP32 **VIN/5V** and LED **5 V** (star topology). Common **GND** among UBEC, ESP32, LEDs, receiver.
- Data: ESP32 **GPIO 18** → level shifter → **330 Ω** → LED **DIN**.
- Throttle PWM in: RX **THR PWM** → ESP32 **GPIO 34** (input‑only OK). GND common.
- Keep LED data short (< 20 cm). Place 1000 µF at ring power pads. Use thick 5 V/GND.

**Thermal/EMI**
- Keep LEDs away from turbine exhaust heat; add venting/diffuser. Route LED/data away from ignition/ECU HV.

---

## 3) BLE GATT design
**Service UUID:** `b5f9a000-2b6c-4f6a-93b1-2f1f5f9ab000`  (Afterburner Service)

| Characteristic | UUID | Type | Props | Notes |
|---|---|---|---|---|
| `mode` | `b5f9a001-2b6c-4f6a-93b1-2f1f5f9ab001` | `u8` | R/W | 0=Linear, 1=Ease, 2=Pulse |
| `startColor` | `b5f9a002-2b6c-4f6a-93b1-2f1f5f9ab002` | `RGB` (3×u8) | R/W | e.g. `FF 64 00` |
| `endColor` | `b5f9a003-2b6c-4f6a-93b1-2f1f5f9ab003` | `RGB` (3×u8) | R/W | e.g. `9A 00 FF` |
| `speedMs` | `u16le` | `b5f9a004-2b6c-4f6a-93b1-2f1f5f9ab004` | R/W | 100–5000 ms/cycle |
| `brightness` | `u8` | `b5f9a005-2b6c-4f6a-93b1-2f1f5f9ab005` | R/W | 10–255 cap (maps to FastLED.setBrightness) |
| `numLeds` | `u16le` | `b5f9a006-2b6c-4f6a-93b1-2f1f5f9ab006` | R/W | default 45 |
| `abThreshold` | `u8` (0–100) | `b5f9a007-2b6c-4f6a-93b1-2f1f5f9ab007` | R/W | e.g. 80 (%) |
| `savePreset` | `u8` | `b5f9a008-2b6c-4f6a-93b1-2f1f5f9ab008` | W | write `1` to persist to NVS |
| `status` | UTF‑8 JSON | `b5f9a009-2b6c-4f6a-93b1-2f1f5f9ab009` | Notify/R | e.g. `{"thr":0.73,"mode":1}` every 200 ms |

**Notes**
- All multi‑byte ints **little‑endian**. Colors are **raw RGB** (0–255 each).
- Device name: `ABurner`. Tx power: medium (P7). Notify period: ~200 ms.

---

## 4) Firmware (ESP32) requirements
**Framework & libs**
- Arduino framework via **PlatformIO**.
- Libraries: `FastLED`, `NimBLE-Arduino`, `Preferences` (bundled).

**Files / structure**
```
/firmware
  platformio.ini
  src/
    main.cpp                  // init, loop
    led_effects.h/.cpp        // core render + AB overlay, modes
    ble_service.h/.cpp        // service, chars, callbacks
    throttle.h/.cpp           // PWM read + smoothing
    settings.h/.cpp           // struct + NVS load/save
```

**Key behaviors**
- **Throttle read**: `pulseIn(THR_PIN=34, HIGH, 25000)`; map 1000–2000 µs → 0.0–1.0; low‑pass smoothing `alpha=0.10`.
- **Core render**: start→end color lerp; brightness proportional to throttle; Per‑pixel flicker via `inoise8`.
- **AB overlay**: triggers above `(abThreshold/100)`; adds purple‑biased core, optional halo; “roar” noise modulation.
- **Modes**:
  - `0 Linear`: straight interpolation.
  - `1 Ease`: ease curve `pow(thr,1.2)` for more low‑end detail.
  - `2 Pulse`: like Ease + periodic multiplicative pulse when in AB.
- **Brightness cap**: `FastLED.setBrightness(brightCap)` (10–255) from BLE.
- **Num LEDs change** dynamically reallocates the `CRGB*` buffer; re‑init FastLED chain.
- **Status notify**: JSON `{"thr":<float2>,"mode":<u8>}` ~5 Hz.
- **NVS**: on `savePreset=1`, persist all controllables. Load at boot.
- **Failsafe**: if PWM timeout, keep last smoothed throttle (no jump to 0).

**Constants (defaults)**
- `NUM_LEDS=45`, `LED_PIN=18`, `THR_PIN=34`, `BRIGHT_CAP=200`, `AB_THR=80%`, `SPEED_MS=1200`.

**platformio.ini (Arduino/ESP32)**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
build_flags = -DCORE_DEBUG_LEVEL=0
lib_deps =
  fastled/FastLED@^3.6.0
  h2zero/NimBLE-Arduino@^1.4.3
```

**Testing hooks**
- Optional `#define DEBUG_SERIAL` to print pulse widths & throttle.
- Add a `demo` flag (no PWM) to scrub throttle 0→1→0 for bench visuals.

---

## 5) React Native app requirements
**Stack**
- React Native (TypeScript). RN CLI or Expo (bare) acceptable.
- BLE lib: `react-native-ble-plx` (or scoped `@dotintent/react-native-ble-plx`).
- Color picker: any lightweight component or 3 numeric RGB fields.

**Files / structure**
```
/app
  src/
    screens/AfterburnerScreen.tsx
    ble/bleManager.ts          // singleton BleManager
    ble/uuids.ts               // service/characteristic UUIDs
    ble/device.ts              // connect, write, monitor helpers
    components/ColorInput.tsx  // RGB inputs with swatch
```

**UI/UX**
- Scan/Connect to device named `ABurner`.
- Show **Mode** (3 buttons), **Start/End color** pickers (RGB or picker), **Speed (ms)**, **Brightness**, **AB Threshold %**.
- Buttons: **Send** (per field), **Push All**, **Save Preset**. Show **Status** JSON.

**BLE specifics**
- Writes are **binary**; base64‑encode for RN BLE.
- Use **writeWithoutResponse** for speed; fall back to with‑response if needed.
- Subscribe to `status` notifications.

**Permissions**
- **Android 12+**: `BLUETOOTH_SCAN`, `BLUETOOTH_CONNECT` (Nearby Devices); older: location.
- **iOS**: `NSBluetoothAlwaysUsageDescription` in Info.plist.

**Dependencies (example)**
```json
{
  "dependencies": {
    "react": "^18.x",
    "react-native": "^0.7x.x",
    "@dotintent/react-native-ble-plx": "^2.x"
  }
}
```

---

## 6) Data model & protocols
**Endianness**: little‑endian for `u16`.
**Color encoding**: 3 bytes, **RGB** order (`[R,G,B]`).
**Status payload**: UTF‑8 JSON `{"thr":0.00..1.00,"mode":0|1|2}`.
**Notify cadence**: ~200 ms.

---

## 7) Algorithms (rendering)
**Throttle smoothing**: `thr_s = thr_s + alpha*(thr - thr_s)` with `alpha=0.10`.

**Core color**
- Interpolate `startColor`→`endColor` by `k = thr` (Linear), `k = thr^1.2` (Ease), or `k = thr` (Pulse).
- Brightness base `val = 30 + 200*thr`.
- Per‑pixel flicker: `n = inoise8(i*12, (t + i*7)*8)`, map to `[-18,+22]`, apply as value scaler.

**Afterburner overlay**
- `ab = clamp01((thr - AB_THR)/ (1 - AB_THR))`.
- If Pulse mode, multiply `ab` by `(0.6 + 0.4*sin(t*0.12))`.
- Core color: blend of `violet‑blue (90,60,255)` and `magenta‑purple (255,90,255)` weighted by eased throttle.
- Spatial profile over ring index: `0.65 + 0.35*sin(2π*pos)`.
- Add sporadic white sparkles when `ab>0.4`.

---

## 8) Persistence (NVS)
Persist on `savePreset=1`:
- `mode:u8`, `start[3]`, `end[3]`, `speed:u16`, `bright:u8`, `num:u16`, `thr:u8`.
Load all at boot; defaults if not present.

---

## 9) Build & flash instructions
**Firmware**
1. Open `/firmware` in Cursor; ensure PlatformIO extension present.
2. `pio run -t upload` (board: `esp32dev`).
3. Power LEDs from UBEC (≥5 A). Ensure common ground. Bench test.

**App**
1. Install RN deps; configure Android/iOS permissions.
2. Run on a physical device (BLE required): `npx react-native run-android` or `run-ios`.
3. Scan, connect to `ABurner`, tweak values, verify live effects.

---

## 10) Acceptance criteria
- **LEDs**: Smooth morph start→end with visible flicker at idle/mid, obvious AB overlay above threshold; max throttle tends to purple.
- **BLE**: App can connect, change each parameter individually, push all, and get status notifications.
- **Persistence**: After Save + reboot, settings restore.
- **Stability**: No LED glitching with quick parameter changes; throttle read is stable (no jittery jumps).

---

## 11) Security & flight hardening (notes)
- For flight: enable BLE bonding + whitelist, or disable advertising after connect.
- Optionally expose a `lock` characteristic to prevent in‑flight changes.

---

## 12) Cursor AI tasks (copy/paste into Cursor)
1. **Create firmware project** under `/firmware` with the structure in §4 and implement modules accordingly.
2. Implement BLE service/characteristics using NimBLE per UUIDs in §3 with binary encodings in §6.
3. Implement LED rendering in `led_effects.*` following algorithms in §7.
4. Implement settings persistence (`Preferences`) per §8.
5. Add a compile‑time `DEMO_MODE` to sweep throttle when no PWM is present.
6. **Create React Native app** under `/app` with screens/components from §5; implement BLE connect/write/notify; basic RGB inputs + preview swatches; buttons for **Push All** and **Save Preset**.
7. Provide README snippets for wiring & permissions.

---

## 13) Reference defaults (for quick testing)
- `startColor = (255,100,0)` (ember)
- `endColor   = (154,0,255)` (purple)
- `mode = 1` (Ease), `speedMs = 1200`, `brightness = 200`, `numLeds = 45`, `abThreshold = 80`
- Device name: `ABurner`

---

## 14) Future extensions (backlog)
- CRSF/SBUS digital throttle, OTA updates, multi‑ring choreography, telemetry bridge (EGT/RPM → color), factory‑reset characteristic, Expo template with permission wizard.

