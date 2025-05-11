#include "transaction.h"

Transaction::Transaction(const std::string& from, const std::string& to, double amount)
    : from_(from), to_(to), amount_(amount) {}

std::string Transaction::getFrom() const { return from_; }
std::string Transaction::getTo() const { return to_; }
double Transaction::getAmount() const { return amount_; }

nlohmann::json Transaction::toJson() const {
    return {
        {"from", from_},
        {"to", to_},
        {"amount", amount_}
    };
}