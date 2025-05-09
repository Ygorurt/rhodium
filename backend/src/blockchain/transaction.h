#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
public:
    Transaction(const std::string& from, const std::string& to, double amount);
    
    std::string getFrom() const;
    std::string getTo() const;
    double getAmount() const;
    nlohmann::json toJson() const;
    
private:
    std::string from_;
    std::string to_;
    double amount_;
};

#endif // TRANSACTION_H
