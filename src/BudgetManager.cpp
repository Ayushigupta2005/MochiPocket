#include "BudgetManager.h"

void BudgetManager::setBudget(const std::string& category, double limit) {
    if (limit >= 0.0) {
        budgets[category] = limit;
    }
}

double BudgetManager::getBudget(const std::string& category) const {
    auto it = budgets.find(category);
    if (it != budgets.end()) {
        return it->second;
    }
    return 0.0; // No budget set
}

void BudgetManager::removeBudget(const std::string& category) {
    budgets.erase(category);
}

bool BudgetManager::hasBudget(const std::string& category) const {
    return budgets.find(category) != budgets.end();
}
