#!/bin/bash
# Builds and validates the public, non-debug Gen-3 release candidate.
# It writes only Gen-3 RC artifacts and never replaces the live Gen-2 installer.
set -euo pipefail

cd "$(dirname "$0")/.."
ROOT="$PWD"
ENV_NAME="release"
VERSION="1.36.1"
CHECK_ONLY=0

if [[ ${1:-} == "--check" ]]; then
  CHECK_ONLY=1
  shift
fi
if [[ $# -ne 0 ]]; then
  echo "Usage: bash tools/build_web_gen3.sh [--check]" >&2
  exit 2
fi
if ! grep -Eq '^#define DEX_COUNT 386$' include/dex.h; then
  echo "Refusing Gen-3 release build: include/dex.h must contain exactly 386 species." >&2
  exit 1
fi
if ! grep -q '#define FW_VERSION "1.36.1"' src/main.cpp; then
  echo "Refusing Gen-3 release build: firmware version does not match $VERSION." >&2
  exit 1
fi

# boot_app0.bin ships inside the Arduino core, not in the PlatformIO build dir.
PIO_CORE="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"
BOOT_APP0_SRC="$PIO_CORE/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
if [[ ! -f $BOOT_APP0_SRC ]]; then
  echo "Refusing Gen-3 release build: $BOOT_APP0_SRC not found." >&2
  exit 1
fi

STAGE="$(mktemp -d "${TMPDIR:-/tmp}/tamapoke-gen3-release.XXXXXX")"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/firmware"

echo "Building the public Gen-3 release with PlatformIO (env: $ENV_NAME)..."
pio run -e "$ENV_NAME"

BUILD_DIR=".pio/build/$ENV_NAME"
cp "$BUILD_DIR/bootloader.bin" "$STAGE/firmware/tamapoke-$VERSION-bootloader.bin"
cp "$BUILD_DIR/partitions.bin" "$STAGE/firmware/tamapoke-$VERSION-partitions.bin"
cp "$BOOT_APP0_SRC" "$STAGE/firmware/tamapoke-$VERSION-boot_app0.bin"
cp "$BUILD_DIR/firmware.bin" "$STAGE/firmware/tamapoke-$VERSION-app.bin"

echo "Packing Gen-3 sprite update..."
python3 tools/pack_bundle.py --gen3 --output "$STAGE/sprites-gen3-update.pak"

python3 tools/validate_web_build.py \
  --dex-header include/dex.h --app "$STAGE/firmware/tamapoke-$VERSION-app.bin" \
  --pak web/sprites.pak --manifest web/manifest-gen3.json \
  --expected-version "$VERSION" --expected-dex 386 --expected-files 503 --expected-thumbs 251
python3 tools/validate_web_build.py \
  --dex-header include/dex.h --app "$STAGE/firmware/tamapoke-$VERSION-app.bin" \
  --pak web/sprites.pak --manifest web/manifest.json \
  --expected-version "$VERSION" --expected-dex 386 --expected-files 503 --expected-thumbs 251
python3 tools/validate_web_build.py \
  --pak "$STAGE/sprites-gen3-update.pak" --expected-files 271 --expected-thumbs 386

# Debug-only commands must remain behind TAMAPOKE_LOCAL_TEST and must not be
# compiled into the public release candidate.
if strings "$STAGE/firmware/tamapoke-$VERSION-app.bin" | grep -Eq 'TESTMON|TESTEVO'; then
  echo "Refusing Gen-3 release build: debug command strings found in firmware." >&2
  exit 1
fi

if [[ $CHECK_ONLY -eq 1 ]]; then
  echo "CHECK OK: Gen-3 release candidate validated; installer files were not changed."
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
echo "OK: Gen-3 release candidate refreshed. Live installer remains unchanged."
