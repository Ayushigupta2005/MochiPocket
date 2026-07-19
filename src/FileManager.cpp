#include "FileManager.h"
#include <fstream>
#include <iostream>

enum class ParserState {
    ROOT,
    UNLOCKED_MOCHIS,
    TRANSACTIONS,
    BUDGETS,
    GOALS
};

FileManager::FileManager(std::string path) : filePath(path) {}

std::string FileManager::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string FileManager::extractStringValue(const std::string& line, const std::string& key) {
    size_t keyPos = line.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return "";
    size_t colonPos = line.find(":", keyPos);
    if (colonPos == std::string::npos) return "";
    size_t firstQuote = line.find("\"", colonPos);
    if (firstQuote == std::string::npos) return "";
    size_t secondQuote = line.find("\"", firstQuote + 1);
    if (secondQuote == std::string::npos) return "";
    return line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

double FileManager::extractDoubleValue(const std::string& line, const std::string& key) {
    size_t keyPos = line.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return 0.0;
    size_t colonPos = line.find(":", keyPos);
    if (colonPos == std::string::npos) return 0.0;
    
    std::string valStr = line.substr(colonPos + 1);
    size_t commaPos = valStr.find(",");
    if (commaPos != std::string::npos) {
        valStr = valStr.substr(0, commaPos);
    }
    valStr = trim(valStr);
    
    try {
        return std::stod(valStr);
    } catch (...) {
        return 0.0;
    }
}

int FileManager::extractIntValue(const std::string& line, const std::string& key) {
    size_t keyPos = line.find("\"" + key + "\"");
    if (keyPos == std::string::npos) return 0;
    size_t colonPos = line.find(":", keyPos);
    if (colonPos == std::string::npos) return 0;
    
    std::string valStr = line.substr(colonPos + 1);
    size_t commaPos = valStr.find(",");
    if (commaPos != std::string::npos) {
        valStr = valStr.substr(0, commaPos);
    }
    valStr = trim(valStr);
    
    try {
        return std::stoi(valStr);
    } catch (...) {
        return 0;
    }
}

bool FileManager::saveState(
    const std::string& activeMochi,
    int streakCount,
    const std::string& lastLogDate,
    const std::vector<std::string>& unlockedMochis,
    const std::vector<Transaction>& transactions,
    const std::unordered_map<std::string, double>& budgets,
    const std::vector<SavingsGoal>& goals
) {
    std::ofstream file(filePath);
    if (!file.is_open()) return false;

    file << "{\n";
    file << "  \"active_mochi\": \"" << activeMochi << "\",\n";
    file << "  \"streak_count\": " << streakCount << ",\n";
    file << "  \"last_log_date\": \"" << lastLogDate << "\",\n";

    file << "  \"unlocked_mochis\": [\n";
    for (size_t i = 0; i < unlockedMochis.size(); ++i) {
        file << "    \"" << unlockedMochis[i] << "\"";
        if (i + 1 < unlockedMochis.size()) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    file << "  \"transactions\": [\n";
    for (size_t i = 0; i < transactions.size(); ++i) {
        const auto& t = transactions[i];
        file << "    {\n";
        file << "      \"id\": " << t.id << ",\n";
        file << "      \"amount\": " << t.amount << ",\n";
        file << "      \"type\": \"" << (t.type == TransactionType::INCOME ? "INCOME" : "EXPENSE") << "\",\n";
        file << "      \"category\": \"" << t.category << "\",\n";
        file << "      \"merchant\": \"" << t.merchant << "\",\n";
        file << "      \"date\": \"" << t.date << "\",\n";
        file << "      \"notes\": \"" << t.notes << "\"\n";
        file << "    }";
        if (i + 1 < transactions.size()) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    file << "  \"budgets\": [\n";
    size_t budgetCount = budgets.size();
    size_t budgetIndex = 0;
    for (const auto& pair : budgets) {
        file << "    {\n";
        file << "      \"category\": \"" << pair.first << "\",\n";
        file << "      \"limit\": " << pair.second << "\n";
        file << "    }";
        if (++budgetIndex < budgetCount) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    file << "  \"savings_goals\": [\n";
    for (size_t i = 0; i < goals.size(); ++i) {
        const auto& g = goals[i];
        file << "    {\n";
        file << "      \"name\": \"" << g.getName() << "\",\n";
        file << "      \"target\": " << g.getTarget() << ",\n";
        file << "      \"current\": " << g.getCurrent() << "\n";
        file << "    }";
        if (i + 1 < goals.size()) file << ",";
        file << "\n";
    }
    file << "  ]\n";
    file << "}\n";

    return true;
}

bool FileManager::loadState(
    std::string& activeMochi,
    int& streakCount,
    std::string& lastLogDate,
    std::vector<std::string>& unlockedMochis,
    std::vector<Transaction>& transactions,
    BudgetManager& budgetManager,
    std::vector<SavingsGoal>& goals
) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    ParserState state = ParserState::ROOT;
    Transaction tempTx{};
    std::string tempCategory = "";
    double tempLimit = 0.0;
    std::string tempGoalName = "";
    double tempGoalTarget = 0.0;
    double tempGoalCurrent = 0.0;

    unlockedMochis.clear();
    transactions.clear();
    budgetManager.clear();
    goals.clear();

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (state == ParserState::ROOT) {
            if (line.find("\"active_mochi\"") != std::string::npos) {
                activeMochi = extractStringValue(line, "active_mochi");
            } else if (line.find("\"streak_count\"") != std::string::npos) {
                streakCount = extractIntValue(line, "streak_count");
            } else if (line.find("\"last_log_date\"") != std::string::npos) {
                lastLogDate = extractStringValue(line, "last_log_date");
            } else if (line.find("\"unlocked_mochis\"") != std::string::npos) {
                state = ParserState::UNLOCKED_MOCHIS;
            } else if (line.find("\"transactions\"") != std::string::npos) {
                state = ParserState::TRANSACTIONS;
            } else if (line.find("\"budgets\"") != std::string::npos) {
                state = ParserState::BUDGETS;
            } else if (line.find("\"savings_goals\"") != std::string::npos) {
                state = ParserState::GOALS;
            }
        }
        else if (state == ParserState::UNLOCKED_MOCHIS) {
            if (line.find("]") != std::string::npos) {
                state = ParserState::ROOT;
            } else {
                size_t firstQuote = line.find("\"");
                size_t lastQuote = line.rfind("\"");
                if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote < lastQuote) {
                    std::string name = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    unlockedMochis.push_back(name);
                }
            }
        }
        else if (state == ParserState::TRANSACTIONS) {
            if (line.find("]") != std::string::npos) {
                state = ParserState::ROOT;
            } else if (line.find("{") != std::string::npos) {
                tempTx = Transaction{};
            } else if (line.find("}") != std::string::npos) {
                transactions.push_back(tempTx);
            } else {
                if (line.find("\"id\"") != std::string::npos) {
                    tempTx.id = extractIntValue(line, "id");
                } else if (line.find("\"amount\"") != std::string::npos) {
                    tempTx.amount = extractDoubleValue(line, "amount");
                } else if (line.find("\"type\"") != std::string::npos) {
                    std::string typeStr = extractStringValue(line, "type");
                    tempTx.type = (typeStr == "INCOME" ? TransactionType::INCOME : TransactionType::EXPENSE);
                } else if (line.find("\"category\"") != std::string::npos) {
                    tempTx.category = extractStringValue(line, "category");
                } else if (line.find("\"merchant\"") != std::string::npos) {
                    tempTx.merchant = extractStringValue(line, "merchant");
                } else if (line.find("\"date\"") != std::string::npos) {
                    tempTx.date = extractStringValue(line, "date");
                } else if (line.find("\"notes\"") != std::string::npos) {
                    tempTx.notes = extractStringValue(line, "notes");
                }
            }
        }
        else if (state == ParserState::BUDGETS) {
            if (line.find("]") != std::string::npos) {
                state = ParserState::ROOT;
            } else if (line.find("{") != std::string::npos) {
                tempCategory = "";
                tempLimit = 0.0;
            } else if (line.find("}") != std::string::npos) {
                budgetManager.setBudget(tempCategory, tempLimit);
            } else {
                if (line.find("\"category\"") != std::string::npos) {
                    tempCategory = extractStringValue(line, "category");
                } else if (line.find("\"limit\"") != std::string::npos) {
                    tempLimit = extractDoubleValue(line, "limit");
                }
            }
        }
        else if (state == ParserState::GOALS) {
            if (line.find("]") != std::string::npos) {
                state = ParserState::ROOT;
            } else if (line.find("{") != std::string::npos) {
                tempGoalName = "";
                tempGoalTarget = 0.0;
                tempGoalCurrent = 0.0;
            } else if (line.find("}") != std::string::npos) {
                goals.emplace_back(tempGoalName, tempGoalTarget, tempGoalCurrent);
            } else {
                if (line.find("\"name\"") != std::string::npos) {
                    tempGoalName = extractStringValue(line, "name");
                } else if (line.find("\"target\"") != std::string::npos) {
                    tempGoalTarget = extractDoubleValue(line, "target");
                } else if (line.find("\"current\"") != std::string::npos) {
                    tempGoalCurrent = extractDoubleValue(line, "current");
                }
            }
        }
    }
    return true;
}
