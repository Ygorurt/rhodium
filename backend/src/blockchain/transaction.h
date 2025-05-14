#ifndef TRANSACTION_H
#define TRANSACTION_H

#pragma once
#include <string>
#include <ctime>
#include "crypto/sha256.h"

class Transaction {
public:
    Transaction(const std::string& from, const std::string& to, double amount);
    
    std::string getId() const;
    std::string getFrom() const;
    std::string getTo() const;
    double getAmount() const;
    time_t getTimestamp() const;
    
    nlohmann::json toJson() const;
    std::string serialize() const;
    static Transaction deserialize(const std::string& data);
    
private:
    std::string id_;
    std::string from_;
    std::string to_;
    double amount_;
    time_t timestamp_;
};

#endif // TRANSACTION_H
