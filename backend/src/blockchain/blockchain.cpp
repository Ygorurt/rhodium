#include "blockchain.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <openssl/sha.h>

Blockchain::Blockchain() {
    // Criar bloco genesis
    Block genesis;
    genesis.height = 0;
    genesis.timestamp = std::time(nullptr);
    genesis.previousHash = "0";
    genesis.nonce = 0;
    genesis.hash = "0000000000000000000000000000000000000000000000000000000000000000";
    
    chain_.push_back(genesis);
}

bool Blockchain::mineBlock(const std::string& minerAddress, bool usePoS) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if(pendingTxs_.empty()) {
        return false;
    }
    
    Block newBlock;
    newBlock.height = chain_.back().height + 1;
    newBlock.timestamp = std::time(nullptr);
    newBlock.previousHash = chain_.back().hash;
    newBlock.miner = minerAddress;
    
    // Adicionar transações (limitar a 1000 por bloco)
    size_t txCount = std::min(pendingTxs_.size(), size_t(1000));
    newBlock.transactions.assign(pendingTxs_.begin(), pendingTxs_.begin() + txCount);
    pendingTxs_.erase(pendingTxs_.begin(), pendingTxs_.begin() + txCount);
    
    if(usePoS) {
        // Lógica PoS simplificada
        double stake = getStake(minerAddress);
        if(stake <= 0) return false;
        
        // "Prova" de stake - quanto maior o stake, mais fácil minerar
        unsigned int target = UINT_MAX * (1.0 - std::min(1.0, stake / 10000.0));
        unsigned int randomValue = rand() % UINT_MAX;
        
        if(randomValue > target) {
            return false;
        }
        
        newBlock.hash = "pos_block_" + std::to_string(newBlock.height);
    } else {
        // Lógica PoW simplificada
        for(int nonce = 0; nonce < 1000000; nonce++) {
            newBlock.nonce = nonce;
            std::string blockData = std::to_string(newBlock.height) + newBlock.previousHash + std::to_string(nonce);
            
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256((unsigned char*)blockData.c_str(), blockData.size(), hash);
            
            std::stringstream ss;
            for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
            }
            
            newBlock.hash = ss.str();
            
            // Dificuldade simplificada - hash deve começar com "00"
            if(newBlock.hash.substr(0, 2) == "00") {
                break;
            }
        }
    }
    
    // Atualizar saldos
    for(const auto& tx : newBlock.transactions) {
        balances_[tx.from] -= tx.amount;
        balances_[tx.to] += tx.amount;
    }
    
    // Recompensa do bloco
    double blockReward = 50.0; // Recompensa fixa simplificada
    balances_[minerAddress] += blockReward;
    
    chain_.push_back(newBlock);
    
    // Broadcast do novo bloco
    if(p2pNetwork_) {
        p2pNetwork_->broadcastBlock(newBlock);
    }
    
    emit blockAdded(newBlock);
    return true;
}

void Blockchain::addTransaction(const std::string& from, const std::string& to, double amount, const std::string& signature) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Verificação simplificada
    if(amount <= 0 || from.empty() || to.empty()) {
        return;
    }
    
    // Verificar saldo (ignorar para transações de coinbase)
    if(from != "0" && getBalance(from) < amount) {
        return;
    }
    
    Transaction tx;
    tx.from = from;
    tx.to = to;
    tx.amount = amount;
    tx.timestamp = std::time(nullptr);
    tx.signature = signature;
    
    // Hash da transação simplificado
    std::string txData = from + to + std::to_string(amount) + std::to_string(tx.timestamp);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)txData.c_str(), txData.size(), hash);
    
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    tx.txHash = ss.str();
    pendingTxs_.push_back(tx);
    
    // Broadcast da transação
    if(p2pNetwork_) {
        p2pNetwork_->broadcastTransaction(tx);
    }
    
    emit transactionAdded(tx);
}

double Blockchain::getBalance(const std::string& address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = balances_.find(address);
    return it != balances_.end() ? it->second : 0.0;
}

double Blockchain::getStake(const std::string& address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = stakes_.find(address);
    return it != stakes_.end() ? it->second : 0.0;
}

void Blockchain::initP2PNetwork(int port) {
    p2pNetwork_ = std::make_unique<P2PNetwork>(this, port);
    connect(p2pNetwork_.get(), &P2PNetwork::peersUpdated, this, &Blockchain::peersUpdated);
}

std::vector<std::string> Blockchain::getPeerList() const {
    if(p2pNetwork_) {
        return p2pNetwork_->getConnectedPeers();
    }
    return {};
}

// Outros métodos implementados de forma similar...
