#include "FinanceManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>

struct Date {
    int y, m, d;
};

static Date parseDate(const std::string& dStr) {
    Date d{2000, 1, 1};
    if (dStr.size() == 10 && dStr[4] == '-' && dStr[7] == '-') {
        try {
            d.y = std::stoi(dStr.substr(0, 4));
            d.m = std::stoi(dStr.substr(5, 2));
            d.d = std::stoi(dStr.substr(8, 2));
        } catch(...) {}
    }
    return d;
}

static int daysSince2000(Date date) {
    const int months[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int days = date.d;
    
    // Add days for years
    for (int yr = 2000; yr < date.y; ++yr) {
        bool isLeap = (yr % 4 == 0 && yr % 100 != 0) || (yr % 400 == 0);
        days += isLeap ? 366 : 365;
    }
    
    // Add days for months in the current year
    for (int m = 1; m < date.m; ++m) {
        days += months[m];
        if (m == 2) {
            bool isLeap = (date.y % 4 == 0 && date.y % 100 != 0) || (date.y % 400 == 0);
            if (isLeap) days += 1;
        }
    }
    
    return days;
}

FinanceManager::FinanceManager(const std::string& dataFilePath)
    : fileManager(dataFilePath), balance(0.0), streakCount(0), lastLogDate(""), nextTxId(1) {
    load();
}

void FinanceManager::updateBalance() {
    double income = analyticsEngine.calculateTotalIncome(transactions);
    double expense = analyticsEngine.calculateTotalExpense(transactions);
    balance = income - expense;
}

void FinanceManager::runAchievementChecks() {
    double goalSavings = analyticsEngine.calculateTotalSavingsFromGoals(goals);
    mochiSystem.checkAndUnlockSavings(goalSavings);

    bool stayUnderBudget = true;
    auto categoryTotals = analyticsEngine.calculateCategoryTotals(transactions);
    for (const auto& pair : budgetManager.getAllBudgets()) {
        auto it = categoryTotals.find(pair.first);
        if (it != categoryTotals.end() && it->second > pair.second) {
            stayUnderBudget = false;
            break;
        }
    }
    if (!budgetManager.getAllBudgets().empty()) {
        mochiSystem.checkAndUnlockBudget(stayUnderBudget);
    }

    mochiSystem.checkAndUnlockStreak(streakCount);

    bool goalCompleted = false;
    for (const auto& g : goals) {
        if (g.isCompleted()) {
            goalCompleted = true;
            break;
        }
    }
    mochiSystem.checkAndUnlockGoalComplete(goalCompleted);

    // Dynamic Active Mochi Mood reacts to balance/budgets
    bool overSpentAny = false;
    for (const auto& pair : budgetManager.getAllBudgets()) {
        auto it = categoryTotals.find(pair.first);
        if (it != categoryTotals.end() && it->second > pair.second) {
            overSpentAny = true;
            break;
        }
    }

    if (goalCompleted) {
        mochiSystem.setMood(MochiMood::CELEBRATING);
    } else if (overSpentAny) {
        mochiSystem.setMood(MochiMood::WORRIED);
    } else if (streakCount >= 3) {
        mochiSystem.setMood(MochiMood::EXCITED);
    } else {
        mochiSystem.setMood(MochiMood::HAPPY);
    }
}

void FinanceManager::addTransaction(double amount, TransactionType type, const std::string& category,
                                    const std::string& merchant, const std::string& date, const std::string& notes) {
    if (amount <= 0.0) return;
    
    Transaction t{nextTxId++, amount, type, category, merchant, date, notes};
    transactions.push_back(t);
    
    updateBalance();
    logActivityToday(date);
    runAchievementChecks();
    save();
}

void FinanceManager::deleteTransaction(int id) {
    auto it = std::find_if(transactions.begin(), transactions.end(), [id](const Transaction& t) {
        return t.id == id;
    });
    
    if (it != transactions.end()) {
        transactions.erase(it);
        updateBalance();
        runAchievementChecks();
        save();
    }
}

void FinanceManager::setBudget(const std::string& category, double limit) {
    budgetManager.setBudget(category, limit);
    runAchievementChecks();
    save();
}

void FinanceManager::removeBudget(const std::string& category) {
    budgetManager.removeBudget(category);
    runAchievementChecks();
    save();
}

void FinanceManager::addSavingsGoal(const std::string& name, double target, double current) {
    if (target <= 0.0) return;
    goals.emplace_back(name, target, current);
    runAchievementChecks();
    save();
}

void FinanceManager::depositToGoal(size_t index, double amount) {
    if (index < goals.size() && amount > 0.0) {
        // Withdraw from current balance and deposit to goal
        // In this implementation, the balance represents total income minus expense,
        // and goal funds are accumulated savings.
        goals[index].deposit(amount);
        runAchievementChecks();
        save();
    }
}

void FinanceManager::withdrawFromGoal(size_t index, double amount) {
    if (index < goals.size() && amount > 0.0) {
        goals[index].withdraw(amount);
        runAchievementChecks();
        save();
    }
}

void FinanceManager::deleteSavingsGoal(size_t index) {
    if (index < goals.size()) {
        goals.erase(goals.begin() + index);
        runAchievementChecks();
        save();
    }
}

void FinanceManager::logActivityToday(const std::string& todayDate) {
    if (todayDate.size() != 10) return;

    if (lastLogDate.empty()) {
        streakCount = 1;
        lastLogDate = todayDate;
    } else {
        Date last = parseDate(lastLogDate);
        Date today = parseDate(todayDate);
        int dayDiff = daysSince2000(today) - daysSince2000(last);
        
        if (dayDiff == 1) {
            streakCount++;
            lastLogDate = todayDate;
        } else if (dayDiff > 1) {
            streakCount = 1;
            lastLogDate = todayDate;
        } else if (dayDiff < 0) {
            // Logging past transaction, streak doesn't reset but date stays as is
        }
    }
}

void FinanceManager::save() {
    fileManager.saveState(
        mochiSystem.getActiveCompanion(),
        streakCount,
        lastLogDate,
        mochiSystem.getUnlockedNames(),
        transactions,
        budgetManager.getAllBudgets(),
        goals
    );
}

void FinanceManager::load() {
    std::string activeMochi = "Original Mochi";
    std::vector<std::string> unlockedNames;
    
    bool loaded = fileManager.loadState(
        activeMochi,
        streakCount,
        lastLogDate,
        unlockedNames,
        transactions,
        budgetManager,
        goals
    );
    
    if (loaded) {
        mochiSystem.loadUnlockedNames(unlockedNames);
        mochiSystem.setActiveCompanion(activeMochi);
        
        // Find next transactions ID
        int maxId = 0;
        for (const auto& t : transactions) {
            if (t.id > maxId) maxId = t.id;
        }
        nextTxId = maxId + 1;
    } else {
        // Save initial state if file doesn't exist
        save();
    }
    updateBalance();
    runAchievementChecks();
}

void FinanceManager::resetAll() {
    transactions.clear();
    budgetManager.clear();
    goals.clear();
    mochiSystem.clearUnlocks();
    balance = 0.0;
    streakCount = 0;
    lastLogDate = "";
    nextTxId = 1;
    save();
}
