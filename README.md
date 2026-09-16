# Rexus Daxa M84 Ultimate — QMK + SignalRGB

Custom QMK firmware and SignalRGB device plugin for the **Rexus Daxa M84 Ultimate** keyboard.
Enables full per-key RGB sync via SignalRGB using a lightweight Raw HID protocol — no Rexus software needed.

---

## ⚠️ Critical Warnings — Read Before Flashing

| | |
|-|--|
| 🔴 | **Wireless is permanently disabled.** This keyboard's 2.4 GHz dongle and Bluetooth modes are not supported by QMK. After flashing, the keyboard works **via USB cable only**. Wireless cannot be restored without re-flashing the original Rexus stock firmware. |
| 🔴 | **Rexus Control software stops working.** The stock app will no longer control RGB or any keyboard settings after flashing. |
| 🟡 | **VIA/Vial is not supported.** Intentionally disabled. To remap keys, edit `keymap.c` and recompile. |
| 🟡 | **Back up your stock firmware first.** Use Sonix Flasher's **Dump** option to save the original `.bin` before flashing if you want the option to revert. |

---

## What This Gives You

- Per-key RGB driven by **SignalRGB** canvas or forced color mode
- **GRB channel correction** — fixes the SN32 driver's wire order and luminance imbalance so colors render accurately
- **Caps Lock LED** always shows white regardless of the active RGB effect
- **FN layer** — media keys, RGB matrix controls, Win-lock, AG-swap, QK_BOOT
- **NKRO** enabled
- No VIA overhead

---

## Repository Structure

```
.
├── keyboards/
│   └── rexus/
│       └── daxa_m84_ultimate/
│           ├── keyboard.json          # Board: USB IDs, matrix pins, RGB LED positions
│           ├── config.h               # RGB row pins, LED count, CapsLock LED index
│           ├── halconf.h              # ChibiOS HAL: PWM/GPT switching by build mode
│           ├── mcuconf.h              # SN32F248BF MCU config
│           └── keymaps/
│               └── default/
│                   ├── keymap.c       # Keymap + Raw HID SignalRGB handler
│                   ├── keymap.json    # QMK build placeholder (empty)
│                   └── rules.mk       # Feature flags
├── Rexus_Daxa_M84_Ultimate.js         # SignalRGB device plugin
├── releases/
│   └── rexus_daxa_m84_ultimate_default.bin
├── .github/
│   └── workflows/
│       └── build.yml                  # CI: compiles on push and tags
├── .gitignore
├── LICENSE
└── README.md
```

---

## Flashing — Pre-built Binary

Download the `.bin` from the [Releases](../../releases/latest) page. No build environment needed.

### Enter bootloader mode

| Method | How |
|--------|-----|
| **Cold boot** | Hold **Esc** while plugging in USB |
| **Already on QMK** | Press **FN + Esc** |
| **Hardware — short BOOT + GND pins** | Use tweezers or a wire to bridge the two pads shown below before plugging in USB. Use this method if the keyboard is unresponsive to the other two options. |

![Daxa M84 Ultimate bootloader pins — short BOOT and GND to enter DFU mode]([https://raw.githubusercontent.com/meghundul/daxa-m84-qmk-srgb/refs/heads/main/Daxa%20M84U%20bootloader.png](https://raw.githubusercontent.com/meghundul/daxa-m84-qmk-signalrgb/refs/heads/main/assets/Daxa_M84U_bootloader.png))

> **How to short:** Open the keyboard case, locate the two pads labelled in the image, bridge them with a bent paperclip or tweezers, then plug in USB while holding the bridge. Release once the device appears as a detected device in Sonix Flasher.

### Flash with Sonix Flasher

1. Download [Sonix Flasher](https://github.com/SonixQMK/sonix-flasher/releases)
2. Enter bootloader mode using one of the methods above
3. Open Sonix Flasher — the keyboard should appear as a detected device
4. **Chip:** select `SN32F24x`
5. **QMK offset:** `0x00` 
6. Click **Flash QMK** → select `rexus_daxa_m84_ultimate_default.bin`
7. Wait for the flash to complete — keyboard reboots automatically

---

## Building from Source

### Requirements

| Tool | Notes |
|------|-------|
| QMK MSYS | Windows build environment — [download here](https://github.com/qmk/qmk_distro_msys/releases/latest) |
| srgbmods QMK fork | Required for SN32 support — [github.com/srgbmods/srgbmods-qmk-firmware](https://github.com/srgbmods/srgbmods-qmk-firmware) |

### Steps

1. Install **QMK MSYS** and open it
2. Clone the srgbmods QMK fork (first time only):
   ```bash
   qmk setup srgbmods/srgbmods-qmk-firmware
   ```
3. Copy the keyboard folder from this repo into your QMK install:
   ```bash
   cp -r keyboards/rexus/daxa_m84_ultimate ~/qmk_firmware/keyboards/rexus/daxa_m84_ultimate
   ```
4. Compile:
   ```bash
   qmk compile -kb rexus/daxa_m84_ultimate -km default
   ```

Binary output: `C:\Users\YOUR_NAME\qmk_firmware\rexus_daxa_m84_ultimate_default.bin`

> Flash the output `.bin` using Sonix Flasher as described above.

---

## SignalRGB Plugin Installation

1. Copy `Rexus_Daxa_M84_Ultimate.js` to:
   ```
   %AppData%\WhirlwindFX\SignalRgb\cache\plugins\Devices\
   ```
2. Restart SignalRGB
3. Keyboard appears as **"Rexus Daxa M84 Ultimate"**
4. Assign any canvas or effect

> Plugin v1.1.9 · Protocol v1.0.6

---

## HID Protocol

All packets are **33 bytes**: `0x00` (Report ID) + 32 payload bytes.

| `data[0]` | Command | Action |
|-----------|---------|--------|
| `0x01` | Enable | Switch to SignalRGB mode, disable hardware effects |
| `0x02` | Color data | Stream LED colors — 9 LEDs per packet, GRB order |
| `0x03` | Sync | Commit buffered frame to matrix |
| `0x04` | Disable | Restore EEPROM hardware effect |

Color packet format:
```
[0x00][0x02][start_idx][count][G][R][B][G][R][B]...
```

Wire order from SignalRGB is **GRB**. Firmware reorders to GRB → RGB and applies calibration.

---

## Color Calibration

The SN32 RGB driver has per-channel luminance imbalance. Fixed in `keymap.c`:

```c
#define RED_BALANCE  125   // Red ~49% of raw
#define BLUE_BALANCE 197   // Blue ~77% of raw
// Green = 100%, used as reference
```

Derived from a `#7DFFC5` white reference test. If your unit's colors are off, tune these and recompile.

---

## FN Layer

| FN + | Action |
|------|--------|
| Esc | QK_BOOT — enter bootloader |
| F1 | Win + D |
| F2 | My Computer |
| F3 | Mail |
| F4 | Media Select |
| F5 / F6 | Prev / Next track |
| F7 | Play/Pause |
| F8 | Stop |
| F9 / F10 / F11 | Vol Up / Down / Mute |
| F12 | Calculator |
| Home | RGB next mode |
| PgUp | RGB toggle |
| Up | RGB brightness up |
| Down | RGB brightness down |
| Left | RGB speed down |
| Right | RGB speed up |
| Tab | Win-lock toggle |
| A | AG Normal |
| S | AG Swap |
| Left Win | Win + L |

---

## Compatibility

| | |
|-|-|
| ✅ | USB wired |
| ✅ | SignalRGB |
| ❌ | 2.4 GHz wireless — not supported by QMK |
| ❌ | Bluetooth — not supported by QMK |
| ❌ | VIA / Vial |
| ❌ | Rexus Control software |

---

## Reverting to Stock

If you saved a backup before flashing: enter bootloader → flash the saved `.bin` via QMK Toolbox.
If you did not back up: check Rexus support or community forums for the stock firmware file.

---

## Credits

Firmware and plugin by **meghundul** ·
Built on [QMK Firmware](https://github.com/qmk/qmk_firmware) ·
SN32 driver via [srgbmods fork](https://github.com/srgbmods/srgbmods-qmk-firmware)

## License

[GPL-2.0](LICENSE)
