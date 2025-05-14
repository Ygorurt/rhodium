#include "blockchain/core.h"
#include "crypto/sha256.h"
#include <algorithm>

namespace BlockchainCore {

    Blockchain::Blockchain() {
        // Create genesis block if chain is empty
        if (chain_.empty()) {
            Block genesis(0, "0", {});
            genesis.setHash(calculateHash(genesis.serialize()));
            chain_.push_back(genesis);
        }
    }

    bool Blockchain::addBlock(const Block& block) {
        if (validateBlock(block)) {
            chain_.push_back(block);
            return true;
        }
        return false;
    }

    bool Blockchain::validateChain() const {
        for (size_t i = 1; i < chain_.size(); ++i) {
            if (!validateBlock(chain_[i]) || 
                chain_[i].getPreviousHash() != chain_[i-1].getHash()) {
                return false;
            }
        }
        return true;
    }

    void Blockchain::addTransaction(const Transaction& tx) {
        pendingTransactions_.push_back(tx);
    }

    Block Blockchain::createNewBlock(const std::string& minerAddress) {
        Block newBlock(
            chain_.size(),
            getLastHash(),
            pendingTransactions_
        );
        newBlock.setMinerAddress(minerAddress);
        return newBlock;
    }

    bool Blockchain::mineBlock(Block& block) {
        // Simple PoW implementation
        while (!block.getHash().starts_with("0000")) {
            block.incrementNonce();
            block.setHash(calculateHash(block.serialize()));
        }
        return addBlock(block);
    }

    // ... (implement other methods)

    std::string calculateHash(const std::string& input) {
        return SHA256::hash(input);
    }

    std::string generateMerkleRoot(const std::vector<Transaction>& transactions) {
        // Simplified merkle root calculation
        std::string concat;
        for (const auto& tx : transactions) {
            concat += tx.getId();
        }
        return calculateHash(concat);
    }

    uint32_t getCurrentTimestamp() {
        return static_cast<uint32_t>(time(nullptr));
    }

} // namespace BlockchainCore
