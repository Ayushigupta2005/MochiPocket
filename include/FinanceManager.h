#ifndef FINANCE_MANAGER_H
#define FINANCE_MANAGER_H

#include <vector>
#include <string>
#include "Transaction.h"
#include "BudgetManager.h"
#include "SavingsGoal.h"
#include "MochiSystem.h"
#include "Analytics.h"
#include "FileManager.h"

class FinanceManager {
private:
    std::vector<Transaction> transactions;
    BudgetManager budgetManager;
    std::vector<SavingsGoal> goals;
    MochiSystem mochiSystem;
    AnalyticsEngine analyticsEngine;
    FileManager fileManager;

    double balance;
    int streakCount;
    std::string lastLogDate;
    int nextTxId;

    void updateBalance();
    void runAchievementChecks();

public:
    explicit FinanceManager(const std::string& dataFilePath);

    double getBalance() const { return balance; }
    int getStreakCount() const { return streakCount; }
    std::string getLastLogDate() const { return lastLogDate; }
    const std::vector<Transaction>& getTransactions() const { return transactions; }
    const BudgetManager& getBudgetManager() const { return budgetManager; }
    BudgetManager& getBudgetManagerModifier() { return budgetManager; }
    const std::vector<SavingsGoal>& getGoals() const { return goals; }
    std::vector<SavingsGoal>& getGoalsModifier() { return goals; }
    const MochiSystem& getMochiSystem() const { return mochiSystem; }
    MochiSystem& getMochiSystemModifier() { return mochiSystem; }
    const AnalyticsEngine& getAnalyticsEngine() const { return analyticsEngine; }

    void addTransaction(double amount, TransactionType type, const std::string& category,
                        const std::string& merchant, const std::string& date, const std::string& notes);
    void deleteTransaction(int id);
    
    void setBudget(const std::string& category, double limit);
    void removeBudget(const std::string& category);

    void addSavingsGoal(const std::string& name, double target, double current = 0.0);
    void depositToGoal(size_t index, double amount);
    void withdrawFromGoal(size_t index, double amount);
    void deleteSavingsGoal(size_t index);

    void logActivityToday(const std::string& todayDate);
    
    void save();
    void load();
    void resetAll();
};

#endif // FINANCE_MANAGER_H
