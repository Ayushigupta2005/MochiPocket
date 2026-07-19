#ifndef BUDGET_MANAGER_H
#define BUDGET_MANAGER_H

#include <string>
#include <unordered_map>
#include "Common.h"

struct Budget {
    std::string category;
    double limit;
};

class BudgetManager {
private:
    std::unordered_map<std::string, double> budgets; // category -> limit

public:
    BudgetManager() = default;

    void setBudget(const std::string& category, double limit);
    double getBudget(const std::string& category) const;
    void removeBudget(const std::string& category);
    bool hasBudget(const std::string& category) const;
    
    const std::unordered_map<std::string, double>& getAllBudgets() const { return budgets; }
    void clear() { budgets.clear(); }
};

#endif // BUDGET_MANAGER_H
