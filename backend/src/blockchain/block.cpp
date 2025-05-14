// block.cpp
#include "block.h"

Block::Block(size_t height, time_t timestamp, std::string previousHash,
             std::string miner, std::vector<Transaction> transactions) :
    height(height),
    timestamp(timestamp),
    previousHash(std::move(previousHash)),
    miner(std::move(miner)),
    transactions(std::move(transactions)),
    nonce(0) {}
