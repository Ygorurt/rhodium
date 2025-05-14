#include "ecdsa.h"
#include "sha256.h"
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <stdexcept>

void ECDSA::generateKeyPair(std::string& privateKey, std::string& publicKey) {
    EC_KEY* key = createNewKey();
    privateKey = keyToHex(key);
    publicKey = publicKeyToAddress(privateKey);
    EC_KEY_free(key);
}

std::string ECDSA::signMessage(const std::string& privateKey, const std::string& message) {
    EC_KEY* key = hexToKey(privateKey, true);
    if (!key) throw std::runtime_error("Invalid private key");
    
    std::string hash = sha256(message);
    
    ECDSA_SIG* sig = ECDSA_do_sign((const unsigned char*)hash.c_str(), hash.size(), key);
    if (!sig) {
        EC_KEY_free(key);
        throw std::runtime_error("Failed to sign message");
    }
    
    unsigned char* der = nullptr;
    int derLen = i2d_ECDSA_SIG(sig, &der);
    if (derLen <= 0) {
        ECDSA_SIG_free(sig);
        EC_KEY_free(key);
        throw std::runtime_error("Failed to convert signature to DER");
    }
    
    std::string signature((char*)der, derLen);
    OPENSSL_free(der);
    ECDSA_SIG_free(sig);
    EC_KEY_free(key);
    
    return signature;
}

bool ECDSA::verifySignature(const std::string& publicKey, const std::string& message, const std::string& signature) {
    EC_KEY* key = hexToKey(publicKey, false);
    if (!key) return false;
    
    std::string hash = sha256(message);
    
    const unsigned char* sigData = (const unsigned char*)signature.data();
    ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &sigData, signature.size());
    if (!sig) {
        EC_KEY_free(key);
        return false;
    }
    
    int result = ECDSA_do_verify((const unsigned char*)hash.c_str(), hash.size(), sig, key);
    
    ECDSA_SIG_free(sig);
    EC_KEY_free(key);
    
    return result == 1;
}

bool ECDSA::isValidAddress(const std::string& address) {
    // Implementação básica - pode ser melhorada com checksum como em endereços Bitcoin
    return address.size() == 64; // Exemplo simplificado
}

std::string ECDSA::publicKeyToAddress(const std::string& publicKey) {
    // Implementação simplificada - na prática seria mais complexo (ex: RIPEMD160(SHA256(pubkey)))
    return sha256(publicKey);
}

// Métodos auxiliares privados
EC_KEY* ECDSA::createNewKey() {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!key) throw std::runtime_error("Failed to create EC key");
    
    if (!EC_KEY_generate_key(key)) {
        EC_KEY_free(key);
        throw std::runtime_error("Failed to generate EC key pair");
    }
    
    return key;
}

std::string ECDSA::keyToHex(const EC_KEY* key) {
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_ECPrivateKey(bio, const_cast<EC_KEY*>(key), nullptr, nullptr, 0, nullptr, nullptr);
    
    char* data = nullptr;
    long len = BIO_get_mem_data(bio, &data);
    std::string result(data, len);
    
    BIO_free(bio);
    return result;
}

EC_KEY* ECDSA::hexToKey(const std::string& hex, bool isPrivate) {
    BIO* bio = BIO_new_mem_buf(hex.data(), hex.size());
    EC_KEY* key = isPrivate ? 
        PEM_read_bio_ECPrivateKey(bio, nullptr, nullptr, nullptr) :
        PEM_read_bio_EC_PUBKEY(bio, nullptr, nullptr, nullptr);
    
    BIO_free(bio);
    return key;
}