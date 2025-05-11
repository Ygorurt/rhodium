#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include "block.h"
#include <nlohmann/json.hpp>

class Blockchain {
public:
    Blockchain();
    
    bool mineBlock(const std::string& minerAddress, bool usePoS = false);
    void saveToFile(const std::string& filename) const;
    std::string lastHash() const;
    void addTransaction(const std::string& from, const std::string& to, double amount);
    double getBalance(const std::string& address) const;
    size_t getChainLength() const;
    size_t getPendingTransactions() const;
    
private:
    std::vector<Block> chain_;
    std::vector<Transaction> pendingTxs_;
    std::unordered_map<std::string, double> balances_;
    mutable std::mutex mutex_;
    
    nlohmann::json toJson() const;
};

#endif // BLOCKCHAIN_H
