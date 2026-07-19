#include "MochiSystem.h"
#include <cmath>
#include <algorithm>

MochiSystem::MochiSystem() {
    companions = {
        { "Original Mochi", "A soft, white mochi that loves cozy desks.", "Unlocked from the start", true },
        { "Matcha Mochi", "A peaceful light-green mochi with a tea leaf hat.", "Unlocked after saving Rs. 10,000", false },
        { "Strawberry Mochi", "A cute pink mochi with a sweet strawberry cap.", "Unlocked after staying under budget for one month", false },
        { "Sakura Mochi", "A pink mochi wrapped in a cherry leaf, celebrating spring.", "Unlocked after logging a 3-day savings streak", false },
        { "Golden Mochi", "A sparkling gold mochi wearing a tiny royal crown.", "Unlocked after completing a savings goal", false }
    };
    activeCompanion = "Original Mochi";
    currentMood = MochiMood::HAPPY;
}

void MochiSystem::setActiveCompanion(const std::string& name) {
    if (isUnlocked(name)) {
        activeCompanion = name;
    }
}

bool MochiSystem::isUnlocked(const std::string& name) const {
    for (const auto& mochi : companions) {
        if (mochi.name == name) {
            return mochi.isUnlocked;
        }
    }
    return false;
}

void MochiSystem::unlock(const std::string& name) {
    for (auto& mochi : companions) {
        if (mochi.name == name) {
            mochi.isUnlocked = true;
        }
    }
}

void MochiSystem::checkAndUnlockSavings(double totalSavings) {
    if (totalSavings >= 10000.0) {
        unlock("Matcha Mochi");
    }
}

void MochiSystem::checkAndUnlockBudget(bool stayUnderBudget) {
    if (stayUnderBudget) {
        unlock("Strawberry Mochi");
    }
}

void MochiSystem::checkAndUnlockStreak(int streakDays) {
    if (streakDays >= 3) {
        unlock("Sakura Mochi");
    }
}

void MochiSystem::checkAndUnlockGoalComplete(bool goalCompleted) {
    if (goalCompleted) {
        unlock("Golden Mochi");
    }
}

void MochiSystem::clearUnlocks() {
    for (size_t i = 1; i < companions.size(); ++i) {
        companions[i].isUnlocked = false;
    }
    activeCompanion = "Original Mochi";
}

std::vector<std::string> MochiSystem::getUnlockedNames() const {
    std::vector<std::string> names;
    for (const auto& m : companions) {
        if (m.isUnlocked) {
            names.push_back(m.name);
        }
    }
    return names;
}

void MochiSystem::loadUnlockedNames(const std::vector<std::string>& names) {
    for (const auto& name : names) {
        unlock(name);
    }
}

// Procedural Drawing
void MochiSystem::drawFace(Vector2 pos, float size, MochiMood mood, float time) const {
    DrawCircle(pos.x - size * 0.35f, pos.y + size * 0.05f, size * 0.09f, ColorAlpha(Colors::SakuraPink, 0.7f));
    DrawCircle(pos.x + size * 0.35f, pos.y + size * 0.05f, size * 0.09f, ColorAlpha(Colors::SakuraPink, 0.7f));

    // Blink calculation (every 4 seconds, close eyes for 0.15 seconds)
    bool isBlinking = ((int)(time) % 4 == 0) && ((time - (int)time) < 0.15f);

    if (isBlinking && mood != MochiMood::WORRIED) {
        DrawLineEx({pos.x - size * 0.3f, pos.y - size * 0.08f}, {pos.x - size * 0.15f, pos.y - size * 0.08f}, 2.5f, Colors::CharcoalText);
        DrawLineEx({pos.x + size * 0.15f, pos.y - size * 0.08f}, {pos.x + size * 0.3f, pos.y - size * 0.08f}, 2.5f, Colors::CharcoalText);
    }
    else if (mood == MochiMood::WORRIED) {
        // Worried down-slanting eyes: \  /
        DrawLineEx({pos.x - size * 0.28f, pos.y - size * 0.12f}, {pos.x - size * 0.16f, pos.y - size * 0.04f}, 2.5f, Colors::CharcoalText);
        DrawLineEx({pos.x + size * 0.28f, pos.y - size * 0.12f}, {pos.x + size * 0.16f, pos.y - size * 0.04f}, 2.5f, Colors::CharcoalText);
        
        float sweatY = pos.y - size * 0.2f + sin(time * 8.0f) * 5.0f;
        DrawCircle(pos.x - size * 0.45f, sweatY, size * 0.05f, Colors::SkyCloud);
        DrawTriangle({pos.x - size * 0.45f, sweatY - size * 0.08f}, {pos.x - size * 0.5f, sweatY}, {pos.x - size * 0.4f, sweatY}, Colors::SkyCloud);
    }
    else if (mood == MochiMood::EXCITED) {
        // squinting happy eyes: >  <
        DrawLineEx({pos.x - size * 0.28f, pos.y - size * 0.12f}, {pos.x - size * 0.16f, pos.y - size * 0.08f}, 2.5f, Colors::CharcoalText);
        DrawLineEx({pos.x - size * 0.28f, pos.y - size * 0.04f}, {pos.x - size * 0.16f, pos.y - size * 0.08f}, 2.5f, Colors::CharcoalText);
        
        DrawLineEx({pos.x + size * 0.28f, pos.y - size * 0.08f}, {pos.x + size * 0.16f, pos.y - size * 0.12f}, 2.5f, Colors::CharcoalText);
        DrawLineEx({pos.x + size * 0.28f, pos.y - size * 0.08f}, {pos.x + size * 0.16f, pos.y - size * 0.04f}, 2.5f, Colors::CharcoalText);
    }
    else if (mood == MochiMood::CELEBRATING) {
        // arched happy eyes: ^  ^
        DrawLineEx({pos.x - size * 0.28f, pos.y - size * 0.05f}, {pos.x - size * 0.22f, pos.y - size * 0.13f}, 2.5f, Colors::CharcoalText);
        DrawLineEx({pos.x - size * 0.22f, pos.y - size * 0.13f}, {pos.x - size * 0.16f, pos.y - size * 0.05f}, 2.5f, Colors::CharcoalText);
        
        DrawLineEx({pos.x + size * 0.16f, pos.y - size * 0.05f}, {pos.x + size * 0.22f, pos.y - size * 0.13f}, 2.5f, Colors::CharcoalText);
        DrawLineEx({pos.x + size * 0.22f, pos.y - size * 0.13f}, {pos.x + size * 0.28f, pos.y - size * 0.05f}, 2.5f, Colors::CharcoalText);
    }
    else {
        DrawCircle(pos.x - size * 0.22f, pos.y - size * 0.08f, size * 0.07f, Colors::CharcoalText);
        DrawCircle(pos.x + size * 0.22f, pos.y - size * 0.08f, size * 0.07f, Colors::CharcoalText);
        
        DrawCircle(pos.x - size * 0.24f, pos.y - size * 0.1f, size * 0.025f, WHITE);
        DrawCircle(pos.x + size * 0.2f, pos.y - size * 0.1f, size * 0.025f, WHITE);
    }

    if (mood == MochiMood::HAPPY || mood == MochiMood::CELEBRATING) {
        // Cute open mouth
        DrawCircle(pos.x, pos.y + size * 0.04f, size * 0.06f, { 235, 137, 137, 255 });
    }
    else if (mood == MochiMood::WORRIED) {
        // Wavy or small sad line
        DrawLineEx({pos.x - size * 0.06f, pos.y + size * 0.04f}, {pos.x + size * 0.06f, pos.y + size * 0.04f}, 2.0f, Colors::CharcoalText);
    }
    else {
        // Normal small dot/curve cat mouth
        DrawCircle(pos.x, pos.y + size * 0.02f, size * 0.035f, Colors::CharcoalText);
    }
}

void MochiSystem::drawLeaf(Vector2 pos, float size, float rotation) const {
    // Leaf base
    Color leafColor = { 108, 169, 101, 255 }; // Match matcha green leaf
    DrawEllipse(pos.x, pos.y, size * 0.18f, size * 0.32f, leafColor);
    DrawEllipse(pos.x, pos.y, size * 0.16f, size * 0.3f, { 129, 195, 121, 255 }); // Leaf light side
    
    // Stem
    DrawLineEx({pos.x, pos.y + size * 0.32f}, {pos.x - size * 0.05f, pos.y + size * 0.45f}, 2.0f, leafColor);
}

void MochiSystem::drawStrawberryCap(Vector2 pos, float size) const {
    Color strawberryRed = { 229, 80, 80, 255 };
    Vector2 p1 = { pos.x, pos.y - size * 0.35f };
    Vector2 p2 = { pos.x - size * 0.28f, pos.y + size * 0.05f };
    Vector2 p3 = { pos.x + size * 0.28f, pos.y + size * 0.05f };
    
    // Draw berry triangle
    DrawTriangle(p1, p2, p3, strawberryRed);
    // Round the bottom corners
    DrawCircle(p2.x, p2.y, size * 0.04f, strawberryRed);
    DrawCircle(p3.x, p3.y, size * 0.04f, strawberryRed);
    DrawCircle((p2.x + p3.x) / 2, p2.y, size * 0.04f, strawberryRed);

    // Yellow seeds
    DrawCircle(pos.x - size * 0.08f, pos.y - size * 0.1f, size * 0.015f, Colors::CozyYellow);
    DrawCircle(pos.x + size * 0.08f, pos.y - size * 0.1f, size * 0.015f, Colors::CozyYellow);
    DrawCircle(pos.x, pos.y, size * 0.015f, Colors::CozyYellow);
    DrawCircle(pos.x - size * 0.14f, pos.y, size * 0.015f, Colors::CozyYellow);
    DrawCircle(pos.x + size * 0.14f, pos.y, size * 0.015f, Colors::CozyYellow);

    // Green leaf/stem on top
    DrawCircle(pos.x, pos.y - size * 0.35f, size * 0.04f, { 90, 160, 80, 255 });
    DrawTriangle({pos.x, pos.y - size * 0.35f}, {pos.x - size * 0.08f, pos.y - size * 0.42f}, {pos.x + size * 0.08f, pos.y - size * 0.42f}, { 90, 160, 80, 255 });
}

void MochiSystem::drawCrown(Vector2 pos, float size) const {
    Color gold = { 244, 208, 63, 255 };
    Color darkGold = { 212, 172, 13, 255 };
    
    float w = size * 0.4f;
    float h = size * 0.25f;
    float topY = pos.y - h;
    
    // Draw crown base
    DrawRectangle(pos.x - w * 0.5f, pos.y, w, size * 0.06f, darkGold);
    DrawTriangle({pos.x - w * 0.5f, pos.y}, {pos.x - w * 0.5f, topY}, {pos.x - w * 0.15f, pos.y}, gold);
    DrawTriangle({pos.x - w * 0.15f, pos.y}, {pos.x, topY * 0.9f}, {pos.x + w * 0.15f, pos.y}, gold);
    DrawTriangle({pos.x + w * 0.15f, pos.y}, {pos.x + w * 0.5f, topY}, {pos.x + w * 0.5f, pos.y}, gold);
    
    // Crown tips
    DrawCircle(pos.x - w * 0.5f, topY, size * 0.03f, { 231, 76, 60, 255 }); // Red gem
    DrawCircle(pos.x, topY * 0.9f, size * 0.03f, { 52, 152, 219, 255 }); // Blue gem
    DrawCircle(pos.x + w * 0.5f, topY, size * 0.03f, { 231, 76, 60, 255 }); // Red gem
}

void MochiSystem::drawOriginalMochi(Vector2 pos, float size, MochiMood mood, float time) const {
    DrawEllipse(pos.x, pos.y + size * 0.35f, size * 1.05f, size * 0.25f, Colors::ShadowGray);

    Color bodyColor = Colors::MochiWhite;
    DrawEllipse(pos.x, pos.y, size * 1.05f, size * 0.85f, { 230, 222, 212, 255 }); // Dark shadow outline
    DrawEllipse(pos.x, pos.y - size * 0.02f, size * 1.02f, size * 0.82f, bodyColor); // Inner body

    DrawEllipse(pos.x - size * 0.2f, pos.y - size * 0.25f, size * 0.4f, size * 0.25f, { 255, 255, 255, 120 });

    drawFace({pos.x, pos.y + size * 0.05f}, size, mood, time);
}

void MochiSystem::drawMatchaMochi(Vector2 pos, float size, MochiMood mood, float time) const {
    DrawEllipse(pos.x, pos.y + size * 0.35f, size * 1.05f, size * 0.25f, Colors::ShadowGray);

    // Leaf stem on head (animated sway)
    float leafSway = sin(time * 3.0f) * 10.0f;
    Vector2 leafPos = { pos.x, pos.y - size * 0.42f };
    drawLeaf(leafPos, size, leafSway);

    // Main body (light green)
    Color bodyColor = { 210, 235, 195, 255 };
    Color shadowColor = { 180, 205, 165, 255 };
    DrawEllipse(pos.x, pos.y, size * 1.05f, size * 0.85f, shadowColor);
    DrawEllipse(pos.x, pos.y - size * 0.02f, size * 1.02f, size * 0.82f, bodyColor);

    drawFace({pos.x, pos.y + size * 0.05f}, size, mood, time);
}

void MochiSystem::drawStrawberryMochi(Vector2 pos, float size, MochiMood mood, float time) const {
    DrawEllipse(pos.x, pos.y + size * 0.35f, size * 1.05f, size * 0.25f, Colors::ShadowGray);

    // Main body (Sakura Pink)
    Color bodyColor = { 255, 232, 232, 255 };
    Color shadowColor = { 235, 205, 205, 255 };
    DrawEllipse(pos.x, pos.y, size * 1.05f, size * 0.85f, shadowColor);
    DrawEllipse(pos.x, pos.y - size * 0.02f, size * 1.02f, size * 0.82f, bodyColor);

    // Strawberry Cap on top
    Vector2 capPos = { pos.x, pos.y - size * 0.48f };
    drawStrawberryCap(capPos, size);

    drawFace({pos.x, pos.y + size * 0.05f}, size, mood, time);
}

void MochiSystem::drawSakuraMochi(Vector2 pos, float size, MochiMood mood, float time) const {
    DrawEllipse(pos.x, pos.y + size * 0.35f, size * 1.05f, size * 0.25f, Colors::ShadowGray);

    // Cherry leaf wrapping behind
    Color leafColor = { 135, 175, 120, 255 };
    DrawEllipse(pos.x + size * 0.3f, pos.y + size * 0.1f, size * 0.6f, size * 0.4f, leafColor);

    // Main body (Pink)
    Color bodyColor = { 255, 218, 224, 255 };
    Color shadowColor = { 235, 190, 198, 255 };
    DrawEllipse(pos.x, pos.y, size * 1.05f, size * 0.85f, shadowColor);
    DrawEllipse(pos.x, pos.y - size * 0.02f, size * 1.02f, size * 0.82f, bodyColor);

    // Cherry leaf wrapping in front
    DrawCircleSector({pos.x + size * 0.35f, pos.y + size * 0.1f}, size * 0.45f, 40, 190, 16, leafColor);
    // Draw leaf stem vein lines
    DrawLineEx({pos.x + size * 0.15f, pos.y + size * 0.2f}, {pos.x + size * 0.5f, pos.y + size * 0.05f}, 1.5f, { 110, 150, 95, 255 });

    drawFace({pos.x, pos.y + size * 0.05f}, size, mood, time);
}

void MochiSystem::drawGoldenMochi(Vector2 pos, float size, MochiMood mood, float time) const {
    DrawEllipse(pos.x, pos.y + size * 0.35f, size * 1.05f, size * 0.25f, Colors::ShadowGray);

    // Main body (Golden Yellow)
    Color bodyColor = { 253, 235, 149, 255 };
    Color shadowColor = { 220, 190, 110, 255 };
    DrawEllipse(pos.x, pos.y, size * 1.05f, size * 0.85f, shadowColor);
    DrawEllipse(pos.x, pos.y - size * 0.02f, size * 1.02f, size * 0.82f, bodyColor);

    // Crown on head
    Vector2 crownPos = { pos.x, pos.y - size * 0.44f };
    drawCrown(crownPos, size);

    // Twinkling stars particle effects around Golden Mochi
    float starTime = time * 2.0f;
    Vector2 star1 = { pos.x - size * 0.8f, pos.y - size * 0.2f + (float)sin(starTime) * 10.0f };
    Vector2 star2 = { pos.x + size * 0.8f, pos.y - size * 0.6f + (float)cos(starTime) * 10.0f };
    
    // Draw small diamond-like stars
    float starScale1 = abs(sin(starTime)) * 0.15f;
    float starScale2 = abs(cos(starTime)) * 0.15f;
    if (starScale1 > 0.02f) {
        DrawTriangle(star1, {star1.x - size * starScale1, star1.y}, {star1.x + size * starScale1, star1.y}, Colors::CozyYellow);
        DrawTriangle(star1, {star1.x, star1.y - size * starScale1}, {star1.x, star1.y + size * starScale1}, Colors::CozyYellow);
    }
    if (starScale2 > 0.02f) {
        DrawTriangle(star2, {star2.x - size * starScale2, star2.y}, {star2.x + size * starScale2, star2.y}, Colors::CozyYellow);
        DrawTriangle(star2, {star2.x, star2.y - size * starScale2}, {star2.x, star2.y + size * starScale2}, Colors::CozyYellow);
    }

    drawFace({pos.x, pos.y + size * 0.05f}, size, mood, time);
}

void MochiSystem::drawCompanion(const std::string& name, Vector2 pos, float size, float time) const {
    // Dynamic offsets based on mood & animations
    float bobOffset = sin(time * 3.0f) * (size * 0.04f); // Breathing
    float hopOffset = 0.0f;
    float rotation = 0.0f;
    Vector2 scale = { 1.0f, 1.0f };

    if (currentMood == MochiMood::HAPPY) {
        // High bouncy hop
        hopOffset = abs(sin(time * 5.0f)) * (size * 0.25f);
    }
    else if (currentMood == MochiMood::CELEBRATING) {
        // Super energetic hop
        hopOffset = abs(sin(time * 7.0f)) * (size * 0.35f);
    }
    else if (currentMood == MochiMood::WORRIED) {
        // Shivering side-to-side rotation
        rotation = sin(time * 24.0f) * 4.0f; // degrees
    }
    else if (currentMood == MochiMood::EXCITED) {
        // Squeezing squash-and-stretch
        scale.y = 1.0f + sin(time * 9.0f) * 0.08f;
        scale.x = 1.0f - sin(time * 9.0f) * 0.08f;
    }

    // Combine offsets into dynamic draw position
    Vector2 drawPos = { pos.x, pos.y + bobOffset - hopOffset };
    float finalSize = size * scale.x;

    // Apply rotation transformation if worried
    if (rotation != 0.0f) {
        // Simple rotation simulator or standard raylib DrawRotated? 
        // We can just add a side-to-side horizontal wobble to drawPos for simplicity, which is less heavy and works great!
        drawPos.x += rotation * 1.5f;
    }

    if (name == "Matcha Mochi") {
        drawMatchaMochi(drawPos, finalSize, currentMood, time);
    }
    else if (name == "Strawberry Mochi") {
        drawStrawberryMochi(drawPos, finalSize, currentMood, time);
    }
    else if (name == "Sakura Mochi") {
        drawSakuraMochi(drawPos, finalSize, currentMood, time);
    }
    else if (name == "Golden Mochi") {
        drawGoldenMochi(drawPos, finalSize, currentMood, time);
    }
    else {
        drawOriginalMochi(drawPos, finalSize, currentMood, time);
    }
}
