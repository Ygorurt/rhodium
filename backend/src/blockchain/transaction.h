#ifndef TRANSACTION_H
#define TRANSACTION_H

#pragma once
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>
#include "crypto/sha256.h"
#include "crypto/ecdsa.h"  // Novo cabeçalho para ECDSA

class Transaction {
public:
    Transaction(const std::string& from, const std::string& to, double amount);
    
    // Métodos existentes
    std::string getId() const;
    std::string getFrom() const;
    std::string getTo() const;
    double getAmount() const;
    time_t getTimestamp() const;
    
    // Métodos de serialização
    nlohmann::json toJson() const;
    std::string serialize() const;
    static Transaction deserialize(const std::string& data);
    
    // Novos métodos para ECDSA
    void sign(const std::string& privateKey);  // Assina a transação
    bool verifySignature() const;              // Verifica a assinatura
    bool isValid() const;                     // Verifica integridade geral
    
    // Métodos para acesso à assinatura
    std::string getSignature() const;
    void setSignature(const std::string& signature);

private:
    std::string id_;
    std::string from_;
    std::string to_;
    double amount_;
    time_t timestamp_;
    std::string signature_;  // Novo campo para a assinatura digital
    
    // Método interno para calcular o hash dos dados da transação
    std::string calculateHash() const;
};

#endif // TRANSACTION_H
