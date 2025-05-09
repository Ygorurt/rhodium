#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>  // Adicione esta linha
#include "transaction.h"

class Block {
public:
    Block(int index, const std::string& previousHash, const std::vector<Transaction>& transactions);
    
    std::string getHash() const;
    void incrementNonce();
    nlohmann::json toJson() const;  // Corrigido
    
private:
    int index_;
    std::time_t timestamp_;
    std::string previousHash_;
    std::vector<Transaction> transactions_;
    int nonce_ = 0;
    std::string hash_;
    
    std::string calculateHash() const;
};

#endif // BLOCK_H
