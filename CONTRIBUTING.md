# Contributing

## Before You Start

Open an issue first for any significant change so we can discuss it before you build it.

## What Is Welcome

- Bug fixes in `keymap.c` or `Rexus_Daxa_M84_Ultimate.js`
- Calibration values from different production batches (share in an issue)
- Additional keymap layers as separate keymaps under `keymaps/`
- Documentation improvements

## What Is Out of Scope

- VIA/Vial support (intentionally disabled)
- Wireless support (not possible in QMK on this hardware)

## Pull Request Process

1. Fork the repo and create a branch from `main`
2. Make your change — keep commits focused and descriptive
3. Test on hardware before submitting
4. Open a PR with a clear description of what changed and why

## Code Style

Follow the existing style in `keymap.c`:
- Match the surrounding indentation and brace style
- Comments only where the logic is non-obvious
- No dead code, no commented-out blocks

## Reporting Issues

Include:
- Firmware version (git tag or commit hash)
- SignalRGB plugin version
- What you expected vs. what happened
- Steps to reproduce
