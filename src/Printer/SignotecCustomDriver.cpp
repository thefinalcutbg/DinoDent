#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "SignotecCustomDriver.h"

// Reading guide:
// 1. SignData and SignatureImage build the exported files locally.
// 2. Session state stores the open handle, encryption key, samples and exports.
// 3. Transport helpers send HID commands and decode incoming pen reports.
// 4. The public ST functions implement the application-facing workflow.
//
// Normal use: Open -> SetHash -> Start -> Confirm -> exports -> Close.
// Retry starts a fresh capture with the same document hash. Stop keeps samples.
// Only one reader uses the HID handle at a time: exchangeReport holds the same
// mutex as the background receiver and handles pen reports while awaiting replies.

#include <hidapi.h>

#include <cmath>
#include <cstdint>
#include <ctime>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <limits>
#include <memory>

#include <mutex>
#include <openssl/rsa.h>
#include <optional>
#include <stdexcept>
#include <thread>
#include <vector>

#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QCryptographicHash>
#include <QImage>
#include <QPainter>

#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

namespace
{
using Bytes = std::vector<unsigned char>;
static void put32(Bytes &b, size_t p, unsigned long value)
{
    for (unsigned i = 0; i < 4; ++i)
        b.at(p + i) = static_cast<unsigned char>(value >> (8 * i));
}
static unsigned long get32(const Bytes &b, size_t p)
{
    unsigned long v = 0;
    for (unsigned i = 0; i < 4; ++i)
        v |= static_cast<unsigned long>(b.at(p + i)) << (8 * i);
    return v;
}
// Reconstructed SIG100 Lite RSA SignData format; no vendor SDK dependency.
// The AES key remains owned by the capture session. This module only receives
// encrypted metadata, the pad-wrapped key, and the original encrypted reports.
namespace direct
{
inline void put16(Bytes &b, size_t p, unsigned v)
{
    b.at(p) = static_cast<unsigned char>(v);
    b.at(p + 1) = static_cast<unsigned char>(v >> 8);
}
inline Bytes signMetadata(const Bytes &documentHash, std::int64_t timestamp)
{
    if (documentHash.size() != 20 && documentHash.size() != 32)
        throw std::runtime_error("SignData requires SHA-1 or SHA-256");
    const unsigned mode = documentHash.size() == 20 ? 1 : 2;
    Bytes metadata(128, 0);
    put16(metadata, 0, 2); // Metadata version.
    for (unsigned i = 0; i < 8; ++i)
        metadata[2 + i] = static_cast<unsigned char>(static_cast<std::uint64_t>(timestamp) >> (8 * i));

    // These offsets and calibration values match the tested SIG100 Lite format.
    put16(metadata, 10, 1); // Sensor format.
    put16(metadata, 12, 4096);
    put16(metadata, 14, 4096);
    put16(metadata, 16, 320);
    put16(metadata, 18, 160);
    put16(metadata, 24, 320); // Full signing area.
    put16(metadata, 26, 160);
    put16(metadata, 28, mode);
    put32(metadata, 30, 16384); // Raw pressure range.
    put16(metadata, 34, 1828);  // Pressure calibration.
    put16(metadata, 36, 4900);
    put16(metadata, 38, 1024);
    put16(metadata, 44, 320);
    put16(metadata, 46, 160);
    put16(metadata, 48, 250);  // Samples per second.
    put16(metadata, 50, mode); // Internal hash identifiers: SHA-1 = 1, SHA-256 = 2.
    put16(metadata, 52, mode);
    put16(metadata, 54, static_cast<unsigned>(documentHash.size()));
    std::copy(documentHash.rbegin(), documentHash.rend(), metadata.begin() + 56);
    return metadata;
}
class SignData
{
    Bytes compressed_, uncompressed_;

  public:
    SignData(const Bytes &wrappedKey, const Bytes &encryptedMetadata, const Bytes &stream)
    {
        if (wrappedKey.size() != 256 || encryptedMetadata.size() != 128 || stream.size() < 20)
            throw std::runtime_error("Invalid SignData components");
        // Until the extended-length format is verified, never truncate a length.
        if (stream.size() > 65535 - 132)
            throw std::runtime_error("SignData stream exceeds the verified 65403-byte format limit");
        for (size_t pos = 20; pos < stream.size();)
        {
            if (stream.size() - pos < 4)
                throw std::runtime_error("Truncated SignData report count");
            auto count = get32(stream, pos);
            if (count < 1 || count > 4)
                throw std::runtime_error("Invalid SignData report count");
            size_t length = 4 + ((count * 12 + 15) / 16) * 16;
            if (length > stream.size() - pos)
                throw std::runtime_error("Truncated SignData report");
            pos += length;
        }
        Bytes payload(4, 0);
        put16(payload, 0, 256);
        put16(payload, 2, 132 + static_cast<unsigned>(stream.size()));
        payload.insert(payload.end(), wrappedKey.begin(), wrappedKey.end());
        payload.resize(264);
        put32(payload, 260, 132);
        payload.insert(payload.end(), encryptedMetadata.begin(), encryptedMetadata.end());
        payload.insert(payload.end(), stream.begin(), stream.end());
        // SDK default normalized geometry for this model, full-screen capture.
        Bytes header(28, 0);
        put32(header, 0, 11);
        put32(header, 4, 1);
        put16(header, 8, 101);
        put16(header, 10, 0x10);
        put16(header, 12, 2242);
        put16(header, 14, 2242);
        put16(header, 16, 8192);
        put16(header, 18, 4096);
        put16(header, 20, 4);
        put16(header, 26, 1023);
        uncompressed_ = header;
        uncompressed_.insert(uncompressed_.end(), payload.begin(), payload.end());
        // qCompress prefixes a four-byte Qt length; the SDK expects only zlib.
        auto compressedPayload = qCompress(reinterpret_cast<const uchar *>(payload.data()), static_cast<qsizetype>(payload.size()), 9);
        if (compressedPayload.size() <= 4)
            throw std::runtime_error("SignData compression failed");
        put16(header, 10, 0x210);
        compressed_ = header;
        compressed_.insert(compressed_.end(), compressedPayload.begin() + 4, compressedPayload.end());
    }
    // SDK-style two-call interface: nullptr queries size; options 0/1 select
    // compressed/uncompressed. A short buffer is untouched and reports size.
    int STRSAGetSignData(unsigned char *output, long *size, int options = 0) const
    {
        if (!size || (options != 0 && options != 1))
            return -1;
        const auto &data = options ? uncompressed_ : compressed_;
        long capacity = *size;
        *size = static_cast<long>(data.size());
        if (!output)
            return 0;
        if (capacity < *size)
            return -2;
        std::copy(data.begin(), data.end(), output);
        return 0;
    }
    Bytes bytes(int options = 0) const
    {
        long size = 0;
        if (STRSAGetSignData(nullptr, &size, options))
            throw std::runtime_error("Invalid SignData options");
        Bytes result(size);
        if (STRSAGetSignData(result.data(), &size, options))
            throw std::runtime_error("SignData export failed");
        return result;
    }
};
} // namespace direct

namespace direct
{
struct PenSample
{
    std::uint32_t time;
    std::uint16_t x, y, z1, z2;
};
struct RenderPoint
{
    double x, y, pressure;
    std::uint32_t time;
};
// SIG100 Lite sensor format 1. Derived from the SDK's raw resistance and
// calibration conversion (3BB91 and 46D8B in the inspected x64 library).
inline std::vector<RenderPoint> calibratedPoints(const std::vector<PenSample> &samples)
{
    std::vector<RenderPoint> result;
    bool afterLift = false;
    std::uint32_t previous = 0;
    for (const auto &p : samples)
    {
        if (p.x > 4096 || p.y > 4096)
            continue;
        double resistance = p.z1 ? static_cast<double>(static_cast<std::int64_t>(p.x) * (int(p.z2) - int(p.z1)) / p.z1) : 0;
        resistance = std::min(resistance, 16383.0);
        double pressure;
        if (resistance < 0)
        {
            pressure = -1;
            afterLift = true;
        }
        else if (afterLift)
        {
            pressure = 0;
            afterLift = false;
        }
        else
        {
            double clamped = std::clamp(resistance, 1828.0, 4900.0);
            pressure = std::clamp((3073.0 - (clamped - 1828.0)) * 1024.0 / 3073.0, 1.0, 1023.0);
        }
        if (static_cast<std::uint32_t>(p.time - previous) > 2)
            pressure = 0;
        previous = p.time;
        result.push_back({double(p.x) * 2, double(p.y), pressure, p.time});
    }
    return result;
}
// The export contract used by the supplied function: PNG, blue, 160 ppi,
// original physical size, crop to ink, transparent, pressure-varying width.
// Qt rasterization/width interpolation is not bit-identical to the SDK renderer.
class SignatureImage
{
    QByteArray cached_;

  public:
    explicit SignatureImage(const std::vector<PenSample> &samples)
    {
        constexpr int ppi = 160;
        auto points = calibratedPoints(samples);
        const double scale = ppi / 2242.0;
        QImage canvas(static_cast<int>(std::ceil(8192 * scale)) + 16, static_cast<int>(std::ceil(4096 * scale)) + 16, QImage::Format_ARGB32_Premultiplied);
        if (canvas.isNull())
            throw std::runtime_error("Cannot allocate signature image");
        canvas.fill(Qt::transparent);
        QPainter painter(&canvas);
        painter.setRenderHint(QPainter::Antialiasing);
        QPointF previous;
        double previousWidth = 0;
        bool down = false;
        for (const auto &p : points)
        {
            QPointF current(8 + p.x * scale, 8 + p.y * scale);
            if (p.pressure <= 0)
            {
                previous = current;
                down = false;
                continue;
            }
            double width = (ppi / 160.0) * (0.6 + 2.4 * std::sqrt(p.pressure / 1023.0));
            painter.setPen(QPen(Qt::blue, down ? (width + previousWidth) / 2 : width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            if (down)
                painter.drawLine(previous, current);
            else
                painter.drawPoint(current);
            previous = current;
            previousWidth = width;
            down = true;
        }
        painter.end();
        int left = canvas.width(), top = canvas.height(), right = -1, bottom = -1;
        for (int y = 0; y < canvas.height(); ++y)
        {
            auto row = reinterpret_cast<const QRgb *>(canvas.constScanLine(y));
            for (int x = 0; x < canvas.width(); ++x)
            {
                if (qAlpha(row[x]))
                {
                    left = std::min(left, x);
                    right = std::max(right, x);
                    top = std::min(top, y);
                    bottom = std::max(bottom, y);
                }
            }
        }
        if (right < left)
            throw std::runtime_error("No drawable pen samples; no signature image");
        auto image = canvas.copy(left, top, right - left + 1, bottom - top + 1);
        image.setDotsPerMeterX(qRound(ppi / 0.0254));
        image.setDotsPerMeterY(qRound(ppi / 0.0254));
        QBuffer buffer(&cached_);
        if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG"))
            throw std::runtime_error("PNG encode failed");
    }
    int STSignatureSaveAsStreamEx(unsigned char *output, long *size, long resolution = 160, long width = 0, long height = 0, int fileType = 1, long penWidth = 0, std::uint32_t color = 0x00ff0000, long options = 0x5000) const
    {
        if (!size)
            return -1;
        // This capture's cached image implements precisely the caller's preset.
        // As in the SDK, the fetch call uses the cached bytes, ignoring settings.
        if (!output && (resolution != 160 || width != 0 || height != 0 || fileType != 1 ||
                        penWidth != 0 || color != 0x00ff0000 || options != 0x5000))
            return -3;
        long capacity = *size;
        *size = static_cast<long>(cached_.size());
        if (!output)
            return 0;
        if (capacity < *size)
            return -2;
        std::copy(cached_.begin(), cached_.end(), output);
        return 0;
    }
    const QByteArray &png() const
    {
        return cached_;
    }
};
} // namespace direct
} // namespace

// Session state. Public calls run on the application thread.
// Pen samples and encrypted stream are written while the reader owns deviceMutex.
const auto vendorId = 0x2133;
hid_device *device = nullptr;
SignotecDriver::TabletType tabletType = SignotecDriver::TabletType::None;
SignotecDriver::HASHALGO s_algorithm;
std::vector<unsigned char> s_digest;

struct PenSample
{
    std::uint32_t counter;
    std::uint16_t x;
    std::uint16_t y;
    std::uint16_t z1;
    std::uint16_t z2;
};

using OutputReport = std::array<unsigned char, 64>;
using InputReport = std::array<unsigned char, 61>;
static std::optional<InputReport> exchangeReport(const OutputReport &request, unsigned char expectedOpcode, int expectedSelector = -1);

static std::vector<PenSample> s_penSamples;
static std::vector<unsigned char> s_encryptedStream;
static EVP_CIPHER_CTX *sampleDecryptor = nullptr;
static std::int64_t s_captureTimestamp = 0;

static std::vector<unsigned char> s_signature;
static bool s_confirmed = false;

static std::vector<unsigned char> s_certificate;

static std::array<unsigned char, 32> s_aesKey{};
static bool s_hasAesKey = false;
static bool s_captureStarted = false;
static long s_openedIndex = -1;
static bool s_imageQueried = false;
static std::unique_ptr<direct::SignData> s_signData;
static std::unique_ptr<direct::SignatureImage> s_image;
static thread_local std::string s_lastError;

static long fail(const char *message)
{
    s_lastError = message;
    return -1;
}

// The hash enum describes the caller's digest; unsupported algorithms return 0.
static size_t digestSize(SignotecDriver::HASHALGO algorithm)
{
    switch (algorithm)
    {
    case SignotecDriver::kSha1:
        return 20;
    case SignotecDriver::kSha256:
        return 32;
    default:
        return 0;
    }
}

static bool hasDocumentHash()
{
    const size_t expectedSize = digestSize(s_algorithm);
    return expectedSize != 0 && s_digest.size() == expectedSize;
}

static void clearExports()
{
    s_signature.clear();
    s_certificate.clear();
    s_confirmed = false;
    s_signData.reset();
    s_image.reset();
    s_imageQueried = false;
}

static void clearAesKey()
{
    OPENSSL_cleanse(s_aesKey.data(), s_aesKey.size());
    s_hasAesKey = false;
}

enum class Setting : std::uint8_t
{
    CaptureMode = 0x20,  // Observed: 1 during setup/stopped, 2 during capture
    CaptureGate = 0x23,  // Observed: 1 before capture, 0 after; exact role uncertain
    SamplingRate = 0x31, // Sending 0xFFFFFFFF returns 250 on our tablet

    RectStartX = 0x32,
    RectStartY = 0x33,
    RectExtentX = 0x34, // Width or ending X: not yet distinguished
    RectExtentY = 0x35  // Height or ending Y: not yet distinguished
};

enum class ExtendedId : std::uint32_t
{
    SigningCertificate = 0x03,
    EncryptedDocumentHash = 0x0F, // Upload encrypted document-hash block
    GenerateSignature = 0x10,     // Observed argument: 1
    WrapBiometricKey = 0x16,      // Observed argument: 0
    ResultBuffer = 0x17,          // Read result of preceding crypto operation
    SampleStreamHash = 0x22,      // Upload hash of captured encrypted stream
    HostRsaExponent = 0x27,       // Set temporary host RSA exponent
    HostRsaModulus = 0x28,        // Upload temporary host RSA modulus
    CryptoOperation = 0x29,       // Observed arguments: 5 and 6
    ResetSession = 0x80
};

enum class CryptoOperation : std::uint32_t
{
    ApplyDocumentHash = 5,
    PrepareSessionKey = 6
};

// Transport helpers: byte order, chunked objects, and incoming pen reports.

std::uint32_t read32(const unsigned char *p) // 4 bytes to 32uint
{
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) | (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}

std::uint16_t read16(const unsigned char *p) // 2 bytes to 16uint
{
    return std::uint16_t(p[0]) | (std::uint16_t(p[1]) << 8);
}

// Upload/download an extended object in chunks.
// Buffers here contain object data, not complete HID reports.
static void write32(unsigned char *destination, std::uint32_t value)
{
    for (unsigned i = 0; i < 4; ++i)
        destination[i] = static_cast<unsigned char>(value >> (8 * i));
}

static bool writeObject(ExtendedId id, const std::vector<unsigned char> &data)
{
    if (!device || data.empty())
        return false;

    if (data.size() > std::numeric_limits<std::uint32_t>::max())
        return false;

    std::size_t offset = 0;

    while (offset < data.size())
    {
        const std::size_t chunkSize =
            (std::min)(std::size_t{48}, data.size() - offset);

        OutputReport request{};

        request[0] = 0x00;
        request[1] = 0xA8;

        write32(request.data() + 2, static_cast<std::uint32_t>(id));
        write32(request.data() + 6, static_cast<std::uint32_t>(offset));
        write32(request.data() + 10, static_cast<std::uint32_t>(chunkSize));

        std::copy_n(data.data() + offset, chunkSize, request.data() + 14);

        if (!exchangeReport(request, 0x90))
            return false;

        offset += chunkSize;
    }

    return true;
}

static std::optional<std::vector<unsigned char>> readObject(ExtendedId id, std::size_t size)
{
    if (!device || size == 0)
        return std::nullopt;

    if (size > std::numeric_limits<std::uint32_t>::max())
        return std::nullopt;

    const auto objectId = static_cast<std::uint32_t>(id);

    std::vector<unsigned char> data;
    data.reserve(size);

    std::size_t offset = 0;

    while (offset < size)
    {
        const std::size_t chunkSize =
            (std::min)(std::size_t{48}, size - offset);

        OutputReport request{};

        request[0] = 0x00;
        request[1] = 0xA8;

        write32(request.data() + 2, objectId);
        write32(request.data() + 6, static_cast<std::uint32_t>(offset));
        write32(request.data() + 10, static_cast<std::uint32_t>(chunkSize));

        const auto response = exchangeReport(request, 0x98);

        if (!response)
            return std::nullopt;

        // Verify that this response belongs to the requested chunk.
        if (read32(response->data() + 1) != objectId ||
            read32(response->data() + 5) != offset ||
            read32(response->data() + 9) != chunkSize)
        {
            return std::nullopt;
        }

        data.insert(data.end(), response->begin() + 13, response->begin() + 13 + chunkSize);

        offset += chunkSize;
    }

    return data;
}

static std::optional<std::vector<unsigned char>> decryptSamples(const unsigned char *ciphertext, std::size_t size)
{
    if (!sampleDecryptor || !ciphertext)
        return std::nullopt;

    // Pen reports contain 16, 32, or 48 encrypted bytes.
    if (size != 16 && size != 32 && size != 48)
        return std::nullopt;

    std::vector<unsigned char> plaintext(size + 16);
    int written = 0;

    const int result = EVP_DecryptUpdate(sampleDecryptor, plaintext.data(), &written, ciphertext, static_cast<int>(size));

    if (result != 1 || written != static_cast<int>(size))
        return std::nullopt;

    plaintext.resize(written);
    return plaintext;
}

static bool processPenReport(const InputReport &report)
{
    if (report[0] < 0x41 || report[0] > 0x44)
        return false;

    unsigned sampleCount = report[0] & 0x0F;

    // Each sample is 12 bytes; encrypted data is aligned to 16 bytes.
    const unsigned encryptedSize = ((sampleCount * 12 + 15) / 16) * 16;

    // Stay within the supported SignData format's size limit.
    if (s_encryptedStream.size() + 4 + encryptedSize > 65403)
        return false;

    const unsigned char *ciphertext = report.data() + 4;

    auto plaintext = decryptSamples(ciphertext, encryptedSize);

    if (!plaintext)
        return false;

    // Save the sample count as a four-byte little-endian integer.
    s_encryptedStream.push_back(static_cast<unsigned char>(sampleCount));
    s_encryptedStream.push_back(0);
    s_encryptedStream.push_back(0);
    s_encryptedStream.push_back(0);

    // Preserve the original ciphertext for signing/export.
    s_encryptedStream.insert(s_encryptedStream.end(), ciphertext, ciphertext + encryptedSize);

    // Decode each sample from the decrypted copy.
    for (unsigned i = 0; i < sampleCount; ++i)
    {
        const unsigned char *sample = plaintext->data() + i * 12;

        s_penSamples.push_back({.counter = read32(sample),
                                .x = read16(sample + 4),
                                .y = read16(sample + 6),
                                .z1 = read16(sample + 8),
                                .z2 = read16(sample + 10)});
    }

    return true;
}

// Receiver lifetime: Start resumes it, Stop pauses it, Close joins it.
static std::thread receiverThread;
static std::mutex deviceMutex;

static std::atomic<bool> receiving{false};
static std::atomic<bool> receiverShutdown{false};
static std::atomic<bool> receiveFailed{false};

// Pause without destroying the thread: Windows cancels pending HID reads
// when their issuing thread exits. Joining is deferred until device close.
static void stopReceiving()
{
    receiving = false;
    std::lock_guard<std::mutex> lock(deviceMutex);
}

static void joinReceiver()
{
    receiving = false;
    receiverShutdown = true;
    if (receiverThread.joinable())
        receiverThread.join();
}

static bool startReceiving()
{
    if (!device || receiving)
        return false;
    receiveFailed = false;
    receiverShutdown = false;
    receiving = true;
    if (receiverThread.joinable())
        return true;
    try
    {
        receiverThread = std::thread([] {
            while (!receiverShutdown)
            {
                try
                {
                    if (receiving)
                    {
                        std::lock_guard<std::mutex> lock(deviceMutex);
                        if (receiving)
                        {
                            std::array<unsigned char, 62> buffer{};
                            int count = hid_read_timeout(device, buffer.data(), buffer.size(), 50);
                            if (count < 0)
                                throw std::runtime_error("HID read failed");
                            if (count > 0)
                            {
                                if (count != 61)
                                    throw std::runtime_error("Unexpected HID report size");
                                InputReport report{};
                                std::copy_n(buffer.begin(), report.size(), report.begin());
                                if (report[0] >= 0x41 && report[0] <= 0x44)
                                {
                                    if (!processPenReport(report))
                                        throw std::runtime_error("Invalid encrypted pen report");
                                }
                                else if (report[0] != 0x40 && report[0] != 0x50)
                                {
                                    throw std::runtime_error("Unexpected response outside command exchange");
                                }
                            }
                        }
                    }
                }
                catch (const std::exception &error)
                {
                    qWarning() << "[Signotec capture]" << error.what();
                    receiveFailed = true;
                    receiving = false;
                }
                catch (...)
                {
                    receiveFailed = true;
                    receiving = false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    catch (...)
    {
        receiving = false;
        return false;
    }
    return true;
}

static bool resetSigningSession()
{
    // Pause the reader before sending reset commands or changing state.
    stopReceiving();

    if (!device)
        return false;

    // Stop capture: 83 -> 93.
    OutputReport stopRequest{};
    stopRequest[1] = 0x83;

    const bool stopped =
        exchangeReport(stopRequest, 0x93).has_value();

    // Reset the volatile signing session: A8 / object 0x80 -> 90.
    OutputReport resetRequest{};
    resetRequest[1] = 0xA8;
    resetRequest[2] = 0x80;

    // Attempt reset even if the stop acknowledgement failed.
    const bool reset =
        exchangeReport(resetRequest, 0x90).has_value();

    return stopped && reset;
}

static std::optional<InputReport> exchangeReport(const OutputReport &request, unsigned char expectedOpcode, int expectedSelector)
{
    std::lock_guard<std::mutex> lock(deviceMutex);

    if (!device)
        return std::nullopt;

    if (hid_write(device, request.data(), request.size()) != 64)
        return std::nullopt;

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);

    while (std::chrono::steady_clock::now() < deadline)
    {
        std::array<unsigned char, 62> buffer{};

        const int count = hid_read_timeout(device, buffer.data(), buffer.size(), 50);

        if (count == 0)
            continue;

        if (count < 0 || count != 61)
            return std::nullopt;

        InputReport response{};

        std::copy_n(buffer.begin(), response.size(), response.begin());

        const auto opcode = response[0];

        // Return the reply we were waiting for.
        if (opcode == expectedOpcode && (expectedSelector < 0 || response[1] == expectedSelector))
            return response;

        switch (opcode)
        {
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
            if (!processPenReport(response))
                return std::nullopt;
            break;

        case 0x40:
        case 0x50:
            // Periodic status report; keep waiting.
            break;

        default:
            return std::nullopt;
        }
    }

    return std::nullopt;
}

// Send an A8 command with a four-byte ID and four-byte argument.
static bool extendedCommand(ExtendedId id, std::uint32_t value = 0)
{
    OutputReport request{};

    request[0] = 0x00; // HID report-ID placeholder
    request[1] = 0xA8; // Extended command

    write32(request.data() + 2, static_cast<std::uint32_t>(id));
    write32(request.data() + 6, value);

    return exchangeReport(request, 0x90).has_value();
}

static std::optional<uint32_t> writeSetting(Setting selector, std::uint32_t value)
{
    auto selectorByte = static_cast<unsigned char>(selector);

    OutputReport request{};

    request[0] = 0x00; // HID report-ID placeholder
    request[1] = 0x10; // Write-setting command
    request[2] = selectorByte;
    request[3] = 4; // Four-byte value

    for (unsigned i = 0; i < 4; ++i)
    {
        request[4 + i] = static_cast<unsigned char>(value >> (8 * i));
    }

    auto response = exchangeReport(request, 0x20, selectorByte);

    if (!response)
        return std::nullopt;

    // Check that the returned value occupies four bytes.
    if ((*response)[2] != 4)
        return std::nullopt;

    return read32(response->data() + 3);
}

static bool setSetting(Setting selector, std::uint32_t value)
{
    const auto result = writeSetting(selector, value);
    return result.has_value() && *result == value;
}

static bool loadCertificate();
static bool verifySignature(const Bytes &streamHash);

// Public API: open, configure, capture, then export.

long SignotecDriver::STDeviceOpen(long index, bool erase)
{
    s_lastError.clear();
    try
    {
        if (device || index < 0)
            return fail("Already open or invalid device index");
        if (hid_init() != 0)
            return fail("HIDAPI initialization failed");
        auto *list = hid_enumerate(vendorId, 0x0001);
        struct Guard
        {
            hid_device_info *list;
            ~Guard()
            {
                hid_free_enumeration(list);
            }
        } guard{list};
        long current = 0;
        for (auto *entry = list; entry; entry = entry->next)
        {
            if (!entry->path || entry->usage_page != 0xffff || entry->usage != 0xff)
                continue;
            if (current++ != index)
                continue;
            device = hid_open_path(entry->path);
            break;
        }
        if (!device)
            return fail("Cannot open the selected Sigma interface");
        s_openedIndex = index;
        tabletType = Sigma;
        receiveFailed = false;
        s_captureStarted = false;
        clearExports();
        s_digest.clear();
        OutputReport query{};
        query[2] = 0x1f;
        auto info = exchangeReport(query, 0x20, 0x1f);
        const bool supported = info && read32(info->data() + 3) == 1 // Model.
                               && read32(info->data() + 19) == 320   // Logical width.
                               && read32(info->data() + 23) == 160   // Logical height.
                               && read32(info->data() + 31) == 4096  // Sensor X range.
                               && read32(info->data() + 35) == 4096  // Sensor Y range.
                               && read32(info->data() + 39) == 16384 // Pressure range.
                               && read32(info->data() + 43) == 1     // Sensor format.
                               && read32(info->data() + 51) == 92780 // Physical dimensions reported by this model.
                               && read32(info->data() + 55) == 46390;
        if (!supported)
        {
            hid_close(device);
            device = nullptr;
            s_openedIndex = -1;
            tabletType = None;
            return fail("Unsupported tablet geometry or failed information query");
        }
        if (!resetSigningSession() || !setSetting(Setting::CaptureMode, 1) || (erase && STErase() < 0))
        {
            STDeviceClose(index);
            return fail("Tablet initialization failed");
        }
        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STErase()
{
    s_lastError.clear();
    try
    {
        if (!device || s_captureStarted || s_confirmed)
            return fail("Erase requires an open, stopped session");

        OutputReport request{};

        request[0] = 0x00; // HID report-ID placeholder
        request[1] = 0x86; // Clear command
        request[2] = 0xFF;
        request[3] = 0xFF;

        auto response = exchangeReport(request, 0x90);

        return response.has_value() ? 0 : fail("STErase: missing acknowledgement");
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STSensorSetSignRect(long x, long y, long width, long height)
{
    s_lastError.clear();
    try
    {
        if (!device || s_captureStarted || s_confirmed || x || y || width || height)
            return fail("Only full-area rectangle before capture is supported");

        return setSetting(Setting::RectStartX, 0) &&
                       setSetting(Setting::RectStartY, 0) &&
                       setSetting(Setting::RectExtentX, 320) &&
                       setSetting(Setting::RectExtentY, 160)
                   ? 0
                   : fail("STSensorSetSignRect: command failed");
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STRSASetHash(const unsigned char *hash, HASHALGO algorithm, long options)
{
    s_lastError.clear();
    try
    {
        if (!device || !hash || options != 0 || (algorithm != kSha1 && algorithm != kSha256))
            return fail("Expected an open device, raw SHA-1 or SHA-256 digest and options 0");
        if (s_captureStarted || s_confirmed)
            return fail("Set the document hash before capture");
        s_digest.assign(hash, hash + digestSize(algorithm));
        s_algorithm = algorithm;
        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STSignatureStart()
{
    s_lastError.clear();
    try
    {
        if (!device)
            return fail("STSignatureStart failed: invalid state, arguments or device response");

        if (s_captureStarted || s_confirmed || !hasDocumentHash())
            return fail("Start requires a fresh SHA-1 or SHA-256 capture session");
        clearExports();

        if (!resetSigningSession())
            return fail("STSignatureStart failed: invalid state, arguments or device response");

        EVP_CIPHER_CTX_free(sampleDecryptor);
        sampleDecryptor = nullptr;
        clearAesKey();

        s_penSamples.clear();
        s_encryptedStream.clear();

        EVP_PKEY_CTX *keyGenerator = nullptr;
        EVP_PKEY *hostKey = nullptr;
        EVP_PKEY_CTX *keyDecryptor = nullptr;
        EVP_CIPHER_CTX *hashEncryptor = nullptr;

        BIGNUM *modulus = nullptr;
        BIGNUM *exponent = nullptr;

        unsigned char unwrappedKey[128] = {};
        unsigned char aesKey[32] = {};

        auto cleanup = [&] {
            EVP_CIPHER_CTX_free(hashEncryptor);
            EVP_PKEY_CTX_free(keyDecryptor);
            EVP_PKEY_free(hostKey);
            EVP_PKEY_CTX_free(keyGenerator);

            BN_free(modulus);
            BN_free(exponent);

            OPENSSL_cleanse(unwrappedKey, sizeof(unwrappedKey));
            OPENSSL_cleanse(aesKey, sizeof(aesKey));
        };

        const auto require = [](bool success) {if (!success)throw std::runtime_error("Signature startup failed"); };

        try
        {
            // 1. Generate the temporary host RSA key pair.
            keyGenerator = EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr);

            require(keyGenerator != nullptr);
            require(EVP_PKEY_keygen_init(keyGenerator) > 0);

            // This size is required by the tested tablet protocol.
            require(EVP_PKEY_CTX_set_rsa_keygen_bits(keyGenerator, 1024) > 0);
            require(EVP_PKEY_generate(keyGenerator, &hostKey) > 0);
            // Extract the public exponent and modulus.
            require(EVP_PKEY_get_bn_param(hostKey, OSSL_PKEY_PARAM_RSA_E, &exponent) == 1);
            require(EVP_PKEY_get_bn_param(hostKey, OSSL_PKEY_PARAM_RSA_N, &modulus) == 1);
            require(BN_num_bits(exponent) <= 32);

            const auto exponentValue =
                static_cast<std::uint32_t>(BN_get_word(exponent));

            // The tablet expects a 256-byte, little-endian modulus field.
            // Our 1024-bit modulus occupies its first 128 bytes.
            std::vector<unsigned char> modulusBytes(256, 0);

            require(BN_bn2lebinpad(modulus, modulusBytes.data(), static_cast<int>(modulusBytes.size())) == 256);
            require(extendedCommand(ExtendedId::HostRsaExponent, exponentValue));
            require(writeObject(ExtendedId::HostRsaModulus, modulusBytes));
            // 2. Receive and decrypt the tablet's AES session key.

            require(extendedCommand(ExtendedId::CryptoOperation, static_cast<std::uint32_t>(CryptoOperation::PrepareSessionKey)));

            const auto wrappedKey =
                readObject(ExtendedId::ResultBuffer, 256);

            require(wrappedKey.has_value());
            require(wrappedKey->size() == 256);

            // Only the first 128 bytes contain the RSA ciphertext.
            // Convert the tablet's byte order to OpenSSL's byte order.
            std::vector<unsigned char> rsaCiphertext(wrappedKey->begin(), wrappedKey->begin() + 128);

            std::reverse(rsaCiphertext.begin(), rsaCiphertext.end());

            keyDecryptor = EVP_PKEY_CTX_new(hostKey, nullptr);

            require(keyDecryptor != nullptr);
            require(EVP_PKEY_decrypt_init(keyDecryptor) > 0);
            require(EVP_PKEY_CTX_set_rsa_padding(keyDecryptor, RSA_PKCS1_PADDING) > 0);

            std::size_t keySize = sizeof(unwrappedKey);

            require(EVP_PKEY_decrypt(keyDecryptor, unwrappedKey, &keySize, rsaCiphertext.data(), rsaCiphertext.size()) > 0);
            require(keySize == 32);

            // The recovered key also uses reversed byte order.
            std::reverse_copy(unwrappedKey, unwrappedKey + 32, aesKey);
            std::copy_n(aesKey, s_aesKey.size(), s_aesKey.begin());
            s_hasAesKey = true;
            // 3. Initialize continuous pen-sample decryption.

            unsigned char iv[16] = {};

            sampleDecryptor = EVP_CIPHER_CTX_new();

            require(sampleDecryptor != nullptr);

            require(EVP_DecryptInit_ex(sampleDecryptor, EVP_aes_256_cbc(), nullptr, aesKey, iv) == 1);

            require(EVP_CIPHER_CTX_set_padding(sampleDecryptor, 0) == 1);

            // Keep sampleDecryptor alive after this function returns.
            // It preserves CBC chaining between incoming reports.
            // 4. Initialize the encrypted stream's identity header.

            OutputReport infoRequest{};
            infoRequest[1] = 0x00;
            infoRequest[2] = 0x1F;

            const auto info = exchangeReport(infoRequest, 0x20, 0x1F);
            require(info.has_value());

            OutputReport capabilitiesRequest{};
            capabilitiesRequest[1] = 0x00;
            capabilitiesRequest[2] = 0x03;

            const auto capabilities =
                exchangeReport(capabilitiesRequest, 0x20, 0x03);

            require(capabilities.has_value());

            s_encryptedStream.resize(20, 0);

            const std::uint32_t header[] = {
                read32(info->data() + 15),                                // Numeric serial
                2,                                                        // Stream format
                read32(capabilities->data() + 39) & ~std::uint32_t(1024), // Capability flags
                (*info)[12],                                              // Firmware fields
                (*info)[11]};

            for (unsigned field = 0; field < 5; ++field)
            {
                for (unsigned byte = 0; byte < 4; ++byte)
                {
                    s_encryptedStream[field * 4 + byte] =
                        static_cast<unsigned char>(header[field] >> (byte * 8));
                }
            }
            // 5. Encrypt and upload the document hash.

            unsigned char hashBlock[64] = {};

            std::reverse_copy(s_digest.begin(), s_digest.end(), hashBlock);

            // Use a separate context: encrypting the hash must not
            // change the pen-report decryption state.
            hashEncryptor = EVP_CIPHER_CTX_new();

            require(hashEncryptor != nullptr);

            require(EVP_EncryptInit_ex(hashEncryptor, EVP_aes_256_cbc(), nullptr, aesKey, iv) == 1);

            require(EVP_CIPHER_CTX_set_padding(hashEncryptor, 0) == 1);

            std::vector<unsigned char> encryptedHash(80);
            int written = 0;
            int finalBytes = 0;

            require(EVP_EncryptUpdate(hashEncryptor, encryptedHash.data(), &written, hashBlock, sizeof(hashBlock)) == 1);

            require(EVP_EncryptFinal_ex(hashEncryptor, encryptedHash.data() + written, &finalBytes) == 1);

            require(written + finalBytes == 64);
            encryptedHash.resize(64);

            require(writeObject(ExtendedId::EncryptedDocumentHash, encryptedHash));

            require(extendedCommand(ExtendedId::CryptoOperation, static_cast<std::uint32_t>(CryptoOperation::ApplyDocumentHash)));
            // 6. Configure and start capture.

            require(setSetting(Setting::CaptureMode, 1));

            const auto samplingRate =
                writeSetting(Setting::SamplingRate, 0xFFFFFFFFu);

            require(samplingRate.has_value() && *samplingRate == 250);

            require(STSensorSetSignRect(0, 0, 0, 0) == 0);
            require(setSetting(Setting::CaptureGate, 1));
            require(STErase() == 0);

            OutputReport startRequest{};
            startRequest[1] = 0x82;
            startRequest[2] = s_algorithm == kSha1 ? 0x01 : 0x02;

            s_captureTimestamp =
                static_cast<std::int64_t>(std::time(nullptr));

            require(exchangeReport(startRequest, 0x92).has_value());

            // exchange() can already process pen reports here:
            // the decryptor and stream header are both initialized.
            require(setSetting(Setting::CaptureMode, 2));

            cleanup();
        }
        catch (...)
        {
            // Reset while the decryptor still exists, in case exchange()
            // receives final pen reports before the acknowledgement.
            try
            {
                resetSigningSession();
            }
            catch (...)
            {
            }

            cleanup();

            EVP_CIPHER_CTX_free(sampleDecryptor);
            sampleDecryptor = nullptr;
            clearAesKey();

            s_penSamples.clear();
            s_encryptedStream.clear();

            return fail("STSignatureStart failed: invalid state, arguments or device response");
        }

        // No other thread should be reading the handle before this point.
        s_captureStarted = true;
        if (!startReceiving())
        {
            s_captureStarted = false;
            try
            {
                resetSigningSession();
            }
            catch (...)
            {
            }

            EVP_CIPHER_CTX_free(sampleDecryptor);
            sampleDecryptor = nullptr;
            clearAesKey();

            s_penSamples.clear();
            s_encryptedStream.clear();

            return fail("STSignatureStart failed: invalid state, arguments or device response");
        }

        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STSignatureStop()
{
    s_lastError.clear();
    try
    {
        stopReceiving();

        if (!device)
            return fail("STSignatureStop failed: invalid state, arguments or device response");

        // Tell the tablet to stop capture.
        OutputReport request{};
        request[1] = 0x83;

        // exchangeReport() processes any final pen reports that arrive
        // before the tablet's stop acknowledgement.
        const bool stopped = exchangeReport(request, 0x93).has_value();

        // Attempt both settings even if an earlier operation failed.
        const bool modeSet = setSetting(Setting::CaptureMode, 1);
        const bool gateDisabled = setSetting(Setting::CaptureGate, 0);

        s_captureStarted = false;
        if (!stopped || !modeSet || !gateDisabled || receiveFailed)
        {
            receiveFailed = true;
            return fail("Capture stopped with an input or protocol error");
        }

        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STDeviceClose(long index)
{
    s_lastError.clear();
    try
    {
        // Close only the device selected by STDeviceOpen.
        if (index < 0 || (device && index != s_openedIndex))
            return fail("Wrong device index");

        // Pause first; after tablet reset, join before closing or clearing anything.
        stopReceiving();

        bool success = true;

        if (device)
        {
            // Attempt to stop/reset the tablet, but close the handle
            // even if it is disconnected or does not acknowledge.
            try
            {
                success = resetSigningSession();
            }
            catch (...)
            {
                success = false;
            }

            joinReceiver();
            hid_close(device);
            device = nullptr;
        }

        joinReceiver();
        EVP_CIPHER_CTX_free(sampleDecryptor);
        sampleDecryptor = nullptr;
        clearAesKey();

        s_penSamples.clear();
        s_encryptedStream.clear();
        s_digest.clear();

        s_captureTimestamp = 0;
        tabletType = TabletType::None;
        receiveFailed = false;
        clearExports();
        s_captureStarted = false;
        s_openedIndex = -1;

        return success ? 0 : fail("Device closed, but stop/reset failed");
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STSignatureRetry()
{
    s_lastError.clear();
    try
    {
        if (!device)
            return fail("STSignatureRetry failed: invalid state, arguments or device response");

        if (!receiving || receiveFailed)
            return fail("STSignatureRetry failed: invalid state, arguments or device response");

        if (STSignatureStop() < 0)
            return fail("STSignatureRetry failed: invalid state, arguments or device response");

        return STSignatureStart();
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STSignatureConfirm()
{
    s_lastError.clear();
    try
    {
        if (s_confirmed)
            return fail("Signature is already confirmed");
        clearExports();

        if (!device)
            return fail("STSignatureConfirm failed: invalid state, arguments or device response");

        if (!hasDocumentHash())
            return fail("STSignatureConfirm failed: invalid state, arguments or device response");

        // Pauses the receiver and collects final reports while waiting
        // for the tablet's stop acknowledgement.
        if (STSignatureStop() < 0)
            return fail("STSignatureConfirm failed: invalid state, arguments or device response");

        // The stream must contain more than its 20-byte header.
        if (s_penSamples.empty() || s_encryptedStream.size() <= 20)
            return fail("STSignatureConfirm failed: invalid state, arguments or device response");

        try
        {
            std::vector<direct::PenSample> renderSamples;
            renderSamples.reserve(s_penSamples.size());
            for (const auto &sample : s_penSamples)
                renderSamples.push_back({sample.counter, sample.x, sample.y, sample.z1, sample.z2});
            s_image = std::make_unique<direct::SignatureImage>(renderSamples);
            // 1. Calculate the selected hash over the complete encrypted stream,
            // including its header and sample-count fields.
            Bytes streamHash(s_digest.size());
            unsigned int hashSize = 0;

            const int result = EVP_Digest(s_encryptedStream.data(), s_encryptedStream.size(), streamHash.data(), &hashSize, s_algorithm == kSha1 ? EVP_sha1() : EVP_sha256(), nullptr);

            if (result != 1 || hashSize != streamHash.size())
                return fail("STSignatureConfirm failed: invalid state, arguments or device response");

            // 2. The tablet expects the digest bytes in reverse order.
            std::vector<unsigned char> tabletHash(streamHash.rbegin(), streamHash.rend());

            if (!writeObject(ExtendedId::SampleStreamHash, tabletHash))
                return fail("STSignatureConfirm failed: invalid state, arguments or device response");

            // 3. Generate the signature.
            // Argument 1 reproduces the tested PSS/combination flow.
            if (!extendedCommand(ExtendedId::GenerateSignature, 1))
                return fail("STSignatureConfirm failed: invalid state, arguments or device response");

            // 4. Retrieve the 256-byte RSA signature.
            auto signature = readObject(ExtendedId::ResultBuffer, 256);

            if (!signature || signature->size() != 256)
                return fail("STSignatureConfirm failed: invalid state, arguments or device response");

            // Convert from the tablet's byte order to the usual
            // big-endian RSA signature representation.
            std::reverse(signature->begin(), signature->end());

            s_signature = std::move(*signature);
            if (!loadCertificate() || !verifySignature(streamHash))
            {
                clearExports();
                return fail("Tablet signature verification failed");
            }
            s_confirmed = true;

            return 0;
        }
        catch (...)
        {
            s_signature.clear();
            s_confirmed = false;
            return fail("STSignatureConfirm failed: invalid state, arguments or device response");
        }
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STRSASign(unsigned char *buffer, long *size, RSASCHEME scheme, HASHVALUE value, long options)
{
    s_lastError.clear();
    try
    {
        if (!size)
            return fail("STRSASign failed: invalid state, arguments or device response");

        // Only this signing combination has been implemented.
        if (scheme != RSASCHEME::kPSS || value != HASHVALUE::kCombination || options != 0)
            return fail("STRSASign failed: invalid state, arguments or device response");

        if (!s_confirmed || s_signature.empty())
            return fail("STRSASign failed: invalid state, arguments or device response");

        long requiredSize = static_cast<long>(s_signature.size());

        if (!buffer)
        {
            *size = requiredSize;
            return 0;
        }

        long capacity = *size;

        *size = requiredSize;

        if (capacity < requiredSize)
            return fail("STRSASign failed: invalid state, arguments or device response");

        std::copy(s_signature.begin(), s_signature.end(), buffer);

        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STRSASaveSigningCertAsStream(unsigned char *buffer, long *size, unsigned type)
{
    s_lastError.clear();
    try
    {
        if (!device || !size || !s_confirmed)
            return fail("STRSASaveSigningCertAsStream failed: invalid state, arguments or device response");

        if (type != CERTTYPE::kCert_DER)
            return fail("STRSASaveSigningCertAsStream failed: invalid state, arguments or device response");

        if (!loadCertificate())
            return fail("Cannot read the tablet signing certificate");

        long requiredSize = static_cast<long>(s_certificate.size());

        if (!buffer)
        {
            *size = requiredSize;
            return 0;
        }

        long capacity = *size;
        *size = requiredSize;

        if (capacity < requiredSize)
            return fail("STRSASaveSigningCertAsStream failed: invalid state, arguments or device response");

        std::copy(s_certificate.begin(), s_certificate.end(), buffer);

        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}
// Retrieve the DER certificate without requiring a completed confirmation yet.
static bool loadCertificate()
{
    if (!s_certificate.empty())
        return true;
    auto header = readObject(ExtendedId::SigningCertificate, 4);
    if (!header || header->size() != 4)
        return false;
    auto size = read32(header->data());
    if (!size || size > 1024 * 1024)
        return false;
    auto object = readObject(ExtendedId::SigningCertificate, size + 4);
    if (!object || object->size() != size + 4 || read32(object->data()) != size)
        return false;
    s_certificate.assign(object->begin() + 4, object->end());
    return true;
}

// The pad signs streamHash || documentHash directly as PSS mHash.
// SHA-1 uses a 20-byte hash/salt; SHA-256 uses 32 bytes.
static bool verifySignature(const Bytes &streamHash)
{
    const size_t hashSize = s_algorithm == SignotecDriver::kSha1 ? 20 : 32;
    if ((s_algorithm != SignotecDriver::kSha1 && s_algorithm != SignotecDriver::kSha256) || streamHash.size() != hashSize || s_digest.size() != hashSize)
        return false;
    const unsigned char *cursor = s_certificate.data();
    std::unique_ptr<X509, decltype(&X509_free)> certificate(d2i_X509(nullptr, &cursor, static_cast<long>(s_certificate.size())), X509_free);
    if (!certificate || cursor != s_certificate.data() + s_certificate.size())
        return false;
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> key(X509_get_pubkey(certificate.get()), EVP_PKEY_free);
    if (!key || !EVP_PKEY_is_a(key.get(), "RSA") || EVP_PKEY_get_bits(key.get()) != 2048 || s_signature.size() != 256)
        return false;
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> context(EVP_PKEY_CTX_new(key.get(), nullptr), EVP_PKEY_CTX_free);
    if (!context || EVP_PKEY_verify_recover_init(context.get()) <= 0 || EVP_PKEY_CTX_set_rsa_padding(context.get(), RSA_NO_PADDING) <= 0)
        return false;
    std::array<unsigned char, 256> encoded{};
    size_t size = encoded.size();
    if (EVP_PKEY_verify_recover(context.get(), encoded.data(), &size, s_signature.data(), s_signature.size()) <= 0 || size != 256)
        return false;
    if (encoded[255] != 0xbc || (encoded[0] & 0x80))
        return false;
    // Recover the PSS data block using MGF1, then check its padding and salt.
    const auto algorithm = hashSize == 20 ? QCryptographicHash::Sha1 : QCryptographicHash::Sha256;
    const size_t dataBlockSize = encoded.size() - hashSize - 1;
    QByteArray encodedHash(reinterpret_cast<const char *>(encoded.data() + dataBlockSize), hashSize);
    QByteArray dataBlockMask;
    for (unsigned counter = 0; dataBlockMask.size() < static_cast<qsizetype>(dataBlockSize); ++counter)
    {
        QByteArray input = encodedHash;
        for (int shift = 24; shift >= 0; shift -= 8)
            input.append(static_cast<char>(counter >> shift));
        dataBlockMask.append(QCryptographicHash::hash(input, algorithm));
    }
    Bytes dataBlock(dataBlockSize);
    for (size_t i = 0; i < dataBlock.size(); ++i)
        dataBlock[i] = encoded[i] ^ static_cast<unsigned char>(dataBlockMask[static_cast<qsizetype>(i)]);
    dataBlock[0] &= 0x7f;
    const size_t delimiter = dataBlockSize - hashSize - 1;
    for (size_t i = 0; i < delimiter; ++i)
        if (dataBlock[i])
            return false;
    if (dataBlock[delimiter] != 1)
        return false;
    // PSS hashes eight zero bytes, both captured hashes, and the recovered salt.
    QByteArray input(8, '\0');
    input.append(reinterpret_cast<const char *>(streamHash.data()), hashSize);
    input.append(reinterpret_cast<const char *>(s_digest.data()), hashSize);
    input.append(reinterpret_cast<const char *>(dataBlock.data() + delimiter + 1), hashSize);
    return QCryptographicHash::hash(input, algorithm) == encodedHash;
}

static std::optional<std::vector<unsigned char>> encryptMetadata(const std::vector<unsigned char> &metadata)
{
    if (!s_hasAesKey || metadata.size() != 128)
        return std::nullopt;
    std::vector<unsigned char> encrypted(144);
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> context(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    unsigned char iv[16] = {};
    int written = 0, finalBytes = 0;
    if (!context || EVP_EncryptInit_ex(context.get(), EVP_aes_256_cbc(), nullptr, s_aesKey.data(), iv) != 1 || EVP_CIPHER_CTX_set_padding(context.get(), 0) != 1)
        return std::nullopt;
    if (EVP_EncryptUpdate(context.get(), encrypted.data(), &written, metadata.data(), static_cast<int>(metadata.size())) != 1)
        return std::nullopt;
    if (EVP_EncryptFinal_ex(context.get(), encrypted.data() + written, &finalBytes) != 1 || written + finalBytes != 128)
        return std::nullopt;
    encrypted.resize(128);
    return encrypted;
}

long SignotecDriver::STRSAGetSignData(unsigned char *buffer, long *size, long options)
{
    s_lastError.clear();
    try
    {
        if (!device || !size || !s_confirmed || receiveFailed || !s_hasAesKey || (s_digest.size() != 20 && s_digest.size() != 32) || (options != 0 && options != 1))
            return fail("No confirmed SignData session or unsupported options");
        if (!s_signData)
        {
            auto encryptedMetadata = encryptMetadata(direct::signMetadata(s_digest, s_captureTimestamp));
            if (!encryptedMetadata)
                return fail("Metadata encryption failed");
            if (!extendedCommand(ExtendedId::WrapBiometricKey, 0))
                return fail("Biometric key wrapping failed");
            auto wrappedKey = readObject(ExtendedId::ResultBuffer, 256);
            if (!wrappedKey)
                return fail("Cannot read wrapped biometric key");
            // SignData uses qCompress(payload, 9), dropping Qt's four-byte size prefix.
            s_signData = std::make_unique<direct::SignData>(*wrappedKey, *encryptedMetadata, s_encryptedStream);
        }
        auto result = s_signData->STRSAGetSignData(buffer, size, static_cast<int>(options));
        return result < 0 ? fail("SignData output buffer too small or invalid") : 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

long SignotecDriver::STSignatureSaveAsStreamEx(unsigned char *buffer, long *size, long resolution, long width, long height, unsigned type, long penWidth, unsigned color, long options)
{
    s_lastError.clear();
    try
    {
        if (!s_confirmed || !s_image || !size)
            return fail("No confirmed signature image");
        if (buffer && !s_imageQueried)
            return fail("Query the PNG size before fetching");
        auto result = s_image->STSignatureSaveAsStreamEx(buffer, size, resolution, width, height, static_cast<int>(type), penWidth, color, options);
        if (result < 0)
            return fail("Unsupported PNG settings or output buffer too small");
        if (!buffer)
            s_imageQueried = true;
        return 0;
    }
    catch (const std::exception &error)
    {
        return fail(error.what());
    }
    catch (...)
    {
        return fail("Unexpected driver error");
    }
}

// Display stubs: the tested Sigma Lite has no LCD.
long SignotecDriver::STDisplaySetFont(const wchar_t *, long, long)
{
    s_lastError.clear();
    return 0;
}

long SignotecDriver::STDisplayGetHeight()
{
    s_lastError.clear();
    return 160;
}

long SignotecDriver::STDisplaySetText(long, long, Align, const wchar_t *)
{
    s_lastError.clear();
    return 0;
}

void SignotecDriver::STControlExit()
{
    STDeviceClose(s_openedIndex >= 0 ? s_openedIndex : 0);
    // HIDAPI can be shared with other components. Do not call global hid_exit here.
}

std::string SignotecDriver::STGetLastError()
{
    return s_lastError;
}

// Fallback cleanup if the application forgets STControlExit(). Constructed after
// the receiver thread, so it joins the receiver before the thread is destroyed.
static struct DriverShutdown
{
    ~DriverShutdown()
    {
        // Do not call public APIs here: thread-local error storage may already
        // have been destroyed during process shutdown.
        joinReceiver();
        if (device)
        {
            hid_close(device);
            device = nullptr;
        }
        EVP_CIPHER_CTX_free(sampleDecryptor);
        sampleDecryptor = nullptr;
        clearAesKey();
    }
} driverShutdown;
