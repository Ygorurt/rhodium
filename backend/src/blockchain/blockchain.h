#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "block.h"
#include "transaction.h"
#include "network/p2p.h"  // Nova inclusão

class Blockchain {
public:
    Blockchain();
    
    // Métodos existentes
    bool mineBlock(const std::string& minerAddress, bool usePoS = false);
    void saveToFile(const std::string& filename) const;
    std::string lastHash() const;
    void addTransaction(const std::string& from, const std::string& to, double amount);
    double getBalance(const std::string& address) const;
    size_t getChainLength() const;
    size_t getPendingTransactions() const;
    nlohmann::json toJson() const;
    
    // Novos métodos P2P
    void initP2PNetwork(int port = 8333);
    void broadcastTransaction(const Transaction& tx);
    void broadcastBlock(const Block& block);
    std::vector<std::string> getPeerList() const;
    
private:
    std::vector<Block> chain_;
    std::vector<Transaction> pendingTxs_;
    std::unordered_map<std::string, double> balances_;
    mutable std::mutex mutex_;
    
    // Rede P2P
    std::unique_ptr<P2PNetwork> p2pNetwork_;
    
    // Novos métodos internos
    void processReceivedTransaction(const Transaction& tx);
    void processReceivedBlock(const Block& block);
    void syncChain();
};

#endif // BLOCKCHAIN_H
