#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "block.h"
#include "transaction.h"
#include "network/p2p.h"

class Blockchain {
public:
    Blockchain();
    
    // Métodos básicos
    bool mineBlock(const std::string& minerAddress, bool usePoS = false);
    void saveToFile(const std::string& filename) const;
    std::string lastHash() const;
    void addTransaction(const std::string& from, const std::string& to, double amount, const std::string& signature = "");
    double getBalance(const std::string& address) const;
    size_t getChainLength() const;
    size_t getPendingTransactions() const;
    nlohmann::json toJson() const;
    
    // Métodos PoS
    double getStake(const std::string& address) const;
    std::vector<std::string> getValidators() const;
    
    // Rede P2P
    void initP2PNetwork(int port = 8333);
    void broadcastTransaction(const Transaction& tx);
    void broadcastBlock(const Block& block);
    std::vector<std::string> getPeerList() const;

signals:
    void blockAdded(const Block& newBlock);
    void transactionAdded(const Transaction& tx);
    void peersUpdated();

private:
    std::vector<Block> chain_;
    std::vector<Transaction> pendingTxs_;
    std::unordered_map<std::string, double> balances_;
    std::unordered_map<std::string, double> stakes_;
    mutable std::mutex mutex_;
    
    std::unique_ptr<P2PNetwork> p2pNetwork_;
    
    void processReceivedTransaction(const Transaction& tx);
    void processReceivedBlock(const Block& block);
    void syncChain();
};

#endif // BLOCKCHAIN_H
