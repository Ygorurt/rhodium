#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <ctime>
#include "transaction.h"
#include <nlohmann/json.hpp>
#include "crypto/sha256.h"

class Block {
public:
    Block(size_t index, const std::string& previousHash, const std::vector<Transaction>& transactions);
    
    size_t getIndex() const;
    std::string getPreviousHash() const;
    std::string getHash() const;
    std::string getMinerAddress() const;
    const std::vector<Transaction>& getTransactions() const;
    
    void incrementNonce();
    nlohmann::json toJson() const;
    std::string serialize() const;
    static Block deserialize(const std::string& data);
    
private:
    size_t index_;
    std::string previousHash_;
    std::string hash_;
    std::vector<Transaction> transactions_;
    int nonce_;
    time_t timestamp_;
    std::string minerAddress_;
    
    std::string calculateHash() const;
};
#endif // BLOCK_H
