#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>  // Adicione esta linha
#include "block.h"

class Blockchain {
public:
    Blockchain();
    
    bool mineBlock(const std::string& minerAddress, bool usePoS = false);
    void saveToFile(const std::string& filename) const;
    std::string lastHash() const;
    void addTransaction(const std::string& from, const std::string& to, double amount);
    double getBalance(const std::string& address) const;
    
    // Adicione estas funções que estavam sendo usadas
    size_t getChainLength() const { return chain_.size(); }
    size_t getPendingTransactions() const { return pendingTxs_.size(); }
    
private:
    std::vector<Block> chain_;
    std::vector<Transaction> pendingTxs_;
    std::unordered_map<std::string, double> balances_;
    mutable std::mutex mutex_;
    
    nlohmann::json toJson() const;
};

#endif // BLOCKCHAIN_H
