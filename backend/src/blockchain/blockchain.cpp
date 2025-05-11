#include "blockchain.h"
#include <fstream>

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
    std::ofstream out(filename);
    out << toJson().dump(4);
}

std::string Blockchain::lastHash() const {
    if (chain_.empty()) {
        return "0";
    }
    return chain_.back().getHash();
}

void Blockchain::addTransaction(const std::string& from, const std::string& to, double amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (amount <= 0) {
        throw std::runtime_error("Transaction amount must be positive");
    }
    pendingTxs_.emplace_back(from, to, amount);
}

double Blockchain::getBalance(const std::string& address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = balances_.find(address);
    return it != balances_.end() ? it->second : 0.0;
}

size_t Blockchain::getChainLength() const {
    return chain_.size();
}

size_t Blockchain::getPendingTransactions() const {
    return pendingTxs_.size();
}

nlohmann::json Blockchain::toJson() const {
    nlohmann::json j;
    j["chain"] = nlohmann::json::array();
    for (const auto& block : chain_) {
        j["chain"].push_back(block.toJson());
    }
    return j;
}
