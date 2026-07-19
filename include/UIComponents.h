#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

struct InputField {
    char buffer[256] = {0};
    int length = 0;
    bool active = false;

    void clear() {
        buffer[0] = '\0';
        length = 0;
        active = false;
    }
    std::string getValue() const { return std::string(buffer); }
    void setValue(const std::string& val) {
        length = (int)val.copy(buffer, 255);
        buffer[length] = '\0';
    }
};

namespace UI {
    void DrawCard(Rectangle rec, Color bgColor, Color borderColor, float radius = 8.0f, float borderThickness = 1.5f);
    void DrawShadow(Rectangle rec, float size = 4.0f);
    
    bool DrawButton(Rectangle rec, const char* text, Color baseColor, Color textColor, bool isHovered, int fontSize = 16);
    void DrawInputField(Rectangle rec, InputField& field, const char* placeholder, int fontSize = 16);
    bool DrawDropdown(Rectangle rec, const std::vector<std::string>& items, int& activeIndex, bool& isOpen, int fontSize = 16);
    
    void DrawProgressBar(Rectangle rec, float progress, Color fillColor, Color baseColor, const char* label = nullptr);
    void DrawMiniGraph(Rectangle rec, const std::vector<float>& values, Color graphColor);
    void DrawCategoryChart(Rectangle rec, const std::unordered_map<std::string, double>& categoryTotals);
    
    void DrawPlant(Vector2 pos, float size, float time);
    void DrawCoffeeCup(Vector2 pos, float size, float time);
    void DrawCloud(Vector2 pos, float size, float speed, float time);
    void DrawTwinklingStars(Vector2 center, float radius, float time);
}

#endif // UI_COMPONENTS_H
