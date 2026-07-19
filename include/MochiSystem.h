#ifndef MOCHI_SYSTEM_H
#define MOCHI_SYSTEM_H

#include <string>
#include <vector>
#include <set>
#include "Common.h"

struct Mochi {
    std::string name;
    std::string description;
    std::string unlockCondition;
    bool isUnlocked;
};

class MochiSystem {
private:
    std::vector<Mochi> companions;
    std::string activeCompanion;
    MochiMood currentMood;

    // Helper draw routines for procedural rendering
    void drawOriginalMochi(Vector2 pos, float size, MochiMood mood, float time) const;
    void drawMatchaMochi(Vector2 pos, float size, MochiMood mood, float time) const;
    void drawStrawberryMochi(Vector2 pos, float size, MochiMood mood, float time) const;
    void drawSakuraMochi(Vector2 pos, float size, MochiMood mood, float time) const;
    void drawGoldenMochi(Vector2 pos, float size, MochiMood mood, float time) const;
    
    // Draw decorations
    void drawLeaf(Vector2 pos, float size, float rotation) const;
    void drawStrawberryCap(Vector2 pos, float size) const;
    void drawCrown(Vector2 pos, float size) const;
    void drawFace(Vector2 pos, float size, MochiMood mood, float time) const;

public:
    MochiSystem();

    const std::vector<Mochi>& getCompanions() const { return companions; }
    std::string getActiveCompanion() const { return activeCompanion; }
    void setActiveCompanion(const std::string& name);
    MochiMood getMood() const { return currentMood; }
    void setMood(MochiMood mood) { currentMood = mood; }

    bool isUnlocked(const std::string& name) const;
    void unlock(const std::string& name);
    void checkAndUnlockSavings(double totalSavings);
    void checkAndUnlockBudget(bool stayUnderBudget);
    void checkAndUnlockStreak(int streakDays);
    void checkAndUnlockGoalComplete(bool goalCompleted);

    // Main draw caller
    void drawCompanion(const std::string& name, Vector2 pos, float size, float time) const;
    
    void clearUnlocks();
    std::vector<std::string> getUnlockedNames() const;
    void loadUnlockedNames(const std::vector<std::string>& names);
};

#endif // MOCHI_SYSTEM_H
