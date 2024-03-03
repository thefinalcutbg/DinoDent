# How to build:

# Building on Linux or MacOS
Use the included Qt pro file. It is configured to use the Homebrew directories for dependencies. As for LimeReport - it is configured to link the library statically (revisit the path to LimeReport source in the *.pro file)

# Building on Windows

## Building dependencies:

For convinience the source of Sqlite3, JsonCpp and TinyXml is already included and configured to be built and linked statically by CMake. You have to build the other dependencies yourself.

### LimeReport
This library is required for generating and printing the ambulatory sheets and invoices. Go to [https://github.com/fralx/LimeReport](https://github.com/fralx/LimeReport) and download the source code. Build it as dynamic library with CMake (or use QtCreator with the provided *.pri file). The additional QZint dependency is not required, so if you are building with Cmake, be sure to turn the option off. If you want to run DinoDent in debug mode, be sure to build another binary of LimeReport in Debug configuration, since the Qt Print Module has different binaries for Debug and Release.

### OpenSsl

Provide Openssl for your system. OpenSsl is a dependency of XmlSec and Libp11. The project has been tested with OpenSsl 1.1.1 distributed by the Qt Maintaince tool.

### LibXml2

This library is a dependency of XmlSec and it is required for the canonnicalization of the xml file during the signing. Download the latest release from [https://github.com/GNOME/libxml2/](https://github.com/GNOME/libxml2/)
It is strongly advisable to build it without libxslt and iconv to reduce dependency hell. If you are on Windows, start configure.js from the win32 folder with cscript from the console and set the parameters and the desired directories using this command:
```
cscript configure.js iconv=no xslt=no include=c:\YourIncludeDir lib=c:\YourBinariesDir
```

Then type:
```
nmake install
```

Wait for the build process to finish. This generates several binaries, but the one you need is called libxml2. The binaries whose names end _a are not required.


### XmlSec

The library is required for generating Xml signatures of xml queries sent to NHIF and NHIS. Download the latest release from [https://github.com/lsh123/xmlsec](https://github.com/lsh123/xmlsec)

There is an optional dependency on LibXslt which again is preferable to be omitted. To build XmlSec on Windows, navigate to the win32 folder of the and start configure.js from the command prompt with:
```
cscript configure.js iconv=no xslt=no static=no include=C:\Dev\xmlTools\include lib=C:\Dev\xmlTools\lib 
```
The script forces you to place BOTH the openssl and libxml2 include and lib folders in the directories configured above. Watch out for the folder names. Libxml include files by default are found in include/libxml2/libxml, but Xmlsec will search only for the libxml folder, so copy only that one to the include dir which you have specidied with the cscript. After you've configured the directories correctly, build the xmlsec using nmake.

### Libp11

This library is responsible for the PKCS11 interface. Go to [https://github.com/OpenSC/libp11](https://github.com/OpenSC/libp11) and download the latest release. Build the library following the instructions, or directly use the provided binaries from earlier releases for your OS. After the build process is finished, there will be 2 binaries - libp11 and pkcs11



If you have followed everything  as described, you'll have all of the required binaries:

- limereport-qt6 (LimeReport)

- libcrypto (OpenSSL)

- libssl (OpenSSL)

- libxml2 (LibXml2)

- libxmlsec (XmlSec)

- libxmlsec-openssl (XmlSec)

- libp11 (Libp11)

- pkcs11 (Libp11)

## Building DinoDent itself

Generate the project file with your IDE of choice, by running the CMakeLists.txt (be sure to modify the Qt directory at line 14 of the CMakeLists file, so it can point to your Qt library folder) or use the included MS Visual Studio project. Configure the Linker Additional Directories, so that they point to the libraries you've just built. The LimeReport debug/release binaries has to be linked according to the current build configuration of the project, so put them in different folders (e.g. lib/debug/limereport and lib/release/limereport). Build DinoDent and place the binaries in the folder of the executable.
