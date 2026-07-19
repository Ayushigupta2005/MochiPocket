#include "Analytics.h"
#include <numeric>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>

double AnalyticsEngine::calculateTotalIncome(const std::vector<Transaction>& transactions) const {
    return std::accumulate(transactions.begin(), transactions.end(), 0.0,
        [](double sum, const Transaction& t) {
            return t.type == TransactionType::INCOME ? sum + t.amount : sum;
        });
}

double AnalyticsEngine::calculateTotalExpense(const std::vector<Transaction>& transactions) const {
    return std::accumulate(transactions.begin(), transactions.end(), 0.0,
        [](double sum, const Transaction& t) {
            return t.type == TransactionType::EXPENSE ? sum + t.amount : sum;
        });
}

std::unordered_map<std::string, double> AnalyticsEngine::calculateCategoryTotals(const std::vector<Transaction>& transactions) const {
    std::unordered_map<std::string, double> totals;
    for (const auto& t : transactions) {
        if (t.type == TransactionType::EXPENSE) {
            totals[t.category] += t.amount;
        }
    }
    return totals;
}

Transaction AnalyticsEngine::getHighestExpense(const std::vector<Transaction>& transactions) const {
    Transaction highest{};
    highest.amount = 0.0;
    for (const auto& t : transactions) {
        if (t.type == TransactionType::EXPENSE && t.amount > highest.amount) {
            highest = t;
        }
    }
    return highest;
}

std::string AnalyticsEngine::getMostVisitedMerchant(const std::vector<Transaction>& transactions) const {
    if (transactions.empty()) return "None";
    
    std::unordered_map<std::string, int> merchantCounts;
    for (const auto& t : transactions) {
        if (t.type == TransactionType::EXPENSE && !t.merchant.empty()) {
            merchantCounts[t.merchant]++;
        }
    }
    
    std::string mostVisited = "None";
    int maxCount = 0;
    for (const auto& pair : merchantCounts) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostVisited = pair.first;
        }
    }
    return mostVisited;
}

double AnalyticsEngine::calculateAverageDailySpending(const std::vector<Transaction>& transactions) const {
    std::set<std::string> uniqueDays;
    double totalExpense = 0.0;
    
    for (const auto& t : transactions) {
        if (t.type == TransactionType::EXPENSE) {
            totalExpense += t.amount;
            uniqueDays.insert(t.date);
        }
    }
    
    if (uniqueDays.empty()) return 0.0;
    return totalExpense / uniqueDays.size();
}

double AnalyticsEngine::calculateTotalSavingsFromGoals(const std::vector<SavingsGoal>& goals) const {
    return std::accumulate(goals.begin(), goals.end(), 0.0,
        [](double sum, const SavingsGoal& g) {
            return sum + g.getCurrent();
        });
}
