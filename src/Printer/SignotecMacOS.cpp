// Self-contained DinoDent integration. No helper executable or temporary files.
#include "SignotecMacOS.h"
#include <QByteArray>
#include <QImage>
#include <QPainter>
#include <QBuffer>
#include <QCryptographicHash>
#include <QString>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>
#else
#if __has_include(<hidapi/hidapi.h>)
#include <hidapi/hidapi.h>
#elif __has_include(<hidapi.h>)
#include <hidapi.h>
#else
#error "HIDAPI headers missing: install HIDAPI and add its include directory to the DinoDent target."
#endif
#if __has_include(<hidapi/hidapi_darwin.h>)
#include <hidapi/hidapi_darwin.h>
#elif __has_include(<hidapi_darwin.h>)
#include <hidapi_darwin.h>
#else
// from HIDAPI's hidapi_darwin.h (available since HIDAPI 0.12).
extern "C" void HID_API_EXPORT_CALL hid_darwin_set_open_exclusive(int open_exclusive);
#endif
#if !defined(HID_API_VERSION) || HID_API_VERSION < HID_API_MAKE_VERSION(0, 14, 0)
#error "SignotecMacOS requires HIDAPI 0.14 or newer (hid_get_report_descriptor)."
#endif
#endif
#include <algorithm>
#include <vector>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <cmath>
#include <ctime>
#include <limits>
#include <iostream>
namespace {
using Bytes=std::vector<unsigned char>;
void put32(Bytes& b,size_t p,unsigned long v){for(unsigned i=0;i<4;++i)b.at(p+i)=static_cast<unsigned char>(v>>(8*i));}
unsigned long get32(const Bytes& b,size_t p){unsigned long v=0;for(unsigned i=0;i<4;++i)v|=static_cast<unsigned long>(b.at(p+i))<<(8*i);return v;}
struct PublicKey {Bytes modulus;std::uint32_t exponent;};
std::uint64_t ticks(){return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
#ifdef _WIN32
struct Handle {
    HANDLE h = INVALID_HANDLE_VALUE;
    explicit Handle(HANDLE value) : h(value) {}
    ~Handle() { if (h != INVALID_HANDLE_VALUE && h != nullptr) CloseHandle(h); }
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
};
void fail(const char* operation) { throw std::runtime_error(std::string(operation) + " Windows error=" + std::to_string(GetLastError())); }
// Always cancel and drain an unfinished operation before freeing its buffer/event.
DWORD transfer(HANDLE device, Bytes& bytes, bool writing, DWORD timeout) {
    Handle event(CreateEventW(nullptr,TRUE,FALSE,nullptr));
    if (!event.h) fail("CreateEvent");
    OVERLAPPED ov{}; ov.hEvent=event.h; DWORD done=0;
    BOOL ok=writing ? WriteFile(device,bytes.data(),static_cast<DWORD>(bytes.size()),&done,&ov)
                    : ReadFile(device,bytes.data(),static_cast<DWORD>(bytes.size()),&done,&ov);
    if (!ok && GetLastError()!=ERROR_IO_PENDING) fail(writing?"WriteFile":"ReadFile");
    if (!ok) {
        DWORD wait=WaitForSingleObject(event.h,timeout);
        if (wait!=WAIT_OBJECT_0) {
            CancelIoEx(device,&ov); GetOverlappedResult(device,&ov,&done,TRUE);
            throw std::runtime_error("HID transfer timed out or wait failed; operation cancelled");
        }
        if (!GetOverlappedResult(device,&ov,&done,FALSE)) fail("GetOverlappedResult");
    }
    return done;
}
struct Device { std::wstring path; HIDP_CAPS caps{}; };
std::vector<Device> discover() {
    GUID guid; HidD_GetHidGuid(&guid);
    HDEVINFO set=SetupDiGetClassDevsW(&guid,nullptr,nullptr,DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);
    if (set==INVALID_HANDLE_VALUE) fail("SetupDiGetClassDevs");
    struct Guard { HDEVINFO s; ~Guard(){SetupDiDestroyDeviceInfoList(s);} } guard{set};
    std::vector<Device> found;
    for (DWORD i=0;;++i) {
        SP_DEVICE_INTERFACE_DATA iface{}; iface.cbSize=sizeof(iface);
        if (!SetupDiEnumDeviceInterfaces(set,nullptr,&guid,i,&iface)) {
            if (GetLastError()==ERROR_NO_MORE_ITEMS) break; fail("SetupDiEnumDeviceInterfaces");
        }
        DWORD required=0;
        SetupDiGetDeviceInterfaceDetailW(set,&iface,nullptr,0,&required,nullptr);
        if (!required) fail("Interface detail size");
        Bytes storage(required);
        auto* detail=reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(storage.data());
        detail->cbSize=sizeof(*detail);
        if (!SetupDiGetDeviceInterfaceDetailW(set,&iface,detail,required,nullptr,nullptr)) fail("Interface detail");
        Handle h(CreateFileW(detail->DevicePath,0,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,0,nullptr));
        std::wstring path=detail->DevicePath;
        bool candidate=path.find(L"vid_2133")!=std::wstring::npos;
        if (h.h==INVALID_HANDLE_VALUE) {
            if (candidate) { std::wcout << L"Candidate: " << path << L"\n"; std::cout << "Metadata open failed: " << GetLastError() << std::endl; }
            continue;
        }
        HIDD_ATTRIBUTES attr{}; attr.Size=sizeof(attr);
        if (!HidD_GetAttributes(h.h,&attr) || attr.VendorID!=0x2133 || attr.ProductID!=0x0001) continue;
        PHIDP_PREPARSED_DATA prep=nullptr; HIDP_CAPS caps{};
        if (!HidD_GetPreparsedData(h.h,&prep)) fail("HidD_GetPreparsedData");
        auto status=HidP_GetCaps(prep,&caps); HidD_FreePreparsedData(prep);
        if (status!=HIDP_STATUS_SUCCESS) throw std::runtime_error("HidP_GetCaps failed");
        std::wcout << L"Found: " << detail->DevicePath << L"\n";
        std::cout << "Windows report sizes IN=" << caps.InputReportByteLength << " OUT=" << caps.OutputReportByteLength << "\n";
        if (caps.UsagePage==0xffff && caps.Usage==0xff && caps.InputReportByteLength==62 && caps.OutputReportByteLength==64)
            found.push_back({detail->DevicePath,caps});
    }
    return found;
}

using DeviceHandle=HANDLE;
struct DeviceConnection : Handle {
    explicit DeviceConnection(const Device& d):Handle(CreateFileW(d.path.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,nullptr)) {
        if(h==INVALID_HANDLE_VALUE)fail("Open tablet (close SDK applications)");
        if(!HidD_FlushQueue(h))fail("HidD_FlushQueue");
    }
};
void writeReport(DeviceHandle h,const Bytes& request) {
    Bytes api(64,0);std::copy(request.begin(),request.end(),api.begin()+1);
    if(transfer(h,api,true,2000)!=api.size())throw std::runtime_error("Short HID write");
}
Bytes readReport(DeviceHandle h,unsigned timeout) {
    Bytes api(62,0);
    if(transfer(h,api,false,timeout)!=62 || api[0]!=0)throw std::runtime_error("Unexpected Windows HID framing");
    api.erase(api.begin());return api;
}

#else
using DeviceHandle=hid_device*;
struct Device {std::string path;};
inline std::runtime_error hidFailure(hid_device* h,const char* operation){
    const wchar_t* e=hid_error(h);
    return std::runtime_error(std::string(operation)+": "+(e?QString::fromWCharArray(e).toStdString():"unknown HID error"));
}
struct HidRuntime {
    HidRuntime(){if(hid_init()!=0)throw hidFailure(nullptr,"Initialize HIDAPI");hid_darwin_set_open_exclusive(1);}
    ~HidRuntime()=default; // Do not shut down HIDAPI shared with other application components.
};
std::vector<Device> discover(){
    static HidRuntime runtime;
    auto* list=hid_enumerate(0x2133,0x0001);
    struct Guard{hid_device_info* p;~Guard(){hid_free_enumeration(p);}}guard{list};
    std::vector<Device> found;
    for(auto* p=list;p;p=p->next) {
        if(p->usage_page!=0xffff || p->usage!=0xff || !p->path)continue;
        if(std::any_of(found.begin(),found.end(),[&](const Device& d){return d.path==p->path;}))continue;
        found.push_back({p->path});std::cout<<"Found: "<<p->path<<'\n';
    }
    return found;
}
struct DeviceConnection {
    hid_device* h=nullptr;
    explicit DeviceConnection(const Device& d):h(hid_open_path(d.path.c_str())) {
        if(!h)throw hidFailure(nullptr,"Open tablet (close other tablet applications)");
        try {
            // Require the descriptor observed on the supported device, including
            // the absence of report IDs. Do not silently reinterpret a new model.
            const Bytes expected={0x06,0xff,0xff,0x09,0xff,0xa1,0x01,0x09,0xff,0x95,0x3d,0x75,0x08,0x15,0x80,0x25,0x7f,0x81,0x00,0x09,0xff,0x95,0x3f,0x75,0x08,0x15,0x80,0x25,0x7f,0x91,0x00,0xc0};
            Bytes descriptor(4096);int n=hid_get_report_descriptor(h,descriptor.data(),descriptor.size());
            if(n<0)throw hidFailure(h,"Read HID descriptor");descriptor.resize(n);
            if(descriptor!=expected)throw std::runtime_error("Unsupported HID report descriptor");
            // Drain stale input with a bound even if the tablet is streaming.
            unsigned char discard[128];auto deadline=ticks()+250;
            for(;;){int count=hid_read_timeout(h,discard,sizeof(discard),0);if(count<0)throw hidFailure(h,"Flush HID input");if(!count)break;if(ticks()>=deadline)throw std::runtime_error("Tablet is still streaming; reconnect before opening");}
        } catch(...) {hid_close(h);h=nullptr;throw;}
    }
    ~DeviceConnection(){if(h)hid_close(h);}
    DeviceConnection(const DeviceConnection&)=delete;DeviceConnection& operator=(const DeviceConnection&)=delete;
};
void writeReport(DeviceHandle h,const Bytes& request){
    Bytes api(64,0);std::copy(request.begin(),request.end(),api.begin()+1);
    int n=hid_write(h,api.data(),api.size());
    if(n<0)throw hidFailure(h,"Write HID report");
    if(n!=64)throw std::runtime_error("Short HID write");
}
Bytes readReport(DeviceHandle h,unsigned timeout){
    // Unlike Windows ReadFile, HIDAPI omits the zero placeholder on reads of
    // unnumbered reports. Byte 0 is the actual vendor opcode and must be kept.
    Bytes b(62);int n=hid_read_timeout(h,b.data(),b.size(),static_cast<int>(timeout));
    if(n<0)throw hidFailure(h,"Read HID report");
    if(!n)throw std::runtime_error("HID read timed out");
    if(n!=61)throw std::runtime_error("Unexpected macOS HID framing");
    b.resize(61);return b;
}

#endif
void wipeSecret(Bytes& b){if(!b.empty())OPENSSL_cleanse(b.data(),b.size());}
inline void cryptoCheck(bool ok,const char* message){if(!ok)throw std::runtime_error(std::string(message)+": "+ERR_error_string(ERR_get_error(),nullptr));}
class Crypto {
    using Key=std::unique_ptr<EVP_PKEY,decltype(&EVP_PKEY_free)>;
    using Context=std::unique_ptr<EVP_PKEY_CTX,decltype(&EVP_PKEY_CTX_free)>;
    using Cipher=std::unique_ptr<EVP_CIPHER_CTX,decltype(&EVP_CIPHER_CTX_free)>;
    Key rsa_{nullptr,EVP_PKEY_free};
    Cipher samples_{nullptr,EVP_CIPHER_CTX_free};
    Bytes aes_;
    PublicKey exportPublic(EVP_PKEY* key){
        cryptoCheck(EVP_PKEY_is_a(key,"RSA"),"Expected RSA key");
        BIGNUM *n=nullptr,*e=nullptr;
        cryptoCheck(EVP_PKEY_get_bn_param(key,OSSL_PKEY_PARAM_RSA_N,&n)==1,"RSA modulus");
        std::unique_ptr<BIGNUM,decltype(&BN_free)> ng(n,BN_free);
        cryptoCheck(EVP_PKEY_get_bn_param(key,OSSL_PKEY_PARAM_RSA_E,&e)==1,"RSA exponent");
        std::unique_ptr<BIGNUM,decltype(&BN_free)> eg(e,BN_free);
        cryptoCheck(BN_num_bits(e)<=32,"RSA exponent out of range");
        Bytes modulus(BN_num_bytes(n));
        cryptoCheck(BN_bn2lebinpad(n,modulus.data(),static_cast<int>(modulus.size()))==static_cast<int>(modulus.size()),"Encode RSA modulus");
        return {modulus,static_cast<std::uint32_t>(BN_get_word(e))};
    }
public:
    Crypto()=default;~Crypto(){wipeSecret(aes_);}
    Crypto(const Crypto&)=delete;Crypto& operator=(const Crypto&)=delete;
    PublicKey createSessionKey(){
        if(rsa_)throw std::runtime_error("Session key already created");
        Context ctx(EVP_PKEY_CTX_new_from_name(nullptr,"RSA",nullptr),EVP_PKEY_CTX_free);
        cryptoCheck(bool(ctx),"RSA context");cryptoCheck(EVP_PKEY_keygen_init(ctx.get())>0,"RSA keygen init");
        // Legacy 1024-bit session exchange is dictated by this pad's protocol.
        cryptoCheck(EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(),1024)>0,"RSA key size");
        EVP_PKEY* key=nullptr;cryptoCheck(EVP_PKEY_generate(ctx.get(),&key)>0,"Generate session key");rsa_.reset(key);
        return exportPublic(rsa_.get());
    }
    Bytes unwrapSessionKey(const Bytes& wrapped){
        if(wrapped.size()!=256 || !rsa_)throw std::runtime_error("Invalid wrapped session key");
        Bytes ciphertext(wrapped.begin(),wrapped.begin()+128);std::reverse(ciphertext.begin(),ciphertext.end());
        Context ctx(EVP_PKEY_CTX_new(rsa_.get(),nullptr),EVP_PKEY_CTX_free);cryptoCheck(bool(ctx),"RSA decrypt context");
        cryptoCheck(EVP_PKEY_decrypt_init(ctx.get())>0 && EVP_PKEY_CTX_set_rsa_padding(ctx.get(),RSA_PKCS1_PADDING)>0,"RSA decrypt init");
        Bytes plain(128);struct Guard{Bytes& b;~Guard(){wipeSecret(b);}}guard{plain};size_t n=plain.size();
        cryptoCheck(EVP_PKEY_decrypt(ctx.get(),plain.data(),&n,ciphertext.data(),ciphertext.size())>0,"RSA unwrap");
        if(n!=32)throw std::runtime_error("Expected 32-byte AES key");
        return Bytes(plain.rend()-32,plain.rend());
    }
    void setAesKey(const Bytes& key){
        if(key.size()!=32 || samples_)throw std::runtime_error("Invalid AES session state");
        aes_=key;samples_.reset(EVP_CIPHER_CTX_new());cryptoCheck(bool(samples_),"AES context");unsigned char iv[16]{};
        cryptoCheck(EVP_DecryptInit_ex(samples_.get(),EVP_aes_256_cbc(),nullptr,aes_.data(),iv)>0 && EVP_CIPHER_CTX_set_padding(samples_.get(),0)>0,"AES decrypt init");
    }
    Bytes encryptIndependent(const Bytes& input){
        if(input.empty() || input.size()%16 || aes_.size()!=32)throw std::runtime_error("Invalid AES input");
        Cipher ctx(EVP_CIPHER_CTX_new(),EVP_CIPHER_CTX_free);cryptoCheck(bool(ctx),"AES encrypt context");unsigned char iv[16]{};
        cryptoCheck(EVP_EncryptInit_ex(ctx.get(),EVP_aes_256_cbc(),nullptr,aes_.data(),iv)>0 && EVP_CIPHER_CTX_set_padding(ctx.get(),0)>0,"AES encrypt init");
        Bytes b(input.size()+16);int n=0,last=0;
        cryptoCheck(EVP_EncryptUpdate(ctx.get(),b.data(),&n,input.data(),static_cast<int>(input.size()))>0 && EVP_EncryptFinal_ex(ctx.get(),b.data()+n,&last)>0,"AES encrypt");
        b.resize(n+last);if(b.size()!=input.size())throw std::runtime_error("Unexpected AES output");return b;
    }
    void decryptSamples(Bytes& b){
        if(b.empty() || b.size()%16 || !samples_)throw std::runtime_error("Invalid AES report");
        int n=0;cryptoCheck(EVP_DecryptUpdate(samples_.get(),b.data(),&n,b.data(),static_cast<int>(b.size()))>0,"AES sample decrypt");
        if(n!=static_cast<int>(b.size()))throw std::runtime_error("Unexpected AES sample size");
    }
    PublicKey certificatePublicKey(const Bytes& cert){
        const auto* p=cert.data();std::unique_ptr<X509,decltype(&X509_free)> ctx(d2i_X509(nullptr,&p,static_cast<long>(cert.size())),X509_free);
        cryptoCheck(bool(ctx) && p==cert.data()+cert.size(),"Parse DER certificate");
        Key key(X509_get_pubkey(ctx.get()),EVP_PKEY_free);cryptoCheck(bool(key),"Certificate public key");return exportPublic(key.get());
    }
};
std::function<void(const Bytes&)> onSamples;
template<class Predicate> Bytes exchange(DeviceHandle h, const Bytes& request, Predicate matches) {
    if (request.size()!=63) throw std::runtime_error("Expected 63-byte wire request");
    writeReport(h,request);
    auto deadline=ticks()+3000;
    while (ticks()<deadline) {
        auto now=ticks();
        if (now>=deadline) break;
        auto remaining=static_cast<unsigned>(deadline-now);
        auto reply=readReport(h,std::max(1u,remaining));
        if (matches(reply)) return reply;
        if ((reply[0]&0xf0)==0x40 && (reply[0]&15) && onSamples) { onSamples(reply); continue; }
        // A periodic status report can arrive between a command and its response.
        if (reply[0]!=0x50 && reply[0]!=0x40) throw std::runtime_error("Unexpected response; stop rather than guessing");
    }
    throw std::runtime_error("Response deadline exceeded");
}
Bytes certificateRead(DeviceHandle h,unsigned long offset,unsigned long length) {
    if (!length || length>48) throw std::runtime_error("Invalid chunk length");
    Bytes cmd(63,0); cmd[0]=0xa8; put32(cmd,1,3); put32(cmd,5,offset); put32(cmd,9,length);
    auto reply=exchange(h,cmd,[=](const Bytes& b) {return b[0]==0x98 && get32(b,1)==3 && get32(b,5)==offset && get32(b,9)==length;});
    return Bytes(reply.begin()+13,reply.begin()+13+length);
}
void command(DeviceHandle h, unsigned char opcode, unsigned long object=0, unsigned long value=0) {
    Bytes b(63,0); b[0]=opcode; put32(b,1,object); put32(b,5,value);
    exchange(h,b,[=](const Bytes& r){return r[0]==(opcode==0x83?0x93:0x90);});
}
// Reconstructed SIG100 Lite RSA SignData format; no vendor SDK dependency.
// The AES key remains owned by the capture session. This module only receives
// encrypted metadata, the pad-wrapped key, and the original encrypted reports.
namespace direct {
inline void put16(Bytes& b,size_t p,unsigned v) {
    b.at(p)=static_cast<unsigned char>(v); b.at(p+1)=static_cast<unsigned char>(v>>8);
}
inline Bytes signMetadata(const Bytes& documentHash,std::int64_t timestamp) {
    if(documentHash.size()!=32) throw std::runtime_error("SignData requires SHA-256");
    Bytes b(128,0); put16(b,0,2);
    for(unsigned i=0;i<8;++i)b[2+i]=static_cast<unsigned char>(static_cast<std::uint64_t>(timestamp)>>(8*i));
    // Sensor format, dimensions, full-screen rectangle, SHA-256 capture mode,
    // raw pressure calibration, sample frequency, and internal hash identifiers.
    const std::pair<size_t,unsigned> fields[]={{10,1},{12,4096},{14,4096},
        {16,320},{18,160},{24,320},{26,160},{28,2},{34,1828},{36,4900},
        {38,1024},{44,320},{46,160},{48,250},{50,2},{52,2},{54,32}};
    for(auto field:fields)put16(b,field.first,field.second);
    put32(b,30,16384);
    std::copy(documentHash.rbegin(),documentHash.rend(),b.begin()+56);
    return b;
}
class SignData {
    Bytes compressed_,uncompressed_;
public:
    SignData(const Bytes& wrappedKey,const Bytes& encryptedMetadata,const Bytes& stream) {
        if(wrappedKey.size()!=256 || encryptedMetadata.size()!=128 || stream.size()<20)
            throw std::runtime_error("Invalid SignData components");
        // Until the extended-length format is verified, never truncate a length.
        if(stream.size()>65535-132)throw std::runtime_error("SignData stream exceeds the verified 65403-byte format limit");
        for(size_t pos=20;pos<stream.size();) {
            if(stream.size()-pos<4)throw std::runtime_error("Truncated SignData report count");
            auto count=get32(stream,pos);
            if(count<1 || count>4)throw std::runtime_error("Invalid SignData report count");
            size_t length=4+((count*12+15)/16)*16;
            if(length>stream.size()-pos)throw std::runtime_error("Truncated SignData report");
            pos+=length;
        }
        Bytes payload(4,0);put16(payload,0,256);put16(payload,2,132+static_cast<unsigned>(stream.size()));
        payload.insert(payload.end(),wrappedKey.begin(),wrappedKey.end());
        payload.resize(264);put32(payload,260,132);
        payload.insert(payload.end(),encryptedMetadata.begin(),encryptedMetadata.end());
        payload.insert(payload.end(),stream.begin(),stream.end());
        // SDK default normalized geometry for this model, full-screen capture.
        Bytes header(28,0);put32(header,0,11);put32(header,4,1);
        put16(header,8,101);put16(header,10,0x10);put16(header,12,2242);put16(header,14,2242);
        put16(header,16,8192);put16(header,18,4096);put16(header,20,4);put16(header,26,1023);
        uncompressed_=header;uncompressed_.insert(uncompressed_.end(),payload.begin(),payload.end());
        // qCompress prefixes a four-byte Qt length; the SDK expects only zlib.
        auto z=qCompress(reinterpret_cast<const uchar*>(payload.data()),static_cast<qsizetype>(payload.size()),9);
        if(z.size()<=4)throw std::runtime_error("SignData compression failed");
        put16(header,10,0x210);compressed_=header;
        compressed_.insert(compressed_.end(),z.begin()+4,z.end());
    }
    // SDK-style two-call interface: nullptr queries size; options 0/1 select
    // compressed/uncompressed. A short buffer is untouched and reports size.
    int STRSAGetSignData(unsigned char* output,long* size,int options=0) const {
        if(!size || (options!=0 && options!=1))return -1;
        const auto& data=options?uncompressed_:compressed_;
        long capacity=*size;*size=static_cast<long>(data.size());
        if(!output)return 0;
        if(capacity<*size)return -2;
        std::copy(data.begin(),data.end(),output);return 0;
    }
    Bytes bytes(int options=0) const {
        long size=0;
        if(STRSAGetSignData(nullptr,&size,options))throw std::runtime_error("Invalid SignData options");
        Bytes result(size);
        if(STRSAGetSignData(result.data(),&size,options))throw std::runtime_error("SignData export failed");
        return result;
    }
};
}


namespace direct {
struct PenSample {std::uint32_t time;std::uint16_t x,y,z1,z2;};
struct RenderPoint {double x,y,pressure;std::uint32_t time;};
// SIG100 Lite sensor format 1. Derived from the SDK's raw resistance and
// calibration conversion (3BB91 and 46D8B in the inspected x64 library).
inline std::vector<RenderPoint> calibratedPoints(const std::vector<PenSample>& samples) {
    std::vector<RenderPoint> result;bool afterLift=false;std::uint32_t previous=0;
    for(const auto& p:samples){
        if(p.x>4096 || p.y>4096)continue;
        double resistance=p.z1?static_cast<double>(static_cast<std::int64_t>(p.x)*(int(p.z2)-int(p.z1))/p.z1):0;
        resistance=std::min(resistance,16383.0);
        double pressure;
        if(resistance<0){pressure=-1;afterLift=true;}
        else if(afterLift){pressure=0;afterLift=false;}
        else {
            double clamped=std::clamp(resistance,1828.0,4900.0);
            pressure=std::clamp((3073.0-(clamped-1828.0))*1024.0/3073.0,1.0,1023.0);
        }
        if(static_cast<std::uint32_t>(p.time-previous)>2)pressure=0;
        previous=p.time;
        result.push_back({double(p.x)*2,double(p.y),pressure,p.time});
    }
    return result;
}
// The export contract used by the supplied function: PNG, blue, 160 ppi,
// original physical size, crop to ink, transparent, pressure-varying width.
// Qt rasterization/width interpolation is not bit-identical to the SDK renderer.
class SignatureImage {
    QByteArray cached_;
public:
    explicit SignatureImage(const std::vector<PenSample>& samples) {
        constexpr int ppi=160;
        auto points=calibratedPoints(samples);
        const double scale=ppi/2242.0;
        QImage canvas(static_cast<int>(std::ceil(8192*scale))+16,static_cast<int>(std::ceil(4096*scale))+16,QImage::Format_ARGB32_Premultiplied);
        if(canvas.isNull())throw std::runtime_error("Cannot allocate signature image");
        canvas.fill(Qt::transparent);QPainter painter(&canvas);painter.setRenderHint(QPainter::Antialiasing);
        QPointF previous;double previousWidth=0;bool down=false;
        for(const auto& p:points){
            QPointF current(8+p.x*scale,8+p.y*scale);
            if(p.pressure<=0){previous=current;down=false;continue;}
            double width=(ppi/160.0)*(0.6+2.4*std::sqrt(p.pressure/1023.0));
            painter.setPen(QPen(Qt::blue,down?(width+previousWidth)/2:width,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
            if(down)painter.drawLine(previous,current);else painter.drawPoint(current);
            previous=current;previousWidth=width;down=true;
        }
        painter.end();int left=canvas.width(),top=canvas.height(),right=-1,bottom=-1;
        for(int y=0;y<canvas.height();++y){auto row=reinterpret_cast<const QRgb*>(canvas.constScanLine(y));for(int x=0;x<canvas.width();++x){if(qAlpha(row[x])){left=std::min(left,x);right=std::max(right,x);top=std::min(top,y);bottom=std::max(bottom,y);}}}
        if(right<left)throw std::runtime_error("No drawable pen samples; no signature image");
        auto image=canvas.copy(left,top,right-left+1,bottom-top+1);
        image.setDotsPerMeterX(qRound(ppi/0.0254));image.setDotsPerMeterY(qRound(ppi/0.0254));
        QBuffer buffer(&cached_);if(!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer,"PNG"))throw std::runtime_error("PNG encode failed");
    }
    int STSignatureSaveAsStreamEx(unsigned char* output,long* size,long resolution=160,
        long width=0,long height=0,int fileType=1,long penWidth=0,
        std::uint32_t color=0x00ff0000,long options=0x5000) const {
        if(!size)return -1;
        // This capture's cached image implements precisely the caller's preset.
        // As in the SDK, the fetch call uses the cached bytes, ignoring settings.
        if(!output && (resolution!=160 || width!=0 || height!=0 || fileType!=1 ||
            penWidth!=0 || color!=0x00ff0000 || options!=0x5000))return -3;
        long capacity=*size;*size=static_cast<long>(cached_.size());
        if(!output)return 0;if(capacity<*size)return -2;
        std::copy(cached_.begin(),cached_.end(),output);return 0;
    }
    const QByteArray& png()const{return cached_;}
};
}
// Included after the transport helpers; no vendor SDK calls.
unsigned short get16(const Bytes& b,size_t p) { return b.at(p)|(b.at(p+1)<<8); }
unsigned long regWrite(DeviceHandle h,unsigned char selector,unsigned long value) {
    Bytes b(63,0); b[0]=0x10; b[1]=selector; b[2]=4; put32(b,3,value);
    auto r=exchange(h,b,[=](const Bytes& r){return r[0]==0x20 && r[1]==selector && r[2]==4;});
    return get32(r,3);
}
Bytes sha256(const Bytes& b) {
    auto result=QCryptographicHash::hash(QByteArray(reinterpret_cast<const char*>(b.data()),b.size()),QCryptographicHash::Sha256);
    return Bytes(result.begin(),result.end());
}
// Raw public RSA operation for the pad's nonstandard 64-byte PSS mHash.
// All arithmetic here is on public values; private-key operations use CryptoAPI.
Bytes addMod(Bytes a,const Bytes& b,const Bytes& modulus) {
    unsigned carry=0;
    for(size_t i=0;i<a.size();++i){unsigned v=a[i]+b[i]+carry; a[i]=v&255; carry=v>>8;}
    bool ge=true;
    for(size_t i=a.size();i-->0;) {if(a[i]!=modulus[i]){ge=a[i]>modulus[i];break;}}
    if(ge){int borrow=0;for(size_t i=0;i<a.size();++i){int v=int(a[i])-modulus[i]-borrow; a[i]=static_cast<unsigned char>(v);borrow=v<0;}}
    return a;
}
Bytes multiplyMod(Bytes a,const Bytes& b,const Bytes& modulus) {
    Bytes out(a.size(),0);
    for(size_t i=0;i<b.size()*8;++i){if((b[i/8]>>(i%8))&1)out=addMod(out,a,modulus);a=addMod(a,a,modulus);}return out;
}
bool verifyPadSignature(Crypto& crypto,const Bytes& cert,const Bytes& sig,const Bytes& docHash,const Bytes& streamHash) {
    auto pub=crypto.certificatePublicKey(cert);
    if(pub.modulus.size()!=256 || sig.size()!=256 || docHash.size()!=32 || streamHash.size()!=32)return false;
    Bytes modulus=pub.modulus;modulus.push_back(0);
    Bytes base(sig.rbegin(),sig.rend());base.push_back(0);
    bool smaller=false;for(size_t i=256;i-->0;){if(base[i]!=modulus[i]){smaller=base[i]<modulus[i];break;}}if(!smaller)return false;
    Bytes value(257,0);value[0]=1;auto exponent=pub.exponent;
    while(exponent){if(exponent&1)value=multiplyMod(value,base,modulus);exponent>>=1;if(exponent)base=multiplyMod(base,base,modulus);}
    value.pop_back();std::reverse(value.begin(),value.end());
    if(value.back()!=0xbc || (value[0]&128))return false;
    Bytes H(value.begin()+223,value.begin()+255), mask;
    for(unsigned i=0;mask.size()<223;++i){auto input=H;input.insert(input.end(),{0,0,0,static_cast<unsigned char>(i)});auto part=sha256(input);mask.insert(mask.end(),part.begin(),part.end());}
    Bytes db(value.begin(),value.begin()+223);for(size_t i=0;i<db.size();++i)db[i]^=mask[i];db[0]&=127;
    if(!std::all_of(db.begin(),db.begin()+190,[](auto b){return b==0;}) || db[190]!=1)return false;
    Bytes input(8,0);input.insert(input.end(),streamHash.begin(),streamHash.end());input.insert(input.end(),docHash.begin(),docHash.end());input.insert(input.end(),db.end()-32,db.end());
    return sha256(input)==H;
}
void objectWrite(DeviceHandle h,unsigned long object,const Bytes& data) {
    for (size_t pos=0;pos<data.size();pos+=48) {
        auto n=std::min(size_t(48),data.size()-pos); Bytes b(63,0); b[0]=0xa8;
        put32(b,1,object); put32(b,5,static_cast<unsigned long>(pos)); put32(b,9,static_cast<unsigned long>(n));
        std::copy_n(data.begin()+pos,n,b.begin()+13); exchange(h,b,[](const Bytes& r){return r[0]==0x90;});
    }
}
Bytes objectRead(DeviceHandle h,unsigned long object,unsigned long size) {
    Bytes out;
    for (unsigned long pos=0;pos<size;pos+=48) {
        auto n=std::min(48UL,size-pos); Bytes b(63,0); b[0]=0xa8;
        put32(b,1,object); put32(b,5,pos); put32(b,9,n);
        auto r=exchange(h,b,[=](const Bytes& r){return r[0]==0x98 && get32(r,1)==object && get32(r,5)==pos && get32(r,9)==n;});
        out.insert(out.end(),r.begin()+13,r.begin()+13+n);
    } return out;
}
Bytes query(DeviceHandle h,unsigned char selector) {
    Bytes b(63,0); b[1]=selector;
    return exchange(h,b,[=](const Bytes& r){return r[0]==0x20 && r[1]==selector;});
}
struct Engine {
    enum Phase{Closed,Open,Capturing,Confirmed,Stopped,Failed};
    Phase phase=Closed;
    std::unique_ptr<DeviceConnection> device;
    std::unique_ptr<Crypto> crypto;
    Bytes hash,stream,signature,certificate,png;
    std::unique_ptr<direct::SignData> signData;
    std::vector<direct::PenSample> points;
    std::int64_t timestamp=0;
    std::string failure;
    bool imageQueried=false;
    void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
    void clearResult(){stream.clear();signature.clear();certificate.clear();png.clear();signData.reset();points.clear();imageQueried=false;}
    void close(){
        std::exception_ptr failure;
        if(device){try{stop();}catch(...){failure=std::current_exception();}}
        onSamples={};crypto.reset();device.reset();hash.clear();clearResult();phase=Closed;
        if(failure)std::rethrow_exception(failure);
    }
    ~Engine(){try{close();}catch(...){}}
    void open(long index,bool erase){
        require(phase==Closed && index==0,"Only closed state and device index 0 are supported");
        auto devices=discover();require(devices.size()==1,"Expected exactly one supported SIG100 Lite");
        device=std::make_unique<DeviceConnection>(devices[0]);
        try{
            auto info=query(device->h,0x1f);
            require(get32(info,3)==1 && get32(info,31)==4096 && get32(info,35)==4096 && get32(info,19)==320 && get32(info,23)==160 && get32(info,39)==16384 && get32(info,43)==1 && get32(info,51)==92780 && get32(info,55)==46390,"Unsupported tablet geometry/model");
            command(device->h,0xa8,0x80);command(device->h,0x83);regWrite(device->h,0x20,1);
            if(erase)command(device->h,0x86,0xffff);
            failure.clear();phase=Open;
        }catch(...){device.reset();throw;}
    }
    void stop(){
        onSamples={};
        if(!device)return;
        // Drain any in-flight pen reports through the stop acknowledgement.
        onSamples=[](const Bytes&){};
        try{command(device->h,0x83);regWrite(device->h,0x20,1);regWrite(device->h,0x23,0);command(device->h,0xa8,0x80);}
        catch(...){onSamples={};crypto.reset();phase=Failed;throw;}
        onSamples={};crypto.reset();phase=Stopped;
    }
    void start(){
        require(device && (phase==Open || phase==Stopped),"Open the device before starting capture");
        require(hash.size()==32,"Set the document SHA-256 before capture");
        clearResult();failure.clear();crypto=std::make_unique<Crypto>();
        auto h=device->h;
        try{
            auto pub=crypto->createSessionKey();
            command(h,0xa8,0x80);command(h,0x83);command(h,0xa8,0x27,pub.exponent);
            Bytes modulus(256,0);std::copy(pub.modulus.begin(),pub.modulus.end(),modulus.begin());objectWrite(h,0x28,modulus);
            command(h,0xa8,0x29,6);
            auto key=crypto->unwrapSessionKey(objectRead(h,0x17,256));
            struct Wipe{Bytes& key;~Wipe(){wipeSecret(key);}}wipe{key};crypto->setAesKey(key);
            auto info=query(h,0x1f),info3=query(h,3);stream.resize(20);
            put32(stream,0,get32(info,15));put32(stream,4,2);put32(stream,8,get32(info3,39)&~1024UL);put32(stream,12,info[12]);put32(stream,16,info[11]);
            Bytes block(64,0);std::copy(hash.rbegin(),hash.rend(),block.begin());objectWrite(h,0x0f,crypto->encryptIndependent(block));command(h,0xa8,0x29,5);
            onSamples=[this](const Bytes& r){
                unsigned count=r[0]&15;require(count>=1 && count<=4,"Invalid sample count");size_t length=((count*12+15)/16)*16;
                require(stream.size()+4+length<=65403,"Signature exceeds supported SignData length; retry with a shorter signature");
                size_t pos=stream.size();stream.resize(pos+4);put32(stream,pos,count);stream.insert(stream.end(),r.begin()+4,r.begin()+4+length);
                Bytes plain(r.begin()+4,r.begin()+4+length);crypto->decryptSamples(plain);
                for(unsigned i=0;i<count;++i){size_t p=i*12;points.push_back({static_cast<std::uint32_t>(get32(plain,p)),get16(plain,p+4),get16(plain,p+6),get16(plain,p+8),get16(plain,p+10)});}
            };
            regWrite(h,0x20,1);require(regWrite(h,0x31,0xffffffff)==250,"Expected 250 Hz sampling mode");
            regWrite(h,0x32,0);regWrite(h,0x33,0);regWrite(h,0x34,320);regWrite(h,0x35,160);regWrite(h,0x23,1);command(h,0x86,0xffff);
            Bytes request(63,0);request[0]=0x82;request[1]=2;timestamp=static_cast<std::int64_t>(std::time(nullptr));
            exchange(h,request,[](const Bytes& r){return r[0]==0x92;});regWrite(h,0x20,2);phase=Capturing;
        }catch(...){auto original=std::current_exception();try{stop();}catch(...){}phase=Failed;std::rethrow_exception(original);}
    }
    void poll(){
        if(phase!=Capturing)return;
        try{
            auto r=readReport(device->h,2000);
            if((r[0]&0xf0)==0x40 && (r[0]&15))onSamples(r);
            else require(r[0]==0x40 || r[0]==0x50,"Unexpected capture report");
        }catch(const std::exception& e){failure=e.what();try{stop();}catch(...){}phase=Failed;}
    }
    void confirm(){
        if(phase==Failed)throw std::runtime_error(failure.empty()?"Capture failed":failure);
        require(phase==Capturing,"No active capture to confirm");auto h=device->h;
        try{
            command(h,0x83);onSamples={};phase=Stopped;
            require(!points.empty(),"No pen samples; nothing signed");direct::SignatureImage image(points);
            auto streamHash=sha256(stream);auto reversed=streamHash;std::reverse(reversed.begin(),reversed.end());objectWrite(h,0x22,reversed);
            regWrite(h,0x20,1);regWrite(h,0x23,0);command(h,0xa8,0x10,1);
            signature=objectRead(h,0x17,256);std::reverse(signature.begin(),signature.end());
            auto size=get32(certificateRead(h,0,4),0);require(size>0 && size<=1024*1024,"Invalid certificate length");
            for(unsigned long pos=0;pos<size;pos+=48){auto part=certificateRead(h,pos+4,std::min(48UL,size-pos));certificate.insert(certificate.end(),part.begin(),part.end());}
            require(verifyPadSignature(*crypto,certificate,signature,hash,streamHash),"Device signature failed verification");
            command(h,0xa8,0x16,0);auto wrapped=objectRead(h,0x17,256);
            signData=std::make_unique<direct::SignData>(wrapped,crypto->encryptIndependent(direct::signMetadata(hash,timestamp)),stream);
            png=Bytes(image.png().begin(),image.png().end());command(h,0xa8,0x80);crypto.reset();phase=Confirmed;
        }catch(...){auto original=std::current_exception();try{stop();}catch(...){}clearResult();phase=Failed;std::rethrow_exception(original);}
    }
    long copy(const Bytes& data,unsigned char* output,long* size){
        require(phase==Confirmed && size && !data.empty(),"No confirmed export or invalid size pointer");
        require(data.size()<=static_cast<size_t>(std::numeric_limits<long>::max()),"Export too large");
        if(!output){*size=static_cast<long>(data.size());return 0;}
        long capacity=*size;*size=static_cast<long>(data.size());require(capacity>=*size,"Output buffer too small; required size returned");std::copy(data.begin(),data.end(),output);return 0;
    }
};
// All HID and crypto state stays on this thread. The application's modal dialog
// remains free to run while input reports are consumed in the background.
class Worker {
    std::mutex mutex_;std::condition_variable wake_;bool quit_=false;
    std::deque<std::function<void(Engine&)>> queue_;std::thread thread_;
public:
    Worker():thread_([this]{
        Engine engine;
        for(;;){
            std::function<void(Engine&)> job;
            {std::unique_lock<std::mutex> lock(mutex_);
                if(queue_.empty() && engine.phase!=Engine::Capturing)wake_.wait(lock,[this]{return quit_ || !queue_.empty();});
                if(quit_)break;
                if(!queue_.empty()){job=std::move(queue_.front());queue_.pop_front();}
            }
            if(job)job(engine);else engine.poll();
        }
    }){}
    ~Worker(){{std::lock_guard<std::mutex> lock(mutex_);quit_=true;}wake_.notify_one();thread_.join();}
    template<class F> long call(F action){
        auto task=std::make_shared<std::packaged_task<long(Engine&)>>(std::move(action));auto result=task->get_future();
        {std::lock_guard<std::mutex> lock(mutex_);queue_.push_back([task](Engine& e){(*task)(e);});}wake_.notify_one();return result.get();
    }
};
Worker& worker(){static Worker instance;return instance;}
thread_local std::string lastError;
template<class F> long invoke(F f)noexcept{try{long rc=worker().call(f);lastError.clear();return rc;}catch(const std::exception& e){lastError=e.what();return -1;}catch(...){lastError="Unknown Signotec error";return -1;}}
}
namespace SignotecMacOS {
long STDeviceOpen(long index,bool erase){return invoke([=](Engine& e){e.open(index,erase);return 0L;});}
long STSensorSetSignRect(long x,long y,long width,long height){return invoke([=](Engine& e){e.require(e.phase==Engine::Open || e.phase==Engine::Stopped,"Set rectangle before Start");e.require(!x && !y && !width && !height,"Only full rectangle (0,0,0,0) is supported");return 0L;});}
long STDisplaySetFont(const wchar_t*,long,long){return 0;}
long STDisplayGetHeight(){return 160;} // Nominal logical height; no LCD access.
long STDisplaySetText(long,long,ALIGN,const wchar_t*){return 0;}
long STRSASetHash(const unsigned char* hash,HASHALGO algorithm,long options){return invoke([=](Engine& e){e.require((e.phase==Engine::Open || e.phase==Engine::Stopped) && hash && algorithm==kSha256 && options==0,"Set a SHA-256 digest before capture; only options 0 supported");e.hash.assign(hash,hash+32);return 0L;});}
long STSignatureStart(){return invoke([](Engine& e){e.start();return 0L;});}
long STSignatureRetry(){return invoke([](Engine& e){e.require(e.phase==Engine::Capturing,"Retry requires active capture");e.stop();e.start();return 0L;});}
long STSignatureStop(){return invoke([](Engine& e){e.require(bool(e.device),"Device is closed");e.stop();e.clearResult();return 0L;});}
long STSignatureConfirm(){return invoke([](Engine& e){e.confirm();return 0L;});}
long STRSASign(unsigned char* buffer,long* size,RSASCHEME scheme,HASHVALUE value,long options){return invoke([=](Engine& e){e.require(scheme==kPSS && value==kCombination && !options,"Only PSS/combination/options 0 supported");return e.copy(e.signature,buffer,size);});}
long STRSAGetSignData(unsigned char* buffer,long* size,long options){return invoke([=](Engine& e){e.require(e.phase==Engine::Confirmed && bool(e.signData) && (options==0 || options==1),"No confirmed SignData or unsupported options");return e.copy(e.signData->bytes(static_cast<int>(options)),buffer,size);});}
long STRSASaveSigningCertAsStream(unsigned char* buffer,long* size,CERTTYPE type){return invoke([=](Engine& e){e.require(type==kCert_DER,"Only DER supported");return e.copy(e.certificate,buffer,size);});}
long STSignatureSaveAsStreamEx(unsigned char* buffer,long* size,long resolution,long width,long height,FILETYPE type,long penWidth,std::uint32_t color,long options){return invoke([=](Engine& e){
    if(!buffer){e.require(resolution==160 && !width && !height && type==kPng && !penWidth && color==0xff0000 && options==0x5000,"Supported PNG preset: 160ppi, native size, automatic blue pen, 0x5000");auto rc=e.copy(e.png,nullptr,size);e.imageQueried=true;return rc;}
    e.require(e.imageQueried,"Query PNG size before fetching");return e.copy(e.png,buffer,size);
});}
long STDeviceClose(long index){return invoke([=](Engine& e){e.require(index==0,"Only index 0 supported");e.close();return 0L;});}
void STControlExit(){(void)invoke([](Engine& e){e.close();return 0L;});}
std::string STGetLastError(){return lastError;}
}
