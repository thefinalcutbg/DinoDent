#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
TOOLS_DIR="$HOME/dev/appimage-tools"
APPDIR="$TOOLS_DIR/DinoDent.AppDir"
OUTPUT_DIR="$PROJECT_DIR/installer/compiled"
DESKTOP_FILE="$SCRIPT_DIR/dinodent.desktop"
ICON_FILE="$SCRIPT_DIR/dinodent.png"
LINUXDEPLOY="$TOOLS_DIR/linuxdeploy-x86_64.AppImage"
QT_PLUGIN="$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"
APPIMAGETOOL="$TOOLS_DIR/appimagetool-x86_64.AppImage"
DINO_BIN="/home/thefinalcut/dev/build-DinoDent-Desktop_Qt_6_8_3-Release/DinoDent"
QMAKE="/home/thefinalcut/dev/Qt/6.8.3/gcc_64/bin/qmake"
FCITX_PLUGIN="$HOME/dev/fcitx5-qt/build/qt6/platforminputcontext/libfcitx5platforminputcontextplugin.so"
FINAL_APPIMAGE="$OUTPUT_DIR/DinoDent-x86_64.AppImage"
OPENSSL_LIB_DIR="/usr/lib/x86_64-linux-gnu"
SSL_LIB="$OPENSSL_LIB_DIR/libssl.so.3"
CRYPTO_LIB="$OPENSSL_LIB_DIR/libcrypto.so.3"

fail() { echo "ERROR: $*" >&2; exit 1; }
require_file() { [[ -f "$1" ]] || fail "File not found: $1"; }
check_deps() {
    local item="$1" missing
    missing="$(LD_LIBRARY_PATH="$APPDIR/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ldd "$item" | grep 'not found' || true)"
    [[ -z "$missing" ]] || fail "Missing dependencies for $item: $missing"
}

require_file "$DINO_BIN"
[[ -x "$DINO_BIN" ]] || fail "Not executable: $DINO_BIN"
require_file "$QMAKE"
require_file "$DESKTOP_FILE"
require_file "$ICON_FILE"
require_file "$FCITX_PLUGIN"
require_file "$SSL_LIB"
require_file "$CRYPTO_LIB"
QT_DIR="$(cd "$(dirname "$QMAKE")/.." && pwd)"
require_file "$QT_DIR/lib/libQt6WaylandClient.so.6"
require_file "$QT_DIR/plugins/platforms/libqwayland-generic.so"
require_file "$QT_DIR/plugins/wayland-shell-integration/libxdg-shell.so"

mkdir -p "$TOOLS_DIR" "$OUTPUT_DIR"
# Prepare a sanitized desktop entry BEFORE linuxdeploy sees it.
# Keep the original repository file unchanged.
DESKTOP_TMPDIR="$(mktemp -d "$TOOLS_DIR/dinodent-desktop.XXXXXX")"
DESKTOP_CLEAN="$DESKTOP_TMPDIR/dinodent.desktop"
cleanup_desktop() { rm -rf -- "$DESKTOP_TMPDIR"; }
trap cleanup_desktop EXIT
sed 's/\r$//' "$DESKTOP_FILE" > "$DESKTOP_CLEAN"
if grep -q '^Categories=' "$DESKTOP_CLEAN"; then
    sed -i 's/^Categories=.*/Categories=Science;MedicalSoftware;/' "$DESKTOP_CLEAN"
else
    printf '\nCategories=Science;MedicalSoftware;\n' >> "$DESKTOP_CLEAN"
fi
if [[ ! -f "$LINUXDEPLOY" ]]; then
    wget -O "$LINUXDEPLOY" https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
fi
if [[ ! -f "$QT_PLUGIN" ]]; then
    wget -O "$QT_PLUGIN" https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
fi
if [[ ! -f "$APPIMAGETOOL" ]]; then
    wget -O "$APPIMAGETOOL" https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage
fi
chmod +x "$LINUXDEPLOY" "$QT_PLUGIN" "$APPIMAGETOOL"

[[ -z "$(ldd "$DINO_BIN" | grep 'not found' || true)" ]] || fail "DinoDent has missing shared libraries"

# The Qt deploy plugin scans SQL drivers in the Qt installation.
# Temporarily hide optional drivers to avoid pulling unavailable DB clients.
SQLDRIVER_DIR="$QT_DIR/plugins/sqldrivers"
SQLDRIVER_BACKUP="$(mktemp -d "$TOOLS_DIR/dinodent-sqldrivers.XXXXXX")"
restore_sql_plugins() {
    if [[ -d "$SQLDRIVER_DIR" && -d "$SQLDRIVER_BACKUP" ]]; then
        shopt -s nullglob
        local f
        for f in "$SQLDRIVER_BACKUP"/*.so; do mv -- "$f" "$SQLDRIVER_DIR/"; done
        shopt -u nullglob
    fi
    rm -rf -- "$SQLDRIVER_BACKUP"
}
trap 'restore_sql_plugins; cleanup_desktop' EXIT
if [[ -d "$SQLDRIVER_DIR" ]]; then
    for driver in libqsqlmimer.so libqsqlodbc.so libqsqlpsql.so libqsqlmysql.so; do
        if [[ -f "$SQLDRIVER_DIR/$driver" ]]; then
            mv -- "$SQLDRIVER_DIR/$driver" "$SQLDRIVER_BACKUP/"
        fi
    done
fi

rm -rf -- "$APPDIR"
mkdir -p "$APPDIR"
export QMAKE="$QMAKE"
export PATH="$TOOLS_DIR:$PATH"

# Deploy executable, Qt and regular runtime dependencies.
# Packaging is intentionally performed separately with appimagetool.
"$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --executable "$DINO_BIN" \
    --desktop-file "$DESKTOP_CLEAN" \
    --icon-file "$ICON_FILE" \
    --library "$QT_DIR/lib/libQt6WaylandClient.so.6" \
    --library "$FCITX_PLUGIN" \
    --library "$SSL_LIB" \
    --library "$CRYPTO_LIB" \
    --library "$QT_DIR/plugins/platforms/libqwayland-generic.so" \
    --library "$QT_DIR/plugins/wayland-shell-integration/libxdg-shell.so" \
    --plugin qt

# Add dynamically loaded plugins not discovered from the executable.
mkdir -p "$APPDIR/usr/plugins/platforminputcontexts" \
         "$APPDIR/usr/plugins/platforms"
cp -L -- "$FCITX_PLUGIN" "$APPDIR/usr/plugins/platforminputcontexts/"
cp -L -- "$QT_DIR"/plugins/platforms/libqwayland*.so "$APPDIR/usr/plugins/platforms/"
mkdir -p "$APPDIR/usr/plugins/wayland-shell-integration"
cp -L -- "$QT_DIR"/plugins/wayland-shell-integration/*.so \
    "$APPDIR/usr/plugins/wayland-shell-integration/"
# Qt loads OpenSSL with dlopen(), so make sure both matching system libraries
# are present even if linuxdeploy did not discover them.
cp -L -- "$CRYPTO_LIB" "$SSL_LIB" "$APPDIR/usr/lib/"

check_deps "$APPDIR/usr/plugins/platforminputcontexts/libfcitx5platforminputcontextplugin.so"
check_deps "$APPDIR/usr/plugins/platforms/libqwayland-generic.so"
check_deps "$APPDIR/usr/plugins/wayland-shell-integration/libxdg-shell.so"
check_deps "$APPDIR/usr/plugins/tls/libqopensslbackend.so"
require_file "$APPDIR/usr/lib/libssl.so.3"
require_file "$APPDIR/usr/lib/libcrypto.so.3"

# Normalize the desktop file: the source repository may use CRLF.
# linuxdeploy may use the temporary basename; install the canonical desktop entry.
DESKTOP_TARGET="$APPDIR/usr/share/applications/dinodent.desktop"
cp -- "$DESKTOP_CLEAN" "$DESKTOP_TARGET"
require_file "$DESKTOP_TARGET"
sed -i 's/\r$//' "$DESKTOP_TARGET"
if grep -q '^Categories=' "$DESKTOP_TARGET"; then
    sed -i 's/^Categories=.*/Categories=Science;MedicalSoftware;/' "$DESKTOP_TARGET"
else
    printf '\nCategories=Science;MedicalSoftware;\n' >> "$DESKTOP_TARGET"
fi

# Ensure appimagetool can locate the icon at the AppDir root.
cp -L -- "$ICON_FILE" "$APPDIR/dinodent.png"
ln -sfn -- usr/share/applications/dinodent.desktop "$APPDIR/dinodent.desktop"

# Self-contained launcher. Do not force Fcitx for users with another IME.
cat > "$APPDIR/AppRun" <<'APPRUN'
#!/bin/sh
HERE="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
export LD_LIBRARY_PATH="$HERE/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="$HERE/usr/plugins"
exec "$HERE/usr/bin/DinoDent" "$@"
APPRUN
chmod +x "$APPDIR/AppRun"

# The output filename is stable and goes directly to installer/compiled.
# Build to a fresh path, then replace the previous output atomically.
# This also avoids appimagetool failing to overwrite a read-only old AppImage.
TEMP_APPIMAGE="$OUTPUT_DIR/.DinoDent-$$-x86_64.AppImage"
trap 'rm -f -- "$TEMP_APPIMAGE"; restore_sql_plugins; cleanup_desktop' EXIT
ARCH=x86_64 "$APPIMAGETOOL" "$APPDIR" "$TEMP_APPIMAGE"
mv -f -- "$TEMP_APPIMAGE" "$FINAL_APPIMAGE"
chmod +x "$FINAL_APPIMAGE"
printf '\nAppImage created: %s\n' "$FINAL_APPIMAGE"
printf 'Test Fcitx5: QT_IM_MODULE=fcitx "%s"\n' "$FINAL_APPIMAGE"
