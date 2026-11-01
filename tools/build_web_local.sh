#!/bin/bash
# Builds and validates the unpublished local Gen-3 candidate.
set -euo pipefail

cd "$(dirname "$0")/.."
ROOT="$PWD"
ENV_NAME="local"
VERSION="1.36.0-gen3-local"
CHECK_ONLY=0

if [[ ${1:-} == "--check" ]]; then
  CHECK_ONLY=1
  shift
fi
if [[ $# -ne 0 ]]; then
  echo "Usage: bash tools/build_web_local.sh [--check]" >&2
  exit 2
fi
if ! grep -Eq '^#define DEX_COUNT 386$' include/dex.h; then
  echo "Refusing local build: include/dex.h must contain exactly 386 species." >&2
  exit 1
fi
if ! grep -q '#define FW_VERSION "1.36.0-gen3-local"' src/main.cpp; then
  echo "Refusing local build: local firmware version does not match $VERSION." >&2
  exit 1
fi

# boot_app0.bin ships inside the Arduino core, not in the PlatformIO build dir.
PIO_CORE="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
BOOT_APP0_SRC="$PIO_CORE/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
if [[ ! -f $BOOT_APP0_SRC ]]; then
  echo "Refusing local build: $BOOT_APP0_SRC not found." >&2
  exit 1
fi

STAGE="$(mktemp -d "${TMPDIR:-/tmp}/tamapoke-local.XXXXXX")"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/firmware"

echo "Building the local Gen-3 candidate with PlatformIO (env: $ENV_NAME)..."
pio run -e "$ENV_NAME"

BUILD_DIR=".pio/build/$ENV_NAME"
cp "$BUILD_DIR/bootloader.bin" "$STAGE/firmware/tamapoke-$VERSION-bootloader.bin"
cp "$BUILD_DIR/partitions.bin" "$STAGE/firmware/tamapoke-$VERSION-partitions.bin"
cp "$BOOT_APP0_SRC" "$STAGE/firmware/tamapoke-$VERSION-boot_app0.bin"
cp "$BUILD_DIR/firmware.bin" "$STAGE/firmware/tamapoke-$VERSION-app.bin"

echo "Packing local Gen-3 sprite update..."
python3 tools/pack_bundle.py --gen3 --output "$STAGE/sprites-gen3-update.pak"

python3 tools/validate_web_build.py \
  --dex-header include/dex.h --app "$STAGE/firmware/tamapoke-$VERSION-app.bin" \
  --pak web/sprites.pak --manifest web/manifest-local.json \
  --expected-version "$VERSION" --expected-dex 386 --expected-files 503 --expected-thumbs 251
python3 tools/validate_web_build.py \
  --pak "$STAGE/sprites-gen3-update.pak" --expected-files 271 --expected-thumbs 386

if [[ $CHECK_ONLY -eq 1 ]]; then
  echo "CHECK OK: local installer files were not changed."
  exit 0
fi

for suffix in bootloader partitions boot_app0 app; do
  src="$STAGE/firmware/tamapoke-$VERSION-$suffix.bin"
  dst="$ROOT/web/firmware/tamapoke-$VERSION-$suffix.bin"
  cp "$src" "$dst.new"
  mv "$dst.new" "$dst"
done
cp "$STAGE/sprites-gen3-update.pak" "$ROOT/web/sprites-gen3-update.pak.new"
mv "$ROOT/web/sprites-gen3-update.pak.new" "$ROOT/web/sprites-gen3-update.pak"
echo "OK: local Gen-3 candidate and sprite bundles refreshed."
