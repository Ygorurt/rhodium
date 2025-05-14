#include "sha256.h"
#include <cstring>
#include <sstream>
#include <iomanip>

SHA256::SHA256() {
    m_blocklen = 0;
    m_bitlen = 0;
    m_state[0] = 0x6a09e667;
    m_state[1] = 0xbb67ae85;
    m_state[2] = 0x3c6ef372;
    m_state[3] = 0xa54ff53a;
    m_state[4] = 0x510e527f;
    m_state[5] = 0x9b05688c;
    m_state[6] = 0x1f83d9ab;
    m_state[7] = 0x5be0cd19;
}

void SHA256::update(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        m_data[m_blocklen++] = data[i];
        if (m_blocklen == 64) {
            transform();
            m_bitlen += 512;
            m_blocklen = 0;
        }
    }
}

void SHA256::update(const std::string& data) {
    update(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
}

std::array<uint8_t, 32> SHA256::digest() {
    std::array<uint8_t, 32> hash;
    pad();
    revert(hash);
    return hash;
}

std::string SHA256::toString(const std::array<uint8_t, 32>& digest) {
    std::stringstream s;
    s << std::setfill('0') << std::hex;
    for (uint8_t i = 0; i < 32; i++) {
        s << std::setw(2) << (unsigned int)digest[i];
    }
    return s.str();
}

std::array<uint8_t, 32> SHA256::hash(const uint8_t* data, size_t length) {
    SHA256 sha;
    sha.update(data, length);
    return sha.digest();
}

std::array<uint8_t, 32> SHA256::hash(const std::string& data) {
    return hash(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
}

std::string SHA256::hashString(const std::string& data) {
    return toString(hash(data));
}

void SHA256::transform() {
    uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
    uint32_t state[8];
    
    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) {
        m[i] = (m_data[j] << 24) | (m_data[j + 1] << 16) | 
               (m_data[j + 2] << 8) | (m_data[j + 3]);
    }
    
    for (uint8_t k = 16; k < 64; k++) {
        uint32_t s0 = (m[k-15] >> 7 | m[k-15] << 25) ^ 
                      (m[k-15] >> 18 | m[k-15] << 14) ^ 
                      (m[k-15] >> 3);
        uint32_t s1 = (m[k-2] >> 17 | m[k-2] << 15) ^ 
                      (m[k-2] >> 19 | m[k-2] << 13) ^ 
                      (m[k-2] >> 10);
        m[k] = m[k-16] + s0 + m[k-7] + s1;
    }
    
    for (uint8_t i = 0; i < 8; i++) {
        state[i] = m_state[i];
    }
    
    for (uint8_t i = 0; i < 64; i++) {
        s1 = (state[4] >> 6 | state[4] << 26) ^ 
             (state[4] >> 11 | state[4] << 21) ^ 
             (state[4] >> 25 | state[4] << 7);
        ch = (state[4] & state[5]) ^ (~state[4] & state[6]);
        temp1 = state[7] + s1 + ch + K[i] + m[i];
        s0 = (state[0] >> 2 | state[0] << 30) ^ 
             (state[0] >> 13 | state[0] << 19) ^ 
             (state[0] >> 22 | state[0] << 10);
        maj = (state[0] & state[1]) ^ (state[0] & state[2]) ^ (state[1] & state[2]);
        temp2 = s0 + maj;
        
        state[7] = state[6];
        state[6] = state[5];
        state[5] = state[4];
        state[4] = state[3] + temp1;
        state[3] = state[2];
        state[2] = state[1];
        state[1] = state[0];
        state[0] = temp1 + temp2;
    }
    
    for (uint8_t i = 0; i < 8; i++) {
        m_state[i] += state[i];
    }
}

void SHA256::pad() {
    uint64_t i = m_blocklen;
    uint8_t end = m_blocklen < 56 ? 56 : 64;
    
    m_data[i++] = 0x80;
    while (i < end) {
        m_data[i++] = 0x00;
    }
    
    if (m_blocklen >= 56) {
        transform();
        memset(m_data, 0, 56);
    }
    
    m_bitlen += m_blocklen * 8;
    m_data[63] = m_bitlen;
    m_data[62] = m_bitlen >> 8;
    m_data[61] = m_bitlen >> 16;
    m_data[60] = m_bitlen >> 24;
    m_data[59] = m_bitlen >> 32;
    m_data[58] = m_bitlen >> 40;
    m_data[57] = m_bitlen >> 48;
    m_data[56] = m_bitlen >> 56;
    transform();
}

void SHA256::revert(std::array<uint8_t, 32>& hash) {
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
        }
    }
}