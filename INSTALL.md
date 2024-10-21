# How to build:

# Building on Linux or macOS
Use the included Qt pro file. It is configured to use the Homebrew directories for dependencies under Linux and MacPorts under macOS (because of the universal libraries). The include.zip file, which contains the headers of the libraries and some header-only libraries, has to be extracted beside the src folder. You will also need the LimeReport source code from GitHub, which is linked statically (revisit the path to LimeReport source in the DinoDent.pro file). The build is tested under Qt 6.5 and OpenSSL 3.

# Building on Windows

## Building dependencies:

For convinience the source of Sqlite3, JsonCpp and TinyXml is already included and configured to be built and linked statically by CMake. You have to build the other dependencies yourself.

### LimeReport
This library is required for generating and printing the ambulatory sheets and invoices. Go to [https://github.com/fralx/LimeReport](https://github.com/fralx/LimeReport) and download the source code. Build it as dynamic library with CMake (or use QtCreator with the provided *.pri file). The additional QZint dependency is not required, so if you are building with Cmake, be sure to turn the option off. If you want to run DinoDent in debug mode, be sure to build another binary of LimeReport in Debug configuration, since the Qt Print Module has different binaries for Debug and Release.

### OpenSsl

Provide OpenSsl for your system. OpenSsl is a dependency of libp11. Use the one distributed by the Qt Maintaince tool.

### libXml2

This library is required for the canonnicalization of the xml file during the signing. Download the latest release from [https://github.com/GNOME/libxml2/](https://github.com/GNOME/libxml2/)
It is strongly advisable to build it without libxslt and iconv to reduce dependency hell. If you are on Windows, start configure.js from the win32 folder with cscript from the console and set the parameters and the desired directories using this command:
```
cscript configure.js iconv=no xslt=no include=c:\YourIncludeDir lib=c:\YourBinariesDir
```

Then type:
```
nmake install
```

Wait for the build process to finish. This generates several binaries, but the one you need is called libxml2. The binaries whose names end _a are not required.


### Libp11

This library is responsible for the PKCS11 interface. Go to [https://github.com/OpenSC/libp11](https://github.com/OpenSC/libp11) and download the latest release. Build the library following the instructions, or directly use the provided binaries from earlier releases for your OS. After the build process is finished, there will be 2 binaries - libp11 and pkcs11



If you have followed everything  as described, you'll have all of the required binaries:

- limereport-qt6 (LimeReport)

- libcrypto (OpenSSL)

- libssl (OpenSSL)

- libxml2 (LibXml2)

- libp11 (Libp11)

- pkcs11 (Libp11)

## Building DinoDent itself

Generate the project file with your IDE of choice, by running the CMakeLists.txt (be sure to modify the Qt directory at line 14 of the CMakeLists file, so it can point to your Qt library folder) or use the included MS Visual Studio project. Configure the Linker Additional Directories, so that they point to the libraries you've just built. The LimeReport debug/release binaries has to be linked according to the current build configuration of the project, so put them in different folders (e.g. lib/debug/limereport and lib/release/limereport). Build DinoDent and place the binaries in the folder of the executable.
