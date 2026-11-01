# TamaPoke Expanded - AI Handoff

As of: 2026-09-07

## Project and Hardware

This repository is a heavily extended, community-oriented firmware for
the Waveshare ESP32-S3 Touch AMOLED 1.75. It is a fork of
`socquique/TamaPoke` and is maintained as the "Expanded" variant.

- Firmware entry point: `src/main.cpp` (PlatformIO project)
- Baseline firmware state: `1.32.1-caught-mark`
- Local Gen-3/watermark/step test: `1.36.0-gen3-local` (via `TAMAPOKE_LOCAL_TEST`)
- Public Gen-3 release: `1.36.0` (via
  `TAMAPOKE_GEN3_RELEASE`, without local debug commands)
- Board: ESP32-S3, 16 MB flash, OPI PSRAM, round 466x466 AMOLED, CST9217 touch,
  PCF85063 RTC, AXP2101 PMU, ES8311 audio
- Build: PlatformIO with the pioarduino platform `55.03.311` (ESP32 Arduino
  core `3.3.11`). The firmware was originally validated with core `3.3.10`
  under the Arduino IDE/`arduino-cli`; the project no longer ships a sketch.
- Libraries: GFX Library for Arduino `1.6.7`, SensorLib `0.4.1` and
  XPowersLib `0.3.3` (pinned in `platformio.ini`).
- Languages: DE, EN, ES, FR, IT, PT. For font reasons, code and UI texts
  largely use ASCII without umlauts/accents.

The firmware stores the game state in ESP32 NVS. During an update the
installer must **not** be run with `Erase device`, otherwise the save is
deleted.

## Important Working Rule

**Never push to GitHub, create releases or update the PR without the user's
explicit request.**

The user wants to test unfinished local versions on his hardware first. Local
building, testing and a local web installer are always allowed and desired.

## Current Gen-3 Work State

- Branch `local/full-gen3`; the tag `local-before-gen3` allows a quick
  return to the working Gen-2 state. The tested Gen-3 feature state
  `da8bfa8` is already on `fork/main`. The hosted installer and the
  GitHub release use the debug-free Gen-3 state `1.36.0`.
- Local dex #1–386 with six languages, uint16 dex paths, Gen-3 stats/types,
  evolution rules and the 3-of-4 stats gate above 40 percent.
- Local first start: choose the region Kanto/Johto/Hoenn, then one of three
  regional starters. Existing saves are not migrated or deleted.
- All 270 Gen-3 SpriteCollab files (normal + shiny) are in
  `tools/sdcard/mons/`; `thumbs.bin` has 386 entries.
- `web/sprites-gen3-update.pak` covers #252–386 (approx. 38 MB). The public
  `web/sprites.pak` remains #1–251. The local button for the full 386 pack
  sends both files one after another; this keeps every GitHub file below
  the 100 MB limit.
- A single file `sprites-gen3-full.pak` no longer exists and must not be
  required in instructions or release texts.
- Local web test: `cd web && python3 -m http.server 8000`, then
  `http://localhost:8000/dev.html` in Chrome/Edge. Step, IMU and
  evolution diagnostics run there via clicks; no `screen` terminal needed.
- Build with `bash tools/build_web_local.sh`; the four local firmware parts
  are then located under `web/firmware/tamapoke-1.36.0-gen3-local-*.bin`.
- The public, debug-free release is built with `bash tools/build_web.sh`.
  `bash tools/build_web_gen3.sh` is the underlying Gen-3 build;
  `web/release-gen3.html` remains a local reference page.
- README, installer and release text state clearly: flashing the firmware
  alone is not enough; afterwards
  step 2 for the microSD sprites is mandatory. With a new/empty/unclear card
  always use the full 386 pack instead of only the Gen-3 update.

## Current Working Tree

The public installer points to the debug-free Gen-3 build `1.36.0`.
For hardware tests `web/dev.html` and
`web/manifest-local.json` still exist; these debug files are deliberately not
part of the public installation flow.

Current local test guards in `src/main.cpp`:

- `TAMAPOKE_LOCAL_TEST` sets the display version to `1.36.0-gen3-local`.
- `TAMAPOKE_GEN3_RELEASE` sets the display version to `1.36.0` and allows the
  386 dex, but does not unlock any debug commands.
- The serial commands `TESTMON`, `TESTEVO`, `CAUGHT`, `CAUGHT <dex>`, `BATTLE`
  and `BATTLE <dex>` are active only in this test build.
- This test version must not accidentally be entered in `web/index.html` or
  `web/manifest.json` or be activated as the public GitHub Pages installer;
  it lives in `web/dev.html` and `web/manifest-local.json` for hardware tests.
- `web/release-gen3.html` and `web/manifest-gen3.json` are the local
  public preview without a debug UI. The single sprite button sends
  `sprites.pak` and `sprites-gen3-update.pak` mandatorily one after another.

Current Git state at the time this file was created:

- Current branch: `local/full-gen3` (the Gen-2 state is preserved historically on
  `local/full-gen2`).
- `fork`: `https://github.com/ShadowEnemyx/TamaPoke.git`
- `origin`: `https://github.com/socquique/TamaPoke.git`
- The starting point is secured locally as `3b24ea8` (`chore: save local baseline before gen2 watermark`)
  and additionally with the tag `local-baseline-before-gen2-watermark`.
- The local feature state is secured as `feat: add gen2 starters and local watermark`;
  the final state was published to `fork/main` after explicit user approval.
- The last tested Gen-2 state is historically marked as tag
  `local-full-gen2-final`. The current working branch is
  `local/full-gen3`; before the bug fixes the local tag
  `local-before-gen3-bugfix-20260825` additionally lies there.
- The working tree contains the published Gen-2, watermark,
  localization, sprite and step state. `web/index.html` and
  `web/manifest.json` point to the public build; only `web/dev.html`
  and `web/manifest-local.json` activate the debug commands.
- The main author left the big Expanded PR open and links the fork,
  but does not want to merge it completely into
  the base project because of scope, branding and binary history. Small, later
  individual PRs would be possible, but only after a user decision.

## Existing Features

### Core Game

- Egg, starter selection, hatching, care, sleep, hygiene, hunger, joy,
  energy, weight, bond, nicknames, evolution, farewell, release and
  runaway.
- Age and care continue with the RTC even when the display is off;
  offline progress is limited.
- Pokedex/Gallery distinguishes bred (`raised`) and caught (`caught`)
  Pokemon.
- Profile, personality, daily goals, medals, progress and
  collector rank are reachable via eight swipe cards.

### Battle

- Manual wild battles via the battle card, plus rare optional
  wild encounter prompts on the main screen.
- Opponent levels are normally close to the pet level, but can also be a few
  levels below. Wild legendaries are not planned in v1.
- Types are visible in battles and moderately influence damage.
- Actions: Quick, Heavy, Dodge/Counter, Rest and Run.
  - Quick: less damage, safer and reduces incoming damage.
  - Heavy: more damage, but the opponent can dodge/counter.
  - Dodge: can prepare a counter for the next attack.
  - Rest: at most twice per battle, heals and grants guard.
- No Pokemon death, no catching on defeat. Wins/losses/streaks
  are persistent and grant moderate training rewards.
- After a win there is exactly one optional catch attempt. On a narrow
  defeat there can be a small "respect catch" chance.
- During battle a small pokeball to the right of the opponent name line indicates
  whether the opposing Pokemon has already been caught. The icon uses only the
  existing `dexCaught` bitmap and does not change the save structure.

### Minigames and Events

- Play menu with Ball, Catch, Memo, Clean and Type.
- Ball is a hard reaction game with fast/unpredictable movement.
- Catch is a reaction game with increasingly short time windows.
- Memo shows a color sequence that must be retyped; the instructions sit
  visually between the pad rows and the pads have their own tones.
- Clean and Type supplement hygiene or type/attack training.
- Rare, voluntary pet events (Berry, Heart, Sparkle) and rare optional
  wild encounters do not interrupt other UI modals.

### Expedition and Inventory

- Eighth card `EXPEDITION` with 15/30/60-minute tours.
- A small main-screen chip appears only when a tour is active, a find is collectible
  or items exist; a tap opens the expedition card directly.
- Tours continue via RTC even when the board is switched off.
- The reward is determined at the start and persisted.
- Inventory: Trail Snack, Energy Tonic, Care Kit and Train Token. A maximum of three
  of each item; Train Token opens the ATK/DEF/SPD selection.
- The main-screen chip opens card 8 directly and shows tour remaining time,
  collectible find or inventory count.

### Settings, Help and Performance

- Settings: language, time, four sound modes and optional Power Save.
- Power Save is off by default; it reduces idle rendering and uses
  light sleep only on dimming/screen-off. Care, RTC and touch wakeup remain
  active.
- Eight integrated help pages describe operation, battle and systems.
- Static screens use dirty rendering; active games/battles remain
  regularly rendered.

### Motion, Time of Day and Evolution

- `dayphase.h` provides a shared morning/day/evening/night phase for
  background, wild pool and care consumption.
- At night FOOD drops more slowly while awake; sleep remains unchanged.
- QMI8658 shaking grants limited JOY. The pedometer counts steps even with
  screen-off; daily/total counters, trail rewards as well as wild shiny/catch bonus
  remain in NVS. The current software counter uses filtered positive
  acceleration impulses and also counts on USB once two rhythmic
  steps are confirmed. The raw QMI pedometer remains visible as diagnostics.
- Evolution is never executed automatically: the red CTA remains visible when the
  level is reached and reminds when care values are too low. Keeping the form is
  offered again at level 100 after one game day.

## Audio

Audio is fully synthetic via ES8311/I2S. No original
Pokemon audio files or ROM sounds are used.

- `SOUND_FULL` / TON VIEL: UI, minigames, battle, ambient and pet cries.
- `SOUND_MED`: important care, battle, event and result sounds.
- `SOUND_LOW`: big events and warnings.
- `SOUND_OFF`: mute.

The species cries and all UI/battle/minigame sounds are synthetic:

- four instead of three tones, longer and without a noise wave
- higher synthesis volume
- with TON VIEL a pet tap has priority over the generic click
- pet tap, gallery detail and wild battle have their own cry/SFX paths
- sound modes remain separate: VIEL, MITTEL, WENIG, AUS

The volume and effect still have to be judged on real hardware; desktop builds
cannot replace the ES8311/speaker path.

## Version History

The following entries document the development state at the time. Not
every test version was published; statements about earlier branches are not a
description of the current working tree. `1.36.0-gen3-local` remains the local
debug state; `1.36.0` is the published debug-free Gen-3 release.

1. `1.29.3-reliability`
   - Reset of interaction/evolution/farewell locks on a new pet.
   - Farewell/release/runaway is completed safely after a reset.
   - Pet level is capped at 100.
   - `millis()` deadlines are rollover-safe.
   - Cards update again on pet ticks.
   - remaining UI literals were assigned to i18n keys.
   - macOS `._*` sidecars inside `.git` were removed; `git fsck` is
     clean afterwards except for one normal dangling commit.
2. `1.29.4-profile-tap`
   - The green circles in the profile were collector frames. They are no longer
     drawn over the portrait.
   - A tap on the profile portrait now plays the species cry.
3. `1.29.5-pet-chirps`
   - Clearer and more audible, dedicated synthetic species cries.
4. `1.30-expedition-hud`
   - Visible, round-screen-suitable entry to tour and inventory on
     the main screen. The chip shows tour remaining time, collectible find or
     inventory count and remains hidden in all modal views.
5. `1.30.1-caught-frames`
   - Wild battles show a small pokeball to the left of the opponent HP bar
     when this Pokemon has already been caught.
   - Profile frames are visible again: six corner/accent variants sit
     behind the portrait and do not cover the sprite.
6. `1.31.0-day-imu`
   - Shared time-of-day API in `dayphase.h` (morning/day/evening/night).
   - Awake at night: FOOD -1/tick instead of -2; sleep unchanged. Offline minutes
     use the time of day of each minute.
   - Wild pool by phase: morning Grass/Flying/Normal/Bug, evening Water/Flying/Fire,
     night Ghost/Poison/Bug. Day remains unfiltered. No legends.
   - Night idle runs less often. A morning overlay once per calendar day,
     without auto-wake.
   - QMI8658: shaking grants JOY on the main screen (cooldown 25 s, 8/day).
     Steps also with screen-off; the build at the time discarded USB steps.
     The later step test build also counted on USB. Serial: `IMU`, `SHAKE`,
     `WALK n`, `STEPS`, `STATS`. The local test page additionally offers clickable
     `IMU`/`STATS` diagnostic buttons; `STATS` also shows `usb=0/1`.
7. `1.31.1-shake-fix`
   - Shake threshold lowered (~1.45 g or gyro), accel+gyro, both I2C addresses.
8. `1.32.0-evo-cta`
   - The red evolution button remains as soon as the level is sufficient, even if a
     bar is below 40. Tapping then reminds about the bars.
   - "Keep form" at level 100 comes back after one game day.
9. `1.32.1-caught-mark`
   - Public installer state with catch marker in the wild battle.
   - The marker sits next to the opponent name line and shows only already
     caught wild Pokemon.
10. `1.32.1-local-test`
   - Local test variant with additional serial commands for catch marker and
     forced battle opponents. Do not publish publicly.
11. `1.34.0-gen2-full-local`
   - Complete dex #1-251 with Gen-2 names, stats, types, six languages
     and 122 evolution rules including Eevee, baby and Tyrogue branches.
   - An evolution strictly requires three of four care values above 40;
     bond, day/night and stat ratios are checked for matching rules.
   - SpriteCollab normal/shiny files and thumbnails for #161-251 are packed
     locally; `sprites-gen2-update.pak` is an incremental update.
   - `web/dev.html` offers selection and test buttons for all 251 species without a
     terminal; the firmware commands are active only in the local test build.
12. `1.35.3-soft-step-local`
   - Persistent daily/total step counter with 500/2,000/5,000 trail rewards
     and trail rank milestones.
   - Daily steps improve the wild shiny chance and the catch chance; shiny wild
     battles are shown and stored in the shiny dex registration.
   - The QMI hardware counter remains visible for diagnostics; steps are detected from
     filtered acceleration impulses. USB no longer blocks counting;
     raw, software and USB status can be checked via `STATS`/`IMU`.
13. `1.36.0-gen3-local`
   - Local dex and sprite stock #1-386 with Hoenn starters, all Gen-3 species,
     six languages and Gen-3 evolution rules.
   - Only `tools/build_web_local.sh` produces this candidate. The public
     Gen-2 build remains unchanged and is built from a pinned commit.
14. `1.36.0` release candidate
   - Public Gen-3 build profile with 386 species, but without local test commands.
   - The local preview requires a single complete sprite step that
     transfers the Gen-1/2 and the Gen-3 pack one after another.
   - Not yet activated on GitHub Pages and not yet a GitHub release.

## Architecture and Important Files

- `platformio.ini`: PlatformIO environments `release` and `local`, the pinned
  pioarduino platform, library versions and the partition table.
- `boards/tamapoke_esp32s3.json`: board definition (16 MB flash, OPI PSRAM,
  USB CDC on boot).
- `src/main.cpp`: UI, touch routing, rendering, cards, minigames, battle screen,
  settings, help, local runtime state, IMU poll and morning overlay. Unlike the
  former sketch it declares every function prototype explicitly.
- `include/dayphase.h`: hour, phase and visual night from the RTC epoch.
- `include/imu.h` / `src/imu.cpp`: QMI8658 poll, shake edge, pedometer delta.
- `include/pet.h` / `src/pet.cpp`: persistent pet model, NVS save/load, care,
  progression, pokedex, catching, events, expedition and items.
- `include/battle.h` / `src/battle.cpp`: testable battle logic and type
  multipliers.
- `include/audio.h` / `src/audio.cpp`: sound queue, ES8311, SFX and species cries.
- `include/species_chirp.h` / `src/species_chirp.cpp`: legally unproblematic
  synthesis profiles generated per dex number.
- `include/i18n.h` / `src/i18n.cpp`: all visible UI texts and dex names.
- `include/time_utils.h`: rollover-safe `deadlineActive`, `deadlineReached` and
  `deadlineRemaining` for `millis()` timers.
- `tests/pet_tests.cpp` and `tests/battle_tests.cpp`: native regression tests.
- `tools/build_web.sh`: builds the `release` environment with PlatformIO and
  produces the four separate public Gen-3 installer binaries.
- `tools/build_web_gen3.sh`: produces and validates the debug-free
  `1.36.0` release without using the local debug build.
- `web/manifest.json` and `web/index.html`: web installer. The parts are
  flashed separately so that NVS/save is preserved during updates.
- `web/dev.html` and `web/manifest-local.json`: local test installer, not for
  the public GitHub Pages flow.
- `web/release-gen3.html` and `web/manifest-gen3.json`: local preview of the
  public Gen-3 installer without test buttons.

## Testing and Local Flashing

Run in the repository folder (`TamaPoke`):

```bash
cd tests && make clean && make test
cd .. && pio run -e release
pio run -e local
bash tools/build_web.sh
python3 -m http.server 8000 --directory web
```

Afterwards open Chrome or Edge on the Mac:

```text
http://127.0.0.1:8000/?v=1.36.0
```

Connect the Waveshare via USB and **leave Erase device disabled** in the installer.
The local server must keep running during flashing. The current
web installer expects these four files:

```text
web/firmware/tamapoke-1.36.0-bootloader.bin
web/firmware/tamapoke-1.36.0-partitions.bin
web/firmware/tamapoke-1.36.0-boot_app0.bin
web/firmware/tamapoke-1.36.0-app.bin
```

### Local Test Installer

For hardware tests the safeguarded local build path is used:

```bash
bash tools/build_web_local.sh
# or without changing existing installer files:
bash tools/build_web_local.sh --check
```

The four generated parts must be located under the `1.36.0-gen3-local` names
given in `web/manifest-local.json`. The browser installer parts
are versioned for reproducible local and public tests. Afterwards:

```bash
python3 -m http.server 8000 --directory web
```

In the browser open `http://127.0.0.1:8000/dev.html`. Also with the local
test update leave `Erase device` disabled.

### Public Gen-3 Release Reference

```bash
bash tools/build_web_gen3.sh
python3 -m http.server 8000 --directory web
```

In the browser open `http://127.0.0.1:8000/release-gen3.html`. This page has
no debug commands and offers exactly one mandatory full sprite button.
The public page `web/index.html` has the same normal flow.

## Verification Before Any Publication

1. `cd tests && make clean && make test`
2. `pio run -e release` and `pio run -e local`
3. `bash tools/build_web.sh --check`, `bash tools/build_web_local.sh --check`
   and `bash tools/build_web_gen3.sh --check`
4. `git diff --check`
5. Hardware smoke test without erase:
   - Save/pet is preserved.
   - Tap the main-screen pet several times: the cry must be clearly audible.
   - Open the profile: cycle through six visible corner/accent frames; check
     tapping the portrait.
   - Briefly check battle, play menu, memo, expedition, sleep, PWR wakeup and
     settings.
   - In the local test build check `TESTMON`, `TESTEVO`, `CAUGHT <dex>` and
     `BATTLE <dex>`; in the public build these debug commands are deliberately
     not present.
6. Only commit/push/publish after explicit user approval.

## Open Topics and Safe Next Steps

- The owner has basically confirmed local flashing, Gen-3 sprites and real step
  counting on hardware. For release approval the extended test remains: serial
  `IMU`, `STEPS`, `WALK n`, software steps with screen-off and USB, trail
  rewards, wild shiny catch, night FOOD, morning overlay,
  sleep remains on shaking. Adjust thresholds after the session.
- Check the synthetic pet cries and the six profile frames visually/acoustically
  on the real Waveshare.
- Do targeted bug hunts again after new user feedback. In reviews always
  check real bugs, save risks and touch hitboxes first.
- Check all UI changes on the round screen for truncated text and
  misinputs.
- No new big battle feature without a user decision; first confirm markers,
  frames, touch hitboxes and save safety on hardware.
- Do not bulk-add historical firmware binaries to Git again. Only
  publish the four current installer parts of an approved version in a targeted way.
