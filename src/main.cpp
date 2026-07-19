#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "raylib.h"
#include "FinanceManager.h"
#include "UIComponents.h"
#include "Common.h"

// Linker compatibility stub for assert
extern "C" {
    void _wassert(const wchar_t* message, const wchar_t* filename, unsigned line) {
        // Linker compatibility stub
    }
    void (*__imp__wassert)(const wchar_t*, const wchar_t*, unsigned) = &_wassert;
}

// Date formatter YYYY-MM-DD
static std::string getTodayDateString() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 720;
    
    // Set config flags for nice anti-aliasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "MochiPocket - Cozy Desktop Finance");
    SetTargetFPS(60);

    FinanceManager fm("data/wallet.json");

    int activeTab = 0; // 0: Desk, 1: Logs, 2: Budgets & Goals, 3: Analytics, 4: Collection
    
    // Transaction Form fields
    InputField txAmountField;
    InputField txMerchantField;
    InputField txNotesField;
    InputField txDateField;
    txDateField.setValue(getTodayDateString());
    TransactionType txType = TransactionType::EXPENSE;
    int selectedCategoryIndex = 0; // default Food
    
    // Budget Form fields
    InputField budgetLimitField;
    int budgetCategoryIndex = 0;
    
    // Savings Goal Form fields
    InputField goalNameField;
    InputField goalTargetField;

    // Scroll Offsets
    float logsScrollOffset = 0.0f;
    float budgetsScrollOffset = 0.0f;
    float goalsScrollOffset = 0.0f;

    // Search and filters
    InputField searchField;
    int filterCategoryIndex = -1; // -1 for "All"

    // Inline quick deposit/withdraw states
    InputField quickGoalAmtField;
    int activeGoalIndexForInput = -1;
    bool isDepositingToGoal = true;

    while (!WindowShouldClose()) {
        float time = (float)GetTime();
        Vector2 mousePos = GetMousePosition();

        if (activeTab == 1) { // Logs tab
            if (txAmountField.active) {
                // Only allow numbers and decimal point
                int key = GetCharPressed();
                while (key > 0) {
                    if (((key >= '0' && key <= '9') || key == '.') && (txAmountField.length < 255)) {
                        txAmountField.buffer[txAmountField.length] = (char)key;
                        txAmountField.length++;
                        txAmountField.buffer[txAmountField.length] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    txAmountField.length--;
                    if (txAmountField.length < 0) txAmountField.length = 0;
                    txAmountField.buffer[txAmountField.length] = '\0';
                }
            } else {
                // Standard text inputs
                txMerchantField.active ? txMerchantField.setValue(txMerchantField.getValue()) : void();
                txNotesField.active ? txNotesField.setValue(txNotesField.getValue()) : void();
                txDateField.active ? txDateField.setValue(txDateField.getValue()) : void();
                
                // Allow standard text input typing
                if (txMerchantField.active) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if (key >= 32 && key <= 125 && txMerchantField.length < 255) {
                            txMerchantField.buffer[txMerchantField.length] = (char)key;
                            txMerchantField.length++;
                            txMerchantField.buffer[txMerchantField.length] = '\0';
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        txMerchantField.length--;
                        if (txMerchantField.length < 0) txMerchantField.length = 0;
                        txMerchantField.buffer[txMerchantField.length] = '\0';
                    }
                }
                if (txNotesField.active) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if (key >= 32 && key <= 125 && txNotesField.length < 255) {
                            txNotesField.buffer[txNotesField.length] = (char)key;
                            txNotesField.length++;
                            txNotesField.buffer[txNotesField.length] = '\0';
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        txNotesField.length--;
                        if (txNotesField.length < 0) txNotesField.length = 0;
                        txNotesField.buffer[txNotesField.length] = '\0';
                    }
                }
                if (txDateField.active) {
                    int key = GetCharPressed();
                    while (key > 0) {
                        if (((key >= '0' && key <= '9') || key == '-') && txDateField.length < 10) {
                            txDateField.buffer[txDateField.length] = (char)key;
                            txDateField.length++;
                            txDateField.buffer[txDateField.length] = '\0';
                        }
                        key = GetCharPressed();
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) {
                        txDateField.length--;
                        if (txDateField.length < 0) txDateField.length = 0;
                        txDateField.buffer[txDateField.length] = '\0';
                    }
                }
            }

            // Search box update
            if (searchField.active) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (key >= 32 && key <= 125 && searchField.length < 255) {
                        searchField.buffer[searchField.length] = (char)key;
                        searchField.length++;
                        searchField.buffer[searchField.length] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    searchField.length--;
                    if (searchField.length < 0) searchField.length = 0;
                    searchField.buffer[searchField.length] = '\0';
                }
            }

            // Mouse scroll list
            float wheel = GetMouseWheelMove();
            if (CheckCollisionPointRec(mousePos, { 430, 240, 530, 440 })) {
                logsScrollOffset += wheel * 30.0f;
                if (logsScrollOffset > 0.0f) logsScrollOffset = 0.0f;
            }
        }
        else if (activeTab == 2) { // Budgets & Goals tab
            // Budget limit field (numeric)
            if (budgetLimitField.active) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (((key >= '0' && key <= '9') || key == '.') && (budgetLimitField.length < 255)) {
                        budgetLimitField.buffer[budgetLimitField.length] = (char)key;
                        budgetLimitField.length++;
                        budgetLimitField.buffer[budgetLimitField.length] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    budgetLimitField.length--;
                    if (budgetLimitField.length < 0) budgetLimitField.length = 0;
                    budgetLimitField.buffer[budgetLimitField.length] = '\0';
                }
            }

            // Savings goal name field
            if (goalNameField.active) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (key >= 32 && key <= 125 && goalNameField.length < 255) {
                        goalNameField.buffer[goalNameField.length] = (char)key;
                        goalNameField.length++;
                        goalNameField.buffer[goalNameField.length] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    goalNameField.length--;
                    if (goalNameField.length < 0) goalNameField.length = 0;
                    goalNameField.buffer[goalNameField.length] = '\0';
                }
            }

            // Savings goal target field (numeric)
            if (goalTargetField.active) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (((key >= '0' && key <= '9') || key == '.') && (goalTargetField.length < 255)) {
                        goalTargetField.buffer[goalTargetField.length] = (char)key;
                        goalTargetField.length++;
                        goalTargetField.buffer[goalTargetField.length] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    goalTargetField.length--;
                    if (goalTargetField.length < 0) goalTargetField.length = 0;
                    goalTargetField.buffer[goalTargetField.length] = '\0';
                }
            }

            // Quick deposit/withdraw field (numeric)
            if (activeGoalIndexForInput != -1 && quickGoalAmtField.active) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (((key >= '0' && key <= '9') || key == '.') && (quickGoalAmtField.length < 255)) {
                        quickGoalAmtField.buffer[quickGoalAmtField.length] = (char)key;
                        quickGoalAmtField.length++;
                        quickGoalAmtField.buffer[quickGoalAmtField.length] = '\0';
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    quickGoalAmtField.length--;
                    if (quickGoalAmtField.length < 0) quickGoalAmtField.length = 0;
                    quickGoalAmtField.buffer[quickGoalAmtField.length] = '\0';
                }
            }

            // Mouse wheel scroll for budgets and goals list
            float wheel = GetMouseWheelMove();
            if (CheckCollisionPointRec(mousePos, { 40, 360, 420, 320 })) {
                budgetsScrollOffset += wheel * 30.0f;
                if (budgetsScrollOffset > 0.0f) budgetsScrollOffset = 0.0f;
            }
            if (CheckCollisionPointRec(mousePos, { 490, 240, 470, 320 })) {
                goalsScrollOffset += wheel * 30.0f;
                if (goalsScrollOffset > 0.0f) goalsScrollOffset = 0.0f;
            }
        }

        BeginDrawing();
        ClearBackground(Colors::SoftLinen);

        // Header Background with floating clouds
        DrawRectangle(0, 0, screenWidth, 90, Colors::MatchaGreen);
        DrawLineEx({0, 90}, {screenWidth, 90}, 2.0f, Colors::SoftBorder);
        
        // Draw Clouds floating slowly
        UI::DrawCloud({0, 20}, 22, 12, time);
        UI::DrawCloud({300, 35}, 16, 8, time);
        UI::DrawCloud({700, 15}, 28, 14, time);

        // Header Title & Cozy Subtitle
        DrawText("MochiPocket", 40, 18, 30, Colors::CharcoalText);
        DrawText("your cozy desk finance companion", 40, 52, 14, Colors::SlateMuted);

        // Top-Right Quick Balance and Streak info
        std::string balStr = "Balance: Rs. " + std::to_string((int)fm.getBalance());
        DrawText(balStr.c_str(), screenWidth - 250, 22, 18, Colors::CharcoalText);

        std::string streakStr = "Streak: " + std::to_string(fm.getStreakCount()) + " days *";
        DrawText(streakStr.c_str(), screenWidth - 250, 48, 14, Colors::SlateMuted);

        // Tab Selection Buttons
        const char* tabNames[] = { "Desk", "Logs", "Budgets & Goals", "Analytics", "Collection" };
        float tabWidth = 160.0f;
        float startX = 40.0f;
        
        for (int i = 0; i < 5; ++i) {
            Rectangle tabRec = { startX + i * (tabWidth + 10), 105, tabWidth, 36 };
            bool isHovered = CheckCollisionPointRec(mousePos, tabRec);
            Color tabBg = (activeTab == i) ? Colors::PureCream : Colors::SoftLinen;
            Color tabText = (activeTab == i) ? Colors::CharcoalText : Colors::SlateMuted;
            
            UI::DrawCard(tabRec, tabBg, (activeTab == i) ? Colors::SakuraPink : Colors::SoftBorder, 0.25f, 1.5f);
            
            int textW = MeasureText(tabNames[i], 14);
            DrawText(tabNames[i], (int)(tabRec.x + (tabRec.width - textW) * 0.5f), (int)(tabRec.y + 11), 14, tabText);
            
            if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                activeTab = i;
            }
        }
        
        DrawLineEx({0, 142}, {screenWidth, 142}, 1.5f, Colors::SoftBorder);

        // Render Active Tab View
        switch (activeTab) {
            case 0: { // ----------------- DESK VIEW (DASHBOARD) -----------------
                // Left Panel: Desk mascot
                Rectangle deskRec = { 40, 160, 440, 520 };
                UI::DrawShadow(deskRec, 4.0f);
                UI::DrawCard(deskRec, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
                
                DrawText("Active Mochi Desk", deskRec.x + 20, deskRec.y + 20, 18, Colors::CharcoalText);
                DrawLineEx({deskRec.x + 20, deskRec.y + 48}, {deskRec.x + deskRec.width - 20, deskRec.y + 48}, 1.0f, Colors::SoftBorder);
                
                // Draw wood desk shelf line
                DrawRectangle(deskRec.x + 30, deskRec.y + 360, deskRec.width - 60, 10, Colors::SoftPeach);
                DrawRectangle(deskRec.x + 25, deskRec.y + 370, deskRec.width - 50, 6, Colors::SoftBorder);

                // Draw Coffee Cup (Right side of shelf)
                Vector2 cupPos = { deskRec.x + 350, deskRec.y + 320 };
                UI::DrawCoffeeCup(cupPos, 45, time);
                
                // Coffee Interaction: Hover shows bubbles
                bool hoverCup = CheckCollisionPointCircle(mousePos, cupPos, 35);
                if (hoverCup) {
                    UI::DrawCard({cupPos.x - 55, cupPos.y - 65, 110, 26}, Colors::CozyYellow, Colors::SoftBorder, 0.4f, 1.0f);
                    DrawText("Warm Matcha Tea", cupPos.x - 48, cupPos.y - 58, 11, Colors::CharcoalText);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        fm.getMochiSystemModifier().setMood(MochiMood::HAPPY);
                    }
                }

                // Draw Potted Plant (Left side of shelf)
                Vector2 plantPos = { deskRec.x + 90, deskRec.y + 310 };
                UI::DrawPlant(plantPos, 55, time);
                
                bool hoverPlant = CheckCollisionPointCircle(mousePos, plantPos, 40);
                if (hoverPlant) {
                    UI::DrawCard({plantPos.x - 50, plantPos.y - 70, 100, 26}, Colors::MatchaGreen, Colors::SoftBorder, 0.4f, 1.0f);
                    DrawText("Tiny Bonsai", plantPos.x - 32, plantPos.y - 63, 11, Colors::CharcoalText);
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        fm.getMochiSystemModifier().setMood(MochiMood::EXCITED);
                    }
                }

                // Draw active Mochi companion floating on shelf
                Vector2 mochiPos = { deskRec.x + 220, deskRec.y + 325 };
                std::string activeMochi = fm.getMochiSystem().getActiveCompanion();
                fm.getMochiSystem().drawCompanion(activeMochi, mochiPos, 80.0f, time);

                // Mochi Info
                int centerTextY = deskRec.y + 395;
                int nameW = MeasureText(activeMochi.c_str(), 18);
                DrawText(activeMochi.c_str(), deskRec.x + (deskRec.width - nameW) * 0.5f, centerTextY, 18, Colors::CharcoalText);
                
                // Find companion description
                std::string desc = "Companion mochi";
                std::string cond = "";
                for (const auto& comp : fm.getMochiSystem().getCompanions()) {
                    if (comp.name == activeMochi) {
                        desc = comp.description;
                        cond = comp.unlockCondition;
                        break;
                    }
                }
                
                int descW = MeasureText(desc.c_str(), 13);
                DrawText(desc.c_str(), deskRec.x + (deskRec.width - descW) * 0.5f, centerTextY + 28, 13, Colors::SlateMuted);
                
                std::string moodStr = "Companion mood: ";
                switch (fm.getMochiSystem().getMood()) {
                    case MochiMood::HAPPY: moodStr += "Happy Mochi <3"; break;
                    case MochiMood::WORRIED: moodStr += "Worried (Over Budget!)"; break;
                    case MochiMood::EXCITED: moodStr += "Excited savings streak!"; break;
                    case MochiMood::CELEBRATING: moodStr += "Celebrating completed goals!"; break;
                }
                int moodW = MeasureText(moodStr.c_str(), 13);
                DrawText(moodStr.c_str(), deskRec.x + (deskRec.width - moodW) * 0.5f, centerTextY + 50, 13, Colors::CharcoalText);

                // Right Panel: Financial status
                Rectangle statusRec = { 510, 160, 450, 520 };
                UI::DrawShadow(statusRec, 4.0f);
                UI::DrawCard(statusRec, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
                
                DrawText("Desk Summary", statusRec.x + 20, statusRec.y + 20, 18, Colors::CharcoalText);
                DrawLineEx({statusRec.x + 20, statusRec.y + 48}, {statusRec.x + statusRec.width - 20, statusRec.y + 48}, 1.0f, Colors::SoftBorder);

                // Sub-cards for Income / Expense / Balance
                Rectangle balCard = { statusRec.x + 20, statusRec.y + 65, 410, 75 };
                UI::DrawCard(balCard, Colors::SoftLinen, Colors::SoftBorder, 0.15f, 1.0f);
                DrawText("Available Wallet Funds", balCard.x + 20, balCard.y + 15, 13, Colors::SlateMuted);
                std::string balValStr = "Rs. " + std::to_string((int)fm.getBalance());
                DrawText(balValStr.c_str(), balCard.x + 20, balCard.y + 35, 24, Colors::CharcoalText);

                // Split metrics: Monthly Expense & Total Savings
                double monthlyExpense = fm.getAnalyticsEngine().calculateTotalExpense(fm.getTransactions());
                double goalSavings = fm.getAnalyticsEngine().calculateTotalSavingsFromGoals(fm.getGoals());

                Rectangle expCard = { statusRec.x + 20, statusRec.y + 155, 195, 75 };
                UI::DrawCard(expCard, Colors::PureCream, Colors::SoftBorder, 0.15f, 1.0f);
                DrawText("Total Expense Logs", expCard.x + 15, expCard.y + 15, 12, Colors::SlateMuted);
                std::string expValStr = "Rs. " + std::to_string((int)monthlyExpense);
                DrawText(expValStr.c_str(), expCard.x + 15, expCard.y + 35, 20, Colors::WarningCoral);

                Rectangle svgCard = { statusRec.x + 235, statusRec.y + 155, 195, 75 };
                UI::DrawCard(svgCard, Colors::PureCream, Colors::SoftBorder, 0.15f, 1.0f);
                DrawText("Goal Savings Balance", svgCard.x + 15, svgCard.y + 15, 12, Colors::SlateMuted);
                std::string svgValStr = "Rs. " + std::to_string((int)goalSavings);
                DrawText(svgValStr.c_str(), svgCard.x + 15, svgCard.y + 35, 20, Colors::MatchaGreen);

                // Recent Activity / Transactions list
                DrawText("Recent Activity Desk", statusRec.x + 20, statusRec.y + 250, 15, Colors::CharcoalText);
                DrawLineEx({statusRec.x + 20, statusRec.y + 272}, {statusRec.x + statusRec.width - 20, statusRec.y + 272}, 1.0f, Colors::SoftBorder);

                const auto& txs = fm.getTransactions();
                int itemsToDraw = std::min((int)txs.size(), 4);
                
                if (itemsToDraw == 0) {
                    DrawText("Your logbook is empty. Record a transaction! 🍡", statusRec.x + 40, statusRec.y + 330, 14, Colors::SlateMuted);
                } else {
                    for (int i = 0; i < itemsToDraw; ++i) {
                        const auto& tx = txs[txs.size() - 1 - i]; // Reverse order
                        float rowY = statusRec.y + 285 + i * 50;
                        Rectangle rowRec = { statusRec.x + 20, rowY, 410, 42 };
                        
                        UI::DrawCard(rowRec, Colors::SoftLinen, Colors::SoftBorder, 0.2f, 1.0f);
                        
                        // Icon circle (Sakura Pink / Matcha Green)
                        Color dotColor = (tx.type == TransactionType::INCOME) ? Colors::MatchaGreen : Colors::SakuraPink;
                        DrawCircle(rowRec.x + 25, rowRec.y + 21, 14, dotColor);
                        DrawText(tx.category.substr(0, 1).c_str(), rowRec.x + 21, rowRec.y + 14, 12, Colors::CharcoalText);

                        // Merchant / Category text
                        std::string label = tx.merchant.empty() ? tx.category : tx.merchant;
                        DrawText(label.c_str(), rowRec.x + 52, rowRec.y + 7, 13, Colors::CharcoalText);
                        DrawText(tx.date.c_str(), rowRec.x + 52, rowRec.y + 23, 11, Colors::SlateMuted);

                        // Amount
                        std::string amtSign = (tx.type == TransactionType::INCOME) ? "+" : "-";
                        std::string amtVal = amtSign + "Rs. " + std::to_string((int)tx.amount);
                        int amtW = MeasureText(amtVal.c_str(), 14);
                        
                        Color amtCol = (tx.type == TransactionType::INCOME) ? Color{ 60, 140, 60, 255 } : Colors::CharcoalText;
                        DrawText(amtVal.c_str(), rowRec.x + rowRec.width - amtW - 20, rowRec.y + 14, 14, amtCol);
                    }
                }
                break;
            }
            case 1: { // ----------------- LOGS VIEW (TRANSACTIONS) -----------------
                // Left Panel: Form to Add
                Rectangle formRec = { 40, 160, 360, 520 };
                UI::DrawShadow(formRec, 4.0f);
                UI::DrawCard(formRec, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
                
                DrawText("Add Log Entry", formRec.x + 20, formRec.y + 20, 18, Colors::CharcoalText);
                DrawLineEx({formRec.x + 20, formRec.y + 48}, {formRec.x + formRec.width - 20, formRec.y + 48}, 1.0f, Colors::SoftBorder);

                // Type Toggle: Income vs Expense
                Rectangle expToggle = { formRec.x + 20, formRec.y + 65, 155, 34 };
                Rectangle incToggle = { formRec.x + 185, formRec.y + 65, 155, 34 };
                
                bool hoverExp = CheckCollisionPointRec(mousePos, expToggle);
                bool hoverInc = CheckCollisionPointRec(mousePos, incToggle);
                
                if (UI::DrawButton(expToggle, "Expense Log", (txType == TransactionType::EXPENSE) ? Colors::SakuraPink : Colors::SoftLinen, Colors::CharcoalText, hoverExp, 13)) {
                    txType = TransactionType::EXPENSE;
                }
                if (UI::DrawButton(incToggle, "Income Log", (txType == TransactionType::INCOME) ? Colors::MatchaGreen : Colors::SoftLinen, Colors::CharcoalText, hoverInc, 13)) {
                    txType = TransactionType::INCOME;
                }

                DrawText("Amount (Rs. )", formRec.x + 20, formRec.y + 115, 13, Colors::CharcoalText);
                Rectangle amtRec = { formRec.x + 20, formRec.y + 133, 320, 32 };
                UI::DrawInputField(amtRec, txAmountField, "0.00", 14);

                DrawText("Category Desk", formRec.x + 20, formRec.y + 180, 13, Colors::CharcoalText);
                float gridStartY = formRec.y + 200;
                float btnW = 95;
                float btnH = 26;
                float gridSpacing = 8;

                for (size_t i = 0; i < DEFAULT_CATEGORIES.size(); ++i) {
                    int row = i / 3;
                    int col = i % 3;
                    Rectangle catBtnRec = { formRec.x + 20 + col * (btnW + gridSpacing), gridStartY + row * (btnH + gridSpacing), btnW, btnH };
                    bool hoverCat = CheckCollisionPointRec(mousePos, catBtnRec);
                    
                    Color btnCol = (selectedCategoryIndex == (int)i) ? Colors::CozyYellow : Colors::SoftLinen;
                    Color borderCol = (selectedCategoryIndex == (int)i) ? Colors::WarningCoral : Colors::SoftBorder;
                    
                    UI::DrawCard(catBtnRec, btnCol, borderCol, 0.3f, 1.0f);
                    int textW = MeasureText(DEFAULT_CATEGORIES[i].c_str(), 11);
                    DrawText(DEFAULT_CATEGORIES[i].c_str(), (int)(catBtnRec.x + (catBtnRec.width - textW)*0.5f), (int)(catBtnRec.y + 7), 11, Colors::CharcoalText);
                    
                    if (hoverCat && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        selectedCategoryIndex = (int)i;
                    }
                }

                DrawText("Merchant / Source", formRec.x + 20, formRec.y + 350, 13, Colors::CharcoalText);
                Rectangle merchRec = { formRec.x + 20, formRec.y + 368, 320, 32 };
                UI::DrawInputField(merchRec, txMerchantField, "e.g. Starbucks, Salary", 14);

                DrawText("Notes", formRec.x + 20, formRec.y + 410, 13, Colors::CharcoalText);
                Rectangle notesRec = { formRec.x + 20, formRec.y + 428, 320, 32 };
                UI::DrawInputField(notesRec, txNotesField, "Short details", 14);

                Rectangle addBtnRec = { formRec.x + 20, formRec.y + 472, 320, 38 };
                bool hoverAdd = CheckCollisionPointRec(mousePos, addBtnRec);
                
                if (UI::DrawButton(addBtnRec, "Record Log Entry", Colors::CozyYellow, Colors::CharcoalText, hoverAdd, 14)) {
                    double amt = 0.0;
                    try { amt = std::stod(txAmountField.getValue()); } catch (...) {}
                    if (amt > 0.0) {
                        std::string merchantVal = txMerchantField.getValue();
                        std::string notesVal = txNotesField.getValue();
                        std::string dateVal = txDateField.getValue();
                        
                        fm.addTransaction(amt, txType, DEFAULT_CATEGORIES[selectedCategoryIndex], merchantVal, dateVal, notesVal);
                        
                        // Clear form
                        txAmountField.clear();
                        txMerchantField.clear();
                        txNotesField.clear();
                        txDateField.setValue(getTodayDateString());
                    }
                }

                // Right Panel: Filter, search & list
                Rectangle listCardRec = { 430, 160, 530, 520 };
                UI::DrawShadow(listCardRec, 4.0f);
                UI::DrawCard(listCardRec, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
                
                DrawText("Transaction Ledger", listCardRec.x + 20, listCardRec.y + 20, 18, Colors::CharcoalText);
                
                Rectangle searchRec = { listCardRec.x + 260, listCardRec.y + 15, 250, 28 };
                UI::DrawInputField(searchRec, searchField, "Search logs...", 12);
                
                DrawLineEx({listCardRec.x + 20, listCardRec.y + 48}, {listCardRec.x + listCardRec.width - 20, listCardRec.y + 48}, 1.0f, Colors::SoftBorder);

                float filterStartX = listCardRec.x + 20;
                float filterY = listCardRec.y + 58;
                
                // We'll show a few filters: All, Food, Coffee, Shopping, Bills
                std::vector<std::string> quickFilters = { "All", "Food", "Coffee", "Shopping", "Bills", "Savings" };
                for (size_t f = 0; f < quickFilters.size(); ++f) {
                    float fW = 68;
                    float fH = 22;
                    Rectangle fRec = { filterStartX + f * (fW + 6), filterY, fW, fH };
                    bool hoverFilter = CheckCollisionPointRec(mousePos, fRec);
                    
                    int currentFilterCheck = (f == 0) ? -1 : (int)std::distance(DEFAULT_CATEGORIES.begin(), std::find(DEFAULT_CATEGORIES.begin(), DEFAULT_CATEGORIES.end(), quickFilters[f]));
                    
                    Color fBg = (filterCategoryIndex == currentFilterCheck) ? Colors::SakuraPink : Colors::SoftLinen;
                    UI::DrawCard(fRec, fBg, Colors::SoftBorder, 0.3f, 1.0f);
                    int textW = MeasureText(quickFilters[f].c_str(), 11);
                    DrawText(quickFilters[f].c_str(), (int)(fRec.x + (fRec.width - textW)*0.5f), (int)(fRec.y + 5), 11, Colors::CharcoalText);
                    
                    if (hoverFilter && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        filterCategoryIndex = currentFilterCheck;
                    }
                }

                DrawLineEx({listCardRec.x + 20, listCardRec.y + 90}, {listCardRec.x + listCardRec.width - 20, listCardRec.y + 90}, 1.0f, Colors::SoftBorder);

                BeginScissorMode((int)listCardRec.x, (int)(listCardRec.y + 95), (int)listCardRec.width, 410);

                const auto& transactions = fm.getTransactions();
                // Filter transactions based on category filter and search keyword
                std::vector<Transaction> filteredTxs;
                std::string query = searchField.getValue();
                std::transform(query.begin(), query.end(), query.begin(), ::tolower);

                for (const auto& tx : transactions) {
                    if (filterCategoryIndex != -1 && tx.category != DEFAULT_CATEGORIES[filterCategoryIndex]) {
                        continue;
                    }
                    if (!query.empty()) {
                        std::string merch = tx.merchant;
                        std::string notes = tx.notes;
                        std::string cat = tx.category;
                        std::transform(merch.begin(), merch.end(), merch.begin(), ::tolower);
                        std::transform(notes.begin(), notes.end(), notes.begin(), ::tolower);
                        std::transform(cat.begin(), cat.end(), cat.begin(), ::tolower);
                        if (merch.find(query) == std::string::npos && 
                            notes.find(query) == std::string::npos && 
                            cat.find(query) == std::string::npos) {
                            continue;
                        }
                    }
                    filteredTxs.push_back(tx);
                }

                int totalListH = filteredTxs.size() * 52;
                if (logsScrollOffset < -totalListH + 390) logsScrollOffset = (float)(-totalListH + 390);
                if (logsScrollOffset > 0.0f) logsScrollOffset = 0.0f;
                if (totalListH <= 390) logsScrollOffset = 0.0f;

                if (filteredTxs.empty()) {
                    DrawText("No matching log entries found.", listCardRec.x + 30, listCardRec.y + 150, 14, Colors::SlateMuted);
                } else {
                    for (size_t i = 0; i < filteredTxs.size(); ++i) {
                        const auto& tx = filteredTxs[filteredTxs.size() - 1 - i]; // Reverse sorted
                        float itemY = listCardRec.y + 100 + i * 52 + logsScrollOffset;
                        
                        Rectangle rowRec = { listCardRec.x + 20, itemY, listCardRec.width - 40, 46 };
                        UI::DrawCard(rowRec, Colors::SoftLinen, Colors::SoftBorder, 0.2f, 1.0f);

                        Color col = (tx.type == TransactionType::INCOME) ? Colors::MatchaGreen : Colors::SakuraPink;
                        DrawCircle(rowRec.x + 24, rowRec.y + 23, 14, col);
                        DrawText(tx.category.substr(0,1).c_str(), rowRec.x + 20, rowRec.y + 16, 12, Colors::CharcoalText);

                        std::string title = tx.merchant.empty() ? tx.category : tx.merchant;
                        DrawText(title.c_str(), rowRec.x + 50, rowRec.y + 8, 13, Colors::CharcoalText);
                        
                        std::string sub = tx.date;
                        if (!tx.notes.empty()) sub += " - " + tx.notes;
                        DrawText(sub.c_str(), rowRec.x + 50, rowRec.y + 24, 11, Colors::SlateMuted);

                        std::string sign = (tx.type == TransactionType::INCOME) ? "+" : "-";
                        std::string amtTxt = sign + "Rs. " + std::to_string((int)tx.amount);
                        Color amtCol = (tx.type == TransactionType::INCOME) ? Color{60, 140, 60, 255} : Colors::CharcoalText;
                        
                        int amtW = MeasureText(amtTxt.c_str(), 14);
                        DrawText(amtTxt.c_str(), rowRec.x + rowRec.width - amtW - 55, rowRec.y + 15, 14, amtCol);

                        Rectangle delRec = { rowRec.x + rowRec.width - 40, rowRec.y + 10, 26, 26 };
                        bool hoverDel = CheckCollisionPointRec(mousePos, delRec);
                        
                        UI::DrawCard(delRec, hoverDel ? Colors::SakuraPink : Colors::PureCream, Colors::SoftBorder, 0.3f, 1.0f);
                        DrawText("X", delRec.x + 9, delRec.y + 6, 13, Colors::CharcoalText);

                        if (hoverDel && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            fm.deleteTransaction(tx.id);
                        }
                    }
                }
                EndScissorMode();
                break;
            }
            case 2: { // ----------------- BUDGETS & GOALS VIEW -----------------
                // Left Panel: Monthly Budgets
                Rectangle budgetCardRec = { 40, 160, 420, 520 };
                UI::DrawShadow(budgetCardRec, 4.0f);
                UI::DrawCard(budgetCardRec, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
                
                DrawText("Monthly Budgets", budgetCardRec.x + 20, budgetCardRec.y + 20, 18, Colors::CharcoalText);
                DrawLineEx({budgetCardRec.x + 20, budgetCardRec.y + 48}, {budgetCardRec.x + budgetCardRec.width - 20, budgetCardRec.y + 48}, 1.0f, Colors::SoftBorder);

                DrawText("Select Category", budgetCardRec.x + 20, budgetCardRec.y + 60, 12, Colors::SlateMuted);
                
                // Horizontally scrollable or small list of budget categories:
                static bool dropOpen = false;
                Rectangle dropRec = { budgetCardRec.x + 20, budgetCardRec.y + 78, 170, 30 };
                UI::DrawDropdown(dropRec, DEFAULT_CATEGORIES, budgetCategoryIndex, dropOpen, 12);

                DrawText("Monthly Limit (Rs. )", budgetCardRec.x + 205, budgetCardRec.y + 60, 12, Colors::SlateMuted);
                Rectangle limitRec = { budgetCardRec.x + 205, budgetCardRec.y + 78, 110, 30 };
                UI::DrawInputField(limitRec, budgetLimitField, "Limit", 12);

                Rectangle setBtn = { budgetCardRec.x + 325, budgetCardRec.y + 78, 75, 30 };
                bool hoverSet = CheckCollisionPointRec(mousePos, setBtn);
                if (UI::DrawButton(setBtn, "Set", Colors::CozyYellow, Colors::CharcoalText, hoverSet, 12)) {
                    double limit = 0.0;
                    try { limit = std::stod(budgetLimitField.getValue()); } catch (...) {}
                    if (limit > 0.0) {
                        fm.setBudget(DEFAULT_CATEGORIES[budgetCategoryIndex], limit);
                        budgetLimitField.clear();
                    }
                }

                DrawLineEx({budgetCardRec.x + 20, budgetCardRec.y + 125}, {budgetCardRec.x + budgetCardRec.width - 20, budgetCardRec.y + 125}, 1.0f, Colors::SoftBorder);

                BeginScissorMode((int)budgetCardRec.x, (int)(budgetCardRec.y + 130), (int)budgetCardRec.width, 370);
                
                const auto& budgets = fm.getBudgetManager().getAllBudgets();
                auto categoryTotals = fm.getAnalyticsEngine().calculateCategoryTotals(fm.getTransactions());

                if (budgets.empty()) {
                    DrawText("No budget limits set. Create one above! 🍡", budgetCardRec.x + 30, budgetCardRec.y + 180, 13, Colors::SlateMuted);
                } else {
                    int bIndex = 0;
                    std::vector<std::pair<std::string, double>> budgetVec(budgets.begin(), budgets.end());
                    int totalBudgetsH = budgetVec.size() * 70;
                    
                    if (budgetsScrollOffset < -totalBudgetsH + 340) budgetsScrollOffset = (float)(-totalBudgetsH + 340);
                    if (budgetsScrollOffset > 0.0f) budgetsScrollOffset = 0.0f;
                    if (totalBudgetsH <= 340) budgetsScrollOffset = 0.0f;

                    for (const auto& pair : budgetVec) {
                        float rowY = budgetCardRec.y + 140 + bIndex * 70 + budgetsScrollOffset;
                        Rectangle rowRec = { budgetCardRec.x + 20, rowY, budgetCardRec.width - 40, 60 };
                        
                        double spent = categoryTotals[pair.first];
                        float progress = (spent / pair.second) * 100.0f;
                        bool over = spent > pair.second;

                        UI::DrawCard(rowRec, Colors::SoftLinen, Colors::SoftBorder, 0.15f, 1.0f);
                        
                        // Category limit details
                        DrawText(pair.first.c_str(), rowRec.x + 15, rowRec.y + 10, 14, Colors::CharcoalText);
                        
                        std::string amtTxt = "Rs. " + std::to_string((int)spent) + " / Rs. " + std::to_string((int)pair.second);
                        int amtW = MeasureText(amtTxt.c_str(), 12);
                        DrawText(amtTxt.c_str(), rowRec.x + rowRec.width - amtW - 40, rowRec.y + 12, 12, Colors::SlateMuted);

                        // Custom progress bar
                        UI::DrawProgressBar({rowRec.x + 15, rowRec.y + 36, rowRec.width - 70, 10}, progress, 
                                            over ? Colors::WarningCoral : Colors::MatchaGreen, Colors::SoftBorder);

                        Rectangle delRec = { rowRec.x + rowRec.width - 35, rowRec.y + 15, 24, 24 };
                        bool hoverDel = CheckCollisionPointRec(mousePos, delRec);
                        UI::DrawCard(delRec, hoverDel ? Colors::SakuraPink : Colors::PureCream, Colors::SoftBorder, 0.3f, 1.0f);
                        DrawText("X", delRec.x + 8, delRec.y + 5, 12, Colors::CharcoalText);

                        if (hoverDel && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            fm.removeBudget(pair.first);
                        }

                        bIndex++;
                    }
                }
                EndScissorMode();
                break;
            }
            case 3: { // ----------------- ANALYTICS VIEW -----------------
                // Left Panel: Category chart
                Rectangle chartCard = { 40, 160, 500, 520 };
                UI::DrawShadow(chartCard, 4.0f);
                UI::DrawCard(chartCard, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
                
                DrawText("Expense Breakdown by Category", chartCard.x + 20, chartCard.y + 20, 18, Colors::CharcoalText);
                DrawLineEx({chartCard.x + 20, chartCard.y + 48}, {chartCard.x + chartCard.width - 20, chartCard.y + 48}, 1.0f, Colors::SoftBorder);

                auto categoryTotals = fm.getAnalyticsEngine().calculateCategoryTotals(fm.getTransactions());
                UI::DrawCategoryChart({chartCard.x + 10, chartCard.y + 60, chartCard.width - 20, 440}, categoryTotals);
                break;
            }
            case 4: { // ----------------- COLLECTION VIEW (STICKER BOOK) -----------------
                Rectangle bookCard = { 40, 160, 920, 520 };
                UI::DrawShadow(bookCard, 4.0f);
                UI::DrawCard(bookCard, Colors::PureCream, Colors::SoftBorder, 0.03f, 1.5f);

                DrawText("Mochi Sticker Book Collection", bookCard.x + 20, bookCard.y + 20, 18, Colors::CharcoalText);
                DrawLineEx({bookCard.x + 20, bookCard.y + 48}, {bookCard.x + bookCard.width - 20, bookCard.y + 48}, 1.0f, Colors::SoftBorder);

                const auto& companions = fm.getMochiSystem().getCompanions();
                float cardW = 265;
                float cardH = 210;
                float startX = bookCard.x + 30;
                float startY = bookCard.y + 70;
                float spaceX = 30;
                float spaceY = 25;

                for (size_t i = 0; i < companions.size(); ++i) {
                    int row = i / 3;
                    int col = i % 3;
                    Rectangle cardRec = { startX + col * (cardW + spaceX), startY + row * (cardH + spaceY), cardW, cardH };

                    bool unlocked = companions[i].isUnlocked;
                    UI::DrawCard(cardRec, Colors::SoftLinen, Colors::SoftBorder, 0.1f, 1.0f);

                    if (unlocked) {
                        // Unlocked Mascot Sticker
                        Vector2 pos = { cardRec.x + cardRec.width * 0.5f, cardRec.y + 55 };
                        fm.getMochiSystem().drawCompanion(companions[i].name, pos, 45.0f, time);

                        int cNameW = MeasureText(companions[i].name.c_str(), 14);
                        DrawText(companions[i].name.c_str(), cardRec.x + (cardRec.width - cNameW)*0.5f, cardRec.y + 90, 14, Colors::CharcoalText);

                        int cDescW = MeasureText(companions[i].description.c_str(), 10);
                        DrawText(companions[i].description.c_str(), cardRec.x + (cardRec.width - cDescW)*0.5f, cardRec.y + 115, 10, Colors::SlateMuted);

                        // Equip Button
                        bool active = fm.getMochiSystem().getActiveCompanion() == companions[i].name;
                        Rectangle eqBtn = { cardRec.x + 40, cardRec.y + 155, cardRec.width - 80, 28 };
                        bool hoverEq = CheckCollisionPointRec(mousePos, eqBtn);

                        if (UI::DrawButton(eqBtn, active ? "Equipped Desk" : "Equip Companion", 
                                            active ? Colors::MatchaGreen : Colors::PureCream, 
                                            Colors::CharcoalText, hoverEq, 11)) {
                            fm.getMochiSystemModifier().setActiveCompanion(companions[i].name);
                            fm.save();
                        }
                    } else {
                        // Locked Mochi representation
                        DrawCircle(cardRec.x + cardRec.width * 0.5f, cardRec.y + 50, 25, ColorAlpha(Colors::SlateMuted, 0.3f));
                        DrawText("?", cardRec.x + cardRec.width * 0.5f - 4, cardRec.y + 40, 20, Colors::SlateMuted);

                        DrawText("Locked Companion", cardRec.x + 72, cardRec.y + 90, 13, Colors::SlateMuted);

                        int condW = MeasureText(companions[i].unlockCondition.c_str(), 11);
                        DrawText(companions[i].unlockCondition.c_str(), cardRec.x + (cardRec.width - condW)*0.5f, cardRec.y + 120, 11, Colors::CharcoalText);
                        
                        // Disabled lock button
                        Rectangle eqBtn = { cardRec.x + 40, cardRec.y + 155, cardRec.width - 80, 28 };
                        UI::DrawCard(eqBtn, ColorAlpha(Colors::SoftBorder, 0.5f), Colors::SoftBorder, 0.3f, 1.0f);
                        int lockTextW = MeasureText("Locked Sticker", 11);
                        DrawText("Locked Sticker", (int)(eqBtn.x + (eqBtn.width - lockTextW)*0.5f), (int)(eqBtn.y + 8), 11, Colors::SlateMuted);
                    }
                }
                break;
            }
        }

        // Draw Right Panel budget goals overlay for Tab 2 (Budgets & Goals tab)
        if (activeTab == 2) {
            // Right Panel: Savings Goals
            Rectangle goalsCardRec = { 490, 160, 470, 520 };
            UI::DrawShadow(goalsCardRec, 4.0f);
            UI::DrawCard(goalsCardRec, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
            
            DrawText("Savings Goals Desk", goalsCardRec.x + 20, goalsCardRec.y + 20, 18, Colors::CharcoalText);
            DrawLineEx({goalsCardRec.x + 20, goalsCardRec.y + 48}, {goalsCardRec.x + goalsCardRec.width - 20, goalsCardRec.y + 48}, 1.0f, Colors::SoftBorder);

            DrawText("Goal Title", goalsCardRec.x + 20, goalsCardRec.y + 420, 11, Colors::SlateMuted);
            Rectangle gNameRec = { goalsCardRec.x + 20, goalsCardRec.y + 438, 140, 28 };
            UI::DrawInputField(gNameRec, goalNameField, "Buy Switch", 12);

            DrawText("Target Value (Rs. )", goalsCardRec.x + 175, goalsCardRec.y + 420, 11, Colors::SlateMuted);
            Rectangle gTgtRec = { goalsCardRec.x + 175, goalsCardRec.y + 438, 100, 28 };
            UI::DrawInputField(gTgtRec, goalTargetField, "35000", 12);

            Rectangle createBtn = { goalsCardRec.x + 290, goalsCardRec.y + 438, 160, 28 };
            bool hoverCreate = CheckCollisionPointRec(mousePos, createBtn);
            if (UI::DrawButton(createBtn, "+ Add Savings Goal", Colors::CozyYellow, Colors::CharcoalText, hoverCreate, 12)) {
                std::string name = goalNameField.getValue();
                double target = 0.0;
                try { target = std::stod(goalTargetField.getValue()); } catch (...) {}
                if (!name.empty() && target > 0.0) {
                    fm.addSavingsGoal(name, target);
                    goalNameField.clear();
                    goalTargetField.clear();
                }
            }

            DrawLineEx({goalsCardRec.x + 20, goalsCardRec.y + 405}, {goalsCardRec.x + goalsCardRec.width - 20, goalsCardRec.y + 405}, 1.0f, Colors::SoftBorder);

            BeginScissorMode((int)goalsCardRec.x, (int)(goalsCardRec.y + 55), (int)goalsCardRec.width, 345);
            
            const auto& goals = fm.getGoals();
            if (goals.empty()) {
                DrawText("No active savings goals. Create one below! 🍡", goalsCardRec.x + 30, goalsCardRec.y + 140, 13, Colors::SlateMuted);
            } else {
                int totalGoalsH = goals.size() * 115;
                if (goalsScrollOffset < -totalGoalsH + 325) goalsScrollOffset = (float)(-totalGoalsH + 325);
                if (goalsScrollOffset > 0.0f) goalsScrollOffset = 0.0f;
                if (totalGoalsH <= 325) goalsScrollOffset = 0.0f;

                for (size_t i = 0; i < goals.size(); ++i) {
                    float rowY = goalsCardRec.y + 60 + i * 115 + goalsScrollOffset;
                    Rectangle cardRec = { goalsCardRec.x + 20, rowY, goalsCardRec.width - 40, 105 };

                    UI::DrawCard(cardRec, Colors::SoftLinen, Colors::SoftBorder, 0.1f, 1.0f);

                    // Name
                    DrawText(goals[i].getName().c_str(), cardRec.x + 15, cardRec.y + 12, 14, Colors::CharcoalText);

                    // Value
                    std::string valStr = "Rs. " + std::to_string((int)goals[i].getCurrent()) + " / Rs. " + std::to_string((int)goals[i].getTarget());
                    int valW = MeasureText(valStr.c_str(), 12);
                    DrawText(valStr.c_str(), cardRec.x + cardRec.width - valW - 35, cardRec.y + 12, 12, Colors::SlateMuted);

                    // Progress bar
                    UI::DrawProgressBar({cardRec.x + 15, cardRec.y + 36, cardRec.width - 55, 10}, (float)goals[i].getProgress(), 
                                        Colors::MatchaGreen, Colors::SoftBorder);

                    // Quick Actions (+1000 and -1000)
                    Rectangle depBtn = { cardRec.x + 15, cardRec.y + 65, 80, 26 };
                    Rectangle wthBtn = { cardRec.x + 105, cardRec.y + 65, 80, 26 };
                    
                    bool hoverDep = CheckCollisionPointRec(mousePos, depBtn);
                    bool hoverWth = CheckCollisionPointRec(mousePos, wthBtn);

                    if (UI::DrawButton(depBtn, "+ Rs. 1,000", Colors::PureCream, Colors::CharcoalText, hoverDep, 11)) {
                        fm.depositToGoal(i, 1000.0);
                    }
                    if (UI::DrawButton(wthBtn, "- Rs. 1,000", Colors::PureCream, Colors::CharcoalText, hoverWth, 11)) {
                        fm.withdrawFromGoal(i, 1000.0);
                    }

                    Rectangle delRec = { cardRec.x + cardRec.width - 32, cardRec.y + 12, 20, 20 };
                    bool hoverDel = CheckCollisionPointRec(mousePos, delRec);
                    UI::DrawCard(delRec, hoverDel ? Colors::SakuraPink : Colors::PureCream, Colors::SoftBorder, 0.3f, 1.0f);
                    DrawText("X", delRec.x + 6, delRec.y + 4, 11, Colors::CharcoalText);

                    if (hoverDel && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        fm.deleteSavingsGoal(i);
                    }
                }
            }
            EndScissorMode();
        }

        // Draw Right Panel stats overlay for Tab 3 (Analytics tab)
        if (activeTab == 3) {
            // Right Panel: Financial Insights
            Rectangle statsCard = { 570, 160, 390, 520 };
            UI::DrawShadow(statsCard, 4.0f);
            UI::DrawCard(statsCard, Colors::PureCream, Colors::SoftBorder, 0.05f, 1.5f);
            
            DrawText("Financial Desk Insights", statsCard.x + 20, statsCard.y + 20, 18, Colors::CharcoalText);
            DrawLineEx({statsCard.x + 20, statsCard.y + 48}, {statsCard.x + statsCard.width - 20, statsCard.y + 48}, 1.0f, Colors::SoftBorder);

            // Sub-card 1: Highest Expense
            Rectangle highCard = { statsCard.x + 20, statsCard.y + 65, 350, 95 };
            UI::DrawCard(highCard, Colors::SoftLinen, Colors::SoftBorder, 0.15f, 1.0f);
            DrawText("Highest Single Expense Log", highCard.x + 15, highCard.y + 12, 12, Colors::SlateMuted);
            
            Transaction highest = fm.getAnalyticsEngine().getHighestExpense(fm.getTransactions());
            if (highest.amount > 0.0) {
                std::string hVal = "Rs. " + std::to_string((int)highest.amount);
                DrawText(hVal.c_str(), highCard.x + 15, highCard.y + 30, 22, Colors::CharcoalText);
                std::string hDetails = highest.category + " at " + (highest.merchant.empty() ? "Unspecified" : highest.merchant);
                DrawText(hDetails.c_str(), highCard.x + 15, highCard.y + 58, 12, Colors::CharcoalText);
                DrawText(highest.date.c_str(), highCard.x + 15, highCard.y + 74, 11, Colors::SlateMuted);
            } else {
                DrawText("No expenses recorded.", highCard.x + 15, highCard.y + 45, 13, Colors::SlateMuted);
            }

            // Sub-card 2: Most Visited Merchant
            Rectangle merchCard = { statsCard.x + 20, statsCard.y + 175, 350, 75 };
            UI::DrawCard(merchCard, Colors::PureCream, Colors::SoftBorder, 0.15f, 1.0f);
            DrawText("Most Visited Merchant", merchCard.x + 15, merchCard.y + 12, 12, Colors::SlateMuted);
            
            std::string faveMerch = fm.getAnalyticsEngine().getMostVisitedMerchant(fm.getTransactions());
            DrawText(faveMerch.c_str(), merchCard.x + 15, merchCard.y + 35, 18, Colors::CharcoalText);

            // Sub-card 3: Average Daily Spending
            Rectangle dailyCard = { statsCard.x + 20, statsCard.y + 265, 350, 75 };
            UI::DrawCard(dailyCard, Colors::PureCream, Colors::SoftBorder, 0.15f, 1.0f);
            DrawText("Average Daily Spending", dailyCard.x + 15, dailyCard.y + 12, 12, Colors::SlateMuted);
            
            double dailyAvg = fm.getAnalyticsEngine().calculateAverageDailySpending(fm.getTransactions());
            std::string avgStr = "Rs. " + std::to_string((int)dailyAvg) + " / day";
            DrawText(avgStr.c_str(), dailyCard.x + 15, dailyCard.y + 35, 18, Colors::CharcoalText);

            // Sub-card 4: Total Goal Savings
            Rectangle saveCard = { statsCard.x + 20, statsCard.y + 355, 350, 75 };
            UI::DrawCard(saveCard, Colors::SoftLinen, Colors::SoftBorder, 0.15f, 1.0f);
            DrawText("Total Saved in Goals", saveCard.x + 15, saveCard.y + 12, 12, Colors::SlateMuted);
            
            double totalSavings = fm.getAnalyticsEngine().calculateTotalSavingsFromGoals(fm.getGoals());
            std::string totalSavingsStr = "Rs. " + std::to_string((int)totalSavings);
            DrawText(totalSavingsStr.c_str(), saveCard.x + 15, saveCard.y + 35, 18, Colors::MatchaGreen);

            Rectangle resetBtn = { statsCard.x + 20, statsCard.y + 455, 350, 38 };
            bool hoverReset = CheckCollisionPointRec(mousePos, resetBtn);
            if (UI::DrawButton(resetBtn, "Reset Ledger & Unlocks", Colors::WarningCoral, Colors::CharcoalText, hoverReset, 13)) {
                fm.resetAll();
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
