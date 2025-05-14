#ifndef RIPEMD160_H
#define RIPEMD160_H

#include <string>
#include <vector>

class RIPEMD160 {
public:
    // Calcula o hash RIPEMD-160 de um array de bytes
    static std::vector<unsigned char> hash(const std::vector<unsigned char>& input);
    
    // Calcula o hash RIPEMD-160 de uma string
    static std::vector<unsigned char> hash(const std::string& input);
    
    // Calcula o hash RIPEMD-160 e retorna como string hex
    static std::string hexHash(const std::vector<unsigned char>& input);
    static std::string hexHash(const std::string& input);
};

#endif // RIPEMD160_H