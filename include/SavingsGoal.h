#ifndef SAVINGS_GOAL_H
#define SAVINGS_GOAL_H

#include <string>

class SavingsGoal {
private:
    std::string name;
    double targetAmount;
    double currentAmount;

public:
    SavingsGoal() : name(""), targetAmount(0.0), currentAmount(0.0) {}
    SavingsGoal(const std::string& name, double target, double current = 0.0);

    std::string getName() const { return name; }
    double getTarget() const { return targetAmount; }
    double getCurrent() const { return currentAmount; }
    
    void setName(const std::string& newName) { name = newName; }
    void setTarget(double target) { targetAmount = target; }
    void setCurrent(double current) { currentAmount = current; }

    void deposit(double amount);
    void withdraw(double amount);
    double getProgress() const; // percentage 0.0 to 100.0
    double getRemaining() const;
    bool isCompleted() const;
};

#endif // SAVINGS_GOAL_H
