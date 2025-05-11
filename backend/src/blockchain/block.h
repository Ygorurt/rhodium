#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <ctime>
#include "transaction.h"
#include <nlohmann/json.hpp>

class Block {
public:
    Block(int index, const std::string& previousHash, 
          const std::vector<Transaction>& transactions);
    
    std::string getHash() const;
    void incrementNonce();
    nlohmann::json toJson() const;
    
private:
    int index_;
    std::time_t timestamp_;
    std::string previousHash_;
    std::vector<Transaction> transactions_;
    int nonce_;
    std::string hash_;
    
    std::string calculateHash() const;
};

#endif // BLOCK_H
