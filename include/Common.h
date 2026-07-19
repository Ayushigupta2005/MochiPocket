#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include "raylib.h"

enum class TransactionType {
    INCOME,
    EXPENSE
};

enum class MochiMood {
    HAPPY,
    WORRIED,
    EXCITED,
    CELEBRATING
};

// Pastel Color Palette Constants
namespace Colors {
    const Color SoftLinen      = { 251, 248, 243, 255 }; // Warm off-white background (#FBF8F3)
    const Color PureCream      = { 255, 255, 255, 255 }; // High-contrast card background
    const Color SakuraPink     = { 255, 211, 211, 255 }; // Pastel Pink accent (#FFD3D3)
    const Color MatchaGreen    = { 213, 232, 212, 255 }; // Pastel Green accent (#D5E8D4)
    const Color SoftPeach      = { 255, 223, 196, 255 }; // Pastel Peach secondary (#FFDFC4)
    const Color SoftLavender   = { 230, 220, 254, 255 }; // Pastel Lavender secondary
    const Color CozyYellow     = { 255, 242, 204, 255 }; // Warm Yellow highlight
    const Color SkyCloud       = { 224, 242, 254, 255 }; // Cloud Blue/Soft Cyan
    const Color MochiWhite     = { 255, 250, 245, 255 }; // Soft white for mochi skin
    
    // UI Text & Border Colors
    const Color CharcoalText   = { 60, 60, 60, 255 };     // Soft black body text (#3C3C3C)
    const Color SlateMuted     = { 140, 140, 140, 255 };   // Subdued secondary text (#8C8C8C)
    const Color SoftBorder     = { 230, 225, 218, 255 };   // Card borders
    const Color WarningCoral   = { 255, 179, 153, 255 };   // Peach/Coral warning (#FFB399)
    const Color ShadowGray     = { 0, 0, 0, 15 };          // Subtle drop shadow
}

// Categories list
const std::vector<std::string> DEFAULT_CATEGORIES = {
    "Food",
    "Coffee",
    "Shopping",
    "Transport",
    "Bills",
    "Education",
    "Entertainment",
    "Healthcare",
    "Gifts",
    "Savings",
    "Other"
};

#endif // COMMON_H
