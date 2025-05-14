#include "blockchain.h"
#include <fstream>
#include <iostream>
#include "crypto/sha256.h"

Blockchain::Blockchain() {
    // Cria bloco genesis
    Block genesis(0, "0", {});
    chain_.push_back(genesis);
    
    // Inicializa rede P2P
    p2pNetwork_ = std::make_unique<P2PNetwork>(this);
}

void Blockchain::initP2PNetwork(int port) {
    p2pNetwork_->start(port);
    
    // Conecta a nós conhecidos
    std::vector<std::string> seedNodes = {
        "seed1.rhodium.org:8333",
        "seed2.rhodium.org:8333"
    };
    p2pNetwork_->connectToSeedNodes(seedNodes);
    
    // Sincroniza cadeia inicial
    syncChain();
}

bool Blockchain::mineBlock(const std::string& minerAddress, bool usePoS) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (pendingTxs_.empty()) return false;

    Block newBlock(chain_.size(), lastHash(), pendingTxs_);
    pendingTxs_.clear();

    if (!usePoS) {
        // PoW mining
        while (newBlock.getHash().substr(0, 4) != "0000") {
            newBlock.incrementNonce();
        }
    }

    chain_.push_back(newBlock);
    balances_[minerAddress] += 50.0; // Recompensa de bloco
    
    // Broadcast do novo bloco
    if (p2pNetwork_) {
        p2pNetwork_->broadcastBlock(newBlock.serialize());
    }
    
    return true;
}

void Blockchain::addTransaction(const std::string& from, const std::string& to, double amount) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (amount <= 0) {
        throw std::runtime_error("Transaction amount must be positive");
    }
    
    Transaction tx(from, to, amount);
    pendingTxs_.emplace_back(tx);
    
    // Broadcast da transação
    if (p2pNetwork_) {
        p2pNetwork_->broadcastTransaction(tx.serialize());
    }
}

void Blockchain::broadcastTransaction(const Transaction& tx) {
    if (p2pNetwork_) {
        p2pNetwork_->broadcastTransaction(tx.serialize());
    }
}

void Blockchain::broadcastBlock(const Block& block) {
    if (p2pNetwork_) {
        p2pNetwork_->broadcastBlock(block.serialize());
    }
}

void Blockchain::processReceivedTransaction(const Transaction& tx) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Verifica se a transação já existe
    for (const auto& pendingTx : pendingTxs_) {
        if (pendingTx.getId() == tx.getId()) {
            return; // Transação já existe
        }
    }
    
    // Adiciona à pool de transações
    pendingTxs_.push_back(tx);
}

void Blockchain::processReceivedBlock(const Block& block) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Verifica se o bloco já existe
    if (block.getIndex() < chain_.size()) {
        return; // Bloco já existe na cadeia
    }
    
    // Valida o bloco
    if (block.getPreviousHash() != lastHash()) {
        std::cerr << "Received block with invalid previous hash" << std::endl;
        return;
    }
    
    if (block.getHash().substr(0, 4) != "0000") {
        std::cerr << "Received block with invalid proof of work" << std::endl;
        return;
    }
    
    // Adiciona o bloco à cadeia
    chain_.push_back(block);
    
    // Atualiza saldos (simplificado)
    balances_[block.getMinerAddress()] += 50.0; // Recompensa de bloco
    for (const auto& tx : block.getTransactions()) {
        balances_[tx.getFrom()] -= tx.getAmount();
        balances_[tx.getTo()] += tx.getAmount();
    }
    
    // Remove transações confirmadas da pool
    auto it = std::remove_if(pendingTxs_.begin(), pendingTxs_.end(),
        [&block](const Transaction& tx) {
            for (const auto& blockTx : block.getTransactions()) {
                if (blockTx.getId() == tx.getId()) {
                    return true;
                }
            }
            return false;
        });
    pendingTxs_.erase(it, pendingTxs_.end());
}

void Blockchain::syncChain() {
    // Implementação simplificada de sincronização
    // Na prática, você precisaria de um protocolo mais sofisticado
    if (p2pNetwork_ && !p2pNetwork_->getPeerList().empty()) {
        // Solicita a cadeia mais longa dos peers
        // Aqui você implementaria o protocolo de sincronização
    }
}

std::vector<std::string> Blockchain::getPeerList() const {
    if (p2pNetwork_) {
        return p2pNetwork_->getPeerList();
    }
    return {};
}

// ... (outros métodos permanecem iguais)
