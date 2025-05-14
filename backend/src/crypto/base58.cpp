#include "crypto/base58.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

// Alfabeto Base58 (excluindo 0, O, I, l para evitar ambiguidade)
static const char* BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string Base58::encode(const std::vector<unsigned char>& data) {
    std::string result;
    
    // Convertendo os bytes para um número grande (big-endian)
    std::vector<unsigned char> digits(data.size() * 138 / 100 + 1); // Log(256)/Log(58)
    size_t digitlen = 1;
    
    for (const unsigned char byte : data) {
        unsigned int carry = byte;
        
        for (size_t j = 0; j < digitlen; j++) {
            carry += (unsigned int)(digits[j]) << 8;
            digits[j] = carry % 58;
            carry /= 58;
        }
        
        while (carry > 0) {
            digits[digitlen++] = carry % 58;
            carry /= 58;
        }
    }
    
    // Convertendo para string Base58
    for (size_t i = 0; i < (data.size() - 1) && data[i] == 0; i++) {
        result.push_back(BASE58_ALPHABET[0]);
    }
    
    for (size_t i = 0; i < digitlen; i++) {
        result.push_back(BASE58_ALPHABET[digits[digitlen - 1 - i]]);
    }
    
    return result;
}

std::vector<unsigned char> Base58::decode(const std::string& base58) {
    std::vector<unsigned char> result;
    
    // Convertendo a string Base58 para um número grande
    std::vector<unsigned char> bytes(base58.size() * 733 / 1000 + 1); // Log(58)/Log(256)
    size_t bytelen = 1;
    
    for (const char c : base58) {
        const char* pos = strchr(BASE58_ALPHABET, c);
        if (!pos) {
            throw std::runtime_error("Invalid Base58 character");
        }
        
        unsigned int carry = pos - BASE58_ALPHABET;
        
        for (size_t j = 0; j < bytelen; j++) {
            carry += (unsigned int)(bytes[j]) * 58;
            bytes[j] = carry & 0xff;
            carry >>= 8;
        }
        
        while (carry > 0) {
            bytes[bytelen++] = carry & 0xff;
            carry >>= 8;
        }
    }
    
    // Convertendo para bytes
    for (size_t i = 0; i < (base58.size() - 1) && base58[i] == BASE58_ALPHABET[0]; i++) {
        result.push_back(0);
    }
    
    for (size_t i = 0; i < bytelen; i++) {
        result.push_back(bytes[bytelen - 1 - i]);
    }
    
    return result;
}

std::string Base58::encodeCheck(const std::vector<unsigned char>& data) {
    // Adiciona checksum (primeiros 4 bytes do SHA256^2)
    std::vector<unsigned char> withChecksum(data);
    std::vector<unsigned char> hash = sha256(data);
    hash = sha256(hash);
    
    withChecksum.insert(withChecksum.end(), hash.begin(), hash.begin() + 4);
    return encode(withChecksum);
}

std::vector<unsigned char> Base58::decodeCheck(const std::string& base58) {
    std::vector<unsigned char> decoded = decode(base58);
    if (decoded.size() < 4) {
        throw std::runtime_error("Invalid Base58Check format");
    }
    
    // Separa dados e checksum
    std::vector<unsigned char> data(decoded.begin(), decoded.end() - 4);
    std::vector<unsigned char> checksum(decoded.end() - 4, decoded.end());
    
    // Verifica checksum
    std::vector<unsigned char> hash = sha256(data);
    hash = sha256(hash);
    
    if (!std::equal(checksum.begin(), checksum.end(), hash.begin())) {
        throw std::runtime_error("Base58Check checksum mismatch");
    }
    
    return data;
}

bool Base58::verifyChecksum(const std::vector<unsigned char>& data) {
    if (data.size() < 4) return false;
    
    std::vector<unsigned char> payload(data.begin(), data.end() - 4);
    std::vector<unsigned char> checksum(data.end() - 4, data.end());
    
    std::vector<unsigned char> hash = sha256(payload);
    hash = sha256(hash);
    
    return std::equal(checksum.begin(), checksum.end(), hash.begin());
}