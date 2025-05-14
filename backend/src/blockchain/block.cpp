#include "block.h"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <sha256.h>

Block::Block(size_t index, const std::string& previousHash, 
            const std::vector<Transaction>& transactions)
    : index_(index), 
      previousHash_(previousHash),
      transactions_(transactions),
      nonce_(0),
      timestamp_(time(nullptr)) {
    hash_ = calculateHash();
}

std::string Block::calculateHash() const {
    std::stringstream ss;
    ss << index_ << previousHash_ << timestamp_ << nonce_;
    for (const auto& tx : transactions_) {
        ss << tx.serialize(); // Assumindo que Transaction tem serialize()
    }
    return sha256(ss.str());
}
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, ss.str().c_str(), ss.str().size());
    SHA256_Final(hash, &sha256);
    
    std::stringstream hashStream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hashStream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return hashStream.str();
}

std::string Block::getHash() const { return hash_; }
void Block::incrementNonce() { nonce_++; hash_ = calculateHash(); }

nlohmann::json Block::toJson() const {
    nlohmann::json j;
    j["index"] = index_;
    j["timestamp"] = timestamp_;
    j["previousHash"] = previousHash_;
    j["hash"] = hash_;
    j["nonce"] = nonce_;
    
    nlohmann::json txJson;
    for (const auto& tx : transactions_) {
        txJson.push_back(tx.toJson());
    }
    j["transactions"] = txJson;
    
    return j;
}
