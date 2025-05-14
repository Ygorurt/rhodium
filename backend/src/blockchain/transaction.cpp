#include "transaction.h"
#include "crypto/ecdsa.h"
#include <sstream>
#include <iomanip>

Transaction::Transaction(const std::string& from, const std::string& to, double amount)
    : from_(from), to_(to), amount_(amount), timestamp_(std::time(nullptr)) {
    // Gera um ID único baseado nos dados da transação
    std::stringstream ss;
    ss << from << to << amount << timestamp_;
    id_ = sha256(ss.str());
}

std::string Transaction::calculateHash() const {
    std::stringstream ss;
    ss << from_ << to_ << amount_ << timestamp_;
    return sha256(ss.str());
}

void Transaction::sign(const std::string& privateKey) {
    std::string txHash = calculateHash();
    signature_ = ECDSA::signMessage(privateKey, txHash);
}

bool Transaction::verifySignature() const {
    // Transações sem remetente (como recompensas de mineração) são válidas
    if (from_.empty()) return true;
    
    // Transações com remetente mas sem assinatura são inválidas
    if (signature_.empty()) return false;
    
    std::string txHash = calculateHash();
    return ECDSA::verifySignature(from_, txHash, signature_);
}

bool Transaction::isValid() const {
    // Verifica assinatura
    if (!verifySignature()) return false;
    
    // Verifica valores básicos
    if (amount_ <= 0) return false;
    if (to_.empty()) return false;
    
    // Se tem remetente, verifica se o endereço é válido
    if (!from_.empty() && !ECDSA::isValidAddress(from_)) return false;
    
    // Verifica se o destinatário é válido
    return ECDSA::isValidAddress(to_);
}

nlohmann::json Transaction::toJson() const {
    return {
        {"id", id_},
        {"from", from_},
        {"to", to_},
        {"amount", amount_},
        {"timestamp", timestamp_},
        {"signature", signature_}  // Adicionado a assinatura ao JSON
    };
}

std::string Transaction::serialize() const {
    return toJson().dump();
}

Transaction Transaction::deserialize(const std::string& data) {
    auto json = nlohmann::json::parse(data);
    Transaction tx(json["from"], json["to"], json["amount"]);
    
    // Como o timestamp e id são gerados no construtor, precisamos restaurar
    tx.timestamp_ = json["timestamp"];
    tx.id_ = json["id"];
    tx.signature_ = json["signature"];
    
    return tx;
}

// Implementação dos getters e setters
std::string Transaction::getId() const { return id_; }
std::string Transaction::getFrom() const { return from_; }
std::string Transaction::getTo() const { return to_; }
double Transaction::getAmount() const { return amount_; }
time_t Transaction::getTimestamp() const { return timestamp_; }
std::string Transaction::getSignature() const { return signature_; }
void Transaction::setSignature(const std::string& signature) { signature_ = signature; }
