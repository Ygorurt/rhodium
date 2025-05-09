#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <map>
#include <nlohmann/json.hpp>
#include "block.h"
#include "transaction.h"

class Blockchain {
public:
    Blockchain();
    ~Blockchain();

    // Core functions
    bool mineBlock(const std::string& minerAddress, bool usePoS = false);
    void addTransaction(const Transaction& tx);
    
    // Wallet operations
    std::string createWallet();
    double getBalance(const std::string& address) const;
    
    // PIX operations
    bool registerPixKey(const std::string& address, const std::string& keyType, const std::string& keyValue);
    std::string sendPix(const std::string& sender, const std::string& pixKey, double amount);
    
    // Persistence
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    
    // Getters
    size_t getChainLength() const { return chain_.size(); }
    std::vector<Transaction> getPendingTransactions() const { return pendingTxs_; }

private:
    std::vector<Block> chain_;
    std::vector<Transaction> pendingTxs_;
    std::map<std::string, double> balances_;
    std::map<std::string, std::string> pixKeys_; // pix key -> address
    mutable std::mutex mutex_;
    
    std::string lastHash() const;
    bool validateBlock(const Block& block) const;
};
#endif // BLOCKCHAIN_H