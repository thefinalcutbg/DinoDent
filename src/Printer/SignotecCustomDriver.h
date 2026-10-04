#pragma once
#include <string>

// Self-contained implementation: HIDAPI + OpenSSL 3 + Qt Core/Gui.
// Call public APIs from one controlling thread. A background reader captures pen reports.
// HID exchanges share a mutex with the reader. Stop pauses it; Close joins it.
// Return -1 on error; STGetLastError() returns the calling thread's last error.
// Tested device: Sigma Lite. SHA-1/SHA-256; full rectangle only. Display stubs.
namespace SignotecDriver
{

enum TabletType : unsigned int
{
    None = 0x0000,
    Sigma = 0x0001,
    Zeta = 0x0005,
    Omega = 0x000B,
    Gamma = 0x000F,
    Delta = 0x0015
};
// STRSASetHash supports SHA-1 and SHA-256; the SDK rejects SHA-512.
enum HASHALGO
{
    kSha1 = 0,
    kSha256 = 1
};
enum HASHVALUE
{
    kCombination = 0,
    kHash1 = 1,
    kHash2 = 2
};
enum RSASCHEME
{
    kNoHashOID = 0,
    kPKCS1_V1_5 = 1,
    kPSS = 2
};
enum CERTTYPE
{
    kCert_DER = 0,
    kCSR_DER = 1,
    kCert_PEM = 2,
    kCSR_PEM = 3
};
enum FileType
{
    kTiff = 0,
    kPng = 1,
    kBmp = 2,
    kJpeg = 3,
    kGif = 4
};
enum Align
{
    kLeft = 0,
    kCenter = 1,
    kRight = 2
};

inline constexpr long STPAD_FONT_BOLD=1;
constexpr std::uint32_t RGB(unsigned r, unsigned g, unsigned b){ return (r&255)|((g&255)<<8)|((b&255)<<16);}

long STDeviceOpen(long index, bool erase);
long STErase();
long STSensorSetSignRect(long x, long y, long width, long height);
long STDisplaySetFont(const wchar_t *name, long size, long options);
long STDisplayGetHeight();
long STDisplaySetText(long x, long y, Align alignment, const wchar_t *text);
long STRSASetHash(const unsigned char *hash, HASHALGO algorithm, long options);
long STSignatureStart();
long STSignatureRetry();
long STSignatureStop();
long STSignatureConfirm();
long STRSASign(unsigned char *buffer, long *size, RSASCHEME scheme, HASHVALUE value, long options);
long STRSAGetSignData(unsigned char *buffer, long *size, long options);
long STRSASaveSigningCertAsStream(unsigned char *buffer, long *size, unsigned type);
long STSignatureSaveAsStreamEx(unsigned char *buffer, long *size, long resolution, long width, long height, unsigned type, long penWidth, unsigned color, long options);
long STDeviceClose(long index);
void STControlExit();
std::string STGetLastError();
} // namespace SignotecDriver
// SignData options: 0 = qCompress/zlib, 1 = uncompressed.
// PNG preset: 160 ppi, width/height/penWidth 0, blue COLORREF 0x00ff0000, options 0x5000.
// PNG is rendered locally, not pixel-identical to the SDK. macOS hardware is unverified.
