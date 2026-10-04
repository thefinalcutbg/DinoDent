#!/usr/bin/env bash

set -e

# ------------------------------------------------------------
# DinoDent AppImage builder
# ------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

TOOLS_DIR="$HOME/dev/appimage-tools"
APPDIR="$TOOLS_DIR/DinoDent.AppDir"

OUTPUT_DIR="$PROJECT_DIR/installer/compiled"

DESKTOP_FILE="$SCRIPT_DIR/dinodent.desktop"
ICON_FILE="$SCRIPT_DIR/dinodent.png"

LINUXDEPLOY="$TOOLS_DIR/linuxdeploy-x86_64.AppImage"
QT_PLUGIN="$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"

DINO_BIN="/home/thefinalcut/dev/build-DinoDent-Desktop_Qt_6_8_3-Release/DinoDent"
QMAKE="/home/thefinalcut/dev/Qt/6.8.3/gcc_64/bin/qmake"

# ------------------------------------------------------------
# Check DinoDent executable
# ------------------------------------------------------------

if [ ! -f "$DINO_BIN" ]; then
    echo "DinoDent executable not found:"
    echo "$DINO_BIN"
    exit 1
fi

if [ ! -x "$DINO_BIN" ]; then
    echo "DinoDent executable is not executable:"
    echo "$DINO_BIN"
    exit 1
fi

# ------------------------------------------------------------
# Check qmake
# ------------------------------------------------------------

if [ ! -f "$QMAKE" ]; then
    echo "qmake not found:"
    echo "$QMAKE"
    exit 1
fi

echo
echo "DinoDent executable:"
echo "  $DINO_BIN"

echo
echo "Using qmake:"
echo "  $QMAKE"

"$QMAKE" -v

# ------------------------------------------------------------
# Check required files
# ------------------------------------------------------------

if [ ! -f "$DESKTOP_FILE" ]; then
    echo "Desktop file not found:"
    echo "$DESKTOP_FILE"
    exit 1
fi

if [ ! -f "$ICON_FILE" ]; then
    echo "Icon file not found:"
    echo "$ICON_FILE"
    exit 1
fi

# ------------------------------------------------------------
# Create directories
# ------------------------------------------------------------

mkdir -p "$TOOLS_DIR"
mkdir -p "$OUTPUT_DIR"

# ------------------------------------------------------------
# Download linuxdeploy if missing
# ------------------------------------------------------------

if [ ! -f "$LINUXDEPLOY" ]; then
    echo
    echo "Downloading linuxdeploy..."

    wget \
        -O "$LINUXDEPLOY" \
        https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
fi

# ------------------------------------------------------------
# Download Qt plugin if missing
# ------------------------------------------------------------

if [ ! -f "$QT_PLUGIN" ]; then
    echo
    echo "Downloading linuxdeploy Qt plugin..."

    wget \
        -O "$QT_PLUGIN" \
        https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
fi

chmod +x "$LINUXDEPLOY"
chmod +x "$QT_PLUGIN"

# ------------------------------------------------------------
# Check executable dependencies
# ------------------------------------------------------------

echo
echo "Checking DinoDent dependencies..."

if ldd "$DINO_BIN" | grep -q "not found"; then
    echo
    echo "ERROR: Missing shared libraries:"
    echo

    ldd "$DINO_BIN" | grep "not found"

    exit 1
fi

# ------------------------------------------------------------
# Temporarily disable unused Qt SQL plugins
# ------------------------------------------------------------

QT_DIR="$(dirname "$(dirname "$QMAKE")")"
SQLDRIVER_DIR="$QT_DIR/plugins/sqldrivers"
SQLDRIVER_BACKUP="$TOOLS_DIR/sql_driver_backup"

mkdir -p "$SQLDRIVER_BACKUP"

restore_sql_plugins()
{
    if [ -d "$SQLDRIVER_BACKUP" ]; then
        for file in "$SQLDRIVER_BACKUP"/*.so; do
            [ -e "$file" ] || continue
            mv "$file" "$SQLDRIVER_DIR/"
        done
    fi
}

trap restore_sql_plugins EXIT

if [ -d "$SQLDRIVER_DIR" ]; then
    echo
    echo "Temporarily disabling unused Qt SQL drivers..."

    for driver in \
        libqsqlmimer.so \
        libqsqlodbc.so \
        libqsqlpsql.so \
        libqsqlmysql.so
    do
        if [ -f "$SQLDRIVER_DIR/$driver" ]; then
            echo "  $driver"
            mv \
                "$SQLDRIVER_DIR/$driver" \
                "$SQLDRIVER_BACKUP/"
        fi
    done
fi

# ------------------------------------------------------------
# Clean previous AppDir / AppImage
# ------------------------------------------------------------

echo
echo "Cleaning previous AppImage build..."

rm -rf "$APPDIR"

find "$TOOLS_DIR" \
    -maxdepth 1 \
    -type f \
    -name "DinoDent*.AppImage" \
    -delete

mkdir -p "$APPDIR"

# ------------------------------------------------------------
# Build AppImage
# ------------------------------------------------------------

echo
echo "Building DinoDent AppImage..."

cd "$TOOLS_DIR"

export QMAKE="$QMAKE"

"$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --executable "$DINO_BIN" \
    --desktop-file "$DESKTOP_FILE" \
    --icon-file "$ICON_FILE" \
    --plugin qt \
    --output appimage

# ------------------------------------------------------------
# Find generated AppImage
# ------------------------------------------------------------

APPIMAGE="$(find "$TOOLS_DIR" \
    -maxdepth 1 \
    -type f \
    -name "DinoDent*.AppImage" \
    ! -name "linuxdeploy*" \
    | head -n 1)"

if [ -z "$APPIMAGE" ]; then
    echo
    echo "ERROR: AppImage was not generated."
    exit 1
fi

chmod +x "$APPIMAGE"

# ------------------------------------------------------------
# Move final AppImage to installer/compiled
# ------------------------------------------------------------

FINAL_APPIMAGE="$OUTPUT_DIR/$(basename "$APPIMAGE")"

rm -f "$FINAL_APPIMAGE"
mv "$APPIMAGE" "$FINAL_APPIMAGE"

echo
echo "========================================"
echo "AppImage created successfully"
echo "========================================"
echo
echo "$FINAL_APPIMAGE"
echo
echo "Run it with:"
echo
echo "  \"$FINAL_APPIMAGE\""
echo
