#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include "Common.h"

struct Transaction {
    int id; // Unique ID
    double amount;
    TransactionType type;
    std::string category;
    std::string merchant;
    std::string date; // YYYY-MM-DD
    std::string notes;
};

#endif // TRANSACTION_H
