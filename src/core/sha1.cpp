#include "sha1.hpp"

#include <array>

namespace {

constexpr std::uint32_t rol(std::uint32_t value, unsigned bits) {
    return (value << bits) | (value >> (32U - bits));
}

}  // namespace

std::string sha1_hex(std::span<const std::uint8_t> data) {
    std::uint32_t h0 = 0x67452301U;
    std::uint32_t h1 = 0xEFCDAB89U;
    std::uint32_t h2 = 0x98BADCFEU;
    std::uint32_t h3 = 0x10325476U;
    std::uint32_t h4 = 0xC3D2E1F0U;

    const std::uint64_t bit_count = static_cast<std::uint64_t>(data.size()) * 8U;
    std::vector<std::uint8_t> buffer(data.begin(), data.end());
    buffer.push_back(0x80);
    while ((buffer.size() % 64U) != 56U) {
        buffer.push_back(0x00);
    }
    for (int i = 7; i >= 0; --i) {
        buffer.push_back(static_cast<std::uint8_t>((bit_count >> (static_cast<unsigned>(i) * 8U)) & 0xFFU));
    }

    for (std::size_t offset = 0; offset < buffer.size(); offset += 64) {
        std::array<std::uint32_t, 80> w{};
        for (int i = 0; i < 16; ++i) {
            w[static_cast<std::size_t>(i)] =
                (static_cast<std::uint32_t>(buffer[offset + static_cast<std::size_t>(i) * 4U]) << 24U) |
                (static_cast<std::uint32_t>(buffer[offset + static_cast<std::size_t>(i) * 4U + 1U]) << 16U) |
                (static_cast<std::uint32_t>(buffer[offset + static_cast<std::size_t>(i) * 4U + 2U]) << 8U) |
                (static_cast<std::uint32_t>(buffer[offset + static_cast<std::size_t>(i) * 4U + 3U]));
        }
        for (int i = 16; i < 80; ++i) {
            w[static_cast<std::size_t>(i)] =
                rol(w[static_cast<std::size_t>(i) - 3U] ^ w[static_cast<std::size_t>(i) - 8U] ^
                        w[static_cast<std::size_t>(i) - 14U] ^ w[static_cast<std::size_t>(i) - 16U],
                    1U);
        }

        std::uint32_t a = h0;
        std::uint32_t b = h1;
        std::uint32_t c = h2;
        std::uint32_t d = h3;
        std::uint32_t e = h4;

        for (int i = 0; i < 80; ++i) {
            std::uint32_t f = 0;
            std::uint32_t k = 0;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999U;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1U;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDCU;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6U;
            }
            const std::uint32_t temp = rol(a, 5U) + f + e + k + w[static_cast<std::size_t>(i)];
            e = d;
            d = c;
            c = rol(b, 30U);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    static constexpr char kHex[] = "0123456789abcdef";
    const std::array<std::uint32_t, 5> digest{h0, h1, h2, h3, h4};
    std::string hex(40, '0');
    for (std::size_t i = 0; i < digest.size(); ++i) {
        for (int nibble = 0; nibble < 8; ++nibble) {
            const unsigned shift = static_cast<unsigned>(28 - nibble * 4);
            hex[i * 8U + static_cast<std::size_t>(nibble)] = kHex[(digest[i] >> shift) & 0xFU];
        }
    }
    return hex;
}

std::string sha1_bytes_hex(const std::vector<std::uint8_t>& data) {
    return sha1_hex(data);
}
