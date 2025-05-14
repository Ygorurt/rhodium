#ifndef BLOCKCHAIN_CORE_H
#define BLOCKCHAIN_CORE_H

#include <vector>
#include <string>
#include <memory>
#include "block.h"
#include "transaction.h"

/**
 * @brief Core blockchain functionality and data structures
 */
namespace BlockchainCore {

    class Blockchain {
    public:
        Blockchain();
        
        // Chain operations
        bool addBlock(const Block& block);
        bool validateChain() const;
        
        // Transaction handling
        void addTransaction(const Transaction& tx);
        std::vector<Transaction> getPendingTransactions() const;
        void clearPendingTransactions();
        
        // Getters
        const std::vector<Block>& getChain() const;
        size_t getCurrentHeight() const;
        std::string getLastHash() const;
        
        // Mining
        Block createNewBlock(const std::string& minerAddress);
        bool mineBlock(Block& block);
        
        // Serialization
        std::string serialize() const;
        bool deserialize(const std::string& data);
        
    private:
        std::vector<Block> chain_;
        std::vector<Transaction> pendingTransactions_;
        
        // Helper functions
        bool validateBlock(const Block& block) const;
        std::string calculateBlockHash(const Block& block) const;
    };

    // Utility functions
    std::string calculateHash(const std::string& input);
    std::string generateMerkleRoot(const std::vector<Transaction>& transactions);
    uint32_t getCurrentTimestamp();

} // namespace BlockchainCore

#endif // BLOCKCHAIN_CORE_H
