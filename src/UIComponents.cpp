#include "UIComponents.h"
#include "Common.h"
#include <cmath>
#include <algorithm>
#include <iostream>

void UI::DrawCard(Rectangle rec, Color bgColor, Color borderColor, float radius, float borderThickness) {
    // Soft card base
    DrawRectangleRounded(rec, radius, 4, bgColor);
    
    // Card border
    if (borderThickness > 0.0f) {
        DrawRectangleRoundedLines(rec, radius, 4, borderThickness, borderColor);
    }
}

void UI::DrawShadow(Rectangle rec, float size) {
    Rectangle shadowRec = { rec.x + size * 0.5f, rec.y + size, rec.width, rec.height };
    DrawRectangleRounded(shadowRec, 0.08f, 4, Colors::ShadowGray);
}

bool UI::DrawButton(Rectangle rec, const char* text, Color baseColor, Color textColor, bool isHovered, int fontSize) {
    Color drawColor = baseColor;
    if (isHovered) {
        // Lighten slightly on hover
        drawColor.r = (unsigned char)std::min(drawColor.r + 20, 255);
        drawColor.g = (unsigned char)std::min(drawColor.g + 20, 255);
        drawColor.b = (unsigned char)std::min(drawColor.b + 20, 255);
    }

    DrawShadow(rec, 3.0f);
    DrawCard(rec, drawColor, Colors::SoftBorder, 0.3f, 1.0f);

    int textW = MeasureText(text, fontSize);
    int textX = (int)(rec.x + (rec.width - textW) * 0.5f);
    int textY = (int)(rec.y + (rec.height - fontSize) * 0.5f);
    
    DrawText(text, textX, textY, fontSize, textColor);

    return isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void UI::DrawInputField(Rectangle rec, InputField& field, const char* placeholder, int fontSize) {
    DrawCard(rec, WHITE, field.active ? Colors::SakuraPink : Colors::SoftBorder, 0.2f, 1.5f);
    
    // Draw placeholder or value
    if (field.length == 0) {
        DrawText(placeholder, (int)(rec.x + 10), (int)(rec.y + (rec.height - fontSize) * 0.5f), fontSize, Colors::SlateMuted);
    } else {
        DrawText(field.buffer, (int)(rec.x + 10), (int)(rec.y + (rec.height - fontSize) * 0.5f), fontSize, Colors::CharcoalText);
    }
    
    // Draw cursor
    if (field.active) {
        float cursorX = rec.x + 12 + MeasureText(field.buffer, fontSize);
        if (((int)(GetTime() * 2.0f) % 2) == 0) {
            DrawLineEx({cursorX, rec.y + 6}, {cursorX, rec.y + rec.height - 6}, 2.0f, Colors::CharcoalText);
        }
    }

    // Toggle active on click
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(GetMousePosition(), rec)) {
            field.active = true;
        } else {
            field.active = false;
        }
    }
}

bool UI::DrawDropdown(Rectangle rec, const std::vector<std::string>& items, int& activeIndex, bool& isOpen, int fontSize) {
    bool selected = false;
    
    // Draw current selected option
    std::string text = items.empty() ? "None" : items[activeIndex];
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), rec);
    
    DrawCard(rec, WHITE, isOpen ? Colors::SakuraPink : Colors::SoftBorder, 0.2f, 1.5f);
    DrawText(text.c_str(), (int)(rec.x + 10), (int)(rec.y + (rec.height - fontSize) * 0.5f), fontSize, Colors::CharcoalText);
    
    // Draw arrow icon (procedural)
    float arrowX = rec.x + rec.width - 25;
    float arrowY = rec.y + rec.height * 0.5f;
    if (isOpen) {
        DrawTriangle({arrowX, arrowY - 4}, {arrowX - 6, arrowY + 2}, {arrowX + 6, arrowY + 2}, Colors::SlateMuted);
    } else {
        DrawTriangle({arrowX, arrowY + 2}, {arrowX + 6, arrowY - 4}, {arrowX - 6, arrowY - 4}, Colors::SlateMuted);
    }
    
    if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        isOpen = !isOpen;
    }
    
    // Draw elements when open
    if (isOpen) {
        float itemH = rec.height;
        float totalH = items.size() * itemH;
        Rectangle dropRec = { rec.x, rec.y + rec.height + 2, rec.width, totalH };
        
        DrawCard(dropRec, WHITE, Colors::SoftBorder, 0.1f, 1.5f);
        
        for (size_t i = 0; i < items.size(); ++i) {
            Rectangle itemRec = { rec.x, rec.y + rec.height + 2 + i * itemH, rec.width, itemH };
            bool itemHover = CheckCollisionPointRec(GetMousePosition(), itemRec);
            
            if (itemHover) {
                DrawRectangleRec(itemRec, ColorAlpha(Colors::SakuraPink, 0.3f));
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    activeIndex = (int)i;
                    isOpen = false;
                    selected = true;
                }
            }
            
            DrawText(items[i].c_str(), (int)(itemRec.x + 10), (int)(itemRec.y + (itemH - fontSize) * 0.5f), fontSize, Colors::CharcoalText);
        }
    }
    
    return selected;
}

void UI::DrawProgressBar(Rectangle rec, float progress, Color fillColor, Color baseColor, const char* label) {
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 100.0f) progress = 100.0f;
    
    // Background bar
    DrawCard(rec, baseColor, baseColor, 0.5f, 0.0f);
    
    // Filled bar
    float fillWidth = rec.width * (progress / 100.0f);
    if (fillWidth > 0.0f) {
        Rectangle fillRec = { rec.x, rec.y, fillWidth, rec.height };
        DrawCard(fillRec, fillColor, fillColor, 0.5f, 0.0f);
    }
    
    // Label text
    if (label != nullptr) {
        DrawText(label, (int)(rec.x), (int)(rec.y - 18), 12, Colors::SlateMuted);
        
        // Progress percent on the right
        std::string pctStr = std::to_string((int)progress) + "%";
        int textW = MeasureText(pctStr.c_str(), 12);
        DrawText(pctStr.c_str(), (int)(rec.x + rec.width - textW), (int)(rec.y - 18), 12, Colors::SlateMuted);
    }
}

void UI::DrawMiniGraph(Rectangle rec, const std::vector<float>& values, Color graphColor) {
    DrawCard(rec, WHITE, Colors::SoftBorder, 0.2f, 1.0f);
    
    if (values.size() < 2) {
        DrawText("Need more logs", rec.x + 20, rec.y + rec.height * 0.5f - 6, 12, Colors::SlateMuted);
        return;
    }
    
    float maxVal = *std::max_element(values.begin(), values.end());
    float minVal = *std::min_element(values.begin(), values.end());
    float diff = maxVal - minVal;
    if (diff == 0.0f) diff = 1.0f;
    
    float padding = 8.0f;
    float graphW = rec.width - padding * 2.0f;
    float graphH = rec.height - padding * 2.0f;
    float stepX = graphW / (values.size() - 1);
    
    // Draw grid lines
    DrawLineEx({rec.x + padding, rec.y + padding + graphH * 0.5f}, {rec.x + padding + graphW, rec.y + padding + graphH * 0.5f}, 1.0f, Colors::SoftBorder);
    
    // Plot lines
    for (size_t i = 0; i < values.size() - 1; ++i) {
        float val1 = values[i];
        float val2 = values[i + 1];
        
        float x1 = rec.x + padding + i * stepX;
        float y1 = rec.y + padding + graphH - ((val1 - minVal) / diff) * graphH;
        
        float x2 = rec.x + padding + (i + 1) * stepX;
        float y2 = rec.y + padding + graphH - ((val2 - minVal) / diff) * graphH;
        
        DrawLineEx({x1, y1}, {x2, y2}, 2.5f, graphColor);
        DrawCircle(x1, y1, 3.5f, graphColor);
    }
    
    DrawCircle(rec.x + padding + (values.size() - 1) * stepX, rec.y + padding + graphH - ((values.back() - minVal) / diff) * graphH, 3.5f, graphColor);
}

void UI::DrawCategoryChart(Rectangle rec, const std::unordered_map<std::string, double>& categoryTotals) {
    if (categoryTotals.empty()) {
        DrawText("No transactions logged yet.", (int)(rec.x + 20), (int)(rec.y + 40), 16, Colors::SlateMuted);
        return;
    }
    
    // Sort categories by amount spent
    std::vector<std::pair<std::string, double>> sortedCats(categoryTotals.begin(), categoryTotals.end());
    std::sort(sortedCats.begin(), sortedCats.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    
    double maxSpend = sortedCats[0].second;
    if (maxSpend <= 0.0) maxSpend = 1.0;
    
    float barHeight = 24.0f;
    float spacing = 16.0f;
    float startY = rec.y + 15.0f;
    
    for (size_t i = 0; i < sortedCats.size(); ++i) {
        float currentY = startY + i * (barHeight + spacing);
        if (currentY + barHeight > rec.y + rec.height) break;
        
        // Draw category icon representation & text
        std::string catName = sortedCats[i].first;
        DrawText(catName.c_str(), (int)(rec.x + 10), (int)(currentY + 4), 13, Colors::CharcoalText);
        
        // Draw bar background
        float barStartX = rec.x + 110.0f;
        float barMaxW = rec.width - 210.0f;
        Rectangle barBg = { barStartX, currentY, barMaxW, barHeight };
        DrawCard(barBg, Colors::SoftBorder, Colors::SoftBorder, 0.4f, 0.0f);
        
        // Draw filled progress bar
        float fillW = barMaxW * (float)(sortedCats[i].second / maxSpend);
        Rectangle barFill = { barStartX, currentY, fillW, barHeight };
        
        // Select custom pastel color based on category index
        Color barColor = Colors::SakuraPink;
        if (i % 3 == 1) barColor = Colors::MatchaGreen;
        else if (i % 3 == 2) barColor = Colors::CozyYellow;
        
        DrawCard(barFill, barColor, barColor, 0.4f, 0.0f);
        
        // Value text
        std::string valStr = "Rs. " + std::to_string((int)sortedCats[i].second);
        DrawText(valStr.c_str(), (int)(barStartX + fillW + 8), (int)(currentY + 4), 13, Colors::CharcoalText);
    }
}

// Cozy Decor drawing routines
void UI::DrawPlant(Vector2 pos, float size, float time) {
    Color potColor = Colors::SoftPeach;
    Color leafColor = { 108, 169, 101, 255 };
    
    DrawEllipse(pos.x, pos.y + size * 0.4f, size * 0.45f, size * 0.1f, Colors::ShadowGray);

    DrawEllipse(pos.x, pos.y - size * 0.15f, size * 0.32f, size * 0.08f, Colors::CharcoalText);
    
    Vector2 p1 = { pos.x - size * 0.28f, pos.y - size * 0.15f };
    Vector2 p2 = { pos.x + size * 0.28f, pos.y - size * 0.15f };
    Vector2 p3 = { pos.x + size * 0.18f, pos.y + size * 0.4f };
    Vector2 p4 = { pos.x - size * 0.18f, pos.y + size * 0.4f };
    
    DrawTriangle(p1, p3, p2, potColor);
    DrawTriangle(p1, p4, p3, potColor);
    
    DrawRectangleRounded({pos.x - size * 0.32f, pos.y - size * 0.22f, size * 0.64f, size * 0.08f}, 0.5f, 4, potColor);
    
    // Swaying Leaves (two stems)
    float sway1 = sin(time * 2.0f) * (size * 0.06f);
    float sway2 = cos(time * 1.7f) * (size * 0.05f);
    
    // Leaf 1 (Left)
    Vector2 leaf1Center = { pos.x - size * 0.24f + sway1, pos.y - size * 0.36f };
    DrawEllipse(leaf1Center.x, leaf1Center.y, size * 0.22f, size * 0.13f, leafColor);
    DrawLineEx({pos.x - size * 0.05f, pos.y - size * 0.18f}, leaf1Center, 2.2f, leafColor);
    
    // Leaf 2 (Right)
    Vector2 leaf2Center = { pos.x + size * 0.24f + sway2, pos.y - size * 0.4f };
    DrawEllipse(leaf2Center.x, leaf2Center.y, size * 0.14f, size * 0.24f, leafColor);
    DrawLineEx({pos.x + size * 0.05f, pos.y - size * 0.18f}, leaf2Center, 2.2f, leafColor);
}

void UI::DrawCoffeeCup(Vector2 pos, float size, float time) {
    Color cupColor = Colors::SoftPeach;
    Color steamColor = Colors::SlateMuted;
    
    DrawEllipse(pos.x, pos.y + size * 0.4f, size * 0.6f, size * 0.08f, Colors::SoftBorder);
    
    DrawCircleSector({pos.x + size * 0.32f, pos.y + size * 0.05f}, size * 0.2f, -90, 90, 12, cupColor);
    DrawCircleSector({pos.x + size * 0.32f, pos.y + size * 0.05f}, size * 0.12f, -90, 90, 12, Colors::SoftLinen);

    DrawRectangleRounded({pos.x - size * 0.35f, pos.y - size * 0.22f, size * 0.7f, size * 0.6f}, 0.12f, 4, cupColor);
    
    // Steaming animation (two wavy lines rising and fading)
    float steamSway = sin(time * 3.5f) * 3.5f;
    Vector2 s1_start = { pos.x - size * 0.12f, pos.y - size * 0.3f };
    Vector2 s1_ctrl = { pos.x - size * 0.18f + steamSway, pos.y - size * 0.48f };
    Vector2 s1_end = { pos.x - size * 0.1f + steamSway * 1.5f, pos.y - size * 0.65f };
    
    Vector2 s2_start = { pos.x + size * 0.12f, pos.y - size * 0.3f };
    Vector2 s2_ctrl = { pos.x + size * 0.08f + steamSway, pos.y - size * 0.48f };
    Vector2 s2_end = { pos.x + size * 0.14f + steamSway * 1.5f, pos.y - size * 0.65f };

    DrawLineBezier(s1_start, s1_end, 2.0f, ColorAlpha(steamColor, 0.35f));
    DrawLineBezier(s2_start, s2_end, 2.0f, ColorAlpha(steamColor, 0.35f));
}

void UI::DrawCloud(Vector2 pos, float size, float speed, float time) {
    float x = pos.x + fmod(time * speed, 1200.0f) - 200.0f;
    Color cloudColor = WHITE;
    
    // Overlapping cloud circles
    DrawCircle((int)x, (int)pos.y, size, cloudColor);
    DrawCircle((int)(x - size * 0.6f), (int)(pos.y + size * 0.15f), size * 0.7f, cloudColor);
    DrawCircle((int)(x + size * 0.6f), (int)(pos.y + size * 0.15f), size * 0.7f, cloudColor);
    DrawRectangle((int)(x - size * 0.6f), (int)(pos.y - size * 0.2f), (int)(size * 1.2f), (int)(size * 1.05f), cloudColor);
}

void UI::DrawTwinklingStars(Vector2 center, float radius, float time) {
    float tw = abs(sin(time * 3.0f));
    if (tw < 0.1f) return;
    
    DrawLineEx({center.x - radius * tw, center.y}, {center.x + radius * tw, center.y}, 1.5f, Colors::CozyYellow);
    DrawLineEx({center.x, center.y - radius * tw}, {center.x, center.y + radius * tw}, 1.5f, Colors::CozyYellow);
}
