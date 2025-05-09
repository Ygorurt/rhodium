#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
public:
    Transaction(const std::string& sender, const std::string& receiver, 
               double amount, const std::string& type = "regular");
    
    std::string getSender() const { return sender_; }
    std::string getReceiver() const { return receiver_; }
    double getAmount() const { return amount_; }
    std::string getType() const { return type_; }
    
private:
    std::string sender_;
    std::string receiver_;
    double amount_;
    time_t timestamp_;
    std::string type_;
    std::string hash_;
    
    void calculateHash();
};
#endif // TRANSACTION_H