#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <unordered_map>
#include <string>
#include "Transaction.h"
#include "SavingsGoal.h"

class AnalyticsEngine {
public:
    AnalyticsEngine() = default;

    double calculateTotalIncome(const std::vector<Transaction>& transactions) const;
    double calculateTotalExpense(const std::vector<Transaction>& transactions) const;
    std::unordered_map<std::string, double> calculateCategoryTotals(const std::vector<Transaction>& transactions) const;
    Transaction getHighestExpense(const std::vector<Transaction>& transactions) const;
    std::string getMostVisitedMerchant(const std::vector<Transaction>& transactions) const;
    double calculateAverageDailySpending(const std::vector<Transaction>& transactions) const;
    
    // Calculates total net savings as: (Total Income - Total Expense - Goal deposits + Goal withdrawals, or simply remaining balance + goal balances)
    // Here we'll sum up savings goal current amounts
    double calculateTotalSavingsFromGoals(const std::vector<SavingsGoal>& goals) const;
};

#endif // ANALYTICS_H
