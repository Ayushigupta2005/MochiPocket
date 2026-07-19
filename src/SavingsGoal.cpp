#include "SavingsGoal.h"

SavingsGoal::SavingsGoal(const std::string& name, double target, double current)
    : name(name), targetAmount(target), currentAmount(current) {
    if (targetAmount < 0.0) targetAmount = 0.0;
    if (currentAmount < 0.0) currentAmount = 0.0;
}

void SavingsGoal::deposit(double amount) {
    if (amount > 0.0) {
        currentAmount += amount;
    }
}

void SavingsGoal::withdraw(double amount) {
    if (amount > 0.0) {
        currentAmount -= amount;
        if (currentAmount < 0.0) {
            currentAmount = 0.0;
        }
    }
}

double SavingsGoal::getProgress() const {
    if (targetAmount <= 0.0) return 0.0;
    double percentage = (currentAmount / targetAmount) * 100.0;
    return (percentage > 100.0) ? 100.0 : percentage;
}

double SavingsGoal::getRemaining() const {
    double diff = targetAmount - currentAmount;
    return (diff < 0.0) ? 0.0 : diff;
}

bool SavingsGoal::isCompleted() const {
    return currentAmount >= targetAmount && targetAmount > 0.0;
}
