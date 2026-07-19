#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include "Transaction.h"
#include "BudgetManager.h"
#include "SavingsGoal.h"
#include "MochiSystem.h"

class FileManager {
private:
    std::string filePath;

    static std::string trim(const std::string& str);
    static std::string extractStringValue(const std::string& line, const std::string& key);
    static double extractDoubleValue(const std::string& line, const std::string& key);
    static int extractIntValue(const std::string& line, const std::string& key);

public:
    explicit FileManager(std::string path);

    bool saveState(
        const std::string& activeMochi,
        int streakCount,
        const std::string& lastLogDate,
        const std::vector<std::string>& unlockedMochis,
        const std::vector<Transaction>& transactions,
        const std::unordered_map<std::string, double>& budgets,
        const std::vector<SavingsGoal>& goals
    );

    bool loadState(
        std::string& activeMochi,
        int& streakCount,
        std::string& lastLogDate,
        std::vector<std::string>& unlockedMochis,
        std::vector<Transaction>& transactions,
        BudgetManager& budgetManager,
        std::vector<SavingsGoal>& goals
    );
};

#endif // FILE_MANAGER_H
