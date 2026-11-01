# TamaPoke Assets — where the sprites come from

The sprites are NOT edited by hand and are not stored here: they are **downloaded from their
sources and packaged** with the scripts in `tools/`. This folder is only
documentation of the flow (it previously contained a proposal for importing via PNG
that became obsolete).

## The real flow

Two formats coexist on the microSD (`/mons/`), both derived from PMD SpriteCollab:

| Format | Script | Source | What it is |
|---|---|---|---|
| **TPK2** `pNNN.bin` / `psNNN.bin` | `pack_pmd.py` | [PMD SpriteCollab](https://github.com/PMDCollab/SpriteCollab) (CC BY-NC) | Multi-action animations (idle, walk, sleep, eat, hurt, attack, gestures) — used **everywhere**: main screen and **Pokédex / gallery** |
| **TPTH** `thumbs.bin` | `make_thumbs.py` | (derives from the TPK2) | 40×40 gallery thumbnails |

```bash
python3 tools/pack_pmd.py     # all included species + shiny -> tools/sdcard/mons/p[s]NNN.bin
python3 tools/make_thumbs.py  # -> tools/sdcard/mons/thumbs.bin
python3 tools/send_sd.py      # sends everything to the board's SD card via USB
```

For the Gen 3 installer there is no single `sprites-gen3-full.pak`: the complete
flow sends `web/sprites.pak` (#1–251) first and afterwards
`web/sprites-gen3-update.pak` (#252–386 and the index of 386 thumbnails). This way
no file hosted on GitHub exceeds the 100 MB limit.

(`s` = shiny variant. `pack_pmd.py` accepts individual Pokédex numbers,
e.g. `python3 tools/pack_pmd.py 7 25`.)

## Binary formats

Defined in the headers of each packer and parsed in `sdmon.cpp`:

- **TPK1** (`SdMon`): `"TPK1"`, `u16 w,h,frames,frameMs`, `u16 palCount`,
  `u16 pal[]` (RGB565), `u8 data[frames*w*h]` (palette index, `0xFF`
  transparent).
- **TPK2** (`PmdMon`): `"TPK2"`, `u8 nActs`, `u16 palCount`, `u16 pal[]`, and per
  action `u8 id,w,h,nFrames` + `u16 ms[nFrames]` + `u8 data[w*h*nFrames]`.
- **TPTH** (`SdThumbs`): `"TPTH"`, `u16 count`, `u32 offset[count]`, and per
  thumbnail `u8 w,h,palCount` + `u16 pal[]` + `u8 data[w*h]`.

The firmware validates sizes on load, so a truncated `.bin` is rejected without
breaking anything.

## Download cache

`pack_pmd.py` caches the original SpriteCollab PNGs in `tools/pmd_cache/`
(ignored by git, regenerable). The final `.bin` files are versioned in
`tools/sdcard/mons/` as a backup.

> See [CREDITS.md](../../CREDITS.md) for the origin and the terms of the
> sprites. They are third-party: do not redistribute for commercial purposes.
