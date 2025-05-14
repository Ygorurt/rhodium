#include "crypto/ripemd160.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <openssl/ripemd.h>

std::vector<unsigned char> RIPEMD160::hash(const std::vector<unsigned char>& input) {
    RIPEMD160_CTX ctx;
    unsigned char digest[RIPEMD160_DIGEST_LENGTH];
    
    RIPEMD160_Init(&ctx);
    RIPEMD160_Update(&ctx, input.data(), input.size());
    RIPEMD160_Final(digest, &ctx);
    
    return std::vector<unsigned char>(digest, digest + RIPEMD160_DIGEST_LENGTH);
}

std::vector<unsigned char> RIPEMD160::hash(const std::string& input) {
    return hash(std::vector<unsigned char>(input.begin(), input.end()));
}

std::string RIPEMD160::hexHash(const std::vector<unsigned char>& input) {
    std::vector<unsigned char> hashBytes = hash(input);
    std::ostringstream oss;
    
    for (unsigned char byte : hashBytes) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    
    return oss.str();
}

std::string RIPEMD160::hexHash(const std::string& input) {
    return hexHash(std::vector<unsigned char>(input.begin(), input.end()));
}