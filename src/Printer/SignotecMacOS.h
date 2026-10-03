#pragma once
#include <cstdint>
#include <string>

namespace SignotecMacOS {
enum HASHALGO { kSha1=0, kSha256=1, kSha512=2 };
enum HASHVALUE { kCombination=0, kHash1=1, kHash2=2 };
enum RSASCHEME { kNoHashOID=0, kPKCS1_V1_5=1, kPSS=2 };
enum CERTTYPE { kCert_DER=0, kCSR_DER=1, kCert_PEM=2, kCSR_PEM=3 };
enum FILETYPE { kTiff=0, kPng=1, kBmp=2, kJpeg=3, kGif=4 };
enum ALIGN { kLeft=0, kCenter=1, kRight=2 };
#ifndef STPAD_FONT_BOLD
inline constexpr long STPAD_FONT_BOLD=1;
#endif
#ifndef RGB
constexpr std::uint32_t RGB(unsigned r,unsigned g,unsigned b){return (r&255)|((g&255)<<8)|((b&255)<<16);}
#endif
long STDeviceOpen(long index,bool erase);
long STSensorSetSignRect(long x,long y,long width,long height);
long STDisplaySetFont(const wchar_t* name,long size,long options);
long STDisplayGetHeight();
long STDisplaySetText(long x,long y,ALIGN alignment,const wchar_t* text);
long STRSASetHash(const unsigned char* hash,HASHALGO algorithm,long options);
long STSignatureStart();
long STSignatureRetry();
long STSignatureStop();
long STSignatureConfirm();
long STRSASign(unsigned char* buffer,long* size,RSASCHEME scheme,HASHVALUE value,long options);
long STRSAGetSignData(unsigned char* buffer,long* size,long options);
long STRSASaveSigningCertAsStream(unsigned char* buffer,long* size,CERTTYPE type);
long STSignatureSaveAsStreamEx(unsigned char* buffer,long* size,long resolution,long width,long height,FILETYPE type,long penWidth,std::uint32_t color,long options);
long STDeviceClose(long index);
void STControlExit();
std::string STGetLastError();
}
