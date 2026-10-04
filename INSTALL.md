# How to build

## Building on Linux

Use the included Qt `.pro` file.

The `include.zip` file contains the bundled headers/source files and several header-only dependencies. Extract it into the main DinoDent project directory, so that the `include` folder is beside the `src` folder.

You will also need the [LimeReport](https://github.com/fralx/LimeReport) source code. It is linked statically through its `.pri` file, so make sure the path to the LimeReport source in `DinoDent.pro` is correct.

The build is tested with Qt 6.8.3 and OpenSSL 3.

### Dependencies

On Linux, system dependencies are installed using the distribution package manager and detected through `pkg-config`.

On Ubuntu/Debian install the required development packages with:

```bash
sudo apt update

sudo apt install \
    build-essential \
    pkg-config \
    libhidapi-dev \
    libxml2-dev \
    libp11-dev \
    libssl-dev
```

### Signature pads

DinoDent uses a custom HID-based implementation for communication with supported signotec/Evolis signature pads on Linux.

Linux normally restricts direct access to HID devices. A udev rule may therefore be required.

## Building on macOS

Use the included Qt `.pro` file.

The `include.zip` file contains the bundled headers/source files and several header-only dependencies. Extract it into the main DinoDent project directory, so that the `include` folder is beside the `src` folder.

You will also need the [LimeReport](https://github.com/fralx/LimeReport) source code. It is linked statically through its `.pri` file, so make sure the path to the LimeReport source in `DinoDent.pro` is correct.

The macOS build uses MacPorts for external dependencies because universal `x86_64`/`arm64` libraries are required.

The build is tested with Qt 6.8.3 and OpenSSL 3.

---

## Building on Windows

### Building dependencies

For convenience, the source code of SQLite3, JsonCpp and TinyXml is already included and configured to be built and linked statically. You have to build the remaining dependencies yourself.

### LimeReport

Go to [https://github.com/fralx/LimeReport](https://github.com/fralx/LimeReport) and download the source code.

Build it as a dynamic library with CMake, or use Qt Creator with the provided `.pri` file.

The additional QZint dependency is not required, so if you are building with CMake, make sure to disable it.

If you want to run DinoDent in Debug mode, build a separate Debug version of LimeReport, since Qt PrintSupport uses different binaries for Debug and Release builds.

### OpenSSL

Provide OpenSSL for your system. OpenSSL is also a dependency of libp11.

You can use the OpenSSL version distributed through the Qt Maintenance Tool.

### libxml2

This library is required for XML canonicalization during signing.

Download the latest release from:

[https://github.com/GNOME/libxml2](https://github.com/GNOME/libxml2)

It is advisable to build it without `libxslt` and `iconv` to reduce unnecessary dependencies.

On Windows, open the `win32` directory and run `configure.js` with `cscript`, specifying the desired include and library directories:

```text
cscript configure.js iconv=no xslt=no include=c:\YourIncludeDir lib=c:\YourBinariesDir
```

Then run:

```text
nmake install
```

After the build finishes, several binaries will be generated. The one required by DinoDent is `libxml2`. Binaries whose names end in `_a` are not required.

### libp11

This library provides the PKCS#11 interface.

Go to:

[https://github.com/OpenSC/libp11](https://github.com/OpenSC/libp11)

and download the latest release.

Build the library according to the project instructions, or use the provided binaries from an earlier compatible release.

After the build process is complete, there will be two relevant binaries:

- `libp11`
- `pkcs11`

If everything has been configured correctly, you should now have all required external binaries:

- `limereport-qt6` (LimeReport)
- `libcrypto` (OpenSSL)
- `libssl` (OpenSSL)
- `libxml2` (libxml2)
- `libp11` (libp11)
- `pkcs11` (libp11)

### Building DinoDent itself

On Windows, use the included Microsoft Visual Studio project file.

Configure your Qt version and set the Linker Additional Library Directories so that they point to the libraries you built above.

The LimeReport Debug and Release binaries must be linked according to the current project build configuration, so keep them in separate directories, for example:

```text
lib/debug/limereport
lib/release/limereport
```
