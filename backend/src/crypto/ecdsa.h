#ifndef ECDSA_H
#define ECDSA_H

#include <string>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

class ECDSA {
public:
    // Geração de chaves
    static void generateKeyPair(std::string& privateKey, std::string& publicKey);
    
    // Assinatura e verificação
    static std::string signMessage(const std::string& privateKey, const std::string& message);
    static bool verifySignature(const std::string& publicKey, const std::string& message, const std::string& signature);
    
    // Validação de endereços
    static bool isValidAddress(const std::string& address);
    
    // Conversão de formatos
    static std::string publicKeyToAddress(const std::string& publicKey);
    
private:
    static EC_KEY* createNewKey();
    static std::string keyToHex(const EC_KEY* key);
    static EC_KEY* hexToKey(const std::string& hex, bool isPrivate);
};

#endif // ECDSA_H