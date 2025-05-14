// block.h
#pragma once
#include <vector>
#include <string>
#include "transaction.h"

class Block {
public:
    Block() = default;
    Block(size_t height, time_t timestamp, std::string previousHash, 
          std::string miner, std::vector<Transaction> transactions);
    
    size_t height;
    time_t timestamp;
    std::string previousHash;
    std::string hash;
    std::string miner;
    uint64_t nonce;
    std::vector<Transaction> transactions;
};
