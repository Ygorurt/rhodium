#ifndef BASE58_H
#define BASE58_H

#include <string>
#include <vector>

class Base58 {
public:
    // Codifica dados binários para Base58
    static std::string encode(const std::vector<unsigned char>& data);
    
    // Decodifica string Base58 para dados binários
    static std::vector<unsigned char> decode(const std::string& base58);
    
    // Codifica com checksum (Base58Check)
    static std::string encodeCheck(const std::vector<unsigned char>& data);
    
    // Decodifica com verificação de checksum (Base58Check)
    static std::vector<unsigned char> decodeCheck(const std::string& base58);

private:
    // Verifica o checksum
    static bool verifyChecksum(const std::vector<unsigned char>& data);
};

#endif // BASE58_H