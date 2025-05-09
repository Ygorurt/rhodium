#include "blockchain.h"
#include <fstream>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

Blockchain::Blockchain() {
    // Create genesis block
    Block genesis(0, "0", {});
    chain_.push_back(genesis);
}

bool Blockchain::mineBlock(const std::string& minerAddress, bool usePoS) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pendingTxs_.empty()) return false;

    Block newBlock(chain_.size(), lastHash(), pendingTxs_);
    pendingTxs_.clear();

    // Simple mining simulation
    if (!usePoS) {
        // PoW mining
        while (newBlock.getHash().substr(0, 4) != "0000") {
            newBlock.incrementNonce();
        }
    }

    chain_.push_back(newBlock);
    balances_[minerAddress] += 50.0; // Block reward
    return true;
}

void Blockchain::saveToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json j;
    // Serialization logic
    std::ofstream out(filename);
    out << j.dump(4);
}