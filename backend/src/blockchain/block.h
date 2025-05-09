#ifndef BLOCK_H
#define BLOCK_H

#include "transaction.h"
#include <vector>
#include <string>
#include <ctime>

class Block {
public:
    Block(int index, const std::string& previousHash, 
          const std::vector<Transaction>& transactions);
    
    std::string getHash() const { return hash_; }
    void incrementNonce() { nonce_++; }
    
private:
    int index_;
    std::string previousHash_;
    std::vector<Transaction> transactions_;
    time_t timestamp_;
    int nonce_;
    std::string hash_;
    
    void calculateHash();
};
#endif // BLOCK_H